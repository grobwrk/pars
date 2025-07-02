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

#include "nngxx/err.h"
#include "nngxx/iface/value.h"
#include "nngxx/msg.h"

#include <nng/nng.h>

template<>
struct clev::iface<nng_aio*> : nngxx::value<nng_aio*>
{
  using value::value;

  static nng_aio* empty() noexcept { return nullptr; }

  [[nodiscard]] static expected<void> destroy(nng_aio** v) noexcept
  {
    return nngxx::invoke(nng_aio_free, *v).and_then(invoke([&] {
      *v = empty();
    }));
  }

  [[nodiscard]] static expected<nng_aio*> alloc(void (*cb)(void*),
                                                void* arg) noexcept
  {
    return nngxx::make(nng_aio_alloc, cb, arg);
  }

  void wait() const noexcept { nng_aio_wait(v); }

  // ReSharper disable once CppMemberFunctionMayBeConst
  void abort(nngxx::c::err err) noexcept
  {
    nng_aio_abort(v, static_cast<int>(err));
  }

  // ReSharper disable once CppMemberFunctionMayBeConst
  void cancel() noexcept { nng_aio_cancel(v); }

  // ReSharper disable once CppMemberFunctionMayBeConst
  void stop() noexcept { nng_aio_stop(v); }

  // ReSharper disable once CppMemberFunctionMayBeConst
  [[nodiscard]] nngxx::msg release_msg() noexcept
  {
    const auto m = nng_aio_get_msg(v);

    nng_aio_set_msg(v, nullptr);

    return m;
  }

  [[nodiscard]] expected<void> result() const noexcept
  {
    // clev::exec_on(&result, *this)
    return nngxx::invoke(nng_aio_result, v);
  }

  // ReSharper disable once CppMemberFunctionMayBeConst
  void set_msg(nngxx::msg m) noexcept { nng_aio_set_msg(v, m.release()); }
};
