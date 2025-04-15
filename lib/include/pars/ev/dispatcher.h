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

#include "pars/ev/event.h"
#include "pars/ev/hf_registry.h"
#include "pars/ev/job.h"
#include "pars/ev/kind.h"
#include "pars/ev/runner.h"
#include "pars/log.h"

#include <condition_variable>
#include <deque>
#include <mutex>

namespace pars::ev
{

class dispatcher
{
public:
  dispatcher(runner& r)
    : runner_m{r}
  {
  }

  /// @name Running Jobs

  void run()
  {
    running_m = true;

    queue_back(fired{init{}, {}});

    for (;;)
    {
      auto lock = std::unique_lock{mtx_m};

      cond_m.wait(lock, [&]() { return !queue_m.empty() || !running_m; });

      if (!running_m)
      {
        cond_m.wait(lock, [&]() { return terminate_m; });

        return;
      }

      // NOTE: exec is executed after mtx_m unlock

      runner_m.exec(next_job(lock));
    }
  }

  void stop_running()
  {
    auto guard = std::lock_guard{mtx_m};

    queue_m.clear();

    runner_m.stop_all_threads();

    running_m = false;

    cond_m.notify_one();
  }

  void terminate_now()
  {
    auto guard = std::lock_guard{mtx_m};

    if (running_m)
      throw std::runtime_error("Call stop_running first!");

    terminate_m = true;

    cond_m.notify_one();
  }

  bool terminating()
  {
    auto guard = std::lock_guard{mtx_m};

    return running_m == false;
  }

  /// @name Managing Queue

  template<template<typename> typename kind_of, event_c event_t>
    requires kind_c<kind_of>
  void queue_back(kind_of<event_t> ke)
  {
    auto guard = std::lock_guard{mtx_m};

    queue(std::move(ke),
          std::bind(std::mem_fn<void(decltype(queue_m)::value_type&&)>(
                      &decltype(queue_m)::push_back),
                    &queue_m, std::placeholders::_1));
  }

  template<template<typename> typename kind_of, event_c event_t>
    requires kind_c<kind_of>
  void queue_front(kind_of<event_t> ke)
  {
    auto guard = std::lock_guard{mtx_m};

    queue(std::move(ke),
          std::bind(std::mem_fn<void(decltype(queue_m)::value_type&&)>(
                      &decltype(queue_m)::push_front),
                    &queue_m, std::placeholders::_1));
  }

private:
  /// @name Multi Threading

  std::mutex mtx_m;
  std::condition_variable cond_m;

  /// @name Running Jobs

  job next_job(auto& lock)
  {
    auto j{std::move(queue_m.front())};

    queue_m.pop_front();

    // we would like to use exec(next_job(std::move(lock)))
    // but it is implementation-defined wether the lock moved into next_job
    // is destroyed before exec is executed or not
    // https://eel.is/c++draft/expr.call#6.sentence-10

    // for this reason, we are going to unlock here
    lock.unlock();

    return j;
  }

  bool terminate_m{false}; ///< terminate run and exit
  bool running_m{false};   ///< wether we're running jobs
  runner& runner_m;

  /// @name Managing Queue

  template<template<typename> typename kind_of, event_c event_t>
    requires kind_c<kind_of>
  void queue(kind_of<event_t> ke, auto push_fn)
  {
    if (!running_m)
      return;

    auto j_id = runner_m.next_job_id();

    push_fn(make_job(j_id, std::move(ke)));

    if constexpr (internal_event_c<event_t>)
    {
      pars::debug(SL, lf::event, "Job #{} pushed [# jobs: {}]", j_id,
                  queue_m.size());
    }
    else if constexpr (network_event_c<event_t>)
    {
      auto p_id = ke.md().pipe().id();

      runner_m.associate_job_to_pipe(j_id, p_id);

      cond_m.notify_one();
    }
  }

  std::deque<job> queue_m; ///< a deque of all jobs
};

} // namespace pars::ev
