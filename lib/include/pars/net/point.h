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
#ifndef PARS_NET_POINT_H
#define PARS_NET_POINT_H

#include "pars/concept/event.h"
#include "pars/concept/kind.h"
#include "pars/ev/enqueuer.h"
#include "pars/ev/event.h"
#include "pars/ev/hf_registry.h"
#include "pars/ev/hf_registry__insert.h"
#include "pars/ev/make_hf.h"
#include "pars/net/asio.h"
#include "pars/net/connect_mode.h"
#include "pars/net/dialer.h"
#include "pars/net/direction.h"
#include "pars/net/io.h"
#include "pars/net/listener.h"
#include "pars/net/pipe.h"

#include <list>
#include <memory>
#include <system_error>
#include <utility>

namespace pars::net
{

/**
 * @brief A network point
 *
 * A dialer is able to estabilish a pipe (a connection) connecting to another
 * point. Do we have to attach the dialer or can we just use them to dial? I
 * think this is going to be important only if we generalize over ASIO to use
 * also transport different than
 *
 * A listener is able to estabilish a pipe each time another point connects to
 * it.
 *
 * A dialer will estabilish a pipe by using a transport socket (tcp or quic
 * will be supported first).
 *
 * A listener will estabilish a pipe by using an acceptor and a socket to
 * estabilish new pipe as soon as someone connect.
 *
 * Sending a message through a point means sending to all estabilished pipes
 * regardless to wether the pipe was estabilished by a dialer or a listener.
 *
 * One will be able to narrow this list of pipes.
 *
 * Because a send (or a receive) on a point (contact) it's just proxied to
 * every pipe associated to it, every operation could fail and must be
 * accounted separately.
 *
 * If the send has to be considered failed if any of the acqual sent as failed
 * or nor it's responsibility of the algorithm associated to the send.
 *
 * You can send through a point using these algorithms:
 *
 * 1. req (send the msg to all pipes, wait for one to answer, discard all
 *    the other potential answer, if all fail then consider the req failed and
 *    notify the sender)
 *
 * 2. push (send the msg to one of the pipe in round robin)
 *
 * 3. pub (send the msg to all pipes that are subscribed to a given topic)
 *
 * 4. sub (send the subscribe msg to all the pipes)
 *
 * When a pipe is established we start an async_read and put
 *
 * While a point recv just wait for a full msg to be received on any of the
 * existing pipes and notify the receiver as soon as the message fully arrive
 * on a pipe.
 */
class point
{
public:
  point(ev::hf_registry& h, ev::enqueuer& e, net::io& io)
    : io_m{io}
    , id_m{io.next_point_id()}
    , hf_registry_m{h}
    , enqueuer_m{e}
  {
  }

  // connect to a given endpoint (connect mode + endpoint address)
  void connect(const net::cmode cmode, const net::asio::tcp::endpoint endpoint)
  {
    if (cmode == cmode::dial)
      dial(endpoint);
    else if (cmode == cmode::listen)
      listen(endpoint);
  }

  void dial(const auto& to)
  {
    auto dialer = net::dialer::make(io_m, id_m);

    dialer->dial(to, [&, _ = std::move(dialer)](net::pipe::pointer pipe,
                                                std::error_code err) {
      pipe->id(io_m.next_pipe_id());

      pipes_m.push_back(std::move(pipe));

      if (!err)
        enqueuer_m.fire(ev::pipe_created{}, pipes_m.back());
      else
        enqueuer_m.fire(ev::network_error{err, net::direction::out},
                        pipes_m.back());
    });
  }

  void listen(const auto& on)
  {
    auto listener = net::listener::make(io_m, on, id_m);

    listener->listen([&, _ = std::move(listener)](net::pipe::pointer pipe,
                                                  std::error_code err) {
      pipe->id(io_m.next_pipe_id());

      pipes_m.push_back(std::move(pipe));

      if (!err)
      {
        enqueuer_m.fire(ev::pipe_created{}, pipes_m.back());

        return true; // keep accepting
      }
      else
      {
        enqueuer_m.fire(ev::network_error{err, net::direction::in},
                        pipes_m.back());

        return false; // don't keep accepting
      }
    });
  }

  template<template<typename> typename kind_of, ev::event_c event_t,
           typename class_t>
    requires ev::kind_c<kind_of>
  void on(void (class_t::*hf)(ev::hf_arg<kind_of, event_t>), class_t* self)
  {
    insert<kind_of, event_t>(ev::make_hf(hf, self));
  }

  template<template<typename> typename kind_of, ev::event_c event_t>
    requires ev::kind_c<kind_of>
  void insert(ev::handler_f<kind_of, event_t> hf)
  {
    hf_registry_m.insert(id_m, std::move(hf));
  }

private:
  net::io& io_m;
  int id_m = -1;
  ev::enqueuer& enqueuer_m;
  ev::hf_registry& hf_registry_m;
  std::list<pipe::pointer> pipes_m;
};

} // namespace pars::net

#endif // PARS_NET_POINT_H
