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
#ifndef PARS_FMT_NNG_H
#define PARS_FMT_NNG_H

#include "pars/net/hash.h"

#include "nngxx/msg.h"
#include "nngxx/msg_header.h"
#include "nngxx/pipe.h"

#include <cstddef>
#include <format>
#include <string>

template<>
struct std::formatter<nngxx::msg> : formatter<std::string>
{
  auto format(const nngxx::msg& m, format_context& ctx) const
    -> decltype(ctx.out())
  {
    if (m.body().size() < sizeof(std::size_t))
    {
      return std::format_to(
        ctx.out(), "size:{}={}+{}, hash:<error>, pipe:0x{:X}",
        m.header().size() + m.body().size(), m.header().size(), m.body().size(),
        m.get_pipe().id());
    }
    else
    {
      std::size_t h = pars::net::hash_from_msg(m);

      return std::format_to(
        ctx.out(), "size:{}={}+{}, hash:0x{:X}, pipe:0x{:X}",
        m.header().size() + m.body().size(), m.header().size(), m.body().size(),
        h, m.get_pipe().id());
    }
  }
};

template<>
struct std::formatter<nngxx::pipe_view> : formatter<std::string>
{
  auto format(const nngxx::pipe_view& p, std::format_context& ctx) const
    -> decltype(ctx.out())
  {
    if (p)
      if (p.id() == -1)
        return std::format_to(ctx.out(), "<ERROR>");
      else
        return std::format_to(ctx.out(), "0x{:08X}", p.id());
    else
      return std::format_to(ctx.out(), "<empty-pipe>");
  }
};

#endif // PARS_FMT_NNG_H
