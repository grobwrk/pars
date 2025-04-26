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

#include <nng/nng.h>

#include <concepts>

namespace nngxx
{

template<typename nng_value_t>
concept nng_value_c = std::same_as<nng_value_t, nng_duration> ||
                      std::same_as<nng_value_t, nng_sockaddr> ||
                      std::same_as<nng_value_t, const char*>;

template<typename nng_t>
concept nng_with_set_opt_c =
  std::same_as<nng_t, nng_socket> || std::same_as<nng_t, nng_ctx> ||
  std::same_as<nng_t, nng_listener> || std::same_as<nng_t, nng_dialer>;

template<typename nng_t>
concept nng_with_get_opt_c =
  nng_with_set_opt_c<nng_t> || std::same_as<nng_t, nng_pipe>;

template<typename nng_t>
concept nng_c = nng_with_get_opt_c<nng_t> || std::same_as<nng_t, nng_msg*> ||
                std::same_as<nng_t, nng_aio*>;

template<typename uint_t>
concept uint_c =
  std::same_as<uint_t, uint16_t> || std::same_as<uint_t, uint32_t> ||
  std::same_as<uint_t, uint64_t>;

template<typename value_t>
concept move_only_constructible_c =
  std::move_constructible<value_t> && !std::copy_constructible<value_t>;

template<typename value_t>
concept move_only_assignable_c =
  std::is_move_assignable_v<value_t> && !std::is_copy_assignable_v<value_t>;

template<typename value_t>
concept movable_only_c = std::movable<value_t> && !std::copyable<value_t>;
} // namespace nngxx
