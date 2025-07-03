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

#include <gtest/gtest.h>

#include "clev/err.h"

template<typename value_t, typename error_t>
struct clev_invoke_f : ::testing::Test
{
  using value_type = value_t;
  using error_type = error_t;
  using expected_type = std::expected<value_type, error_type>;

  value_type some_value{"some-value"};
  value_type other_value{val_to_val_static_fn(some_value)};
  error_type some_error{42, std::generic_category()};
  error_type other_error{99, std::generic_category()};

  static constexpr auto with_value(value_type value)
  {
    return expected_type{std::move(value)};
  }

  static constexpr auto with_error(error_type error)
  {
    return expected_type{std::unexpected{std::move(error)}};
  }

  static value_type val_to_val_static_fn(value_type) { return "other-value"; }

  value_type val_to_val_mem_fn(value_type) { return other_value; }

  static constexpr auto val_to_val(value_type value)
  {
    return [value = std::move(value)](value_type) { //
      return std::move(value);
    };
  }

  static constexpr auto val_to_err(error_type error)
  {
    return [error = std::move(error)](value_type) { //
      return std::move(error);
    };
  }

  static constexpr auto val_to_void()
  {
    return [](value_type) { return; };
  }

  static constexpr auto err_to_err(error_type error)
  {
    return [error = std::move(error)](error_type) { //
      return std::move(error);
    };
  }

  static constexpr auto err_to_val(value_type value)
  {
    return [value = std::move(value)](error_type) { //
      return std::move(value);
    };
  }

  static constexpr auto err_to_void()
  {
    return [](error_type) { return; };
  }

  static constexpr auto void_to_val(value_type value)
  {
    return [value = std::move(value)] { //
      return std::move(value);
    };
  }

  static constexpr auto void_to_err(error_type error)
  {
    return [error = std::move(error)] { //
      return std::move(error);
    };
  }
};
