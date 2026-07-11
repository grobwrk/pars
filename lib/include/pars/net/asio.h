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
#ifndef PARS_NET_ASIO_H
#define PARS_NET_ASIO_H

#include "pars/init.h" // IWYU pragma: keep

#include <asio/buffer.hpp>
#include <asio/detail/socket_types.hpp>
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/basic_resolver_entry.hpp>
#include <asio/ip/basic_resolver_results.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>

namespace pars::net::asio
{

/// ASIO ALIASES

using ::asio::io_context;

using ::asio::const_buffer;

using ::asio::executor_work_guard;

using ::asio::make_work_guard;

/// ASIO IP ALIASES

using ::asio::ip::address;

using ::asio::ip::tcp;

using ::asio::ip::udp;

using ::asio::ip::basic_resolver_entry;

using ::asio::ip::basic_resolver_results;

using ::asio::detail::socket_type;

} // namespace pars::net::asio

#include "pars/fmt.h"

#include <string>

template<>
struct pars::formatter<pars::net::asio::tcp> : formatter<std::string>
{
  auto format(const pars::net::asio::tcp& x, format_context& ctx) const
    -> decltype(ctx.out())
  {
    switch (x.family())
    {
    case ASIO_OS_DEF_AF_INET:
      return format_to(ctx.out(), "tcp4");

    case ASIO_OS_DEF_AF_INET6:
      return format_to(ctx.out(), "tcp6");

    default:
      return format_to(ctx.out(), "unknown");
    }
  }
};

template<>
struct pars::formatter<pars::net::asio::address> : formatter<std::string>
{
  auto format(const pars::net::asio::address& x, format_context& ctx) const
    -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "{}", x.to_string());
  }
};

template<>
struct pars::formatter<pars::net::asio::tcp::endpoint>
  : formatter<std::string>
{
  auto format(const pars::net::asio::tcp::endpoint& x,
              format_context& ctx) const -> decltype(ctx.out())
  {
    return format_to(ctx.out(), "{}://{}:{}", x.protocol(), x.address(),
                          x.port());
  }
};

template<typename ip_t>
struct pars::formatter<pars::net::asio::basic_resolver_entry<ip_t>>
  : formatter<std::string>
{
  auto format(const pars::net::asio::basic_resolver_entry<ip_t>& x,
              format_context& ctx) const -> decltype(ctx.out())
  {
    return format_to(ctx.out(), "{}", x.endpoint());
  }
};

template<typename ip_t>
struct pars::formatter<pars::net::asio::basic_resolver_results<ip_t>>
  : formatter<std::string>
{
  auto format(const pars::net::asio::basic_resolver_results<ip_t>& xs,
              format_context& ctx) const -> decltype(ctx.out())
  {
    format_to(ctx.out(), "{}, ", xs.size());

    for (auto it = xs.begin(); it != xs.end(); ++it)
      format_to(ctx.out(), "{}{}", *it,
                     std::next(it) == xs.end() ? "" : ", ");

    return ctx.out();
  }
};

#endif // PARS_NET_ASIO_H
