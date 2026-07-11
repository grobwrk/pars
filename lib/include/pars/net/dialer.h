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
#ifndef PARS_NET_DIALER_H
#define PARS_NET_DIALER_H

#include "pars/net/asio.h"
#include "pars/net/io.h"
#include "pars/net/pipe.h"

#include <functional>
#include <memory>
#include <system_error>
#include <utility>

namespace pars::net
{

class dialer
{
private:
  dialer(io& io, int point_id)
    : io_m{io.lower_context()}
    , point_id_m{point_id}
  {
  }

public:
  using pointer = std::unique_ptr<dialer>;

  static pointer make(io& io, int point_id)
  {
    return pointer{new dialer{io, point_id}};
  }

  void dial(const asio::tcp::endpoint& e, net::pipe::function<void> pf)
  {
    auto pipe = pipe::make(io_m, point_id_m);

    pipe->socket().async_connect(
      e, [pipe = std::move(pipe), pf = std::move(pf)](
           std::error_code err) mutable { pf(std::move(pipe), err); });
  }

private:
  net::asio::io_context& io_m;
  int point_id_m;
};

} // namespace pars::net

#endif // PARS_NET_DIALER_H
