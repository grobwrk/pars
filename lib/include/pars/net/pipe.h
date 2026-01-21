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

#include <cstdint>
#include <functional>
#include <memory>
#include <system_error>

namespace pars::net
{

struct pipe
{
  using pointer = std::unique_ptr<pipe>;

  static pointer make(ip::io_context& io);

  ip::tcp::socket& socket() const;

  uint64_t socket_id() const
  {
    return asio::detail::socket_type{socket().native_handle()};
  }
};

using pipe_function =
  std::move_only_function<bool(pars::net::pipe::pointer, std::error_code)>;

using pipe_function2 =
  std::move_only_function<void(pars::net::pipe::pointer, std::error_code)>;

} // namespace pars::net

#include "pars/fmt/formattable.h" // IWYU pragma: keep

#endif // PARS_NET_PIPE_H

/// PROTO SHIT

// class proto
// {
// public:
//   enum name
//   {
//     tcp4,
//     udp4,
//     tcp6,
//     udp6
//   };

//   proto(const ip::tcp& p)
//     : proto_m{p}
//   {
//   }

//   proto(const ip::udp& p)
//     : proto_m{p}
//   {
//   }

//   proto(const name n)
//     : proto_m{init_proto(n)}
//   {
//   }

//   auto name() const
//   {
//     return std::visit(
//       overloaded{[](const ip::tcp& x) {
//                    return x.family() == ASIO_OS_DEF_AF_INET ? tcp4 : tcp6;
//                  },
//                  [](const ip::udp& x) {
//                    return x.family() == ASIO_OS_DEF_AF_INET ? udp4 : udp6;
//                  }},
//       proto_m);
//   }

//   auto is_tcp() const { return name() == tcp4 || name() == tcp6; }

//   auto is_udp() const { return name() == udp4 || name() == udp6; }

//   auto format_to(std::format_context& ctx) const -> decltype(ctx.out())
//   {
//     switch (name())
//     {
//     case tcp4:
//       return std::format_to(ctx.out(), "tcp4");

//     case tcp6:
//       return std::format_to(ctx.out(), "tcp6");

//     case udp4:
//       return std::format_to(ctx.out(), "udp4");

//     case udp6:
//       return std::format_to(ctx.out(), "udp6");
//     }
//   }

// private:
//   static ip::protocol init_proto(const enum name n)
//   {
//     switch (n)
//     {
//     case tcp4:
//       return ip::tcp::v4();

//     case tcp6:
//       return ip::tcp::v6();

//     case udp4:
//       return ip::udp::v4();

//     case udp6:
//       return ip::udp::v6();
//     }
//   }

//   ip::protocol proto_m;
// };

/// BASIC SOCKET

// template<typename ip_t>
// struct basic_socket;

// template<>
// struct basic_socket<ip::tcp> : ::asio::ip::tcp::socket
// {
//   using endpoint = basic_endpoint<ip::tcp>;
// };

// template<>
// struct basic_socket<ip::udp> : ::asio::ip::udp::socket
// {
//   using endpoint = basic_endpoint<ip::udp>;
// };

/// INTERNET PROTOCOL

// template<>
// struct std::formatter<::pars::net::ip::internet_protocol>
//   : std::formatter<std::string>
// {
//   auto format(const ::pars::net::ip::internet_protocol& x,
//               format_context& ctx) const -> decltype(ctx.out())
//   {
//     switch (x.family() + x.protocol())
//     {
//     case ASIO_OS_DEF_AF_INET + ASIO_OS_DEF_IPPROTO_TCP:
//       return std::format_to(ctx.out(), "tcp4");

//     case ASIO_OS_DEF_AF_INET + ASIO_OS_DEF_IPPROTO_UDP:
//       return std::format_to(ctx.out(), "udp4");

//     case ASIO_OS_DEF_AF_INET6 + ASIO_OS_DEF_IPPROTO_TCP:
//       return std::format_to(ctx.out(), "tcp6");

//     case ASIO_OS_DEF_AF_INET6 + ASIO_OS_DEF_IPPROTO_UDP:
//       return std::format_to(ctx.out(), "udp6");
//     }

//     return std::format_to(ctx.out(), "unknown");
//   }
// };
