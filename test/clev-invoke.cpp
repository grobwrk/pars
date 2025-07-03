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

#include "clev-invoke.h"

using clev_invoke = clev_invoke_f<std::string, std::error_code>;

/// prerequisites
TEST_F(clev_invoke, prerequisites)
{
  {
    EXPECT_TRUE(with_value(some_value).has_value());
    EXPECT_EQ(with_value(some_value).value(), some_value);
  }

  {
    EXPECT_FALSE(with_error(some_error).has_value());
    EXPECT_EQ(with_error(some_error).error(), some_error);
  }

  {
    EXPECT_EQ(val_to_val(other_value)(some_value), other_value);
    EXPECT_EQ(val_to_err(other_error)(some_value), other_error);
    val_to_void()(some_value);

    EXPECT_EQ(err_to_err(other_error)(some_error), other_error);
    EXPECT_EQ(err_to_val(other_value)(some_error), other_value);
    err_to_void()(some_error);

    EXPECT_EQ(void_to_val(other_value)(), other_value);
    EXPECT_EQ(void_to_err(other_error)(), other_error);
  }
}

/// value -> value
TEST_F(clev_invoke, value_to_value)
{
  {
    const auto with_value_and_then_value_static = //
      with_value(some_value)                      //
        .and_then(clev::invoke<value_type>(val_to_val_static_fn));

    EXPECT_TRUE(with_value_and_then_value_static.has_value());
    EXPECT_EQ(with_value_and_then_value_static.value(), other_value);
  }

  {
    const auto with_value_and_then_value_mem_fn = //
      with_value(some_value)                      //
        .and_then(clev::invoke<value_type>(std::bind(
          &clev_invoke::val_to_val_mem_fn, this, std::placeholders::_1)));

    EXPECT_TRUE(with_value_and_then_value_mem_fn.has_value());
    EXPECT_EQ(with_value_and_then_value_mem_fn.value(), other_value);
  }

  {
    const auto with_value_and_then_value = //
      with_value(some_value)               //
        .and_then(clev::invoke<value_type>(val_to_val(other_value)));

    EXPECT_TRUE(with_value_and_then_value.has_value());
    EXPECT_EQ(with_value_and_then_value.value(), other_value);
  }
}

/// value -> void
TEST_F(clev_invoke, value_to_void)
{
  const auto with_value_and_then_void = //
    with_value(some_value)              //
      .and_then(clev::invoke<value_type>(val_to_void()));

  EXPECT_TRUE(with_value_and_then_void.has_value());
  EXPECT_EQ(with_value_and_then_void.value(), some_value);
}

/// value -> error
TEST_F(clev_invoke, value_to_error)
{
  const auto with_value_and_then_error = //
    with_value(some_value)               //
      .and_then(clev::invoke<value_type>(val_to_err(some_error)));

  EXPECT_FALSE(with_value_and_then_error.has_value());
  EXPECT_EQ(with_value_and_then_error.error(), some_error);
}

/// error -> error
TEST_F(clev_invoke, error_to_error)
{
  const auto with_error_or_else_error = //
    with_error(some_error)
      .or_else(clev::invoke<value_type>(err_to_err(other_error)));

  EXPECT_FALSE(with_error_or_else_error.has_value());
  EXPECT_EQ(with_error_or_else_error.error(), other_error);
}

/// error -> void
TEST_F(clev_invoke, error_to_void)
{
  const auto with_error_or_else_void = //
    with_error(some_error).or_else(clev::invoke<value_type>(err_to_void()));

  EXPECT_FALSE(with_error_or_else_void.has_value());
  EXPECT_EQ(with_error_or_else_void.error(), some_error);
}

/// error -> value
TEST_F(clev_invoke, error_to_value)
{
  const auto with_error_or_else_value = //
    with_error(some_error)
      .or_else(clev::invoke<value_type>(err_to_val(some_value)));

  EXPECT_TRUE(with_error_or_else_value.has_value());
  EXPECT_EQ(with_error_or_else_value.value(), some_value);
}
