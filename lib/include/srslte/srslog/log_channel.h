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
#include <cassert>

namespace srslog {

/// Log channel configuration settings.
struct log_channel_config {
  log_channel_config() = default;
  log_channel_config(std::string n, char tag, bool should_print_context) :
    name(std::move(n)),
    tag(tag),
    should_print_context(should_print_context)
  {}

  /// Optional log channel name. If set, will get printed for each log entry.
  /// Disabled by default.
  std::string name;
  /// Optional log channel tag. If set, will get printed for each log entry.
  /// Disabled by default.
  char tag = '\0';
  /// When set to true, each log entry will get printed with the context value.
  /// Disabled by default.
  bool should_print_context = false;
};

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
    log_channel(std::move(id), s, backend, {})
  {}

  log_channel(std::string id,
              sink& s,
              detail::log_backend& backend,
              log_channel_config config) :
    log_id(std::move(id)),
    log_sink(s),
    backend(backend),
    log_name(std::move(config.name)),
    log_tag(config.tag),
    should_print_context(config.should_print_context),
    ctx_value(0),
    hex_max_size(0),
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

  /// Set the log channel context to the specified value.
  void set_context(uint32_t x) { ctx_value = x; }

  /// Set the maximum number of bytes to can be printed in a hex dump.
  /// Set to -1 to indicate no hex dump limit.
  void set_hex_dump_max_size(int size) { hex_max_size = size; }

  /// Builds the provided log entry and passes it to the backend. When the
  /// channel is disabled the log entry will be discarded.
  template <typename... Args>
  void operator()(const std::string& fmtstr, Args&&... args)
  {
    if (!enabled()) {
      return;
    }
    assert(&log_sink);
    // Populate the store with all incoming arguments.
    fmt::dynamic_format_arg_store<fmt::printf_context> store;
    (void)std::initializer_list<int>{(store.push_back(args), 0)...};

    // Send the log entry to the backend.
    detail::log_entry entry = {&log_sink,
                               std::chrono::high_resolution_clock::now(),
                               {ctx_value, should_print_context},
                               fmtstr,
                               std::move(store),
                               log_name,
                               log_tag};
    backend.push(std::move(entry));
  }

  /// Builds the provided log entry and passes it to the backend. When the
  /// channel is disabled the log entry will be discarded.
  template <typename... Args>
  void operator()(const uint8_t* buffer,
                  size_t len,
                  const std::string& fmtstr,
                  Args&&... args)
  {
    if (!enabled()) {
      return;
    }

    assert(&log_sink);
    // Populate the store with all incoming arguments.
    fmt::dynamic_format_arg_store<fmt::printf_context> store;
    (void)std::initializer_list<int>{(store.push_back(args), 0)...};

    // Calculate the length to capture in the buffer.
    if (hex_max_size >= 0)
      len = std::min<size_t>(len, hex_max_size);

    // Send the log entry to the backend.
    detail::log_entry entry = {&log_sink,
                               std::chrono::high_resolution_clock::now(),
                               {ctx_value, should_print_context},
                               fmtstr,
                               std::move(store),
                               log_name,
                               log_tag,
                               std::vector<uint8_t>(buffer, buffer + len)};
    backend.push(std::move(entry));
  }

private:
  const std::string log_id;
  sink& log_sink;
  detail::log_backend& backend;
  const std::string log_name;
  const char log_tag;
  const bool should_print_context;
  detail::shared_variable<uint32_t> ctx_value;
  detail::shared_variable<int> hex_max_size;
  detail::shared_variable<bool> is_enabled;
};

} // namespace srslog

#endif // SRSLOG_LOG_CHANNEL_H
