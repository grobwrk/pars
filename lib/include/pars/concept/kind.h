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
#include "pars/ev/kind_decl.h"

namespace pars::ev
{

template<template<typename> typename kind_of>
concept kind_c = (std::is_same_v<kind_of<void>, sent<void>> ||
                  std::is_same_v<kind_of<void>, received<void>> ||
                  std::is_same_v<kind_of<void>, fired<void>>);

template<typename kind_of_event_t>
concept sync_kind_c = kind_c<kind_of_event_t::template kind_type> &&
                      sync_event_c<typename kind_of_event_t::event_type,
                                   kind_of_event_t::template kind_type>;

template<typename kind_of_event_t>
concept async_kind_c = kind_c<kind_of_event_t::template kind_type> &&
                       async_event_c<typename kind_of_event_t::event_type,
                                     kind_of_event_t::template kind_type>;

template<template<typename> typename kind_of,
         template<typename> typename event_t>
  requires kind_c<kind_of>
constexpr auto is_same_kind_v = std::is_same_v<kind_of<void>, event_t<void>>;

} // namespace pars::ev
