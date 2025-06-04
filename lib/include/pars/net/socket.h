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
#include "nngxx/ctx.h"
#include "nngxx/dialer.h"
#include "nngxx/listener.h"
#include "nngxx/pipe.h"

#include "pars/ev/enqueuer.h"
#include "pars/net/op.h"
#include "pars/net/socket_opt.h"

#include <string_view>
#include <vector>

namespace pars::net
{

enum class cmode
{
  dial,
  listen
};

static cmode cmode_from_string(const char* str)
{
  auto str_view = std::string_view(str);

  if (str_view.compare("dial") == 0)
    return cmode::dial;
  else if (str_view.compare("listen") == 0)
    return cmode::listen;

  throw std::runtime_error(fmt::format("Unable to parse {} to CMODE", str));
}

/**
 * @brief Represents an nng_socket
 */
class socket
{
public:
  /// Construct a socket
  socket(ev::enqueuer& r, nngxx::socket&& s)
    : router_m{r}
    , socket_m{std::move(s)}
  {
    register_pipe_notify();
  }

  ~socket() { stop(); }

  operator tool_view() { return tool_view{socket_m}; }

  void set_options(const socket_opt opts)
  {
    if (opts.recv_timeout)
      socket_m.set_recv_timeout(*opts.recv_timeout).or_else(clev::abort_now());

    if (opts.send_timeout)
      socket_m.set_send_timeout(*opts.send_timeout).or_else(clev::abort_now());

    if (opts.req_resend_time)
      socket_m.set_req_resend_time(*opts.req_resend_time)
        .or_else(clev::abort_now());

    if (opts.req_resend_tick)
      socket_m.set_req_resend_tick(*opts.req_resend_tick)
        .or_else(clev::abort_now());
  }

  socket_opt options() const
  {
    return {
      .recv_timeout = socket_m.get_recv_timeout()
                        .or_else(clev::abort_now<nng_duration>())
                        .value(),

      .send_timeout = socket_m.get_send_timeout()
                        .or_else(clev::abort_now<nng_duration>())
                        .value(),

      .req_resend_time = socket_m.get_req_resend_time()
                           .or_else(clev::abort_now<nng_duration>())
                           .value(),

      .req_resend_tick = socket_m.get_req_resend_tick()
                           .or_else(clev::abort_now<nng_duration>())
                           .value(),
    };
  }

  void dial(const char* addr)
  {
    emplace_dialer(addr).start().or_else(clev::abort_now());
  }

  void listen(const char* addr)
  {
    emplace_listener(addr).start().or_else(clev::abort_now());
  }

  void connect(const char* addr, const cmode mode)
  {
    switch (mode)
    {
    case cmode::dial:
      dial(addr);

      break;
    case cmode::listen:
      listen(addr);

      break;
    }
  }

  nngxx::ctx make_ctx()
  {
    return nngxx::make_ctx(socket_m)
      .or_else(clev::abort_now<nngxx::ctx>())
      .value();
  }

  void send_aio(nngxx::aio_view& a) { socket_m.send(a); }

  void recv_aio(nngxx::aio_view& a) { socket_m.recv(a); }

  template<ev::event_c event_t>
  void send(event_t ev, pipe p = {})
  {
    op_m.send(router_m, *this, p, ev);
  }

  void recv() { op_m.recv(router_m, *this); }

  void stop() { op_m.stop(); }

  int id() const { return socket_m.id(); }

  int socket_id() const { return id(); }

  const char* proto_name() const
  {
    return socket_m.proto_name().value_or("<not-found>");
  }

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "Socket #{}-{}", id(), proto_name());
  }

private:
  void pipe_cb(nng_pipe cp, nng_pipe_ev ev)
  {
    auto pv = nngxx::pipe_view{cp};

    switch (ev)
    {
    case NNG_PIPE_EV_ADD_PRE: {
      pars::debug(SL, lf::net, "Pipe 0x{:X} creating! [{}]", pv.id(), *this);

      router_m.queue_fire(ev::creating_pipe{}, id(), *this, net::pipe{pv});
    }
    break;

    case NNG_PIPE_EV_ADD_POST: {
      pars::debug(SL, lf::net, "Pipe 0x{:X} created! [{}]", pv.id(), *this);

      router_m.queue_fire(ev::pipe_created{}, id(), *this, net::pipe{pv});
    }
    break;

    case NNG_PIPE_EV_REM_POST: {
      pars::debug(SL, lf::net, "Pipe 0x{:X} removed! [{}]", pv.id(), *this);

      router_m.queue_fire(ev::pipe_removed{}, id(), *this, net::pipe{pv});
    }
    break;

    case NNG_PIPE_EV_NUM: {
      pars::debug(SL, lf::net, "Pipe 0x{:X} num notified", pv.id());
    }
    break;
    }
  }

  void register_pipe_notify()
  {
    static auto pipe_cb = [](nng_pipe p, nng_pipe_ev ev, void* self) {
      static_cast<socket*>(self)->pipe_cb(p, ev);
    };

    // NOTE: in the following calles we pass this, hence we cant move socket

    socket_m.pipe_notify(nngxx::pipe_ev::add_pre, pipe_cb, this)
      .or_else(clev::abort_now());

    socket_m.pipe_notify(nngxx::pipe_ev::add_post, pipe_cb, this)
      .or_else(clev::abort_now());

    socket_m.pipe_notify(nngxx::pipe_ev::rem_post, pipe_cb, this)
      .or_else(clev::abort_now());

    socket_m.pipe_notify(nngxx::pipe_ev::num, pipe_cb, this)
      .or_else(clev::abort_now());
  }

  nngxx::listener& emplace_listener(const char* addr)
  {
    listeners_m.push_back(nngxx::make_listener(socket_m, addr)
                            .or_else(clev::abort_now<nngxx::listener>())
                            .value());

    return listeners_m.back();
  }

  nngxx::dialer& emplace_dialer(const char* addr)
  {
    dialers_m.push_back(nngxx::make_dialer(socket_m, addr)
                          .or_else(clev::abort_now<nngxx::dialer>())
                          .value());

    return dialers_m.back();
  }

  ev::enqueuer& router_m;
  op op_m;
  nngxx::socket socket_m;
  std::vector<nngxx::dialer> dialers_m;
  std::vector<nngxx::listener> listeners_m;
};

} // namespace pars::net
