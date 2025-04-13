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

#include "pars/concept/event.h"
#include "pars/concept/kind.h"
#include "pars/ev/metadata.h"

namespace pars::ev
{

template<template<typename> typename kind_of, event_c event_t>
  requires kind_c<kind_of>
struct common_kind
{
  template<typename event2_t>
  using kind_type = kind_of<event2_t>;

  using event_type = event_t;

  using metadata_type = metadata<kind_of, event_t>;

  event_type& event() { return event_m; }

  const event_type& event() const { return event_m; }

  metadata_type& metadata() { return metadata_m; }

  std::tuple<const event_type&, metadata_type&> as_tuple()
  {
    return std::forward_as_tuple(event_m, metadata_m);
  }

  common_kind(event_type e, metadata_type md)
    : event_m{std::move(e)}
    , metadata_m{std::move(md)}
  {
  }

protected:
  event_type event_m;
  metadata_type metadata_m;
};

template<template<typename> typename kind_of, event_c event_t>
  requires kind_c<kind_of>
struct base_kind;

template<template<typename> typename kind_of,
         sync_internal_event_c<kind_of> event_t>
  requires kind_c<kind_of>
struct base_kind<kind_of, event_t> : common_kind<kind_of, event_t>
{
  using common_kind<kind_of, event_t>::common_kind;
};

template<template<typename> typename kind_of,
         sync_network_event_c<kind_of> event_t>
  requires kind_c<kind_of>
struct base_kind<kind_of, event_t> : common_kind<kind_of, event_t>
{
  using common_kind<kind_of, event_t>::common_kind;
};

template<template<typename> typename kind_of,
         async_internal_event_c<kind_of> event_t>
  requires kind_c<kind_of>
struct base_kind<kind_of, event_t> : common_kind<kind_of, event_t>
{
  using common_kind<kind_of, event_t>::common_kind;
};

template<template<typename> typename kind_of,
         async_network_event_c<kind_of> event_t>
  requires kind_c<kind_of>
struct base_kind<kind_of, event_t> : common_kind<kind_of, event_t>
{
  using common_kind<kind_of, event_t>::common_kind;
};

} // namespace pars::ev
