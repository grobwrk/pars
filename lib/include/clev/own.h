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

#include "clev/err.h"
#include "clev/iface.h"

namespace clev
{

template<typename wrap_t>
struct own : iface<wrap_t>
{
  using parent = iface<wrap_t>;

  using parent::parent;

  own(const own& rhs) noexcept
    requires requires(wrap_t* d, const wrap_t s) {
      { own::copy(d, s) } -> std::convertible_to<clev::expected<void>>;
    }
  {
    if (rhs)
      std::ignore = own::copy(&(own::v), rhs.v).or_else(clev::exit_now());
  }

  own& operator=(const own& rhs) noexcept
    requires requires(wrap_t* d, const wrap_t s) {
      { own::copy(d, s) } -> std::convertible_to<clev::expected<void>>;
    }
  {
    if (this == &rhs)
      return *this;

    if (*this)
      own::destroy(&(own::v)).or_else(clev::exit_now());

    if (rhs)
      own::copy(&(own::v), rhs.v).or_else(clev::exit_now());

    return *this;
  }

  own(const own& rhs)
    requires(!requires(wrap_t* d, const wrap_t s) {
              { own::copy(d, s) } -> std ::convertible_to<clev::expected<void>>;
            })
  = delete;

  own& operator=(const own&)
    requires(!requires(wrap_t* d, const wrap_t s) {
              { own::copy(d, s) } -> std ::convertible_to<clev::expected<void>>;
            })
  = delete;

  own(own&& rhs) noexcept
  {
    own::v = rhs.v;

    rhs.v = own::empty();
  }

  own& operator=(own&& rhs) noexcept
  {
    if (this == &rhs)
      return *this;

    std::swap(own::v, rhs.v);

    return *this;
  }

  ~own()
  {
    if (*this)
      std::ignore = own::destroy(&(own::v)).or_else(clev::exit_now());
  }
};

} // namespace clev

static constexpr bool clev_own_is_really_needed_v = true;
