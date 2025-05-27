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
#include "event.h"
#include "fib.h"

namespace pars_example::apps
{

using namespace event;
using namespace resource;

/// Runs the backend component as a single application (rep).
class server_backend : public app::single<comp::backend>
{
private:
  /// @name Types

  using parent_type = self_type;
  using self = server_backend;

  /// @name Network Parameters

  const milli cleanup_timeout{1000};
  const milli rep_recv_timeout{-1};
  const milli rep_send_timeout{1000};

  /// @name Input Parameters

  component_type::connect_p connect_p;
  int max_allowed{128}; ///< concurrent contextes
  int max_served{0};    ///< shutdown when reached (0 = inf)

  /// @name App State

  std::atomic<int> tot_served{0}; ///< total client served
  app::state_machine<server_state> state = {server_state::creating};
  app::resources<int, pipe_resource> resources;

  /// @name Constructors

  using parent_type::parent_type;

  /// @name Initialization

  void startup(int argc, char** argv) override
  {
    enable_source_loc_logging();

    /// 1. create state transition
    auto ts = state.tx(server_state::creating, server_state::initializing);

    /// 2. grab the input
    switch (argc)
    {
    case 5:
      max_allowed = std::stoi(argv[4]);

      [[fallthrough]];

    case 4:
      max_served = std::stoi(argv[3]);

      [[fallthrough]];

    case 3:
      connect_p.service_cmode = net::cmode_from_string(argv[1]);

      connect_p.service_addr = argv[2];

      break;

    default:
      throw std::invalid_argument("Usage: ./server_backend service_cmode "
                                  "service_addr [max_served [max_allowed]]");
    }

    /// 3. insert handler functions
    hfs().on<fired, init>(&self::initialize, this);

    hfs().on<fired, shutdown>(&self::terminate, this);

    comp().rep().on<received, fib_requested>(&self::fire_compute, this);

    comp().rep().on<fired, fib_requested>(&self::compute_answer, this);

    comp().rep().on<sent, fib_computed>(&self::recv_next, this);

    comp().rep().on<fired, creating_pipe>(&self::accept_pipe, this);

    comp().rep().on<fired, pipe_created>(&self::log_new_pipe, this);

    comp().rep().on<fired, pipe_removed>(&self::log_rem_pipe, this);

    comp().rep().on<fired, network_error>(&self::close_pipe, this);

    /// 4. commit state transition
    ts.commit();

    pars::info(SL, "Application Started!");
  }

  /// @name Handler Functions

  /// spawn all contexts and start a recv operation on each of them
  void initialize(hf_arg<fired, init> fired)
  {
    auto ts = state.tx(server_state::initializing, server_state::running);

    comp().init({.num_ctxs = max_allowed,
                 .rep_opts = {.recv_timeout = rep_recv_timeout.count(),
                              .send_timeout = rep_send_timeout.count()}});

    comp().connect(connect_p);

    ts.commit();

    pars::info(SL, "Fired {}, Application Initialized!", fired.event());
  }

  /// accept or reject client
  void accept_pipe(hf_arg<fired, creating_pipe> fired)
  {
    state.ensure(server_state::running);

    auto [ev, md] = fired.as_tuple();

    auto resources_count = resources.count();

    if (resources_count < max_allowed)
    {
      resources.emplace(md.pipe().id(), pipe_state::waiting_work);

      pars::info(SL, "{}: Fired {}, Pipe Accepted! [# resources: {} < {}]", md,
                 ev, resources_count, max_allowed);
    }
    else
    {
      md.pipe().close().or_abort();

      pars::info(SL, "{}: Fired {}, Pipe Rejected! [# resources: {} >= {}]", md,
                 ev, resources_count, max_allowed);
    }
  }

  /// log connected client
  void log_new_pipe(hf_arg<fired, pipe_created> fired)
  {
    state.ensure(server_state::running);

    auto [ev, md] = fired.as_tuple();

    pars::info(SL, "{}: Fired {} [# resources: {}]", md, ev, resources.count());
  }

  /// log client disconnected
  void log_rem_pipe(hf_arg<fired, pipe_removed> fired)
  {
    state.ensure(server_state::running);

    auto [ev, md] = fired.as_tuple();

    pars::info(SL, "{}: Fired {} [# resources: {}]", md, ev, resources.count());
  }

  /// queue_fire the computation
  void fire_compute(hf_arg<received, fib_requested> recv)
  {
    state.ensure(server_state::running);

    auto [ev, md] = recv.as_tuple();

    auto locked = resources.locked_resource(md.pipe().id());

    auto& pipe_resource = locked.resource();

    /// return if already working
    if (pipe_resource.state.current() == pipe_state::working)
    {
      pars::info(SL, "{}: Received {}, Already Working: Returning ...", md, ev);

      return;
    }

    auto ts =
      pipe_resource.state.tx(pipe_state::waiting_work, pipe_state::working);

    pipe_resource.save_tool(md.tool());

    router().queue_fire(ev, md);

    ts.commit();

    pars::info(SL, "{}: Received {}, Fire {}!", md, ev, ev);
  }

  /// compute fib_b then answer
  void compute_answer(hf_arg<fired, fib_requested> fired)
  {
    state.ensure(server_state::running);

    auto [ev, md] = fired.as_tuple();

    auto p = md.pipe();

    auto locked = resources.locked_resource(p.id());

    auto& pipe_resource = locked.resource();

    auto ts = pipe_resource.state.tx(pipe_state::working, pipe_state::done);

    /// compute fib_n

    std::size_t fib_n;

    try
    {
      fib_n = compute::fib(ev.n, ev.use_fast_fib, md);
    }
    catch (const compute::stop_requested&)
    {
      locked.guard().unlock();

      ts.rollback();

      resources.delete_resource(p.id());

      pars::info(SL, "{}: Fired {}, Stop Requested! [# resources: {}]", md, ev,
                 resources.count());

      return;
    }

    /// send the outcome event using the ctx where we received from

    auto out_ev = fib_computed{ev.work_id, fib_n};

    auto& ctx = comp().rep().ctxs().of(pipe_resource.load_tool());

    ctx.send(out_ev, p);

    ts.commit();

    pars::info(SL, "{}: Fired {}, Send {}!", md, ev, out_ev);
  }

  /// start a recv operation
  void recv_next(hf_arg<sent, fib_computed> sent)
  {
    state.ensure(server_state::running);

    auto [ev, md] = sent.as_tuple();

    auto& p = md.pipe();

    resources.delete_resource(p.id());

    if (++tot_served == max_served)
    {
      auto shutdown_ev = shutdown{};

      router().queue_fire(shutdown_ev);

      pars::info(SL, "{}: Sent {}, Fire {}! [{} succesfully served]", md, ev,
                 shutdown_ev, max_served);

      return;
    }

    /// we're ready to receive the next work

    auto& ctx = comp().rep().ctxs().of(md.tool());

    ctx.recv();

    pars::info(SL, "{}: Sent {}, Receiving! [# served: {}]", md, ev,
               tot_served.load());
  }

  /// close pipe
  void close_pipe(hf_arg<fired, network_error> fired)
  {
    state.ensure(server_state::running);

    auto [ev, md] = fired.as_tuple();

    auto& p = md.pipe();

    resources.delete_resource(p.id());

    p.close().or_abort();

    auto& ctx = comp().rep().ctxs().of(md.tool());

    ctx.recv();

    auto& [e, dir] = ev;

    pars::info(SL, "{}: Fired {} (during {}), Pipe Closed!", md, ev, dir);
  }

  /// graceful terminate
  void terminate(hf_arg<fired, shutdown> fired)
  {
    state.ensure(server_state::running);

    graceful_terminate();

    pars::info(SL, "Fired {}, Graceful Terminated!", fired.event());

    std::cout << "SHUTDOWN ..." << std::endl;
  }
};

} // namespace pars_example::apps

int main(int argc, char** argv)
{
  auto app = ::pars_example::apps::server_backend{};

  try
  {
    return app.exec(argc, argv);
  }
  catch (std::exception& e)
  {
    std::cout << fmt::format("Error: {}", e.what()) << "\n";

    return EXIT_FAILURE;
  }
}
