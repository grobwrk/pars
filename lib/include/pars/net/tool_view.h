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

#include "pars/init.h"

#include <fmt/format.h>
#include <nngpp/ctx_view.h>
#include <nngpp/socket_view.h>

#include <typeinfo>
#include <variant>

namespace pars::net
{

/**
 * @brief Represents an nng_socket or nng_ctx view
 */
class tool_view
{
public:
  /// Construct a tool_view from an nng_ctx view
  explicit tool_view(nng::ctx_view c)
    : tool_m{c}
  {
  }

  /// Construct a tool_view from an nng_socket view
  explicit tool_view(nng::socket_view s)
    : tool_m{s}
  {
  }

  /// Get the std::type_info of the underlying variant
  const std::type_info& type() const
  {
    return std::visit([](auto& t) { return std::ref(typeid(t)); }, tool_m);
  }

  /// Get a string that represents the type of the underlying variant
  const char* who() const { return tool_m.index() == 0 ? "Context" : "Socket"; }

  /// The id of the underlying variant
  int id() const
  {
    return std::visit([](const auto& t) { return t.id(); }, tool_m);
  }

  /// Formatter for debugging purpose
  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "{} #{}", who(), id());
  }

private:
  /// The underlying variant that represents either an nng_socket or nng_ctx
  const std::variant<nng::ctx_view, nng::socket_view> tool_m;
};

} // namespace pars::net
