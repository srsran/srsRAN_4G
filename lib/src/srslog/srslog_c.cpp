/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/srslog/srslog_c.h"
#include "srslte/srslog/srslog.h"
#include <cassert>
#include <cstdarg>

using namespace srslog;

template <typename To, typename From>
static inline To* c_cast(From* x)
{
  return reinterpret_cast<To*>(x);
}

/// Helper to format the input argument list writing it into a channel.
static void log_to(log_channel& c, const char* fmt, std::va_list args)
{
  char buffer[1024];
  std::vsnprintf(buffer, sizeof(buffer) - 1, fmt, args);
  c(buffer);
}

void srslog_init(void)
{
  init();
}

void srslog_set_default_sink(srslog_sink* s)
{
  assert(s && "Expected a valid sink");
  set_default_sink(*c_cast<sink>(s));
}

srslog_sink* srslog_get_default_sink(void)
{
  return c_cast<srslog_sink>(&get_default_sink());
}

srslog_log_channel* srslog_fetch_log_channel(const char* id)
{
  return c_cast<srslog_log_channel>(&fetch_log_channel(id));
}

srslog_log_channel* srslog_find_log_channel(const char* id)
{
  return c_cast<srslog_log_channel>(find_log_channel(id));
}

void srslog_set_log_channel_enabled(srslog_log_channel* channel,
                                    srslog_bool enabled)
{
  assert(channel && "Expected a valid channel");
  c_cast<log_channel>(channel)->set_enabled(enabled);
}

srslog_bool srslog_is_log_channel_enabled(srslog_log_channel* channel)
{
  assert(channel && "Expected a valid channel");
  return c_cast<log_channel>(channel)->enabled();
}

const char* srslog_get_log_channel_id(srslog_log_channel* channel)
{
  assert(channel && "Expected a valid channel");
  return c_cast<log_channel>(channel)->id().c_str();
}

void srslog_log(srslog_log_channel* channel, const char* fmt, ...)
{
  assert(channel && "Expected a valid channel");

  std::va_list args;
  va_start(args, fmt);
  log_to(*c_cast<log_channel>(channel), fmt, args);
  va_end(args);
}

srslog_logger* srslog_fetch_default_logger(const char* id)
{
  return c_cast<srslog_logger>(&fetch_basic_logger(id));
}

srslog_logger* srslog_find_default_logger(const char* id)
{
  return c_cast<srslog_logger>(find_logger<basic_logger>(id));
}

void srslog_debug(srslog_logger* log, const char* fmt, ...)
{
  assert(log && "Expected a valid logger");

  std::va_list args;
  va_start(args, fmt);
  log_to(c_cast<basic_logger>(log)->debug, fmt, args);
  va_end(args);
}

void srslog_info(srslog_logger* log, const char* fmt, ...)
{
  assert(log && "Expected a valid logger");

  std::va_list args;
  va_start(args, fmt);
  log_to(c_cast<basic_logger>(log)->info, fmt, args);
  va_end(args);
}

void srslog_warning(srslog_logger* log, const char* fmt, ...)
{
  assert(log && "Expected a valid logger");

  std::va_list args;
  va_start(args, fmt);
  log_to(c_cast<basic_logger>(log)->warning, fmt, args);
  va_end(args);
}

void srslog_error(srslog_logger* log, const char* fmt, ...)
{
  assert(log && "Expected a valid logger");

  std::va_list args;
  va_start(args, fmt);
  log_to(c_cast<basic_logger>(log)->error, fmt, args);
  va_end(args);
}

const char* srslog_get_logger_id(srslog_logger* log)
{
  assert(log && "Expected a valid logger");
  return c_cast<basic_logger>(log)->id().c_str();
}

/// Translate the C API level enum to basic_levels.
static basic_levels convert_c_enum_to_basic_levels(srslog_log_levels lvl)
{
  switch (lvl) {
    case srslog_lvl_debug:
      return basic_levels::debug;
    case srslog_lvl_info:
      return basic_levels::info;
    case srslog_lvl_warning:
      return basic_levels ::warning;
    case srslog_lvl_error:
      return basic_levels::error;
  }

  assert(false && "Invalid enum value");
  return basic_levels::error;
}

void srslog_set_logger_level(srslog_logger* log, srslog_log_levels lvl)
{
  assert(log && "Expected a valid logger");
  c_cast<basic_logger>(log)->set_level(convert_c_enum_to_basic_levels(lvl));
}

srslog_sink* srslog_find_sink(const char* id)
{
  return c_cast<srslog_sink>(find_sink(id));
}

srslog_sink* srslog_fetch_stdout_sink(void)
{
  return c_cast<srslog_sink>(&fetch_stdout_sink());
}

srslog_sink* srslog_fetch_stderr_sink(void)
{
  return c_cast<srslog_sink>(&fetch_stderr_sink());
}

srslog_sink* srslog_fetch_file_sink(const char* path, size_t max_size)
{
  return c_cast<srslog_sink>(&fetch_file_sink(path, max_size));
}
