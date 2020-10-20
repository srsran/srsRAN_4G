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

#include "srslte/srslog/event_trace.h"
#include "srslte/srslog/srslog.h"
#include <cassert>
#include <ctime>

#undef trace_duration_begin
#undef trace_duration_end
#undef trace_complete_event

using namespace srslog;

/// Log channel where event traces will get sent.
static log_channel* tracer = nullptr;

void srslog::event_trace_init()
{
  // Nothing to do if the user previously set a custom channel or this is not
  // the first time this function is called.
  if (tracer) {
    return;
  }

  // Default file name where event traces will get stored.
  static constexpr char default_file_name[] = "event_trace.log";

  // Create the default event trace channel.
  //:TODO: handle name reservation.
  sink* s = create_file_sink(default_file_name);
  assert(s && "Default event file sink is reserved");
  tracer = create_log_channel("event_trace_channel", *s);
  assert(tracer && "Default event trace channel is reserved");
}

void srslog::event_trace_init(log_channel& c)
{
  // Nothing to set when a channel has already been installed.
  if (!tracer) {
    tracer = &c;
  }
}

/// Fills in the input buffer with the current time.
static void format_time(char* buffer, size_t len)
{
  std::time_t t = std::time(nullptr);
  std::tm lt{};
  ::localtime_r(&t, &lt);
  std::strftime(buffer, len, "%FT%T", &lt);
}

namespace srslog {

void trace_duration_begin(const std::string& category, const std::string& name)
{
  if (!tracer) {
    return;
  }

  char fmt_time[24];
  format_time(fmt_time, sizeof(fmt_time));
  (*tracer)("[%s] [TID:%0u] Entering \"%s\": %s",
            fmt_time,
            (unsigned)::pthread_self(),
            category,
            name);
}

void trace_duration_end(const std::string& category, const std::string& name)
{
  if (!tracer) {
    return;
  }

  char fmt_time[24];
  format_time(fmt_time, sizeof(fmt_time));
  (*tracer)("[%s] [TID:%0u] Leaving \"%s\": %s",
            fmt_time,
            (unsigned)::pthread_self(),
            category,
            name);
}

} // namespace srslog

/// Private implementation of the complete event destructor.
srslog::detail::scoped_complete_event::~scoped_complete_event()
{
  if (!tracer)
    return;

  auto end = std::chrono::steady_clock::now();
  unsigned long long diff =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start)
          .count();

  char fmt_time[24];
  format_time(fmt_time, sizeof(fmt_time));
  (*tracer)("[%s] [TID:%0u] Complete event \"%s\" (duration %lld us): %s",
            fmt_time,
            (unsigned)::pthread_self(),
            category,
            diff,
            name);
}
