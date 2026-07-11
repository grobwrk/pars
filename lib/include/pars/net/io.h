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
#ifndef PARS_NET_IO_H
#define PARS_NET_IO_H

#include "pars/log.h"
#include "pars/net/asio.h"

#include <functional>
#include <stdexcept>
#include <thread>

namespace pars::net
{

struct io
{
private:
  using context_t = asio::io_context;

  using executor_work_guard_t =
    asio::executor_work_guard<context_t::executor_type>;

  context_t io_context_m;

  executor_work_guard_t work_m;

  std::thread thread_m;

  int next_point_id_m = 1;

  int next_pipe_id_m = 1;

  void run()
  {
    warn(SL, "Running!");

    io_context_m.run();

    warn(SL, "Exiting!");
  }

public:
  io()
    : work_m{asio::make_work_guard(io_context_m)}
  {
  }

  void start()
  {
    if (thread_m.joinable())
      throw new std::runtime_error("Another thread running!");

    thread_m = std::thread{std::bind(&io::run, this)};
  }

  bool joinable() { return thread_m.joinable(); }

  void join() { thread_m.join(); }

  void stop() { work_m.reset(); }

  asio::io_context& lower_context() { return io_context_m; }

  auto next_point_id() { return next_point_id_m++; }

  auto next_pipe_id() { return next_pipe_id_m++; }
};

} // namespace pars::net

#endif // PARS_NET_IO_H
