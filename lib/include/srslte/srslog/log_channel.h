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

#ifndef SRSLOG_LOG_CHANNEL_H
#define SRSLOG_LOG_CHANNEL_H

#include "srslte/srslog/detail/log_backend.h"
#include "srslte/srslog/detail/support/thread_utils.h"

namespace srslog {

/// A log channel is the entity used for logging messages.
///
/// It can deliver a log entry to one or more different sinks, for example a
/// file or a console.
/// Generated log entries by the application will be ignored by the channel when
/// set to disabled.
/// NOTE: Thread safe class.
class log_channel
{
public:
  log_channel(std::string id, sink& s, detail::log_backend& backend) :
    log_id(std::move(id)),
    log_sink(s),
    backend(backend),
    is_enabled(true)
  {}

  log_channel(const log_channel& other) = delete;
  log_channel& operator=(const log_channel& other) = delete;

  /// Controls when the channel accepts incoming log entries.
  void set_enabled(bool enabled) { is_enabled = enabled; }

  /// Returns true if the channel is accepting incoming log entries, otherwise
  /// false.
  bool enabled() const { return is_enabled; }

  /// Returns the id string of the channel.
  const std::string& id() const { return log_id; }

  /// Builds the provided log entry and passes it to the backend. When the
  /// channel is disabled the log entry will be discarded.
  template <typename... Args>
  void operator()(const std::string& fmtstr, Args&&... args)
  {
    if (!enabled()) {
      return;
    }

    // Populate the store with all incoming arguments.
    fmt::dynamic_format_arg_store<fmt::printf_context> store;
    (void)std::initializer_list<int>{(store.push_back(args), 0)...};

    // Send the log entry to the backend.
    detail::log_entry entry = {&log_sink, fmtstr, std::move(store)};
    backend.push(std::move(entry));
  }

private:
  const std::string log_id;
  sink& log_sink;
  detail::log_backend& backend;
  detail::shared_variable<bool> is_enabled;
};

} // namespace srslog

#endif // SRSLOG_LOG_CHANNEL_H
