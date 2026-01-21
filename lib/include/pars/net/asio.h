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

#include <asio/detail/socket_types.hpp>
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/basic_resolver_results.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>

namespace pars::net::ip
{

/// ASIO ALIASES

using io_context = ::asio::io_context;

// template<typename ip_t>
// using basic_endpoint = ::asio::ip::basic_endpoint<ip_t>;

// template<typename ip_t>
// using basic_resolver = ::asio::ip::basic_resolver<ip_t>;

template<typename ip_t>
using basic_resolver_results = ::asio::ip::basic_resolver_results<ip_t>;

// template<typename ip_t>
// using basic_stream_socket = ::asio::basic_stream_socket<ip_t>;

// template<typename ip_t>
// using basic_datagram_socket = ::asio::basic_datagram_socket<ip_t>;

template<typename executor_t>
using executor_work_guard = ::asio::executor_work_guard<executor_t>;

using ::asio::make_work_guard;

using address = ::asio::ip::address;

using tcp = ::asio::ip::tcp;

using udp = ::asio::ip::udp;

using socket_type = ::asio::detail::socket_type;

} // namespace pars::net::ip

#include <format>
#include <string>

template<>
struct std::formatter<pars::net::ip::tcp::endpoint>
  : std::formatter<std::string>
{
  auto format(const pars::net::ip::tcp::endpoint& x, format_context& ctx) const
    -> decltype(ctx.out())
  {
    return std::format_to(ctx.out(), "tcp::endpoint");
  }
};

#endif // PARS_NET_ASIO_H
