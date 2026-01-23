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
#ifndef PARS_COMP_BACKEND_H
#define PARS_COMP_BACKEND_H

#include "pars/ev/enqueuer.h"
#include "pars/ev/hf_registry.h"
#include "pars/net/io.h"
#include "pars/net/point.h"

#include <format>

namespace pars::comp
{

class backend
{
public:
  backend(ev::hf_registry& h, ev::enqueuer& r, net::io& io)
    : rep_m{h, r, io}
  {
  }

  struct init_p
  {
  };

  void init(const init_p& params) {}

  void graceful_terminate() {}

  net::point& rep() { return rep_m; }

  auto format_to(std::format_context& ctx) const -> decltype(ctx.out())
  {
    return std::format_to(ctx.out(), "comp::backend");
  }

private:
  net::point rep_m;
};

} // namespace pars::comp

#include "pars/fmt/formattable.h" // IWYU pragma: export

#endif // PARS_COMP_BACKEND_H
