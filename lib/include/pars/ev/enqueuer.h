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
#include "pars/concept/net.h"
#include "pars/ev/dispatcher.h"
#include "pars/ev/event.h"
#include "pars/ev/hf_registry.h"
#include "pars/ev/runner.h"

namespace pars::ev
{

class enqueuer
{
public:
  enqueuer(dispatcher& d, runner& r)
    : dispatcher_m{d}
    , runner_m{r}
  {
  }

  template<internal_event_c event_t>
  void queue_fire(event_t ev)
  {
    dispatcher_m.queue_back(fired{std::move(ev), {}});
  }

  template<template<typename> typename kind_of, network_event_c event_t,
           network_event_c event2_t>
    requires kind_c<kind_of>
  void queue_fire(event_t ev, ev::metadata<kind_of, event2_t> md)
  {
    queue_fire(ev, md.pipe().socket_id(), md.tool(), md.pipe());
  }

  template<network_event_c event_t, net::tool_c tool_t>
  void queue_fire(event_t ev, const int s_id, tool_t& t, const net::pipe& p)
  {
    if constexpr (std::is_same_v<event_t, creating_pipe> ||
                  std::is_same_v<event_t, pipe_created> ||
                  std::is_same_v<event_t, pipe_removed>)
    {
      if (dispatcher_m.terminating())
      {
        p.close();

        return;
      }

      if constexpr (std::is_same_v<event_t, creating_pipe>)
        runner_m.add_pipe(p);
      else if constexpr (std::is_same_v<event_t, pipe_removed>)
        runner_m.remove_pipe(p);

      if (!runner_m.can_exec(s_id, spec<fired<event_t>>::hash))
        return;
    }

    dispatcher_m.queue_back(fired{std::move(ev), {s_id, t, p}});
  }

  template<network_event_c event_t, net::tool_c tool_t>
  void queue_sent(event_t ev, int s_id, tool_t& t, net::pipe p)
  {
    dispatcher_m.queue_back(sent{std::move(ev), {s_id, t, p}});
  }

  template<net::tool_c tool_t>
  void queue_received(nng::msg m, int s_id, tool_t& t, net::pipe p)
  {
    dispatcher_m.queue_back(received{std::move(m), {s_id, t, p}});
  }

private:
  dispatcher& dispatcher_m;
  runner& runner_m;
};

} // namespace pars::ev
