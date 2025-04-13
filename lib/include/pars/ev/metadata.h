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

#include "pars/concept/event.h"
#include "pars/concept/kind.h"
#include "pars/net/pipe.h"
#include "pars/net/tool_view.h"

#include <stop_token>

namespace pars::ev
{

template<template<typename> typename kind_of, event_c event_t>
  requires kind_c<kind_of>
struct common_metadata
{
  template<typename event2_t>
  using kind_type = kind_of<event2_t>;

  using event_type = event_t;

  int job_id() const { return job_id_m; }

  void set_job_id(int j_id) { job_id_m = j_id; }

private:
  int job_id_m;
};

struct base_internal_metadata
{
  base_internal_metadata() {}

  int socket_id() const { return 0; }

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(), "<internal-metadata>");
  }
};

struct base_network_metadata
{
  base_network_metadata(int s_id, net::tool_view t, net::pipe p)
    : id_m{s_id}
    , tool_m{t}
    , pipe_m{p}
  {
  }

  const net::tool_view& tool() const { return tool_m; }

  const net::pipe& pipe() const { return pipe_m; }

  int socket_id() const { return id_m; }

  auto format_to(fmt::format_context& ctx) const -> decltype(ctx.out())
  {
    // FIXME: duplicated
    return fmt::format_to(ctx.out(), "Pipe #{:X} {} {}", pipe().id(),
                          tool().who(), tool().id());
  }

private:
  int id_m;
  net::tool_view tool_m;
  net::pipe pipe_m;
};

struct base_sync_metadata
{
};

struct base_async_metadata
{
  std::stop_token stop_token() const { return stop_token_m; }

  void set_stop_token(std::stop_token tk) { stop_token_m = tk; }

private:
  std::stop_token stop_token_m;
};

template<template<typename> typename kind_of, event_c event_t>
  requires kind_c<kind_of>
struct metadata;

template<template<typename> typename kind_of,
         sync_internal_event_c<kind_of> event_t>
  requires kind_c<kind_of>
struct metadata<kind_of, event_t> : base_internal_metadata,
                                    base_sync_metadata,
                                    common_metadata<kind_of, event_t>
{
  using base_internal_metadata::base_internal_metadata;
};

template<template<typename> typename kind_of,
         sync_network_event_c<kind_of> event_t>
  requires kind_c<kind_of>
struct metadata<kind_of, event_t>
  : base_network_metadata, base_sync_metadata, common_metadata<kind_of, event_t>
{
  using base_network_metadata::base_network_metadata;
};

template<template<typename> typename kind_of,
         async_internal_event_c<kind_of> event_t>
  requires kind_c<kind_of>
struct metadata<kind_of, event_t> : base_internal_metadata,
                                    base_async_metadata,
                                    common_metadata<kind_of, event_t>
{
  using base_internal_metadata::base_internal_metadata;
};

template<template<typename> typename kind_of,
         async_network_event_c<kind_of> event_t>
  requires kind_c<kind_of>
struct metadata<kind_of, event_t> : base_network_metadata,
                                    base_async_metadata,
                                    common_metadata<kind_of, event_t>
{
  using base_network_metadata::base_network_metadata;
};

} // namespace pars::ev

#define PARS_MD(ke)                                                            \
  decltype([]<template<typename> typename kind_of, event_c event_t>(           \
             kind_of<event_t> ke) -> pars::ev::METADATA<kind_of, event_t> {    \
  }(ke))
