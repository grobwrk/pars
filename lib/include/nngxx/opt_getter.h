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

#include "nngxx/concept.h"
#include "nngxx/err.h"
#include "nngxx/opt.h"

namespace nngxx
{

template<opt opt_v, nng_with_get_opt_c nng_t>
struct opt_getter
{
  using nng_type = nng_t;

  static constexpr auto opt = opt_to_nng_value<opt_v>();

  static constexpr auto overload = opt_to_nng_overload<opt_v>();

  using return_type = opt_to_nng_type_t<opt_v>;

  [[nodiscard]] inline clev::expected<return_type>
  operator()(nng_type obj) noexcept
  {
    int (*get)(nng_type, const char*, return_type*);

    if constexpr (std::is_same_v<nng_type, nng_socket>)
    {
      if constexpr (overload == opt_overload::boolean)
        get = nng_socket_get_bool;
      else if constexpr (overload == opt_overload::integer)
        get = nng_socket_get_int;
      else if constexpr (overload == opt_overload::size)
        get = nng_socket_get_size;
      else if constexpr (overload == opt_overload::uint64)
        get = nng_socket_get_uint64;
      else if constexpr (overload == opt_overload::string)
        get = nng_socket_get_string;
      else if constexpr (overload == opt_overload::duration)
        get = nng_socket_get_ms;
      else if constexpr (overload == opt_overload::sockaddr)
        get = nng_socket_get_addr;
    }
    else if constexpr (std::is_same_v<nng_type, nng_ctx>)
    {
      if constexpr (overload == opt_overload::boolean)
        get = nng_ctx_get_bool;
      else if constexpr (overload == opt_overload::integer)
        get = nng_ctx_get_int;
      else if constexpr (overload == opt_overload::size)
        get = nng_ctx_get_size;
      else if constexpr (overload == opt_overload::uint64)
        get = nng_ctx_get_uint64;
      else if constexpr (overload == opt_overload::string)
        get = nng_ctx_get_string;
      else if constexpr (overload == opt_overload::duration)
        get = nng_ctx_get_ms;
    }
    else if constexpr (std::is_same_v<nng_type, nng_listener>)
    {
      if constexpr (overload == opt_overload::boolean)
        get = nng_listener_get_bool;
      else if constexpr (overload == opt_overload::integer)
        get = nng_listener_get_int;
      else if constexpr (overload == opt_overload::size)
        get = nng_listener_get_size;
      else if constexpr (overload == opt_overload::uint64)
        get = nng_listener_get_uint64;
      else if constexpr (overload == opt_overload::string)
        get = nng_listener_get_string;
      else if constexpr (overload == opt_overload::duration)
        get = nng_listener_get_ms;
      else if constexpr (overload == opt_overload::sockaddr)
        get = nng_listener_get_addr;
    }
    else if constexpr (std::is_same_v<nng_type, nng_dialer>)
    {
      if constexpr (overload == opt_overload::boolean)
        get = nng_dialer_get_bool;
      else if constexpr (overload == opt_overload::integer)
        get = nng_dialer_get_int;
      else if constexpr (overload == opt_overload::size)
        get = nng_dialer_get_size;
      else if constexpr (overload == opt_overload::uint64)
        get = nng_dialer_get_uint64;
      else if constexpr (overload == opt_overload::string)
        get = nng_dialer_get_string;
      else if constexpr (overload == opt_overload::duration)
        get = nng_dialer_get_ms;
      else if constexpr (overload == opt_overload::sockaddr)
        get = nng_dialer_get_addr;
    }
    else if constexpr (std::is_same_v<nng_type, nng_pipe>)
    {
      if constexpr (overload == opt_overload::boolean)
        get = nng_pipe_get_bool;
      else if constexpr (overload == opt_overload::integer)
        get = nng_pipe_get_int;
      else if constexpr (overload == opt_overload::size)
        get = nng_pipe_get_size;
      else if constexpr (overload == opt_overload::uint64)
        get = nng_pipe_get_uint64;
      else if constexpr (overload == opt_overload::string)
        get = nng_pipe_get_string;
      else if constexpr (overload == opt_overload::duration)
        get = nng_pipe_get_ms;
      else if constexpr (overload == opt_overload::sockaddr)
        get = nng_pipe_get_addr;
    }

    return_type val;

    return nngxx::invoke(get, obj, opt, &val).transform_to(std::move(val));
  }
};

} // namespace nngxx
