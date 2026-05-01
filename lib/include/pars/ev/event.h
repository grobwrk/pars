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
#ifndef PARS_EV_EVENT_H
#define PARS_EV_EVENT_H

#include "pars/ev/klass.h"
#include "pars/fmt/stl.h" // IWYU pragma: keep
#include "pars/net/asio.h"
#include "pars/net/direction.h"
#include "pars/fmt.h"

#include <chrono>
#include <exception>
#include <string>
#include <string_view>
#include <system_error>

namespace pars::ev
{

struct creating_pipe
{
  auto format_to(pars::format_context& ctx) const -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "creating_pipe()");
  }
};

template<>
struct klass<creating_pipe> : base_klass<creating_pipe>
{
  static constexpr std::string_view uuid =
    "adf2e44f-b005-449b-b849-e2b46377c122";
};

struct pipe_created
{
  auto format_to(pars::format_context& ctx) const -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "pipe_created()");
  }
};

template<>
struct klass<pipe_created> : base_klass<pipe_created>
{
  static constexpr std::string_view uuid =
    "2410aea6-ce8e-46d7-b3c4-0ef8ab598ef9";
};

struct pipe_removed
{
  auto format_to(pars::format_context& ctx) const -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "pipe_removed()");
  }
};

template<>
struct klass<pipe_removed> : base_klass<pipe_removed>
{
  static constexpr std::string_view uuid =
    "5fe36da8-c46a-4ef4-872d-5f11d610eaeb";
};

struct network_error
{
  std::error_code error;
  net::direction dir;

  auto format_to(pars::format_context& ctx) const -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "network_error({}, {})", error,
                          (dir == net::direction::out ? "out" : "in"));
  }
};

template<>
struct klass<network_error> : base_klass<network_error>
{
  static constexpr std::string_view uuid =
    "53b44f06-c5b3-400f-8e7e-522cb39c1168";
};

struct exception
{
  std::exception_ptr eptr;

  std::string str() const
  {
    try
    {
      std::rethrow_exception(eptr);
    }
    catch (std::exception& e)
    {
      return pars::format("{}", e.what());
    }
    catch (...)
    {
      return "Unknown!";
    }
    return "<empty-exception>";
  }

  auto format_to(pars::format_context& ctx) const -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "{}", str());
  }
};

template<>
struct klass<exception> : base_klass<exception>
{
  static constexpr std::string_view uuid =
    "25d02d6b-38d1-414b-a5ff-60d93c7746c9";

  static constexpr bool requires_network = false;
};

struct init
{
  auto format_to(pars::format_context& ctx) const -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "init({})", creation_time);
  }

private:
  std::chrono::system_clock::time_point creation_time =
    std::chrono::system_clock::now();
};

template<>
struct klass<init> : base_klass<init>
{
  static constexpr std::string_view uuid =
    "bacffaf3-3f6a-4859-8495-c3b821ec69d5";

  static constexpr bool requires_network = false;
};

struct deinit
{
  auto format_to(pars::format_context& ctx) const -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "deinit({})", creation_time);
  }

private:
  std::chrono::system_clock::time_point creation_time =
    std::chrono::system_clock::now();
};

template<>
struct klass<deinit> : base_klass<deinit>
{
  static constexpr std::string_view uuid =
    "47c543bb-ba37-4442-a5bd-4b2dcfbf1e02";

  static constexpr bool requires_network = false;
};

template<typename ip_t>
struct resolved
{
  net::asio::basic_resolver_results<ip_t> results;

  auto format_to(pars::format_context& ctx) const -> decltype(ctx.out())
  {
    return pars::format_to(ctx.out(), "resolved({})", results);
  }
};

template<>
struct klass<resolved<net::asio::tcp>> : base_klass<resolved<net::asio::tcp>>
{
  static constexpr std::string_view uuid =
    "d7db42c4-0313-4ef4-8265-35fa6870834d";

  static constexpr bool requires_network = false;
};
} // namespace pars::ev

#include "pars/fmt/formattable.h" // IWYU pragma: export

#endif // PARS_EV_EVENT_H
