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

#include "pars/app/setup.h"
#include "pars/ev/dispatcher.h"
#include "pars/ev/enqueuer.h"
#include "pars/ev/hf_registry.h"
#include "pars/ev/runner.h"
#include "pars/log.h"

namespace pars::app
{

template<typename component_t>
class single : public with_default_setup
{
public:
  using component_type = component_t;
  using self_type = single<component_type>;

  single()
    : runner_m{hf_registry_m}
    , hf_registry_m{runner_m}
    , dispatcher_m{runner_m}
    , router_m{dispatcher_m, runner_m}
    , component_m{hf_registry_m, router_m}
  {
  }

  int exec(int argc, char** argv)
  {
    atexit(nng_fini);

    setup();

    startup(argc, argv);

    return run();
  }

protected:
  component_type& comp() { return component_m; }

  ev::enqueuer& router() { return router_m; }

  ev::hf_registry& hfs() { return hf_registry_m; }

  virtual void startup(int argc, char** argv) = 0;

  void graceful_terminate()
  {
    dispatcher_m.stop_running();

    component_m.graceful_terminate();

    dispatcher_m.terminate_now();
  }

  void terminate_now() { dispatcher_m.terminate_now(); }

  void stop_job_thread(const int j_id) { runner_m.stop_thread(j_id); }

  int run()
  {
    try
    {
      dispatcher_m.run();

      return EXIT_SUCCESS;
    }
    catch (std::exception& e)
    {
      pars::err(SL, lf::app, "Error while running single application: {}",
                e.what());

      return EXIT_FAILURE;
    }
  }

private:
  ev::runner runner_m;
  ev::hf_registry hf_registry_m;
  ev::dispatcher dispatcher_m;
  ev::enqueuer router_m;
  component_type component_m;
};

} // namespace pars::app
