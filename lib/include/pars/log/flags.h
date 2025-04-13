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

#include <spdlog/spdlog.h>

namespace pars
{

enum class lf
{
  app = 1,
  comp = 2,
  event = 4,
  net = 8,
  user = 1073741824,
};

constexpr static int operator|(const lf& a, const lf& b)
{
  return static_cast<int>(a) | static_cast<int>(b);
}

constexpr static int operator|(const int& a, const lf& b)
{
  return a | static_cast<int>(b);
}

constexpr static int operator|(const lf& a, const int& b)
{
  return static_cast<int>(a) | b;
}

constexpr static int operator&(const lf& a, const int& b)
{
  return static_cast<int>(a) & b;
}

constexpr static int operator&(const int& a, const lf& b)
{
  return a & static_cast<int>(b);
}

constexpr static int operator&(const lf& a, const lf& b)
{
  return static_cast<int>(a) & static_cast<int>(b);
}

namespace f
{

struct lf
{
  lf(::pars::lf flags)
    : val{static_cast<int>(flags)}
  {
  }

  lf(int flags)
    : val{flags}
  {
  }

  int val;
};

} // namespace f

} // namespace pars

template<>
struct fmt::formatter<::pars::lf> : formatter<std::string>
{
  auto format(const ::pars::lf& flag, format_context& ctx) const
    -> decltype(ctx.out())
  {
    switch (flag)
    {
    case ::pars::lf::app:
      return fmt::format_to(ctx.out(), "app");
    case ::pars::lf::comp:
      return fmt::format_to(ctx.out(), "comp");
    case ::pars::lf::event:
      return fmt::format_to(ctx.out(), "event");
    case ::pars::lf::net:
      return fmt::format_to(ctx.out(), "net");
    case ::pars::lf::user:
      return fmt::format_to(ctx.out(), "user");
    default:
      return fmt::format_to(ctx.out(), "<lf-{}>", static_cast<int>(flag));
    }
  }
};

template<>
struct fmt::formatter<::pars::f::lf> : formatter<std::string>
{
  auto format(const ::pars::f::lf& flags, format_context& ctx) const
    -> decltype(ctx.out())
  {
    auto it = ctx.out();

    for (const auto& f : {::pars::lf::app, ::pars::lf::comp, ::pars::lf::event,
                          ::pars::lf::net, ::pars::lf::user})
    {
      if (!(flags.val & f))
        continue;

      it = fmt::format_to(it, " {}", f);
    }

    return it;
  }
};
