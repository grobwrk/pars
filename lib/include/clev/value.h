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

#include "clev/iface.h"

namespace clev
{

template<typename value_t>
struct value
{
  using value_type = value_t;

  value(value_type v) noexcept
    : v{v}
  {
  }

  value() noexcept
    : value{iface<value_type>::empty()}
  {
  }

  [[nodiscard]] operator value_type() noexcept { return v; }

  [[nodiscard]] operator const value_type() const noexcept { return v; }

  [[nodiscard]] value_type release() noexcept
  {
    auto ret = v;

    v = iface<value_type>::empty();

    return ret;
  }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return iface<value_type>::is_valid(v);
  }

protected:
  value_type v;
};

} // namespace clev

#include <concepts>

static_assert(std::copyable<clev::value<int>>);

static_assert(std::copyable<clev::value<int*>>);
