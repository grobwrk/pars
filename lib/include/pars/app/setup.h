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

#include "pars/log.h"

namespace pars::app
{

struct with_default_setup
{
private:
  const char* default_pattern()
  {
    return "%^[%H:%M:%S.%f %z] [%6P %6t] [%L]: %$%v";
  }

  const char* default_pattern_with_source_loc()
  {
    return "%^[%H:%M:%S.%f %z] [%6P %6t] [%L]: %$%v \x1b[90m(%s:%#)\x1b[0m";
  }

public:
  void enable_source_loc_logging()
  {
    spdlog::set_pattern(default_pattern_with_source_loc());
  }

  void setup()
  {
    if constexpr (pars_log_enabled)
    {
      using namespace spdlog;

      /// sinks

      std::vector<sink_ptr> sinks;

      auto stderr_s = std::make_shared<sinks::stderr_color_sink_mt>();

      if constexpr (pars_log_enable_stderr)
      {
        auto stderr_sink = std::make_shared<sinks::stderr_color_sink_mt>();

        sinks.push_back(stderr_s);
      }

      if constexpr (pars_log_enable_file)
      {
        auto file_s = std::make_shared<sinks::basic_file_sink_mt>("pars.log");

        sinks.push_back(file_s);
      }

#if defined(PARS_LOG_ENABLE_MSVC)
      auto msvc_s = std::make_shared<sinks::msvc_sink_mt>();

      sinks.push_back(msvc_s);
#endif

#if defined(PARS_LOG_ENABLE_SYSLOG)
      auto syslog_s = std::make_shared<sinks::syslog_sink_mt>(
        "pars", LOG_CONS | LOG_NDELAY, LOG_LOCAL2, true);

      sinks.push_back(syslog_s);
#endif

      /// register pars logger and set as default

      {
        auto default_l = std::make_shared<logger>("pars");

        // default_l->set_pattern(default_pattern);

        default_l->sinks() = sinks;

        register_logger(default_l);

        set_pattern(default_pattern());

        set_default_logger(default_l);

        set_level(static_cast<level::level_enum>(SPDLOG_ACTIVE_LEVEL));
      }
    }
  }

protected:
  with_default_setup() = default;
};

} // namespace pars::app
