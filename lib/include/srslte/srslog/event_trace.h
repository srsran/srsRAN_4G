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

#ifndef SRSLOG_EVENT_TRACE_H
#define SRSLOG_EVENT_TRACE_H

#include <chrono>
#include <string>

namespace srslog {

class log_channel;

/// The event tracing framework allows to trace any kind of event inside an
/// application.
/// To enable event tracing the ENABLE_SRSLOG_EVENT_TRACE macro symbol should
/// be defined, otherwise calls to the tracing framework will be ignored. This
/// is important to avoid the overhead of tracing when it is not required.
/// For details about each event trace type please refer to:
/// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit

/// Initializes the event trace framework saving the trace events to a
/// "event_trace.log" file.
void event_trace_init();

/// Initializes the event trace framework using the specified log channel to log
/// all trace events.
void event_trace_init(log_channel& c);

#ifdef ENABLE_SRSLOG_EVENT_TRACE

/// Generates the begin phase of a duration event.
void trace_duration_begin(const std::string& category, const std::string& name);

/// Generates the end phase of a duration event.
void trace_duration_end(const std::string& category, const std::string& name);

/// Generates a complete event.
#define trace_complete_event(C, N)                                             \
  auto scoped_complete_event_variable = detail::scoped_complete_event(C, N)

#else

/// No-ops.
#define trace_duration_begin(C, N)
#define trace_duration_end(C, N)
#define trace_complete_event(C, N)

#endif

namespace detail {

/// Scoped type object for implementing a complete event.
class scoped_complete_event
{
public:
  scoped_complete_event(std::string cat, std::string n) :
    category(std::move(cat)),
    name(std::move(n)),
    start(std::chrono::steady_clock::now())
  {}

  ~scoped_complete_event();

private:
  const std::string category;
  const std::string name;
  std::chrono::time_point<std::chrono::steady_clock> start;
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_EVENT_TRACE_H
