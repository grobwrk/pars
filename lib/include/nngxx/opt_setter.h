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

template<opt opt_v, nng_with_set_opt_c nng_t>
struct opt_setter
{
  using nng_type = nng_t;

  static constexpr auto opt = opt_to_nng_value<opt_v>();

  static constexpr auto overload = opt_to_nng_overload<opt_v>();

  template<nng_value_c nng_value_t>
  [[nodiscard]] inline clev::expected<void> operator()(nng_type obj,
                                                       nng_value_t val) noexcept
  {
    int (*set)(nng_type, const char*, nng_value_t);

    if constexpr (std::is_same_v<nng_type, nng_socket>)
    {
      if constexpr (overload == opt_overload::boolean)
        set = nng_socket_set_bool;
      else if constexpr (overload == opt_overload::integer)
        set = nng_socket_set_int;
      else if constexpr (overload == opt_overload::size)
        set = nng_socket_set_size;
      else if constexpr (overload == opt_overload::uint64)
        set = nng_socket_set_uint64;
      else if constexpr (overload == opt_overload::string)
        set = nng_socket_set_string;
      else if constexpr (overload == opt_overload::duration)
        set = nng_socket_set_ms;
      else if constexpr (overload == opt_overload::sockaddr)
        set = nng_socket_set_addr;
    }
    else if constexpr (std::is_same_v<nng_type, nng_ctx>)
    {
      if constexpr (overload == opt_overload::boolean)
        set = nng_ctx_set_bool;
      else if constexpr (overload == opt_overload::integer)
        set = nng_ctx_set_int;
      else if constexpr (overload == opt_overload::size)
        set = nng_ctx_set_size;
      else if constexpr (overload == opt_overload::uint64)
        set = nng_ctx_set_uint64;
      else if constexpr (overload == opt_overload::string)
        set = nng_ctx_set_string;
      else if constexpr (overload == opt_overload::duration)
        set = nng_ctx_set_ms;
    }
    else if constexpr (std::is_same_v<nng_type, nng_listener>)
    {
      if constexpr (overload == opt_overload::boolean)
        set = nng_listener_set_bool;
      else if constexpr (overload == opt_overload::integer)
        set = nng_listener_set_int;
      else if constexpr (overload == opt_overload::size)
        set = nng_listener_set_size;
      else if constexpr (overload == opt_overload::uint64)
        set = nng_listener_set_uint64;
      else if constexpr (overload == opt_overload::string)
        set = nng_listener_set_string;
      else if constexpr (overload == opt_overload::duration)
        set = nng_listener_set_ms;
      else if constexpr (overload == opt_overload::sockaddr)
        set = nng_listener_set_addr;
    }
    else if constexpr (std::is_same_v<nng_type, nng_dialer>)
    {
      if constexpr (overload == opt_overload::boolean)
        set = nng_dialer_set_bool;
      else if constexpr (overload == opt_overload::integer)
        set = nng_dialer_set_int;
      else if constexpr (overload == opt_overload::size)
        set = nng_dialer_set_size;
      else if constexpr (overload == opt_overload::uint64)
        set = nng_dialer_set_uint64;
      else if constexpr (overload == opt_overload::string)
        set = nng_dialer_set_string;
      else if constexpr (overload == opt_overload::duration)
        set = nng_dialer_set_ms;
      else if constexpr (overload == opt_overload::sockaddr)
        set = nng_dialer_set_addr;
    }

    return nngxx::invoke(set, obj, opt, val);
  }
};

} // namespace nngxx
