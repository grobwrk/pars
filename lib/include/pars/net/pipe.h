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
#ifndef PARS_NET_PIPE_H
#define PARS_NET_PIPE_H

#include "pars/net/asio.h"
#include "pars/fmt.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <system_error>

namespace pars::net
{

struct pipe
{
  using pointer = std::unique_ptr<pipe>;

  template<typename ret_t>
  using function =
    std::move_only_function<ret_t(pars::net::pipe::pointer, std::error_code)>;

  static pointer make(asio::io_context& io, int point_id)
  {
    return pointer{new pipe{io, point_id}};
  }

  asio::tcp::socket& socket() { return socket_m; }

  int id() const { return id_m; }

  void id(int id) { id_m = id; }

  auto point_id() const { return point_id_m; }

  auto socket_id() const
  {
    return static_cast<uint64_t>(asio::socket_type{
      const_cast<asio::tcp::socket&>(socket_m).native_handle()});
  }

  auto format_to(pars::format_context& ctx) const -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "Pipe #{}-{:X}", id(), socket_id());
  }

private:
  pipe(asio::io_context& io, int point_id)
    : socket_m{io}
    , point_id_m{point_id}
  {
  }

  int id_m = -1;
  int point_id_m = -1;
  asio::tcp::socket socket_m;
};

} // namespace pars::net

#include "pars/fmt/formattable.h" // IWYU pragma: keep

#endif // PARS_NET_PIPE_H
