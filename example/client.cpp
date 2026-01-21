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

#include "common.h"
#include "event.h"

#include <pars/app/single.h>
#include <pars/app/state_machine.h>
#include <pars/comp/client2.h>
#include <pars/ev/event.h>
#include <pars/ev/kind_decl.h>
#include <pars/ev/make_hf.h>
#include <pars/init.h>
#include <pars/log.h>
#include <pars/net2/asio.h>
#include <pars/net2/connect_mode.h>
#include <pars/net2/resolver.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <winsock2.h>

namespace pars_example::apps
{

using namespace event;
using namespace resource;
using namespace pars;
using namespace pars::ev;

/// Runs the client component as an single application (req)
class client2 : public app::single2<comp::client2>
{
private:
  /// @name Types

  using parent_type = self_type;
  using self = client2;

  /// @name Network Parameters

  milli req_recv_timeout{-1};
  milli req_send_timeout{1000};

  /// @name Input Parameters

  net::resolver::params resolve_p;
  std::size_t work_id{0};
  bool fast_fib{false};
  uint64_t n{0};

  /// @name App State

  app::state_machine<client_state> state{client_state::creating};

  /// @name Constructors

  using parent_type::parent_type;

  /// @name Initialization

  void usage()
  {
    // TODO: introduce location to get in one string
    // {connect_mode}+{protocol}://{host}:{service}
    throw std::invalid_argument("Usage: ./client2 connect_mode protocol host "
                                "service work_id fast_or_slow N");
  }

  void startup(int argc, char** argv) override
  {
    enable_source_loc_logging();

    /// 1. create state transition
    auto ts = state.tx(client_state::creating, client_state::initializing);

    /// 2. get the input
    switch (argc)
    {
    case 7:

      resolve_p.connect_mode = net::cmode_from_string(argv[1]);

      // TODO: protocol can be tcp4 or tcp6

      resolve_p.host = std::string_view(argv[2]);

      resolve_p.service = std::string_view(argv[3]);

      work_id = std::stoull(argv[4]);

      if (std::string_view(argv[5]).compare("fast") == 0)
        fast_fib = true;
      else if (std::string_view(argv[5]).compare("slow") == 0)
        fast_fib = false;
      else
        usage();

      n = std::stoull(argv[6]);

      break;

    default:
      usage();
    }

    /// 3. insert handler functions
    hfs().on<fired, init>(&self::resolve, this);

    hfs().on<fired, exception>(&self::terminate, this);

    hfs().on<fired, resolved_results<net::ip::tcp>>(&self::connect, this);

    comp().req().on<fired, pipe_created>(&self::send_work, this);

    comp().req().on<sent, fib_requested>(&self::recv_answer, this);

    comp().req().on<received, fib_computed>(&self::terminate, this);

    comp().req().on<fired, network_error>(&self::terminate, this);

    comp().req().on<fired, pipe_removed>(&self::terminate, this);

    /// 4. commit state transition
    ts.commit();

    pars::info(SL, "Application Started!");
  }

  /// @name Handler Functions

  void resolve(hf_arg<fired, init> fired)
  {
    auto ts = state.tx(client_state::initializing, client_state::resolving);

    comp().init({.req_opts = {.recv_timeout = req_recv_timeout.count(),
                              .send_timeout = req_send_timeout.count()}});

    resolver().resolve(resolve_p);

    ts.commit();

    pars::info(SL, "Fired {}, Application Initialized!", fired.event());
  }

  void connect(hf_arg<fired, resolved_results<net::ip::tcp>> fired)
  {
    auto ts = state.tx(client_state::resolving, client_state::connecting);

    comp().req().connect(net::cmode::dial,
                         fired.event().results.begin()->endpoint());

    ts.commit();

    pars::info(SL, "Fired {}, Address Resolved!", fired.event());
  }

  void send_work(hf_arg<fired, pipe_created> fired)
  {
    auto ts = state.tx(client_state::connecting, client_state::sending_work);

    auto [ev, md] = fired.as_tuple();

    auto out_ev = fib_requested::make(work_id, n, fast_fib);

    pars::info(SL, "Fired {}, Sent {}!", ev, out_ev);

    // use the default context on the sock to send the event
    // comp().req().sock().send(std::move(out_ev), md.pipe());

    ts.commit();
  }

  void recv_answer(hf_arg<sent, fib_requested> sent)
  {
    auto ts =
      state.tx(client_state::sending_work, client_state::waiting_work_done);

    // recv on the default context of the sock
    // comp().req().sock().recv();

    ts.commit();

    pars::info(SL, "Sent {}, Receiving!", sent.event());
  }

  void terminate(hf_arg<received, fib_computed> recv)
  {
    auto ts =
      state.tx(client_state::waiting_work_done, client_state::terminating);

    auto& ev = recv.event();

    graceful_terminate();

    ts.commit();

    pars::info(SL, "Received {}, Application Terminated!", ev);

    std::cout << "WORK(" << ev.table()->work_id() << ") FIB(" << n
              << ") = " << ev.table()->fib_n() << "\n";
  }

  void terminate(hf_arg<fired, exception> fired)
  {
    auto ts = state.tx(client_state::terminated);

    auto& ev = fired.event();

    graceful_terminate();

    ts.commit();

    pars::info(SL, "Fired {} while \"{}\", Application Terminated!", ev,
               state.current());

    std::cout << std::format("ERROR: {}", ev) << std::endl;
  }

  void terminate(hf_arg<fired, network_error> fired)
  {
    auto ts = state.tx(client_state::terminated);

    auto& ev = fired.event();

    graceful_terminate();

    ts.commit();

    pars::info(SL, "Fired {} while \"{}\", Application Terminated!", ev,
               state.current());

    std::cout << std::format("ERROR: {}", ev.error) << std::endl;
  }

  void terminate(hf_arg<fired, pipe_removed> fired)
  {
    auto ts = state.tx(client_state::terminated);

    graceful_terminate();

    ts.commit();

    pars::info(SL, "Fired {} while \"{}\", Application Terminated!",
               fired.event(), state.current());

    std::cout << std::format("Client Disconnected!") << std::endl;
  }
};

} // namespace pars_example::apps

int main(int argc, char** argv)
{
  auto app = ::pars_example::apps::client2{};

  try
  {
    return app.exec(argc, argv);
  }
  catch (std::exception& e)
  {
    std::cout << std::format("Error: {}", e.what()) << "\n";

    return EXIT_FAILURE;
  }
}
