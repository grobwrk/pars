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

#include "nngxx/msg.h"

#include "pars/concept/event.h"
#include "pars/ev/klass.h"
#include "pars/log.h"

#include <flatbuffers/flatbuffers.h>

#include <span>
#include <variant>

namespace pars::ev
{

template<typename ev_t>
struct builder_for
{
private:
  using event_type = ev_t;

  using builder_type = event_type::builder_type;

  using table_type = event_type::table_type;

  builder_for()
    : fbb_m{}
    , obb_m{fbb_m}
  {
  }

  builder_for(size_t size)
    : fbb_m{size}
    , obb_m{fbb_m}
  {
  }

  flatbuffers::FlatBufferBuilder fbb_m;

  builder_type obb_m;

public:
  static builder_for default_size() { return builder_for{}; }

  static builder_for using_size(size_t size) { return builder_for{size}; }

  static event_type from(nngxx::msg m) { return event_type{std::move(m)}; }

  template<typename fn_t, typename... args_t>
  auto and_then(this auto&& self, fn_t&& mem_f, args_t&&... args)
    -> decltype(self)
  {
    auto fn = std::bind(std::forward<fn_t>(mem_f), std::placeholders::_1,
                        std::forward<args_t>(args)...);

    std::invoke(fn, self.obb_m);

    return self;
  }

  event_type build(this auto&& self)
  {
    self.fbb_m.Finish(self.obb_m.Finish());

    return event_type{self.fbb_m.Release()};
  }
};

template<typename builder_t, typename event_t>
struct fb
{
private:
  friend builder_for<event_t>;

  fb(flatbuffers::DetachedBuffer b)
    : buf_m{std::make_shared<flatbuffers::DetachedBuffer>(std::move(b))}
  {
  }

  fb(nngxx::msg m)
    : buf_m{std::make_shared<nngxx::msg>(std::move(m))}
  {
  }

public:
  using event_type = event_t;

  using builder_type = builder_t;

  using table_type = builder_type::Table;

  using offset_type = flatbuffers::Offset<table_type>;

  std::span<uint8_t> span() const
  {
    return std::visit(
      [](auto&& b) {
        using T = std::decay_t<decltype(b)>;

        if constexpr (std::is_same_v<
                        T, std::shared_ptr<flatbuffers::DetachedBuffer>>)
        {
          auto buf =
            *static_cast<const std::shared_ptr<flatbuffers::DetachedBuffer>*>(
              &b);

          return std::span<uint8_t>(buf->begin(),
                                    std::distance(buf->begin(), buf->end()));
        }
        else if constexpr (std::is_same_v<T, std::shared_ptr<nngxx::msg>>)
        {
          auto msg = *static_cast<const std::shared_ptr<nngxx::msg>*>(&b);

          return std::span<uint8_t>(msg->body().data<uint8_t>() + 8,
                                    msg->body().size() - 8);
        }
        else
          static_assert(!std::is_same_v<T, T>, "non-exhaustive visitor!");
      },
      buf_m);
  }

  table_type const* table() const
  {
    auto table = flatbuffers::GetRoot<table_type>(span().data());

    return table;
  }

  static builder_for<event_type> builder(std::size_t size = 1024)
  {
    return builder_for<event_type>::using_size(size);
  }

private:
  std::variant<std::shared_ptr<nngxx::msg>,
               std::shared_ptr<flatbuffers::DetachedBuffer>>
    buf_m;
};

struct serialize
{
  template<event_c event_t>
  static nngxx::msg to_network(event_t& ev)
  {
    // 1. get the serialized data
    std::span<uint8_t> span = ev.span();

    // 2. compute the event_hash
    auto event_hash = uuid<klass<event_t>>::hash;

    // 3. create the nngxx::msg to hold the hash+event
    auto m = nngxx::make_msg(sizeof(event_hash) + span.size()).value_or_abort();
    auto b = m.body();

    // 4. append the event hash
    std::memcpy(b.data<char>(), &event_hash, sizeof(event_hash));

    // 5. append the serialized event
    memcpy(b.data<uint8_t>() + sizeof(event_hash), span.data(), span.size());

    pars::debug(SL, lf::event, "Serialized Event [{}] to Message [{}]", ev, m);

    return m;
  }

  template<event_c event_t>
  static event_t to_event(const nngxx::msg& m)
  {
    // 1. compute received and requested event hash
    auto recv_event_hash = hash_from_msg(m);
    auto req_event_hash = uuid<klass<event_t>>::hash;

    // 2. check they correspond
    if (recv_event_hash - req_event_hash != 0)
      throw std::runtime_error("Requested event mismatch!");

    // 3. deserialize event
    auto ev = builder_for<event_t>::from(std::move(m));

    pars::debug(SL, lf::event, "Deserialized Message [{}] to Event [{}]", m,
                ev);

    return ev;
  }
};

} // namespace pars::ev
