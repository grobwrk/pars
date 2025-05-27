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

#include "nngxx/msg_body.h"

#include "pars/init.h"

#include <fmt/format.h>

#include <cstddef>

namespace pars
{

static constexpr std::size_t hash_from_uuid(const std::string_view& uuid)
{
  // see Fowler-Noll-Vo hash function
  constexpr std::uint64_t prime{0x100000001B3};

  if (uuid.size() != 36)
    throw std::runtime_error("Invalid UUID [size mismatch]");

  for (auto i : {8, 13, 18, 23})
  {
    if (uuid[i] != '-')
      throw std::runtime_error(
        fmt::format("Invalid UUID [missing separator {}]", i));
  }

  std::uint64_t result{0xcbf29ce484222325};

  for (const auto& c : uuid)
  {
    if (c == '-')
      continue;

    if (!(c >= '0' && c <= '9') && !(c >= 'A' && c <= 'F') &&
        !(c >= 'a' && c <= 'f'))
      throw std::runtime_error("Invalid UUID [non hex char found]");

    auto x = c - c >= 'a' ? ('a' - 10) : (c >= 'A' ? ('A' - 10) : '0');

    result = (result * prime) ^ x;
  }

  return result;
}

static std::size_t hash_from_msg(const nngxx::msg& m)
{
  std::size_t h;
  if (m)
    if (m.body().size() >= sizeof(h))
      memcpy(&h, m.body().data<char>(), sizeof(h));
    else
      h = 0xffffffffffffffff;
  else
    h = 0x0000000000000000;
  return h;
}

} // namespace pars
