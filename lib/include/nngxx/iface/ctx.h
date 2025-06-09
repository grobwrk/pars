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
#include "nngxx/iface/value.h"
#include "nngxx/socket.h"

#include <nng/nng.h>

template<>
struct clev::iface<nng_ctx> : nngxx::value<nng_ctx>
{
  using value::value;

  [[nodiscard]] inline static nng_ctx empty() noexcept
  {
    return NNG_CTX_INITIALIZER;
  };

  [[nodiscard]] inline static clev::expected<void> destroy(nng_ctx* v) noexcept
  {
    return nngxx::invoke(nng_ctx_close, *v);
  }

  [[nodiscard]] static inline clev::expected<nng_ctx>
  open(nngxx::socket_view& s) noexcept
  {
    return nngxx::make(nng_ctx_open, static_cast<nng_socket>(s));
  }

  [[nodiscard]] inline int id() const noexcept { return nng_ctx_id(v); }

  inline void send(nngxx::aio_view& a) noexcept { nng_ctx_send(v, a); }

  inline void recv(nngxx::aio_view& a) noexcept { nng_ctx_recv(v, a); }
};

static_assert(nngxx_socket_is_really_needed_v);
