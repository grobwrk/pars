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
#define CLEV_HAS_EXCEPTIONS false
#else
#define CLEV_HAS_EXCEPTIONS true
#endif

constexpr static bool clev_exception_disabled_v = !(CLEV_HAS_EXCEPTIONS);

inline static void
abort_now(const std::error_code err,
          const std::string_view msg = "") noexcept(clev_exception_disabled_v)
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

using unexpected = std::unexpected<std::error_code>;

template<typename value_t>
using expected = std::expected<value_t, std::error_code>;

template<typename value_t = void>
inline static auto
abort_now(const std::string_view msg = "") noexcept(clev_exception_disabled_v)
{
  return [=](const std::error_code err) -> expected<value_t> {
    abort_now(err, msg);

    return unexpected{err};
  };
}

template<typename value_t = void>
inline static auto exit_now() noexcept(clev_exception_disabled_v)
{
  return [](const std::error_code err) -> expected<value_t> {
    exit_now(err);

    return unexpected{err};
  };
}

template<typename enum_t>
inline clev::unexpected make_unexpected(const int err) noexcept
{
  return clev::unexpected{static_cast<enum_t>(err)};
}

template<typename enum_t>
inline clev::expected<void> make_expected(const int err) noexcept
{
  if (err != 0)
    return make_unexpected<enum_t>(err);

  return {};
}

} // namespace clev
