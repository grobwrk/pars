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

#include "pars/ev/klass.h"
#include "pars/log/nametype.h"

#include <cereal/cereal.hpp>
#include <fmt/format.h>
#include <nngpp/msg.h>

namespace pars::ev
{

template<typename kind_of_event_t>
class spec
{
public:
  using kind_of_event_type = kind_of_event_t;

  template<typename event_t>
  using kind_type = kind_of_event_type::template kind_type<event_t>;

  using event_type = kind_of_event_type::event_type;

  static constexpr std::size_t hash =
    uuid<kind_type<event_type>>::hash ^ (uuid<klass<event_type>>::hash << 1);

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(
      ctx.out(), "{} \x1b[90m[hash:{:X}, kind:{}({:X}), event:{}({:X})]\x1b[0m",
      nametype<kind_type<event_type>>(), hash, kind_type<event_type>::uuid,
      uuid<kind_type<event_type>>::hash, klass<event_type>::uuid,
      uuid<klass<event_type>>::hash);
  }
};

} // namespace pars::ev
