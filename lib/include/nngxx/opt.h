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

#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>

namespace nngxx
{

enum class opt
{
  sockname,
  raw,
  // proto, // deprecated
  // protoname, // deprecated
  // peer, // deprecated
  // peername, // deprecated
  recvbuf,
  sendbuf,
  recvfd,
  sendfd,
  recvtimeo,
  sendtimeo,
  locaddr,
  remaddr,
  url,
  maxttl,
  recvmaxsz,
  reconnmint,
  reconnmaxt,
  peergid,
  peerpid,
  peeruid,
  peerzoneid,
  req_resend_time,
  req_resend_tick
};

enum class opt_overload
{
  integer,
  boolean,
  string,
  size,
  sockaddr,
  duration,
  uint64
};

template<opt o>
[[nodiscard]] static constexpr const char* opt_to_nng_value() noexcept
{
  if constexpr (o == opt::sockname)
  {
    return NNG_OPT_SOCKNAME;
  }
  else if constexpr (o == opt::raw)
  {
    return NNG_OPT_RAW;
  }
  else if constexpr (o == opt::recvbuf)
  {
    return NNG_OPT_RECVBUF;
  }
  else if constexpr (o == opt::sendbuf)
  {
    return NNG_OPT_SENDBUF;
  }
  else if constexpr (o == opt::recvfd)
  {
    return NNG_OPT_RECVFD;
  }
  else if constexpr (o == opt::sendfd)
  {
    return NNG_OPT_SENDFD;
  }
  else if constexpr (o == opt::recvtimeo)
  {
    return NNG_OPT_RECVTIMEO;
  }
  else if constexpr (o == opt::sendtimeo)
  {
    return NNG_OPT_SENDTIMEO;
  }
  else if constexpr (o == opt::locaddr)
  {
    return NNG_OPT_LOCADDR;
  }
  else if constexpr (o == opt::remaddr)
  {
    return NNG_OPT_REMADDR;
  }
  else if constexpr (o == opt::url)
  {
    return NNG_OPT_URL;
  }
  else if constexpr (o == opt::maxttl)
  {
    return NNG_OPT_MAXTTL;
  }
  else if constexpr (o == opt::recvmaxsz)
  {
    return NNG_OPT_RECVMAXSZ;
  }
  else if constexpr (o == opt::reconnmint)
  {
    return NNG_OPT_RECONNMINT;
  }
  else if constexpr (o == opt::reconnmaxt)
  {
    return NNG_OPT_RECONNMAXT;
  }
  else if constexpr (o == opt::peergid)
  {
    return NNG_OPT_PEER_GID;
  }
  else if constexpr (o == opt::peerpid)
  {
    return NNG_OPT_PEER_UID;
  }
  else if constexpr (o == opt::peeruid)
  {
    return NNG_OPT_PEER_UID;
  }
  else if constexpr (o == opt::peerzoneid)
  {
    return NNG_OPT_PEER_ZONEID;
  }
  else if constexpr (o == opt::req_resend_time)
  {
    return NNG_OPT_REQ_RESENDTIME;
  }
  else if constexpr (o == opt::req_resend_tick)
  {
    return NNG_OPT_REQ_RESENDTICK;
  }
}

template<opt o>
[[nodiscard]] static constexpr opt_overload opt_to_nng_overload() noexcept
{
  if constexpr (o == opt::sockname)
  {
    return opt_overload::string;
  }
  else if constexpr (o == opt::raw)
  {
    return opt_overload::boolean;
  }
  else if constexpr (o == opt::recvbuf)
  {
    return opt_overload::integer;
  }
  else if constexpr (o == opt::sendbuf)
  {
    return opt_overload::integer;
  }
  else if constexpr (o == opt::recvfd)
  {
    return opt_overload::integer;
  }
  else if constexpr (o == opt::sendfd)
  {
    return opt_overload::integer;
  }
  else if constexpr (o == opt::recvtimeo)
  {
    return opt_overload::duration;
  }
  else if constexpr (o == opt::sendtimeo)
  {
    return opt_overload::duration;
  }
  else if constexpr (o == opt::locaddr)
  {
    return opt_overload::sockaddr;
  }
  else if constexpr (o == opt::remaddr)
  {
    return opt_overload::sockaddr;
  }
  else if constexpr (o == opt::url)
  {
    return opt_overload::string;
  }
  else if constexpr (o == opt::maxttl)
  {
    return opt_overload::integer;
  }
  else if constexpr (o == opt::recvmaxsz)
  {
    return opt_overload::size;
  }
  else if constexpr (o == opt::reconnmint)
  {
    return opt_overload::duration;
  }
  else if constexpr (o == opt::reconnmaxt)
  {
    return opt_overload::duration;
  }
  else if constexpr (o == opt::peergid)
  {
    return opt_overload::uint64;
  }
  else if constexpr (o == opt::peerpid)
  {
    return opt_overload::uint64;
  }
  else if constexpr (o == opt::peeruid)
  {
    return opt_overload::uint64;
  }
  else if constexpr (o == opt::peerzoneid)
  {
    return opt_overload::uint64;
  }
  else if constexpr (o == opt::req_resend_time)
  {
    return opt_overload::duration;
  }
  else if constexpr (o == opt::req_resend_tick)
  {
    return opt_overload::duration;
  }
}

template<opt o>
struct opt_to_nng_type;

template<opt o>
using opt_to_nng_type_t = opt_to_nng_type<o>::return_type;

template<>
struct opt_to_nng_type<opt::sockname>
{
  using return_type = const char*;
};

template<>
struct opt_to_nng_type<opt::recvtimeo>
{
  using return_type = nng_duration;
};

template<>
struct opt_to_nng_type<opt::sendtimeo>
{
  using return_type = nng_duration;
};

template<>
struct opt_to_nng_type<opt::req_resend_time>
{
  using return_type = nng_duration;
};

template<>
struct opt_to_nng_type<opt::req_resend_tick>
{
  using return_type = nng_duration;
};

} // namespace nngxx
