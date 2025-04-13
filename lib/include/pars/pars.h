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

#include "pars/app/resources.h"
#include "pars/app/setup.h"
#include "pars/app/single.h"
#include "pars/app/state_machine.h"
#include "pars/comp/backend.h"
#include "pars/comp/client.h"
#include "pars/concept/event.h"
#include "pars/concept/formattable.h"
#include "pars/concept/hashable.h"
#include "pars/concept/kind.h"
#include "pars/concept/net.h"
#include "pars/ev/dispatcher.h"
#include "pars/ev/enqueuer.h"
#include "pars/ev/event.h"
#include "pars/ev/hf_registry.h"
#include "pars/ev/hf_registry__insert.h"
#include "pars/ev/job.h"
#include "pars/ev/kind.h"
#include "pars/ev/kind_base.h"
#include "pars/ev/kind_decl.h"
#include "pars/ev/klass.h"
#include "pars/ev/make_hf.h"
#include "pars/ev/metadata.h"
#include "pars/ev/runner.h"
#include "pars/ev/serializer.h"
#include "pars/ev/spec.h"
#include "pars/log/demangle.h"
#include "pars/log/flags.h"
#include "pars/log/nametype.h"
#include "pars/net/context.h"
#include "pars/net/context_opt.h"
#include "pars/net/context_registry.h"
#include "pars/net/dir.h"
#include "pars/net/hash.h"
#include "pars/net/op.h"
#include "pars/net/pipe.h"
#include "pars/net/pull.h"
#include "pars/net/push.h"
#include "pars/net/rep.h"
#include "pars/net/req.h"
#include "pars/net/socket.h"
#include "pars/net/socket_opt.h"
#include "pars/net/tool_view.h"
