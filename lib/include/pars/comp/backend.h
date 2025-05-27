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

#include "pars/ev/enqueuer.h"
#include "pars/net/rep.h"
#include "pars/net/socket_opt.h"

namespace pars::comp
{

class backend
{
public:
  backend(ev::hf_registry& h, ev::enqueuer& r)
    : rep_m{h, r}
  {
  }

  struct init_p
  {
    int num_ctxs = 1;
    net::socket_opt rep_opts;
  };

  void init(const init_p& params)
  {
    rep_m.sock().set_options(params.rep_opts);

    rep_m.ctxs().start_recv(params.num_ctxs);
  }

  struct connect_p
  {
    net::cmode service_cmode = net::cmode::listen; ///< connect mode for rep
    char* service_addr = nullptr;                  ///< connect addr for rep
  };

  void connect(const connect_p& params)
  {
    rep_m.sock().connect(params.service_addr, params.service_cmode);
  }

  net::rep& rep() { return rep_m; }

  void graceful_terminate() { rep_m.stop(); }

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "[rep:{}]", rep_m.sock());
  }

private:
  net::rep rep_m;
};

} // namespace pars::comp
