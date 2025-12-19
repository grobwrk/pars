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
#ifndef PARS_NET_CONTEXT_H
#define PARS_NET_CONTEXT_H

#include "pars/concept/event.h"
#include "pars/ev/enqueuer.h"
#include "pars/net/context_opt.h"
#include "pars/net/op.h"
#include "pars/net/pipe.h"
#include "pars/net/socket.h"
#include "pars/net/tool_view.h"

#include "nngxx/aio.h"
#include "nngxx/ctx.h"

#include <format>
#include <utility>

namespace pars::net
{

class context
{
public:
  context(ev::enqueuer& e, nngxx::ctx&& c, const net::socket& s)
    : enqueuer_m{e}
    , ctx_m{std::move(c)}
    , sock_m{s}
  {
  }

  ~context() { stop(); }

  operator tool_view() { return tool_view{ctx_m}; }

  void set_options(context_opt opts)
  {
    if (opts.recv_timeout)
      ctx_m.set_recv_timeout(*opts.recv_timeout).or_abort();

    if (opts.send_timeout)
      ctx_m.set_send_timeout(*opts.send_timeout).or_abort();
  }

  context_opt options() const
  {
    return {.recv_timeout = ctx_m.get_recv_timeout().value_or_abort(),
            .send_timeout = ctx_m.get_send_timeout().value_or_abort()};
  }

  void send_aio(nngxx::aio_view& a) { ctx_m.send(a); }

  void recv_aio(nngxx::aio_view& a) { ctx_m.recv(a); }

  template<ev::event_c event_t>
  void send(event_t ev, pipe p)
  {
    op_m.send(enqueuer_m, *this, p, std::move(ev));
  }

  void recv() { op_m.recv(enqueuer_m, *this); }

  void stop() { op_m.stop(); }

  int id() const { return ctx_m.id(); }

  int socket_id() const { return sock_m.id(); }

  const net::socket& sock() const { return sock_m; }

  auto format_to(std::format_context& ctx) const -> decltype(ctx.out())
  {
    return std::format_to(ctx.out(), "context #{}", id());
  }

private:
  ev::enqueuer& enqueuer_m;
  op op_m;
  nngxx::ctx ctx_m;
  const net::socket& sock_m;
};

} // namespace pars::net

#endif // PARS_NET_CONTEXT_H
