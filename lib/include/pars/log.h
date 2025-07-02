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

#include "init.h"

#include <spdlog/common.h>

// include generated config file
#include "config.h"

// include spdlog
#undef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL pars_log_level
#include <spdlog/spdlog.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#if defined(_WIN32)
#define PARS_LOG_ENABLE_MSVC
#include <spdlog/sinks/msvc_sink.h>
#else
#define PARS_LOG_ENABLE_SYSLOG
#include <spdlog/sinks/syslog_sink.h>
#endif

// spdlog helper macro

#define SL (spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION})

namespace pars
{

template<typename... args_t>
void log(spdlog::source_loc loc, const lf flags, spdlog::level::level_enum lvl,
         spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  if constexpr (pars_log_enabled)
  {
    if (pars_log_flags & flags)
      ::spdlog::log(loc, lvl, fmt, std::forward<args_t>(args)...);
  }
}

template<typename... args_t>
void log(const lf flags, spdlog::level::level_enum lvl,
         spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  if constexpr (pars_log_enabled)
  {
    if (pars_log_flags & flags)
      ::spdlog::log(lvl, fmt, std::forward<args_t>(args)...);
  }
}

template<typename... args_t>
void log(spdlog::source_loc loc, spdlog::level::level_enum lvl,
         spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(loc, lf::user, lvl, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void log(spdlog::level::level_enum lvl, spdlog::format_string_t<args_t...> fmt,
         args_t&&... args)
{
  ::pars::log(lf::user, lvl, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void trace(spdlog::source_loc loc, const lf flags,
           spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(loc, flags, spdlog::level::trace, fmt,
              std::forward<args_t>(args)...);
}

template<typename... args_t>
void trace(const lf flags, spdlog::format_string_t<args_t...> fmt,
           args_t&&... args)
{
  ::pars::log(flags, spdlog::level::trace, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void trace(spdlog::source_loc loc, spdlog::format_string_t<args_t...> fmt,
           args_t&&... args)
{
  ::pars::log(loc, spdlog::level::trace, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void trace(spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(spdlog::level::trace, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void debug(spdlog::source_loc loc, const lf flags,
           spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(loc, flags, spdlog::level::debug, fmt,
              std::forward<args_t>(args)...);
}

template<typename... args_t>
void debug(const lf flags, spdlog::format_string_t<args_t...> fmt,
           args_t&&... args)
{
  ::pars::log(flags, spdlog::level::debug, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void debug(spdlog::source_loc loc, spdlog::format_string_t<args_t...> fmt,
           args_t&&... args)
{
  ::pars::log(loc, spdlog::level::debug, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void debug(spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(spdlog::level::debug, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void info(spdlog::source_loc loc, const lf flags,
          spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(loc, flags, spdlog::level::info, fmt,
              std::forward<args_t>(args)...);
}

template<typename... args_t>
void info(const lf flags, spdlog::format_string_t<args_t...> fmt,
          args_t&&... args)
{
  ::pars::log(flags, spdlog::level::info, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void info(spdlog::source_loc loc, spdlog::format_string_t<args_t...> fmt,
          args_t&&... args)
{
  ::pars::log(loc, spdlog::level::info, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void info(spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(spdlog::level::info, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void warn(spdlog::source_loc loc, const lf flags,
          spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(loc, flags, spdlog::level::warn, fmt,
              std::forward<args_t>(args)...);
}

template<typename... args_t>
void warn(const lf flags, spdlog::format_string_t<args_t...> fmt,
          args_t&&... args)
{
  ::pars::log(flags, spdlog::level::warn, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void warn(spdlog::source_loc loc, spdlog::format_string_t<args_t...> fmt,
          args_t&&... args)
{
  ::pars::log(loc, spdlog::level::warn, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void warn(spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(spdlog::level::warn, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void err(spdlog::source_loc loc, const lf flags,
         spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(loc, flags, spdlog::level::err, fmt,
              std::forward<args_t>(args)...);
}

template<typename... args_t>
void err(const lf flags, spdlog::format_string_t<args_t...> fmt,
         args_t&&... args)
{
  ::pars::log(flags, spdlog::level::err, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void err(spdlog::source_loc loc, spdlog::format_string_t<args_t...> fmt,
         args_t&&... args)
{
  ::pars::log(loc, spdlog::level::err, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void err(spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(spdlog::level::err, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void critical(spdlog::source_loc loc, const lf flags,
              spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(flags, loc, spdlog::level::critical, fmt,
              std::forward<args_t>(args)...);
}

template<typename... args_t>
void critical(const lf flags, spdlog::format_string_t<args_t...> fmt,
              args_t&&... args)
{
  ::pars::log(flags, spdlog::level::critical, fmt,
              std::forward<args_t>(args)...);
}

template<typename... args_t>
void critical(spdlog::source_loc loc, spdlog::format_string_t<args_t...> fmt,
              args_t&&... args)
{
  ::pars::log(loc, spdlog::level::critical, fmt, std::forward<args_t>(args)...);
}

template<typename... args_t>
void critical(spdlog::format_string_t<args_t...> fmt, args_t&&... args)
{
  ::pars::log(spdlog::level::critical, fmt, std::forward<args_t>(args)...);
}

} // namespace pars
