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

#include <fmt/format.h>
#include <nng/nng.h>

#include <expected>

namespace nngxx
{

enum class err : int;

[[nodiscard]] inline static const std::error_category& error_category() noexcept
{
  static struct : std::error_category
  {
    virtual const char* name() const noexcept override { return "nngxx"; }

    virtual std::string message(int e) const override
    {
      return nng_strerror(e);
    }
  } error_category;

  return error_category;
}

[[nodiscard]] inline std::error_code make_error_code(err e) noexcept
{
  return std::error_code(static_cast<int>(e), error_category());
}

} // namespace nngxx

template<>
struct std::is_error_code_enum<nngxx::err> : std::true_type
{
};

namespace nngxx
{

enum class err
{
  success = 0,
  intr = NNG_EINTR,
  nomem = NNG_ENOMEM,
  inval = NNG_EINVAL,
  busy = NNG_EBUSY,
  timedout = NNG_ETIMEDOUT,
  connrefused = NNG_ECONNREFUSED,
  closed = NNG_ECLOSED,
  again = NNG_EAGAIN,
  notsup = NNG_ENOTSUP,
  addrinuse = NNG_EADDRINUSE,
  state = NNG_ESTATE,
  noent = NNG_ENOENT,
  proto = NNG_EPROTO,
  unreachable = NNG_EUNREACHABLE,
  addrinval = NNG_EADDRINVAL,
  perm = NNG_EPERM,
  msgsize = NNG_EMSGSIZE,
  connaborted = NNG_ECONNABORTED,
  connreset = NNG_ECONNRESET,
  canceled = NNG_ECANCELED,
  nofiles = NNG_ENOFILES,
  nospc = NNG_ENOSPC,
  exist = NNG_EEXIST,
  readonly = NNG_EREADONLY,
  writeonly = NNG_EWRITEONLY,
  crypto = NNG_ECRYPTO,
  peerauth = NNG_EPEERAUTH,
  noarg = NNG_ENOARG,
  ambiguous = NNG_EAMBIGUOUS,
  badtype = NNG_EBADTYPE,
  connshut = NNG_ECONNSHUT,
  internal = NNG_EINTERNAL,
  syserr = NNG_ESYSERR,
  tranerr = NNG_ETRANERR
};

template<typename ret_t, typename arg_t, typename... args_t>
[[nodiscard]] inline clev::expected<std::remove_pointer_t<arg_t>>
make(ret_t (*f)(arg_t, args_t...), args_t... args) noexcept
{
  using return_type = decltype(make(f, args...));

  using value_type = return_type::value_type;

  return return_type{}.and_then([&](value_type v) -> return_type {
    if (auto e = f(&v, args...); e)
      return clev::make_unexpected<err>(e);

    return v;
  });
}

template<typename return_t>
[[nodiscard]] inline auto read(auto f) noexcept -> clev::expected<return_t>
{
  using return_type = clev::expected<return_t>;

  using value_type = return_type::value_type;

  return return_type{}.and_then([=](value_type v) -> return_type {
    if (auto e = f(&v); e)
      return clev::make_unexpected<err>(e);

    return v;
  });
}

template<typename ret_t, typename... args_t>
[[nodiscard]] inline clev::expected<void> invoke(ret_t (*f)(args_t...),
                                                 args_t... args) noexcept
{
  return clev::make_expected<err>(f(args...));
}

template<typename... args_t>
[[nodiscard]] inline clev::expected<void> invoke(void (*f)(args_t...),
                                                 args_t... args) noexcept
{
  f(args...);

  return {};
}

} // namespace nngxx

static_assert(std::convertible_to<nngxx::err, std::error_code>);

static_assert(std::constructible_from<std::error_code, nngxx::err>);
