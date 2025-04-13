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

#include <functional>
#include <future>
#include <thread>
#include <unordered_map>

namespace pars::ev
{

struct runner
{
public:
  runner(hf_registry& hfs)
    : hf_registry_m{hfs}
  {
  }

  std::size_t next_job_id() { return next_job_id_m++; }

  void start_thread(std::size_t spec_hash,
                    std::packaged_task<void(std::stop_token, job)> task, job j)
  {
    auto j_id = j.id();

    auto s_id = j.socket_id();

    auto guard = std::lock_guard{mtx_m};

    futures_m.emplace_back(j_id, s_id, spec_hash, task.get_future());

    auto th_res = threads_m.try_emplace(j_id, std::move(task), std::move(j));

    if (!th_res.second)
      futures_m.pop_back();
  }

  auto count_threads()
  {
    auto guard = std::lock_guard{mtx_m};

    return futures_m.size();
  }

  void stop_all_threads()
  {
    auto guard = std::lock_guard{mtx_m};

    for (const auto& t : threads_m)
    {
      if (stop_possible(t.first))
        request_stop(t.first);
    }

    while (!futures_m.empty())
      process_futures();

    pipe_jobs_m.clear();
  }

  bool can_exec(int s_id, std::size_t spec_hash)
  {
    auto lock = hf_registry_m.lock();

    return hf_registry_m.has_handler_for(s_id, spec_hash);
  }

  void exec(job j)
  {
    auto spec_hash = j.spec_hash();

    auto s_id = j.socket_id();

    auto lock = hf_registry_m.lock();

    if (!hf_registry_m.has_handler_for(s_id, spec_hash))
    {
      pars::err(SL, lf::event,
                "Unable to find handler for Spec 0x{:X} on Socket {}, skip "
                "message ...",
                spec_hash, s_id);

      return;
    }

    auto& hf = hf_registry_m.handler_for(s_id, spec_hash);

    lock.unlock();

    pars::debug(SL, lf::event, "Job #{}: Running Handler [{}]", j.id(),
                demangle(hf_registry_m.type_for(spec_hash)->name()));

    try
    {
      {
        auto guard = std::lock_guard{mtx_m};

        process_futures();
      }

      hf(std::move(j));
    }
    catch (...)
    {
      process_exception(s_id, spec_hash);
    }
  }

  template<template<typename> typename kind_of, event_c event_t>
    requires kind_c<kind_of>
  void exec(kind_of<event_t> ke)
  {
    exec(make_job(next_job_id(), std::move(ke)));
  }

  void add_pipe(const net::pipe& p)
  {
    auto guard = std::lock_guard{mtx_m};

    auto p_id = p.id();

    pipe_jobs_m.insert({p_id, {}});
  }

  /// stop all running and remove all pending jobs for pipe p
  void remove_pipe(const net::pipe& p)
  {
    auto guard = std::lock_guard{mtx_m};

    auto p_id = p.id();

    if (!pipe_jobs_m.at(p_id).empty())
    {
      // stop all running jobs for pipe p
      stop_runnings(pipe_jobs_m.at(p_id));
    }

    pipe_jobs_m.erase(p_id);
  }

  void associate_job_to_pipe(const int j_id, const int p_id)
  {
    if (j_id <= 0)
      throw std::runtime_error(fmt::format("Job #{}: invalid Job!", p_id));

    if (p_id <= 0)
      return;

    auto guard = std::lock_guard{mtx_m};

    // add j_id to the jobs related to p, unless p was removed with a previous
    // call to remove_pipe(p) (ie: pipe_jobs_m does not contains p.id())
    if (pipe_jobs_m.contains(p_id))
    {
      pipe_jobs_m.at(p_id).push_back(j_id);

      pars::debug(SL, lf::event,
                  "Job #{} pushed and associated to Pipe {:X} [# size: {}]",
                  j_id, p_id, pipe_jobs_m.at(p_id).size());
    }
  }

  void stop_thread(const int j_id)
  {
    auto guard = std::lock_guard(mtx_m);

    if (stop_possible(j_id))
      request_stop(j_id);
  }

private:
  std::unordered_set<std::size_t> stop_runnings(std::vector<std::size_t> j_ids)
  {
    std::unordered_set<std::size_t> pending_jobs;

    for (const auto& j_id : j_ids)
    {
      if (thread_running(j_id))
      {
        // stop running job, if possible
        if (stop_possible(j_id))
          request_stop(j_id);
      }
      else
      {
        pending_jobs.insert(j_id);
      }
    }

    return pending_jobs;
  }

  void process_futures()
  {
    for (auto it = futures_m.begin(); it != futures_m.end();)
    {
      using namespace std::chrono_literals;

      auto& [j_id, s_id, spec_hash, f] = *it;

      if (f.wait_for(0ms) == std::future_status::ready)
      {
        try
        {
          f.get();
        }
        catch (...)
        {
          pars::debug(SL, lf::event,
                      "Job #{}: Throws, processing exceptions !!", j_id);

          process_exception(s_id, spec_hash);
        }

        threads_m.erase(j_id);

        pars::debug(SL, lf::event, "Job #{}: Done! [# futures: {}]", j_id,
                    futures_m.size());

        it = futures_m.erase(it);
      }
      else
      {
        ++it;
      }
    }
  }

  void process_exception(auto s_id, auto spec_hash)
  {
    auto e_ptr = std::current_exception();

    try
    {
      std::rethrow_exception(e_ptr);
    }
    catch (std::exception& e)
    {
      pars::err(SL, lf::event, "Handler for 0x{:X} throws: {}", spec_hash,
                e.what());
    }
    catch (...)
    {
      pars::err(SL, lf::event, "Handler for 0x{:X} throws: Unknown Exception",
                spec_hash);
    }

    auto e_hash = spec<fired<exception>>::hash;

    auto lock = hf_registry_m.lock();

    if (!hf_registry_m.has_handler_for(s_id, e_hash))
      return;

    auto& hf = hf_registry_m.handler_for(s_id, e_hash);

    lock.unlock();

    try
    {
      hf(make_job(next_job_id(), fired{exception{e_ptr}, {}}));
    }
    catch (std::exception& e)
    {
      pars::err(SL, lf::event, "Exception Handler for 0x{:X} throws: {}",
                spec_hash, e.what());
    }
    catch (...)
    {
      pars::err(SL, lf::event,
                "Exeption Handler for 0x{:X} throws: Unknown Exception",
                spec_hash);
    }
  }

  void request_stop(const std::size_t j_id)
  {
    threads_m[j_id].get_stop_source().request_stop();
  }

  bool thread_running(const std::size_t j_id)
  {
    return threads_m.contains(j_id);
  }

  bool stop_possible(const std::size_t j_id)
  {
    return threads_m[j_id].get_stop_token().stop_possible();
  }

  std::mutex mtx_m; ///< protects futures_m, threads_m

  using futures_value_type =
    std::tuple<std::size_t, int, std::size_t, std::future<void>>;

  std::vector<futures_value_type>
    futures_m; ///< all job ids, socket ids, spec hashes
               ///< and futures from threads that are running

  std::unordered_map<std::size_t, std::jthread>
    threads_m; ///< the jthread for a given job id, if any

  hf_registry& hf_registry_m;

  std::unordered_map<int, std::vector<std::size_t>>
    pipe_jobs_m; ///< all queued ids of jobs for a given pipe id

  std::atomic<std::size_t> next_job_id_m{0};
};

} // namespace pars::ev
