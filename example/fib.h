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

#include <pars/concept/event.h>

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <stop_token>

namespace pars_example::compute
{

struct stop_requested : std::runtime_error
{
  stop_requested()
    : std::runtime_error("Stop requested!")
  {
  }
};

static uint64_t slow_fib(const uint16_t n, std::optional<std::stop_token> tk)
{
  switch (n)
  {
  case 0:
    return 0;
  case 1:
    return 1;
  }
  if (tk && tk->stop_requested())
    throw stop_requested();
  return slow_fib(n - 2, tk) + slow_fib(n - 1, tk);
}

static uint64_t fast_fib(uint16_t n, std::optional<std::stop_token>)
{
  uint64_t fib[3] = {0, 1, 1};
  for (; n > 2; --n)
  {
    fib[0] = fib[1];
    fib[1] = fib[2];
    fib[2] = fib[1] + fib[0];
  }
  return fib[n];
}

template<typename metadata_t>
static uint64_t fib(const uint64_t n, const bool use_fast_fib,
                    const metadata_t& md)
{
  auto fib_fn = use_fast_fib ? fast_fib : slow_fib;

  if constexpr (pars::ev::async_event_c<typename metadata_t::event_type,
                                        metadata_t::template kind_type>)
    return fib_fn(n, md.stop_token());
  else
    return fib_fn(n, {});
}

} // namespace pars_example::compute
