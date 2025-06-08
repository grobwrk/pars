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
  nngxx::msg invalid_msg;

  nngxx::msg valid_empty_msg = nngxx::make_msg(0).value();

  uint64_t value = 42;

  nngxx::msg valid_nonempty_msg =
    nngxx::make_msg(sizeof(decltype(value)))
      .and_then([&](nngxx::msg m) {
        return m.body().write(value).transform([&]() { return std::move(m); });
      })
      .value();

  static void expect_invalid(const nngxx::msg& m) { EXPECT_FALSE(m); }

  static void expect_valid_empty(const nngxx::msg& m)
  {
    EXPECT_TRUE(m);
    EXPECT_EQ(m.body().size(), 0);
    EXPECT_EQ(m.header().size(), 0);
  }

  void expect_valid_nonempty(const nngxx::msg& m) const
  {
    EXPECT_TRUE(m);
    EXPECT_EQ(m.body().size(), 8);
    EXPECT_EQ(m.body().read<uint64_t>().value(), value);
    EXPECT_EQ(m.header().size(), 0);
  }
};
