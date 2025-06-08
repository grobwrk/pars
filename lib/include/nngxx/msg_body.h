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

#include "nngxx/concept.h"
#include "nngxx/err.h"
#include "nngxx/msg.h"

namespace nngxx
{

struct msg_body
{
  msg_body(nng_msg* m) noexcept
    : m{m}
  {
  }

  [[nodiscard]] inline std::size_t size() const noexcept
  {
    return nng_msg_len(m);
  }

  template<typename char_t>
  [[nodiscard]] inline char_t* data() noexcept
  {
    return static_cast<char_t*>(nng_msg_body(m));
  }

  template<typename char_t>
  [[nodiscard]] inline const char_t* data() const noexcept
  {
    return static_cast<const char_t*>(nng_msg_body(m));
  }

  template<uint_c uint_t>
  [[nodiscard]] inline clev::expected<void> append(uint_t x) noexcept
  {
    int (*append)(nng_msg*, uint_t);

    if constexpr (std::is_same_v<uint_t, uint16_t>)
    {
      append = nng_msg_append_u16;
    }
    else if constexpr (std::is_same_v<uint_t, uint32_t>)
    {
      append = nng_msg_append_u32;
    }
    else if constexpr (std::is_same_v<uint_t, uint64_t>)
    {
      append = nng_msg_append_u64;
    }

    return nngxx::invoke(append, m, x);
  }

  template<uint_c uint_t>
  [[nodiscard]] inline clev::expected<void> insert(uint_t x) noexcept
  {
    int (*insert)(nng_msg*, uint_t);

    if constexpr (std::is_same_v<uint_t, uint16_t>)
    {
      insert = nng_msg_insert_u16;
    }
    else if constexpr (std::is_same_v<uint_t, uint32_t>)
    {
      insert = nng_msg_insert_u32;
    }
    else if constexpr (std::is_same_v<uint_t, uint64_t>)
    {
      insert = nng_msg_insert_u64;
    }

    return nngxx::invoke(insert, m, x);
  }

  template<uint_c uint_t>
  [[nodiscard]] inline clev::expected<uint_t> trim() noexcept
  {
    uint_t x;

    int (*trim)(nng_msg*, uint_t*);

    if constexpr (std::is_same_v<uint_t, uint16_t>)
    {
      trim = nng_msg_trim_u16;
    }
    else if constexpr (std::is_same_v<uint_t, uint32_t>)
    {
      trim = nng_msg_trim_u32;
    }
    else if constexpr (std::is_same_v<uint_t, uint64_t>)
    {
      trim = nng_msg_trim_u64;
    }

    return nngxx::invoke(trim, m, &x).transform([&]() { return x; });
  }

  [[nodiscard]] inline clev::expected<void> trim(std::size_t sz) noexcept
  {
    return nngxx::invoke(nng_msg_trim, m, sz);
  }

  template<uint_c uint_t>
  [[nodiscard]] inline clev::expected<uint_t> chop() noexcept
  {
    uint_t x;

    int (*chop)(nng_msg*, uint_t*);

    if constexpr (std::is_same_v<uint_t, uint16_t>)
    {
      chop = nng_msg_chop;
    }
    else if constexpr (std::is_same_v<uint_t, uint32_t>)
    {
      chop = nng_msg_chop_u32;
    }
    else if constexpr (std::is_same_v<uint_t, uint64_t>)
    {
      chop = nng_msg_chop_u64;
    }

    return nngxx::invoke(chop, m, &x).transform([&]() { return x; });
  }

  [[nodiscard]] inline clev::expected<void> chop(std::size_t sz) noexcept
  {
    return nngxx::invoke(nng_msg_chop, m, sz);
  }

  template<uint_c uint_t>
  [[nodiscard]] inline clev::expected<uint_t> read() const noexcept
  {
    uint_t v;

    std::memcpy(static_cast<void*>(&v), data<void>(), sizeof(decltype(v)));

    return v;
  }

  template<uint_c uint_t>
  [[nodiscard]] inline clev::expected<void> write(uint_t v) noexcept
  {
    std::memcpy(data<void>(), static_cast<void*>(&v), sizeof(decltype(v)));

    return {};
  }

private:
  nng_msg* m;
};

} // namespace nngxx

nngxx::msg_body nngxx::msg_view::body() noexcept
{
  return nngxx::msg_body{v};
}

const nngxx::msg_body nngxx::msg_view::body() const noexcept
{
  return nngxx::msg_body{v};
}

static_assert(std::copyable<nngxx::msg_body>);
