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

#include "pars/ev/hf_registry.h"
#include "pars/ev/runner.h"

namespace pars::ev
{

template<template<typename> typename kind_of, event_c event_t>
  requires kind_c<kind_of>
void hf_registry::insert(int s_id, handler_f<kind_of, event_t> hf)
{
  auto hf_ptr = std::make_shared<handler_f<kind_of, event_t>>(std::move(hf));

  auto guard = std::lock_guard{mtx_m};

  if constexpr (async_kind_c<kind_of<event_t>>)
  {
    return insert_jhf<kind_of, event_t>(s_id, [&, hf_ptr](job j) {
      auto task = std::packaged_task([hf_ptr](std::stop_token tk, job j) {
        auto ke = j.event<kind_of, event_t>();

        auto& md = ke.md();

        md.set_job_id(j.id());

        md.set_stop_token(tk);

        (*hf_ptr)(std::move(ke));
      });

      runner_m.start_thread(spec<kind_of<event_t>>::hash, std::move(task),
                            std::move(j));
    });
  }
  else
  {
    return insert_jhf<kind_of, event_t>(s_id, [hf_ptr](job j) {
      auto ke = j.event<kind_of, event_t>();

      ke.md().set_job_id(j.id());

      (*hf_ptr)(ke);
    });
  }
}

} // namespace pars::ev
