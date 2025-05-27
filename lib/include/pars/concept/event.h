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

#include "pars/ev/kind_decl.h"

#include <concepts>
#include <string_view>

namespace pars::ev
{

enum class executes
{
  sync,
  async
};

template<typename>
struct klass;

// event_t is an event if klass<event_t> meets these requirements
template<typename event_t>
concept event_c = requires {
  requires std::default_initializable<klass<event_t>>;
  { klass<event_t>::uuid } -> std::same_as<const std::string_view&>;
  { klass<event_t>::requires_network } -> std::same_as<const bool&>;
  { klass<event_t>::template exec_policy<sent>() } -> std::same_as<executes>;
  {
    klass<event_t>::template exec_policy<received>()
  } -> std::same_as<executes>;
  { klass<event_t>::template exec_policy<fired>() } -> std::same_as<executes>;
};

// an internal event event_t does not requires network
template<typename event_t>
concept internal_event_c =
  event_c<event_t> && !klass<event_t>::requires_network;

// a network event event_t requires network
template<typename event_t>
concept network_event_c = event_c<event_t> && klass<event_t>::requires_network;

// a synchronous event event_t requires sync exec_policy for kind_of
template<typename event_t, template<typename> typename kind_of>
concept sync_event_c =
  event_c<event_t> &&
  klass<event_t>::template exec_policy<kind_of>() == executes::sync;

// a synchronous internal event event_t
template<typename event_t, template<typename> typename kind_of>
concept sync_internal_event_c =
  internal_event_c<event_t> && sync_event_c<event_t, kind_of>;

// an synchronous network event event_t
template<typename event_t, template<typename> typename kind_of>
concept sync_network_event_c =
  network_event_c<event_t> && sync_event_c<event_t, kind_of>;

// an asynchronous event event_t requires async exec_policy for kind_of
template<typename event_t, template<typename> typename kind_of>
concept async_event_c =
  event_c<event_t> &&
  klass<event_t>::template exec_policy<kind_of>() == executes::async;

// an asynchronous internal event event_t for kind_of
template<typename event_t, template<typename> typename kind_of>
concept async_internal_event_c =
  async_event_c<event_t, kind_of> && internal_event_c<event_t>;

// an asynchronous network event event_t for kind_of
template<typename event_t, template<typename> typename kind_of>
concept async_network_event_c =
  async_event_c<event_t, kind_of> && network_event_c<event_t>;

} // namespace pars::ev
