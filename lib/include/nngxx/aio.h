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

#include "nngxx/iface/aio.h"

namespace nngxx
{

using aio_view = clev::iface<nng_aio*>;

using aio = clev::own<nng_aio*>;

inline static void sleep(nng_duration ms, aio_view& aio) noexcept
{
  nng_sleep_aio(ms, aio);
}

[[nodiscard]] inline static clev::expected<aio> make_aio(void (*cb)(void*),
                                                         void* arg) noexcept
{
  return aio::alloc(cb, arg).transform_to<aio>();
}

[[nodiscard]] inline static clev::expected<aio>
make_aio(void (*cb)(void*), void* arg, msg m) noexcept
{
  return make_aio(cb, arg).and_then([&](aio op) {
    op.set_msg(std::move(m));

    return clev::expected{std::move(op)};
  });
}

} // namespace nngxx

static_assert(std::copyable<nngxx::aio_view>);

static_assert(nngxx::movable_only_c<nngxx::aio>);
