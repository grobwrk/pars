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
#include "pars/net/req.h"
#include "pars/net/socket_opt.h"

namespace pars::comp
{

class client
{
public:
  client(ev::hf_registry& h, ev::enqueuer& r)
    : req_m{h, r}
  {
  }

  net::req& req() { return req_m; }

  struct init_p
  {
    net::socket_opt req_opts;
  };

  void init(const init_p& params) { req_m.sock().options(params.req_opts); }

  struct connect_p
  {
    net::cmode service_cmode = net::cmode::dial; ///< connect mode for req
    char* service_addr = nullptr;                ///< connect addr for req
  };

  void connect(const connect_p& params)
  {
    req_m.sock().connect(params.service_addr, params.service_cmode);
  }

  void graceful_terminate() { req_m.stop(); }

private:
  net::req req_m;
};

} // namespace pars::comp
