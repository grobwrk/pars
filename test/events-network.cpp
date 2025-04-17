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
#include <pars/pars.h>

#include <gtest/gtest.h>

#include <type_traits>

namespace pars::tests
{

using NetworkEvents =
  ::testing::Types<nng::msg, ev::network_error, ev::creating_pipe,
                   ev::pipe_created, ev::pipe_removed>;

template<typename event_t>
struct NetworkKinds : testing::Test
{
  using event_type = event_t;
};

TYPED_TEST_SUITE(NetworkKinds, NetworkEvents);

TYPED_TEST(NetworkKinds, EventsAreRecognized)
{
  using event_type = NetworkKinds<TypeParam>::event_type;

  // event_type is recognized as and event
  EXPECT_TRUE(ev::event_c<event_type>);

  // event_type do requires network data
  EXPECT_TRUE(ev::klass<event_type>::requires_network);

  // event_type is recognized as an network_event_c
  EXPECT_TRUE(ev::network_event_c<event_type>);
}

TYPED_TEST(NetworkKinds, EventsCanBeFired)
{
  using event_type = NetworkKinds<TypeParam>::event_type;

  // we can instantiate a fired<event_type>
  EXPECT_TRUE((std::is_constructible_v<ev::fired<event_type>, event_type,
                                       ev::metadata<ev::fired, event_type>>));

  if (std::default_initializable<event_type>)
  {
    // we can instantiate event_type{} without specifing the type event_type
    ev::fired{event_type{}, {1, net::tool_view{nng::ctx_view{}}, net::pipe{}}};

    // we can instantiate specifing the type event_type
    ev::fired<event_type>{{},
                          {1, net::tool_view{nng::ctx_view{}}, net::pipe{}}};
  }
}

TYPED_TEST(NetworkKinds, EventsCanBeSent)
{
  using event_type = NetworkKinds<TypeParam>::event_type;

  // we can instantiate a sent<event_type>
  EXPECT_TRUE((std::is_constructible_v<ev::sent<event_type>, event_type,
                                       ev::metadata<ev::sent, event_type>>));

  if (std::default_initializable<event_type>)
  {
    // we can instantiate event_type{} without specifing the type event_type
    ev::sent{event_type{}, {1, net::tool_view{nng::ctx_view{}}, net::pipe{}}};

    // we can instantiate specifing the type event_type
    ev::sent<event_type>{{}, {1, net::tool_view{nng::ctx_view{}}, net::pipe{}}};
  }
}

TYPED_TEST(NetworkKinds, EventsCanBeReceived)
{
  using event_type = NetworkKinds<TypeParam>::event_type;

  // we can instantiate a received<event_type>
  EXPECT_TRUE(
    (std::is_constructible_v<ev::received<event_type>, event_type,
                             ev::metadata<ev::received, event_type>>));

  if (std::default_initializable<event_type>)
  {
    // we can instantiate event_type{} without specifing the type event_type
    ev::received{event_type{},
                 {1, net::tool_view{nng::ctx_view{}}, net::pipe{}}};

    // we can instantiate specifing the type event_type
    ev::received<event_type>{{},
                             {1, net::tool_view{nng::ctx_view{}}, net::pipe{}}};
  }
}

} // namespace pars::tests
