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

#include "pars/net/context.h"
#include "pars/net/socket.h"
#include "pars/net/tool_view.h"

#include <tuple>
#include <unordered_map>

namespace pars::net
{

class context_registry
{
public:
  context_registry(ev::enqueuer& r, net::socket& s)
    : router_m{r}
    , sock_m{s}
  {
  }

  void stop_all()
  {
    for (auto& c : ctx_map_m)
      c.second.stop();
  }

  auto& emplace()
  {
    auto ctx = sock_m.make_ctx();

    auto id = ctx.id();

    auto res = ctx_map_m.emplace(
      std::piecewise_construct, std::forward_as_tuple(id),
      std::forward_as_tuple(router_m, std::move(ctx), sock_m));

    if (!res.second)
      throw new std::runtime_error("Unable to emplace a context");

    return res.first->second;
  }

  context& of(const net::tool_view& t)
  {
    if (t.type() != typeid(nng::ctx_view))
      throw new std::runtime_error("We need a context here");

    if (ctx_map_m.find(t.id()) == ctx_map_m.end())
      throw new std::runtime_error(fmt::format("Unknown context {}", t.id()));

    return ctx_map_m.at(t.id());
  }

  void start_recv(int num_ctxs)
  {
    for (int i = 0; i < num_ctxs; ++i)
      emplace().recv();
  }

private:
  ev::enqueuer& router_m;
  socket& sock_m;
  std::unordered_map<int, context> ctx_map_m;
};

} // namespace pars::net
