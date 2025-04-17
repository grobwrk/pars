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

#include "pars/concept/kind.h"
#include "pars/ev/job.h"
#include "pars/ev/make_hf.h"
#include "pars/ev/spec.h"
#include "pars/fmt/formattable.h"
#include "pars/log.h"

#include <mutex>
#include <unordered_map>

namespace pars::ev
{

class runner;

} // namespace pars::ev

namespace pars::net
{

class rep;
class req;

} // namespace pars::net

namespace pars::ev
{

using job_handler_f = std::function<void(job)>;

struct hf_registry
{
public:
  hf_registry(runner& r)
    : runner_m{r}
  {
  }

  template<template<typename> typename kind_of, ev::event_c event_t,
           typename class_t>
    requires ev::kind_c<kind_of>
  void on(void (class_t::*mem_fn)(hf_arg<kind_of, event_t>), class_t* self)
  {
    insert<kind_of, event_t>(make_hf(mem_fn, self));
  }

private:
  friend net::rep;
  friend net::req;
  friend runner;

  template<template<typename> typename kind_of, event_c event_t>
    requires kind_c<kind_of>
  void insert(handler_f<kind_of, event_t> hf)
  {
    insert(0, std::move(hf));
  }

  /// Insert an handler_f for a kind_of<event_t> on a socket s_id
  template<template<typename> typename kind_of, event_c event_t>
    requires kind_c<kind_of>
  void insert(int s_id, handler_f<kind_of, event_t> hf);

  auto lock() { return std::unique_lock{mtx_m}; }

  bool has_handler_for(int s_id, std::size_t spec_hash)
  {
    return handlers_m[s_id].contains(spec_hash);
  }

  const std::type_info* const type_for(std::size_t spec_hash)
  {
    return types_m[spec_hash];
  }

  const job_handler_f& handler_for(int s_id, std::size_t spec_hash)
  {
    return handlers_m[s_id][spec_hash];
  }

  template<template<typename> typename kind_of, event_c event_t>
    requires kind_c<kind_of>
  auto insert_jhf(int s_id, job_handler_f hf)
  {
    auto spec_hash = spec<kind_of<event_t>>::hash;

    if (!handlers_m[s_id].try_emplace(spec_hash, std::move(hf)).second)
      throw std::runtime_error(fmt::format(
        "Unable to emplace the handler_f for Socket #{} and Spec {:X}", s_id,
        spec_hash));

    // register the type for logging purpose
    types_m[spec_hash] = &typeid(kind_of<event_t>);

    pars::debug(SL, lf::event, "Socket {}: Registered {}!", s_id,
                spec<kind_of<event_t>>{});
  }

  std::mutex mtx_m; /// protects handlers_m, types_m
  std::unordered_map<int, std::unordered_map<std::size_t, job_handler_f>>
    handlers_m; ///< job_handler_f of a spec hash
  std::unordered_map<std::size_t, const std::type_info*>
    types_m; ///< type_info of a spec hash, for debugging purpose only

  runner& runner_m;
};

} // namespace pars::ev
