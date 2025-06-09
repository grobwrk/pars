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
#include <gtest/gtest.h>

#include "nngxx/msg.h"
#include "nngxx/msg_body.h"
#include "nngxx/msg_header.h"

using namespace nngxx;

class nngxx_msg : public ::testing::Test
{
protected:
  nngxx::msg make_invalid_msg() { return nngxx::msg{}; }

  nngxx::msg make_valid_empty_msg(std::size_t sz = 0)
  {
    return nngxx::make_msg(sz).value();
  }

  nngxx::msg make_valid_nonempty_msg() { return make_valid_empty_msg(1); }

  static void expect_invalid(const nngxx::msg& m) { EXPECT_FALSE(m); }

  static void expect_valid_empty(const nngxx::msg& m)
  {
    EXPECT_TRUE(m);
    EXPECT_EQ(m.body().size(), 0);
    EXPECT_EQ(m.header().size(), 0);
  }

  void expect_valid_nonempty(const nngxx::msg& m,
                             std::optional<uint64_t> v = {}) const
  {
    using value_type = decltype(v)::value_type;

    EXPECT_TRUE(m);
    EXPECT_EQ(m.body().size(), 1);
    EXPECT_EQ(m.header().size(), 0);
  }

  void expect_resource_addr(const nngxx::msg& m, const nng_msg* const expected)
  {
    auto m_addr = static_cast<const nng_msg* const>(m);

    EXPECT_EQ(m_addr, expected);
  }
};

using uints = ::testing::Types<uint16_t, uint32_t, uint64_t>;

template<uint_c uint_t>
struct nngxx_msg_typed : public nngxx_msg
{
  using uint_type = uint_t;
};
