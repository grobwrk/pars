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

#include "pars/ev/enqueuer.h"
#include "pars/net/op.h"
#include "pars/net/socket_opt.h"

#include <nng/protocol/reqrep0/req.h>
#include <nngpp/ctx.h>
#include <nngpp/dialer.h>
#include <nngpp/listener.h>
#include <nngpp/pipe.h>
#include <nngpp/socket.h>

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

  throw new std::runtime_error(fmt::format("Unable to parse {} to CMODE", str));
}

/**
 * @brief Represents an nng_socket
 */
class socket
{
public:
  /// Construct a socket
  socket(ev::enqueuer& r, nng::socket&& s)
    : router_m{r}
    , socket_m{std::move(s)}
  {
    register_pipe_notify();
  }

  ~socket() { stop(); }

  operator tool_view() { return tool_view{socket_m}; }

  void options(const socket_opt opts)
  {
    if (opts.recv_timeout)
      socket_m.set_opt_ms(NNG_OPT_RECVTIMEO, *opts.recv_timeout);

    if (opts.send_timeout)
      socket_m.set_opt_ms(NNG_OPT_SENDTIMEO, *opts.send_timeout);

    if (opts.req_resend_time)
      socket_m.set_opt_ms(NNG_OPT_REQ_RESENDTIME, *opts.send_timeout);

    if (opts.req_resend_tick)
      socket_m.set_opt_ms(NNG_OPT_REQ_RESENDTICK, *opts.send_timeout);
  }

  socket_opt options() const
  {
    return {
      .recv_timeout = socket_m.get_opt_ms(NNG_OPT_RECVTIMEO),
      .send_timeout = socket_m.get_opt_ms(NNG_OPT_SENDTIMEO),
      .req_resend_time = socket_m.get_opt_ms(NNG_OPT_REQ_RESENDTIME),
      .req_resend_tick = socket_m.get_opt_ms(NNG_OPT_REQ_RESENDTICK),
    };
  }

  void dial(const char* addr) { emplace_dialer(addr).start(); }

  void listen(const char* addr) { emplace_listener(addr).start(); }

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

  nng::ctx make_ctx() const { return nng::make_ctx(socket_m); }

  void send_aio(nng::aio_view a) { socket_m.send(a); }

  void recv_aio(nng::aio_view a) { socket_m.recv(a); }

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
    const char* proto_name_ptr;

    nng_socket_proto_name(socket_m.get(), &proto_name_ptr);

    return proto_name_ptr;
  }

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "Socket #{}-{}", id(), proto_name());
  }

private:
  void pipe_cb(nng_pipe cp, nng_pipe_ev ev)
  {
    auto pv = nng::pipe_view{cp};

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

    // NOTE: pass this, cant move socket
    socket_m.pipe_notify(nng::pipe_ev::add_pre, pipe_cb, this);
    socket_m.pipe_notify(nng::pipe_ev::add_post, pipe_cb, this);
    socket_m.pipe_notify(nng::pipe_ev::rem_post, pipe_cb, this);
    socket_m.pipe_notify(nng::pipe_ev::num, pipe_cb, this);
  }

  nng::listener& emplace_listener(const char* addr)
  {
    return listeners_m.emplace_back(socket_m, addr);
  }

  nng::dialer& emplace_dialer(const char* addr)
  {
    return dialers_m.emplace_back(socket_m, addr);
  }

  ev::enqueuer& router_m;
  op op_m;
  nng::socket socket_m;
  std::vector<nng::dialer> dialers_m;
  std::vector<nng::listener> listeners_m;
};

} // namespace pars::net
