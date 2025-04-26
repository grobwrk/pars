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
#include "pars/ev/hf_registry__insert.h"
#include "pars/ev/make_hf.h"
#include "pars/net/context_registry.h"
#include "pars/net/socket.h"

namespace pars::net
{

/**
 * @brief Represents an nng_req protocol
 */
class req
{
public:
  /// Construct a req
  req(ev::hf_registry& h, ev::enqueuer& r)
    : sock_m{r, nngxx::req::v0::make_socket().value_or_abort()}
    , ctx_registry_m{r, sock_m}
    , hf_registry_m{h}
  {
  }

  /// Get the socket
  socket& sock() { return sock_m; }

  /// Get the socket
  const socket& sock() const { return sock_m; }

  /// Get the context_registry
  context_registry& ctxs() { return ctx_registry_m; }

  /// Stop socket and all contexts
  void stop()
  {
    sock_m.stop();

    ctx_registry_m.stop_all();
  }

  template<template<typename> typename kind_of, ev::event_c event_t,
           typename class_t>
    requires ev::kind_c<kind_of>
  void on(void (class_t::*hf)(ev::hf_arg<kind_of, event_t>), class_t* self)
  {
    insert<kind_of, event_t>(ev::make_hf(hf, self));
  }

  template<template<typename> typename kind_of, ev::event_c event_t>
    requires ev::kind_c<kind_of>
  void insert(ev::handler_f<kind_of, event_t> hf)
  {
    hf_registry_m.insert(sock_m.id(), std::move(hf));
  }

private:
  socket sock_m;
  context_registry ctx_registry_m;
  ev::hf_registry& hf_registry_m;
};

} // namespace pars::net
