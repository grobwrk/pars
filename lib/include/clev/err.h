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

#include <fmt/format.h>
#include <nng/nng.h>

#include <expected>
#include <functional>

namespace clev
{

struct exception : std::runtime_error
{
  explicit exception(std::error_code code) noexcept
    : std::runtime_error(code.message())
  {
  }
};

#if __cpp_exceptions < 199711
constexpr static bool clev_exception_disabled_v = true;
#else
constexpr static bool clev_exception_disabled_v = false;
#endif
inline static void
abort_now(const std::error_code err) noexcept(clev_exception_disabled_v)
{
  if constexpr (clev_exception_disabled_v)
    std::quick_exit(err.value());
  else
    throw exception{err};
}

inline static void exit_now(const std::error_code err) noexcept
{
  std::quick_exit(err.value());
}

struct unexpected : std::unexpected<std::error_code>
{
private:
  using parent = std::unexpected<std::error_code>;

public:
  using parent::parent;
};

template<typename value_t>
struct expected;

template<typename>
struct is_expected_specialization : std::false_type
{
};

template<template<typename> typename expected_t, typename value_t>
struct is_expected_specialization<expected_t<value_t>>
  : std::is_same<expected_t<value_t>, expected<value_t>>
{
};

template<typename expected_t>
inline constexpr bool is_expected_specialization_v =
  is_expected_specialization<expected_t>::value;

template<>
struct expected<void> : public std::expected<void, std::error_code>
{
private:
  using parent = std::expected<void, std::error_code>;

public:
  using parent::parent;

  constexpr void or_abort() noexcept(clev_exception_disabled_v)
  {
    if (!*this)
      abort_now(expected::error());
  }

  constexpr void or_exit() noexcept
  {
    if (!*this)
      exit_now(expected::error());
  }

  template<typename to_value_t>
  [[nodiscard]] auto transform_to(to_value_t&& v) && noexcept
  {
    if (*this)
      return expected<to_value_t>(std::move(v));
    else
      return expected<to_value_t>(unexpected(std::move(parent::error())));
  }
};

template<typename value_t>
struct expected : public std::expected<value_t, std::error_code>
{
private:
  using parent = std::expected<value_t, std::error_code>;

  using error_type = parent::error_type;

public:
  using parent::parent;

  [[nodiscard]] expected::value_type&
  value_or_abort() & noexcept(clev_exception_disabled_v)
  {
    if (!*this)
      abort_now(expected::error());

    return expected::value();
  }

  [[nodiscard]] expected::value_type&&
  value_or_abort() && noexcept(clev_exception_disabled_v)
  {
    if (!*this)
      abort_now(expected::error());

    return std::move(expected::value());
  }

  [[nodiscard]] constexpr const expected::value_type&
  value_or_abort() const& noexcept(clev_exception_disabled_v)
  {
    if (!*this)
      abort_now(expected::error());

    return expected::value();
  }

  [[nodiscard]] constexpr const expected::value_type&&
  value_or_abort() const&& noexcept(clev_exception_disabled_v)
  {
    if (!*this)
      abort_now(expected::error());

    return std::move(expected::value());
  }

  [[nodiscard]] expected<std::reference_wrapper<value_t>>
  and_assign_to(value_t& dest) && noexcept
  {
    if (*this)
    {
      dest = std::move(expected::value());

      return std::ref(dest);
    }

    return unexpected{std::move(expected::error())};
  }

  template<typename fn_t>
    requires std::is_constructible_v<error_type, error_type&>
  [[nodiscard]] auto and_then(fn_t&& f) && noexcept
  {
    using exp_t = std::remove_cvref_t<
      std::invoke_result_t<fn_t, decltype(std::move(expected::value()))>>;

    static_assert(is_expected_specialization_v<exp_t>,
                  "expected<value_t>::and_then(fn_t) must specialize");

    static_assert(std::is_same_v<typename exp_t::error_type, error_type>,
                  "expected<value_t>::and_then(fn_t) requires same error");

    if (*this)
      return std::invoke(std::forward<fn_t>(f), std::move(expected::value()));
    else
      return exp_t{std::unexpect, std::move(expected::error())};
  }

  template<typename to_value_t>
  [[nodiscard]] auto transform_to() && noexcept
  {
    if (*this)
      return expected<to_value_t>(std::move(parent::value()));
    else
      return expected<to_value_t>(unexpected(std::move(parent::error())));
  }

  template<typename to_value_t>
  [[nodiscard]] auto transform_to(to_value_t&& v) && noexcept
  {
    if (*this)
      return expected<value_t>(std::move(v));
    else
      return std::move(*this);
  }

  [[nodiscard]] auto discard_value() && noexcept
  {
    if (*this)
      return expected<void>{};
    else
      return expected<void>{std::unexpect, expected::error()};
  }
};

template<typename value_t>
expected(value_t&&) -> expected<value_t>;

template<typename enum_t>
inline clev::expected<void> make_expected(const int e) noexcept
{
  if (e != 0)
    return clev::unexpected(static_cast<enum_t>(e));

  return {};
}

} // namespace clev
