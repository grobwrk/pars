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

#include "nngxx-msg.h"

/// CTOR: msg()

TEST_F(nngxx_msg, main_ctor)
{
  nngxx::msg m{};

  expect_invalid(m);
}

/// CTOR: msg(nng_msg*)

TEST_F(nngxx_msg, value_ctor_invalid)
{
  nng_msg* m_ptr = nullptr;

  nngxx::msg m{m_ptr};

  expect_invalid(m);
}

TEST_F(nngxx_msg, value_ctor_valid_empty)
{
  nng_msg* m_ptr = nullptr;
  nng_msg_alloc(&m_ptr, 0);

  nngxx::msg m{m_ptr};

  expect_valid_empty(m);
}

TEST_F(nngxx_msg, value_ctor_valid_nonempty)
{
  nng_msg* m_ptr = nullptr;
  nng_msg_alloc(&m_ptr, 1);

  nngxx::msg m{m_ptr};

  expect_valid_nonempty(m);
}

/// CTOR: msg(const msg&)

TEST_F(nngxx_msg, copy_ctor_invalid)
{
  nng_msg* m_ptr = nullptr;
  nngxx::msg m_from{m_ptr};

  auto m{m_from};

  expect_invalid(m);
}

TEST_F(nngxx_msg, copy_ctor_valid_empty)
{
  nng_msg* m_ptr = nullptr;
  nng_msg_alloc(&m_ptr, 0);
  nngxx::msg m_from{m_ptr};

  auto m{m_from};

  expect_valid_empty(m);
}

TEST_F(nngxx_msg, copy_ctor_valid_nonempty)
{
  nng_msg* m_ptr = nullptr;
  nng_msg_alloc(&m_ptr, 1);
  nngxx::msg m_from{m_ptr};

  auto m{m_from};

  expect_valid_nonempty(m);
}

/// CTOR: msg(msg&&)

TEST_F(nngxx_msg, move_ctor_invalid)
{
  nng_msg* m_ptr = nullptr;
  nngxx::msg m{nngxx::msg{m_ptr}};

  expect_invalid(m);
}

TEST_F(nngxx_msg, move_ctor_valid_empty)
{
  nng_msg* m_ptr = nullptr;
  nng_msg_alloc(&m_ptr, 0);
  nngxx::msg m{nngxx::msg{m_ptr}};

  expect_valid_empty(m);
}

TEST_F(nngxx_msg, move_ctor_valid_nonempty)
{
  nng_msg* m_ptr = nullptr;
  nng_msg_alloc(&m_ptr, 1);
  nngxx::msg m{nngxx::msg{m_ptr}};

  expect_valid_nonempty(m);
}

/// FUNC: alloc()

TEST_F(nngxx_msg, alloc_valid_empty)
{
  auto m = nngxx::msg::alloc(0).value();

  expect_valid_empty(m);
}

TEST_F(nngxx_msg, alloc_valid_nonempty)
{
  auto m = nngxx::msg::alloc(1).value();

  expect_valid_nonempty(m);
}

/// FUNC: make_msg()

TEST_F(nngxx_msg, make_msg_empty)
{
  auto m = nngxx::make_msg(0).value();

  expect_valid_empty(m);
}

TEST_F(nngxx_msg, make_msg_nonempty)
{
  auto m = nngxx::make_msg(1).value();

  expect_valid_nonempty(m);
}

/// ***** NOW WE CAN START USING make_msg IN TEST ***** ///

TEST_F(nngxx_msg, make_invalid_msg)
{
  auto m{make_invalid_msg()};

  expect_invalid(m);
}

TEST_F(nngxx_msg, make_valid_empty_msg)
{
  auto m{make_valid_empty_msg()};

  expect_valid_empty(m);
}

TEST_F(nngxx_msg, make_valid_nonempty_msg)
{
  auto m{make_valid_nonempty_msg()};

  expect_valid_nonempty(m);
}

/// FUNC: operator=(const msg&)

TEST_F(nngxx_msg, copy_assign_self)
{
  auto m1{make_valid_empty_msg()};

  auto m1_addr = static_cast<const nng_msg* const>(m1);

  m1 = m1;

  expect_valid_empty(m1);
  expect_resource_addr(m1, m1_addr);
}

TEST_F(nngxx_msg, copy_assign_invalid)
{
  auto m1{make_invalid_msg()};
  auto m2{make_invalid_msg()};

  m2 = m1;

  expect_invalid(m1);
  expect_invalid(m2);
}

TEST_F(nngxx_msg, copy_assign_valid)
{
  auto m1{make_valid_empty_msg()};
  auto m2{make_valid_empty_msg()};

  auto m1_addr = static_cast<const nng_msg* const>(m1);

  m2 = m1;

  expect_valid_empty(m1);
  expect_valid_empty(m2);

  expect_resource_addr(m1, m1_addr);
}

TEST_F(nngxx_msg, copy_assign_valid_invalid)
{
  auto m1{make_valid_empty_msg()};
  auto m2{make_invalid_msg()};

  auto m1_addr = static_cast<const nng_msg* const>(m1);

  m2 = m1;

  expect_valid_empty(m1);
  expect_valid_empty(m2);

  expect_resource_addr(m1, m1_addr);
}

TEST_F(nngxx_msg, copy_assign_invalid_valid)
{
  auto m1{make_invalid_msg()};
  auto m2{make_valid_empty_msg()};

  auto m1_addr = static_cast<const nng_msg* const>(m1);

  m2 = m1;

  expect_invalid(m1);
  expect_invalid(m2);

  expect_resource_addr(m1, m1_addr);
}

/// FUNC: operator=(msg&&)

TEST_F(nngxx_msg, move_assign_self)
{
  auto m1{make_valid_empty_msg()};

  auto m1_addr = static_cast<const nng_msg* const>(m1);

  m1 = std::move(m1);

  expect_valid_empty(m1);
  expect_resource_addr(m1, m1_addr);
}

TEST_F(nngxx_msg, move_assign_invalid)
{
  auto m1{make_invalid_msg()};
  auto m2{make_invalid_msg()};

  auto m1_addr = static_cast<nng_msg*>(m1);
  auto m2_addr = static_cast<nng_msg*>(m2);

  m2 = std::move(m1);

  expect_invalid(m1);
  expect_invalid(m2);

  expect_resource_addr(m1, m2_addr);
  expect_resource_addr(m2, m1_addr);
}

TEST_F(nngxx_msg, move_assign_valid)
{
  auto m1{make_valid_empty_msg()};
  auto m2{make_valid_empty_msg()};

  auto m1_addr = static_cast<nng_msg*>(m1);
  auto m2_addr = static_cast<nng_msg*>(m2);

  m2 = std::move(m1);

  expect_valid_empty(m1);
  expect_valid_empty(m2);

  expect_resource_addr(m1, m2_addr);
  expect_resource_addr(m2, m1_addr);
}

TEST_F(nngxx_msg, move_assign_valid_invalid)
{
  auto m1{make_valid_empty_msg()};
  auto m2{make_invalid_msg()};

  auto m1_addr = static_cast<nng_msg*>(m1);
  auto m2_addr = static_cast<nng_msg*>(m2);

  m2 = std::move(m1);

  expect_invalid(m1);
  expect_valid_empty(m2);

  expect_resource_addr(m1, m2_addr);
  expect_resource_addr(m2, m1_addr);
}

TEST_F(nngxx_msg, move_assign_invalid_valid)
{
  auto m1{make_invalid_msg()};
  auto m2{make_valid_empty_msg()};

  auto m1_addr = static_cast<nng_msg*>(m1);
  auto m2_addr = static_cast<nng_msg*>(m2);

  m2 = std::move(m1);

  expect_valid_empty(m1);
  expect_invalid(m2);

  expect_resource_addr(m1, m2_addr);
  expect_resource_addr(m2, m1_addr);
}

/// FUNC: release()

TEST_F(nngxx_msg, release)
{
  auto m = make_valid_empty_msg();

  auto m_ptr = m.release();

  EXPECT_FALSE(m);
  EXPECT_TRUE(m_ptr != nullptr);
}

/// FUNC: set_pipe(), get_pipe()

TEST_F(nngxx_msg, pipe)
{
  auto pv = nngxx::pipe_view{nng_pipe{255}};
  auto m = make_valid_empty_msg();

  EXPECT_EQ(m.get_pipe().id(), -1);

  m.set_pipe(pv);

  EXPECT_EQ(m.get_pipe().id(), 255);
}

/// FUNC: header()

TEST_F(nngxx_msg, header_empty)
{
  auto m = make_valid_empty_msg();

  EXPECT_EQ(m.header().size(), 0);
  EXPECT_EQ(static_cast<const msg*>(&m)->header().size(), 0);
}

/// FUNC: body()

TEST_F(nngxx_msg, body_empty)
{
  auto m = make_valid_empty_msg();

  EXPECT_EQ(m.body().size(), 0);
  EXPECT_EQ(static_cast<const msg*>(&m)->body().size(), 0);
}

/// FUNC: body().write(), body().read()

TEST_F(nngxx_msg, write_read)
{
  auto c = uint64_t{42};
  auto m = make_valid_empty_msg(sizeof(c) * 2);

  EXPECT_TRUE(m.body().write(c));
  EXPECT_TRUE(m.body().write(c + 1, sizeof(c)));

  EXPECT_TRUE(m);
  EXPECT_EQ(m.body().size(), 2 * sizeof(c));
  EXPECT_EQ(m.header().size(), 0);

  EXPECT_EQ(m.body().read<decltype(c)>().value(), c);
  EXPECT_EQ(m.body().read<decltype(c)>(sizeof(c)).value(), c + 1);
}

TEST_F(nngxx_msg, write_error)
{
  auto m = make_valid_empty_msg();

  auto ret = m.body().write(uint64_t{42});

  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), nngxx::cpp::err::invalid_memory);
}

TEST_F(nngxx_msg, read_error)
{
  auto m = make_valid_empty_msg();

  auto ret = m.body().read<uint64_t>();

  EXPECT_FALSE(ret);
  EXPECT_EQ(ret.error(), nngxx::cpp::err::invalid_memory);
}

/// FUNC: body().insert(), body().trim()

TYPED_TEST_SUITE(nngxx_msg_typed, uints);

TYPED_TEST(nngxx_msg_typed, insert_trim)
{
  using class_type = nngxx_msg_typed<TypeParam>;
  using uint_type = class_type::uint_type;

  auto v = uint_type{std::numeric_limits<uint_type>::max()};
  auto m = class_type::make_valid_empty_msg();

  EXPECT_TRUE(m.body().insert(v));
  auto ret = m.body().template trim<decltype(v)>();

  EXPECT_TRUE(ret);
  EXPECT_EQ(ret.value(), v);
}

TEST_F(nngxx_msg, insert_trim_sz)
{
  const uint8_t vs[2] = {10, 11};
  auto m = make_valid_empty_msg();

  EXPECT_TRUE(m.body().insert(static_cast<const void*>(vs), 2));
  EXPECT_EQ(m.body().size(), 2);

  {
    auto ret = m.body().read<uint16_t>();

    EXPECT_TRUE(ret);
    EXPECT_EQ(ret.value(), vs[1] * 256 + vs[0]);
  }

  {
    auto ret = m.body().trim(2);

    EXPECT_TRUE(ret);
    EXPECT_EQ(m.body().size(), 0);
  }
}

/// FUNC: body().append(), body().chop()

TYPED_TEST(nngxx_msg_typed, append_chop)
{
  using class_type = nngxx_msg_typed<TypeParam>;
  using uint_type = class_type::uint_type;

  auto v = uint_type{std::numeric_limits<uint_type>::max()};
  auto m = class_type::make_valid_empty_msg();

  EXPECT_TRUE(m.body().append(v));
  auto ret = m.body().template chop<decltype(v)>();

  EXPECT_TRUE(ret);
  EXPECT_EQ(ret.value(), v);
}

TEST_F(nngxx_msg, append_chop_sz)
{
  const uint8_t vs[2] = {10, 11};
  auto m = make_valid_empty_msg();

  EXPECT_TRUE(m.body().append(static_cast<const void*>(vs), 2));
  EXPECT_EQ(m.body().size(), 2);

  {
    auto ret = m.body().read<uint16_t>();

    EXPECT_TRUE(ret);
    EXPECT_EQ(ret.value(), vs[1] * 256 + vs[0]);
  }

  {
    auto ret = m.body().chop(2);

    EXPECT_TRUE(ret);
    EXPECT_EQ(m.body().size(), 0);
  }
}
