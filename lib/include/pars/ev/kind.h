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

#include "pars/ev/kind_base.h"
#include "pars/ev/kind_decl.h"
#include "pars/ev/klass.h"

#include <string_view>

namespace pars::ev
{

template<network_event_c event_t>
struct sent<event_t> : base_kind<sent, event_t>
{
  using event_type = event_t;

  using base_kind<sent, event_type>::base_kind;

  static constexpr std::string_view uuid =
    "665b247d-ab3f-421e-8956-1a8c02d42f35";
};

template<typename event_t>
sent(event_t, metadata<sent, event_t>) -> sent<event_t>;

template<network_event_c event_t>
struct received<event_t> : base_kind<received, event_t>
{
  using event_type = event_t;

  using base_kind<received, event_type>::base_kind;

  static constexpr std::string_view uuid =
    "b63dab32-1bc2-46ef-9ca0-0e1530f5c026";
};

template<typename event_t>
received(event_t, metadata<received, event_t>) -> received<event_t>;

template<>
struct received<nngxx::msg> : base_kind<received, nngxx::msg>
{
  using event_type = nngxx::msg;

  using base_kind<received, event_type>::base_kind;

  static constexpr std::string_view uuid =
    "b63dab32-1bc2-46ef-9ca0-0e1530f5c026";

  std::size_t msg_hash() const
  {
    auto h1 = hash_from_uuid(uuid);

    auto h2 = hash_from_msg(event());

    return h1 ^ (h2 << 1);
  }
};

template<event_c event_t>
struct fired<event_t> : base_kind<fired, event_t>
{
  using event_type = event_t;

  using base_kind<fired, event_type>::base_kind;

  static constexpr std::string_view uuid =
    "280c62eb-4c24-4185-964d-e2b242671da7";
};

template<typename event_t>
fired(event_t, metadata<fired, event_t>) -> fired<event_t>;

} // namespace pars::ev
