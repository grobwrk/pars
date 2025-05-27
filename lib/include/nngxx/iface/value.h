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

#include "clev/own.h"
#include "clev/value.h"
#include "nngxx/concept.h"
#include "nngxx/opt.h"
#include "nngxx/opt_getter.h"
#include "nngxx/opt_setter.h"

#include <type_traits>

namespace nngxx
{

template<nng_c nng_t>
struct value : clev::value<nng_t>
{
  using parent = clev::value<nng_t>;

  using parent::parent;

  using nng_type = parent::value_type;

  [[nodiscard]] inline static bool is_valid(nng_type v) noexcept
  {
    if constexpr (std::is_pointer_v<nng_type>)
      return v != nullptr;
    else
      return v.id != 0;
  }

  /// @name options setters

  [[nodiscard]] inline clev::expected<void>
  set_recv_timeout(nng_duration d) noexcept
  {
    return set_option<opt::recvtimeo>(d);
  }

  [[nodiscard]] inline clev::expected<void>
  set_send_timeout(nng_duration d) noexcept
  {
    return set_option<opt::sendtimeo>(d);
  }

  [[nodiscard]] inline clev::expected<void>
  set_req_resend_time(nng_duration d) noexcept
  {
    return set_option<opt::req_resend_time>(d);
  }

  [[nodiscard]] inline clev::expected<void>
  set_req_resend_tick(nng_duration d) noexcept
  {
    return set_option<opt::req_resend_tick>(d);
  }

  /// @name options getters

  // TODO: use me
  [[nodiscard]] inline clev::expected<const char*>
  get_sock_name() const noexcept
  {
    return get_option<opt::sockname>();
  }

  [[nodiscard]] inline clev::expected<nng_duration>
  get_recv_timeout() const noexcept
  {
    return get_option<opt::recvtimeo>();
  }

  [[nodiscard]] inline clev::expected<nng_duration>
  get_send_timeout() const noexcept
  {
    return get_option<opt::sendtimeo>();
  }

  [[nodiscard]] inline clev::expected<nng_duration>
  get_req_resend_time() const noexcept
  {
    return get_option<opt::req_resend_time>();
  }

  [[nodiscard]] inline clev::expected<nng_duration>
  get_req_resend_tick() const noexcept
  {
    return get_option<opt::req_resend_tick>();
  }

private:
  template<opt opt_v, nng_value_c nng_value_t>
  [[nodiscard]] inline clev::expected<void> set_option(nng_value_t val)
  {
    return opt_setter<opt_v, nng_type>{}(parent::v, val);
  }

  template<opt opt_v>
  [[nodiscard]] inline clev::expected<opt_to_nng_type_t<opt_v>>
  get_option() const
  {
    return opt_getter<opt_v, nng_type>{}(parent::v);
  }
};

} // namespace nngxx

static_assert(ownxx_own_is_really_needed_v);
