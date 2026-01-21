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
#ifndef PARS_NET_LISTENER_H
#define PARS_NET_LISTENER_H

#include "pars/net/asio.h"
#include "pars/net/io.h"
#include "pars/net/pipe.h"

#include <memory>
#include <system_error>
#include <utility>

namespace pars::net
{

class listener
{
private:
  listener(net::io& io, const ip::tcp::endpoint& endpoint)
    : io_m{io.lower_context()}
    , acceptor_m{io_m, endpoint.protocol()}
  {
  }

public:
  using pointer = std::unique_ptr<listener>;

  static pointer make(net::io& io, const ip::tcp::endpoint& endpoint)
  {
    return pointer{new listener(io, endpoint)};
  }

  void listen(net::pipe_function pf)
  {
    auto pipe = net::pipe2::make(io_m);

    acceptor_m.async_accept(
      pipe->socket(), [&, pipe = std::move(pipe),
                       pf = std::move(pf)](std::error_code err) mutable {
        if (pf(std::move(pipe), err)) // pf handle the new pipe or error
          listen(std::move(pf));      // keep listening if pf returns true
      });
  }

private:
  ip::io_context& io_m;
  ip::tcp::acceptor acceptor_m;
};

} // namespace pars::net

#endif // PARS_NET_LISTENER_H
