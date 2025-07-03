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

#include <expected>
#include <stdexcept>
#include <system_error>

namespace clev
{

struct exception final : std::runtime_error
{
  explicit exception(const std::error_code code) noexcept
    : std::runtime_error(code.message())
  {
  }
};

#if __cpp_exceptions < 199711
#define CLEV_HAS_EXCEPTIONS false
#else
#define CLEV_HAS_EXCEPTIONS true
#endif

constexpr static bool clev_exception_disabled_v = !(CLEV_HAS_EXCEPTIONS);

static void
abort_now(const std::error_code err,
          const std::string_view msg = "") noexcept(clev_exception_disabled_v)
{
  if constexpr (clev_exception_disabled_v)
    std::quick_exit(err.value());
  else
    throw exception{err};
}

static void exit_now(const std::error_code err) noexcept
{
  std::quick_exit(err.value());
}

using unexpected = std::unexpected<std::error_code>;

template<typename value_t>
using expected = std::expected<value_t, std::error_code>;

template<typename value_t = void>
static auto
abort_now(const std::string_view msg = "") noexcept(clev_exception_disabled_v)
{
  return [=](const std::error_code err) -> expected<value_t> {
    abort_now(err, msg);

    return unexpected{err};
  };
}

template<typename value_t = void>
static auto exit_now() noexcept(clev_exception_disabled_v)
{
  return [](const std::error_code err) -> expected<value_t> {
    exit_now(err);

    return unexpected{err};
  };
}

template<typename enum_t>
unexpected make_unexpected(const int err) noexcept
{
  return unexpected{static_cast<enum_t>(err)};
}

template<typename enum_t>
unexpected make_unexpected(const enum_t err) noexcept
{
  return unexpected{err};
}

template<typename enum_t>
unexpected make_unexpected(const std::error_code err) noexcept
{
  return unexpected{err};
}

template<typename enum_t>
expected<void> make_expected(const int err) noexcept
{
  if (err != 0)
    return make_unexpected<enum_t>(err);

  return {};
}

static constexpr expected<void> done = {};

auto invoke_void(auto f)
{
  return [f = std::move(f)]<typename... args_t>(args_t&&... v) {
    f(std::forward<args_t>(v)...);

    return done;
  };
}

template<typename value_t>
constexpr auto invoke(auto&& f)
{
  using value_type = value_t;
  using error_type = typename expected<value_type>::error_type;

  return [&]<typename expected_content_t>(
           expected_content_t&& x) -> clev::expected<value_type> {
    using function_type = decltype(f);
    using argument_type = expected_content_t&&;
    using return_type = std::invoke_result_t<function_type, argument_type>;

    constexpr bool arg_is_value =
      std::is_same_v<expected_content_t, value_type>;
    constexpr bool arg_is_error =
      std::is_same_v<expected_content_t, error_type>;

    constexpr bool ret_is_value = std::is_same_v<return_type, value_type>;
    constexpr bool ret_is_error = std::is_same_v<return_type, error_type>;
    constexpr bool ret_is_void = std::is_same_v<return_type, void>;

    static_assert((arg_is_value || arg_is_error) &&
                  (ret_is_value || ret_is_error || ret_is_void));

    if constexpr (ret_is_value)
    {
      const auto result = std::invoke(std::forward<function_type>(f),
                                      std::forward<argument_type>(x));

      return std::move(result);
    }
    else if constexpr (ret_is_error)
    {
      const auto result = std::invoke(std::forward<function_type>(f),
                                      std::forward<argument_type>(x));

      return std::unexpected{std::move(result)};
    }
    else if constexpr (ret_is_void)
    {
      expected_content_t x_copy = x;

      if constexpr (arg_is_value)
      {
        std::invoke(std::forward<function_type>(f),
                    std::forward<argument_type>(x));

        return x_copy;
      }

      if constexpr (arg_is_error)
      {
        std::invoke(std::forward<function_type>(f),
                    std::forward<argument_type>(x));

        return std::unexpected{x_copy};
      }
    }

    return {};
  };
}

} // namespace clev
