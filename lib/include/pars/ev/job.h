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

#include "pars/ev/kind.h"
#include "pars/ev/metadata.h"
#include "pars/ev/serializer.h"
#include "pars/ev/spec.h"

#include <nngpp/msg.h>
#include <nngpp/view.h>

#include <any>
#include <type_traits>

namespace pars::ev
{

class job
{
public:
  job(std::size_t j_id, int s_id, std::size_t h, std::any ke)
    : id_m{j_id}
    , socket_id_m{s_id}
    , spec_hash_m{h}
    , event_kind_m{std::move(ke)}
  {
  }

  job(const job&) = delete;

  job(job&&) = default;

  job& operator=(const job&) = delete;

  job& operator=(job&& j) = default;

  template<template<typename> typename kind_of, event_c event_t>
    requires(!is_same_kind_v<kind_of, received> ||
             std::is_same_v<event_t, nng::msg>)
  kind_of<event_t> event()
  {
    return std::any_cast<kind_of<event_t>>(std::move(event_kind_m));
  }

  template<template<typename> typename kind_of, event_c event_t>
    requires(is_same_kind_v<kind_of, received> &&
             !std::is_same_v<event_t, nng::msg>)
  received<event_t> event()
  {
    // get the received<nng::msg>
    auto r = event<received, nng::msg>();

    auto& md = r.md();

    // return a received<event_t>
    return received{
      serialize::to_event<event_t>(r.event()),
      metadata<received, event_t>{md.socket_id(), md.tool(), md.pipe()}};
  }

  std::size_t id() const { return id_m; }

  int socket_id() const { return socket_id_m; }

  std::size_t spec_hash() const { return spec_hash_m; }

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "spec:0x{:X}", spec_hash());
  }

  void set_id(std::size_t id) { id_m = id; }

private:
  std::size_t id_m;
  int socket_id_m;
  std::size_t spec_hash_m;
  std::any event_kind_m;
};

template<template<typename> typename kind_of, event_c event_t>
  requires kind_c<kind_of>
static std::size_t compute_spec_hash(const kind_of<event_t>& ke)
{
  if constexpr (std::is_same_v<kind_of<event_t>, received<nng::msg>>)
    return ke.msg_hash();
  else
    return spec<kind_of<event_t>>::hash;
}

template<template<typename> typename kind_of, event_c event_t>
  requires kind_c<kind_of>
static job make_job(std::size_t j_id, kind_of<event_t> ke)
{
  auto h = compute_spec_hash(ke);

  return job(j_id, ke.md().socket_id(), h,
             std::make_any<kind_of<event_t>>(std::move(ke)));
}

} // namespace pars::ev
