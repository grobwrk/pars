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

#include "common.h"

#include <pars/pars.h>

#include <string_view>

namespace pars_example::event
{

// a client send this to request a fib_n computation
struct fib_requested
{
  std::size_t work_id = 0;
  uint64_t n = 0;
  bool use_fast_fib = false;

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "fib_requested({},{},{})", work_id, n,
                          use_fast_fib ? "fast_fib" : "slow_fib");
  }
};

// a server backend send this in response for a fib_n computation
struct fib_computed
{
  std::size_t work_id = 0;
  uint64_t fib_n = 0;

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "fib_computed({},{})", work_id, fib_n);
  }
};

struct stop_compute
{
  int pipe_id = 0;

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "stop_compute({})", pipe_id);
  }
};

} // namespace pars_example::event

template<>
struct pars::ev::klass<::pars_example::event::fib_requested>
  : base_klass<::pars_example::event::fib_requested>
{
  static constexpr std::string_view uuid =
    "9e85e1e6-28b1-4e52-85d6-12ca0110049f";

  template<typename Archive>
  static void serialize(event_type& ev, Archive& ar)
  {
    ar(ev.work_id, ev.n, ev.use_fast_fib);
  }

  template<template<typename> typename kind_of>
    requires kind_c<kind_of>
  static constexpr executes exec_policy()
  {
    if constexpr (enable_compute_fib_async && is_same_kind_v<kind_of, fired>)
      return executes::async;
    else
      return base_klass::exec_policy<kind_of>();
  }
};

template<>
struct pars::ev::klass<::pars_example::event::fib_computed>
  : base_klass<::pars_example::event::fib_computed>
{
  static constexpr std::string_view uuid =
    "3dc87ab0-bfe0-4eea-8f71-76a16bda62ff";

  template<typename Archive>
  static void serialize(event_type& ev, Archive& ar)
  {
    ar(ev.work_id, ev.fib_n);
  }
};

template<>
struct pars::ev::klass<::pars_example::event::stop_compute>
  : base_klass<::pars_example::event::stop_compute>
{
  static constexpr std::string_view uuid =
    "8194470a-3ad8-4f37-a544-d44ff4e5bd29";

  template<typename Archive>
  static void serialize(event_type& ev, Archive& ar)
  {
    ar(ev.pipe_id);
  }
};
