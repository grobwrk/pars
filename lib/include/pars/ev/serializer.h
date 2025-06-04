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

#include <cereal/archives/binary.hpp>

#include <istream>
#include <ostream>
#include <spanstream>
#include <sstream>
#include <string_view>

namespace pars::ev
{

struct serialize
{
  template<event_c event_t>
  static nngxx::msg to_network(event_t& ev)
  {
    // 1. serialize the event to a stringstream
    auto ostring = std::ostringstream();
    auto ostream = std::ostream(ostring.rdbuf());
    auto ar = cereal::BinaryOutputArchive(ostream);
    klass<event_t>::serialize(ev, ar);

    // 2. compute the event_hash
    auto event_hash = uuid<klass<event_t>>::hash;

    // 3. create the nngxx::msg to hold the hash+event
    auto serialization = ostring.rdbuf()->view();
    auto m = nngxx::make_msg(sizeof(event_hash) + serialization.size())
               .value_or_abort();
    auto b = m.body();

    // 4. append the event hash
    memcpy(b.template data<char>(), &event_hash, sizeof(event_hash));

    // 5. append the serialized event
    memcpy(b.template data<char>() + sizeof(event_hash), serialization.data(),
           serialization.size());

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
    auto body = m.body();
    auto view =
      std::string_view(body.data<char>() + sizeof(uint64_t), body.size());
    auto istring = std::ispanstream(view);
    auto istream = std::istream(istring.rdbuf());
    auto ar = cereal::BinaryInputArchive(istream);

    auto ev = event_t{};
    klass<event_t>::serialize(ev, ar);

    pars::debug(SL, lf::event, "Serialized Message [{}] to Event [{}]", m, ev);

    return ev;
  }
};

} // namespace pars::ev
