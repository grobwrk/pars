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

#include "nngxx/msg.h"

#include "pars/concept/kind.h"
#include "pars/net/hash.h"

#include <string_view>

namespace pars::ev
{

template<typename class_t>
struct uuid
{
  using class_type = class_t;

  static constexpr std::size_t hash = hash_from_uuid(class_type::uuid);
};

template<typename event_t>
struct base_klass
{
  using event_type = event_t;

  /// by default, an event_t requires network
  static constexpr bool requires_network = true;

  /// an event_t executes synchronously in every possibile kind_of<event_t>
  template<template<typename> typename kind_of>
    requires kind_c<kind_of>
  static constexpr executes exec_policy()
  {
    return executes::sync;
  }
};

template<typename event_t>
struct klass
{
  using event_type = event_t;
};

template<>
struct klass<nngxx::msg> : base_klass<nngxx::msg>
{
  using event_type = nngxx::msg;

  static constexpr std::string_view uuid =
    "a7c09171-c503-4cb2-97e4-de8d3fe621b3";
};

template<event_c event_t>
struct klass<std::shared_ptr<event_t>> : base_klass<event_t>
{
  using event_type = std::shared_ptr<event_t>;

  using inner_event_type = event_t;

  static constexpr std::string_view uuid = klass<inner_event_type>::uuid;

  template<template<typename> typename kind_of>
    requires kind_c<kind_of>
  static constexpr executes exec_policy()
  {
    return klass<event_t>::template exec_policy<kind_of>();
  }

  template<typename Archive>
  static void serialize(event_type& ev, Archive& ar)
  {
    ev.reset(new inner_event_type{});

    klass<inner_event_type>::serialize(*ev, ar);
  }
};

} // namespace pars::ev
