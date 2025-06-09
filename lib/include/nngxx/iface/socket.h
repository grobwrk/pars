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
#include "nngxx/iface/value.h"
#include "nngxx/pipe.h"
#include "nngxx/socket_decl.h"

template<>
struct clev::iface<nng_socket> : nngxx::value<nng_socket>
{
  using value::value;

  [[nodiscard]] inline static nng_socket empty() noexcept
  {
    return NNG_SOCKET_INITIALIZER;
  };

  [[nodiscard]] inline static clev::expected<void>
  destroy(nng_socket* v) noexcept
  {
    return nngxx::invoke(nng_socket_close, *v);
  }

  [[nodiscard]] inline int id() const noexcept { return nng_socket_id(v); }

  inline void send(nngxx::aio_view& a) noexcept { nng_send_aio(v, a); }

  inline void recv(nngxx::aio_view& a) noexcept { nng_recv_aio(v, a); }

  [[nodiscard]] inline clev::expected<void>
  pipe_notify(nngxx::pipe_ev ev, nng_pipe_cb cb, void* arg) noexcept
  {
    return nngxx::invoke(nng_pipe_notify, v, cast_pipe_ev(ev), cb, arg);
  }

  [[nodiscard]] inline clev::expected<const char*> proto_name() const noexcept
  {
    return nngxx::read<const char*>(
      std::bind(nng_socket_proto_name, v, std::placeholders::_1));
  }
};
