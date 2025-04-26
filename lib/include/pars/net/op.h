/*
Copyright (c) 2025 Giuseppe Roberti.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "nngxx/aio.h"
#include "nngxx/err.h"
#include "nngxx/msg.h"
#include "nngxx/pipe.h"

#include "pars/ev/enqueuer.h"
#include "pars/ev/event.h"
#include "pars/ev/serializer.h"
#include "pars/fmt/helpers.h"
#include "pars/net/dir.h"

#include <spdlog/spdlog.h>

#include <expected>
#include <functional>

namespace pars::net
{

using cb_f = std::function<void(clev::expected<void>, nngxx::msg)>;

class op
{
public:
  ~op()
  {
    if (aio_m)
      aio_m.wait();
  }

  explicit operator bool() { return static_cast<bool>(aio_m); }

  template<ev::event_c event_t, tool_c tool_t>
  clev::expected<void> send(ev::enqueuer& r, tool_t& t, pipe p, event_t ev)
  {
    auto m = ev::serialize::to_network(ev);

    if (p)
      m.set_pipe(p);

    pars::debug(SL, lf::net, "{}: Send Message [{}]!", f::pntl{p, t}, m);

    // replace the callback with the new one
    cb_m = [&, p, ev = std::move(ev)](clev::expected<void> res,
                                      nngxx::msg m) mutable {
      if (res)
      {
        // NOTE: m is empty on success

        pars::debug(SL, lf::net, "{}: Sent Event [{}]!", f::pntl{p, t}, ev);

        r.queue_sent(std::move(ev), t.socket_id(), t, p);
      }
      else
      {
        // NOTE: m is not empty on failure

        auto pv = m.get_pipe();

        pars::err(SL, lf::net, "{}: Error Sending {}! [msg:{},err:{}]",
                  f::pntl{pv, t}, nametype(ev), m, res.error());

        r.queue_fire(ev::network_error{res.error(), dir::out}, t.socket_id(), t,
                     pv);
      }
    };

    return nngxx::make_aio(op::send_cb, this, std::move(m))
      .and_assign_to(aio_m)
      .and_then([&](auto aio) {
        t.send_aio(aio);

        return clev::expected<void>{};
      });
  }

  template<tool_c tool_t>
  void recv(ev::enqueuer& r, tool_t& t)
  {
    pars::debug(SL, lf::net, "{}: Receive Message!", f::pntl{{}, t});

    // replace the operation with the new one
    cb_m = [&](clev::expected<void> res, nngxx::msg m) {
      if (res)
      {
        // NOTE: m is not empty on success

        auto pv = m.get_pipe();

        pars::debug(SL, lf::net, "{}: Received Message! [{}]", f::pntl{pv, t},
                    m);

        r.queue_received(std::move(m), t.socket_id(), t, pv);
      }
      else
      {
        // NOTE: m is empty on failure

        auto pv = nngxx::pipe_view();

        pars::err(SL, lf::net, "{}: Error Receiving! [{}]", f::pntl{pv, t},
                  res.error());

        r.queue_fire(ev::network_error{res.error(), dir::in}, t.socket_id(), t,
                     pv);
      }
    };

    // make aio - NOTE: pass this, cant move op
    aio_m = nngxx::make_aio(op::send_cb, this).value_or_abort();

    // start recv
    t.recv_aio(aio_m);
  }

  void sleep(nng_duration ms, std::function<void()> f)
  {
    cb_m = [&, f](clev::expected<void> res, nngxx::msg m) {
      if (res)
      {
        // the sleep completed successfully, execute f
        f();
      }
    };

    // make aio - NOTE: pass this, cant move op
    aio_m = nngxx::make_aio(op::send_cb, this).value_or_abort();

    nngxx::sleep(ms, aio_m);
  }

  void reset_sleep(nng_duration ms)
  {
    stop();

    // make aio - NOTE: pass this, cant move op
    aio_m = nngxx::make_aio(op::send_cb, this).value_or_abort();

    nngxx::sleep(ms, aio_m);
  }

  /**
   * @brief return result of asynchronous operation
   * @return
   *
   * The nng_aio_result() returns the result of the operation associated with
   * the handle aio. If the operation was successful, then 0 is returned.
   * Otherwise a non-zero error code is returned.
   */
  std::error_code result() const
  {
    return aio_m.result().error_or(nngxx::err::success);
  }

  /**
   * @brief abort asynchronous I/O operation
   * @param err
   *
   * The abort() function aborts an operation previously started with the handle
   * aio.
   *
   * If the operation is aborted, then the callback for the handle will be
   * called, and the function result() will return the error err.
   */
  void abort(nngxx::err err) { aio_m.abort(err); }

  /**
   * @brief cancel asynchronous I/O operation
   *
   * The nng_aio_cancel() function aborts an operation previously started with
   * the handle aio. If the operation is aborted, then the callback for the
   * handle will be called, and the function nng_aio_result() will return the
   * error NNG_ECANCELED.
   *
   * This function does not wait for the operation to be fully aborted, but
   * returns immediately.
   *
   * If no operation is currently in progress (either because it has already
   * finished, or no operation has been started yet), then this function has no
   * effect.
   *
   * Same as abort(nngxx::error::canceled)
   */
  void cancel() { aio_m.cancel(); }

  /**
   * @brief wait for asynchronous I/O operation
   *
   * The nng_aio_wait() function waits for an asynchronous I/O operation to
   * complete. If the operation has not been started, or has already completed,
   * then it returns immediately.
   */
  void wait() const { aio_m.wait(); }

  /**
   * @brief stop asynchronous I/O operation
   *
   * The stop() function stops the asynchronous I/O operation associated with
   * aio by aborting with NNG_ECANCELED, and then waits for it to complete or to
   * be completely aborted, and for the callback associated with the aio to have
   * completed executing.
   *
   * Same as cancel() + wait().
   */
  void stop() { aio_m.stop(); }

private:
  static void send_cb(void* arg)
  {
    auto self = static_cast<op*>(arg);

    // get the result
    auto res = self->aio_m.result();

    nngxx::msg msg;

    if (res)
    {
      // take ownership of the message
      msg = self->aio_m.release_msg();
    }

    // execute the callback passing ownership of the message
    self->cb_m(res, std::move(msg));
  }

  static void recv_cb(void* arg)
  {
    auto self = static_cast<op*>(arg);

    // get the result
    auto res = self->aio_m.result();

    nngxx::msg msg;

    if (res)
    {
      // take ownership of the message
      msg = self->aio_m.release_msg();
    }

    // execute the callback passing ownership of the message
    self->cb_m(res, std::move(msg));
  }

  static void sleep_cb(void* arg)
  {
    auto self = static_cast<op*>(arg);

    auto ok = self->aio_m.result();

    self->cb_m(ok, nngxx::msg{});
  }

  nngxx::aio aio_m;
  cb_f cb_m;
};

} // namespace pars::net
