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

#include "pars/ev/klass.h"
#include "pars/fmt/stl.h"
#include "pars/net/dir.h"

#include <cereal/types/vector.hpp>
#include <fmt/base.h>

#include <chrono>
#include <vector>

namespace pars::ev
{

struct creating_pipe
{
  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "creating_pipe()");
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
  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "pipe_created()");
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
  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "pipe_removed()");
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
  net::dir dir;

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "network_error({}, {})", error,
                          (dir == net::dir::out ? "out" : "in"));
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
    catch (nng::exception& e)
    {
      return fmt::format("{} during {}", e.what(), e.who());
    }
    catch (std::exception& e)
    {
      return fmt::format("{}", e.what());
    }
    catch (...)
    {
      return "Unknown!";
    }
    return "<empty-exception>";
  }

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "{}", str());
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
  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "init({})", creation_time);
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

struct shutdown
{
  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "shutdown({})", creation_time);
  }

private:
  std::chrono::system_clock::time_point creation_time =
    std::chrono::system_clock::now();
};

template<>
struct klass<shutdown> : base_klass<shutdown>
{
  static constexpr std::string_view uuid =
    "47c543bb-ba37-4442-a5bd-4b2dcfbf1e02";

  static constexpr bool requires_network = false;
};

} // namespace pars::ev
