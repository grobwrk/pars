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

#include "pars/init.h"

#include "pars/ev/event.h"

#include <fmt/format.h>

#include <chrono>
#include <iomanip>
#include <sstream>

template<>
struct fmt::formatter<std::chrono::system_clock::time_point>
  : fmt::formatter<std::string>
{
  auto format(const std::chrono::system_clock::time_point& t,
              fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    auto t2 = std::chrono::system_clock::to_time_t(t);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t2), "%F %T");

    return fmt::format_to(ctx.out(), "{}", oss.str());
  }
};

template<>
struct fmt::formatter<std::error_code> : fmt::formatter<std::string>
{
  auto format(const std::error_code& e, fmt::format_context& ctx) const
    -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "{}", e.message());
  }
};

template<pars::ev::event_c event_t>
struct fmt::formatter<std::shared_ptr<event_t>> : fmt::formatter<std::string>
{
  auto format(const std::shared_ptr<event_t>& x, fmt::format_context& ctx) const
    -> decltype(ctx.out())
  {
    if (x)
      return fmt::format_to(ctx.out(), "{}", *x);
    else
      return fmt::format_to(ctx.out(), "<empty-shared_ptr>");
  }
};
