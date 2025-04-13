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

#include <functional>
#include <ratio>

namespace pars::ev
{

template<typename callable_t>
struct hf_traits;

template<template<typename> typename kind_of, event_c event_t>
  requires kind_c<kind_of>
using hf_arg = kind_of<event_t>;

template<template<typename> typename kind_of, event_c event_t>
  requires kind_c<kind_of>
using handler_f = std::move_only_function<void(hf_arg<kind_of, event_t>)>;

template<
  typename mem_fn_t,
  template<typename> typename kind_of = hf_traits<mem_fn_t>::template kind_type,
  typename event_t = hf_traits<mem_fn_t>::event_type,
  typename class_t = hf_traits<mem_fn_t>::class_type>
  requires kind_c<kind_of> && event_c<event_t>
handler_f<kind_of, event_t> make_hf(mem_fn_t& mem_fn, class_t* self)
{
  using namespace std::placeholders;

  static constexpr std::size_t arity = hf_traits<mem_fn_t>::arity;

  if constexpr (std::is_same_v<class_t, void>)
  {
    return handler_f<kind_of, event_t>(std::bind(mem_fn, _1));
  }
  else if constexpr (std::is_same_v<std::integral_constant<std::size_t, arity>,
                                    std::integral_constant<std::size_t, 0>>)
  {
    return handler_f<kind_of, event_t>(
      std::bind(mem_fn, static_cast<class_t*>(self)));
  }
  else if constexpr (std::is_same_v<std::integral_constant<std::size_t, arity>,
                                    std::integral_constant<std::size_t, 1>>)
  {
    return handler_f<kind_of, event_t>(
      std::bind(mem_fn, static_cast<class_t*>(self), _1));
  }
  else if constexpr (std::is_same_v<std::integral_constant<std::size_t, arity>,
                                    std::integral_constant<std::size_t, 2>>)
  {
    return handler_f<kind_of, event_t>(
      std::bind(mem_fn, static_cast<class_t*>(self), _1, _2));
  }

  static_assert(std::ratio_less_equal_v<std::ratio<arity>, std::ratio<2>>,
                "Only fn with arity <= 2 are supported");
}

template<typename return_t, typename class_t,
         template<typename> typename kind_of, event_c event_t>
struct hf_traits<return_t (class_t::*)(hf_arg<kind_of, event_t>)>
{
  using return_type = return_t;

  using class_type = class_t;

  template<typename event2_t>
  using kind_type = kind_of<event2_t>;

  using event_type = event_t;

  static constexpr std::size_t arity = 1;
};

} // namespace pars::ev
