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

#ifndef SRSLOG_SRSLOG_H
#define SRSLOG_SRSLOG_H

#include "srslte/srslog/detail/support/any.h"
#include "srslte/srslog/logger.h"
#include "srslte/srslog/shared_types.h"

namespace srslog {

///
/// Log channel management functions.
///

/// Creates a new log channel instance with the specified id string and sink,
/// then registers it in the log channel repository so that it can be later
/// retrieved in other parts of the application. Returns a pointer to the
/// newly created channel, otherwise when a channel is already registered with
/// the same id it returns nullptr.
log_channel* create_log_channel(const std::string& id, sink& s);

/// Finds a log channel with the specified id string in the repository. On
/// success returns a pointer to the requested log channel, otherwise nullptr.
log_channel* find_log_channel(const std::string& id);

///
/// Logger management functions.
///

namespace detail {

/// Internal helper functions.
detail::any* create_logger(const std::string& id, detail::any&& logger);
detail::any* find_logger(const std::string& id);

} // namespace detail

/// Creates a new logger instance with the specified id string, type and channel
/// references, registering it into the logger repository so that it can be
/// later retrieved in other parts of the application. Returns a pointer to the
/// newly created logger, otherwise when a logger is already registered with the
/// same id it returns nullptr.
/// NOTE: T should be a type that is a logger.
template <typename T, typename... Args>
inline T* create_logger(const std::string& id, Args&&... args)
{
  static_assert(detail::is_logger<T>::value, "T should be a logger type");
  auto logger = detail::make_any<T>(id, std::forward<Args>(args)...);
  detail::any* p = detail::create_logger(id, std::move(logger));

  return detail::any_cast<T>(p);
}

/// Finds a logger with the specified id string and type in the repository. On
/// success returns a pointer to the requested logger, otherwise nullptr.
/// NOTE: T should be a type that is a logger.
template <typename T>
inline T* find_logger(const std::string& id)
{
  static_assert(detail::is_logger<T>::value, "T should be a logger type");
  detail::any* p = detail::find_logger(id);

  return detail::any_cast<T>(p);
}

///
/// Sink management functions.
///

/// Finds a sink with the specified id string in the repository. On
/// success returns a pointer to the requested sink, otherwise nullptr.
sink* find_sink(const std::string& id);

/// Creates a new sink that writes into the stdout stream and registers it into
/// a sink repository so that it can be later retrieved in other parts of the
/// application. Different stdout sinks can be created by providing different
/// names. Returns a pointer to the newly created sink or nullptr if a sink with
/// the same name was already registered.
sink* create_stdout_sink(const std::string& name = "stdout");

/// Creates a new sink that writes into the stderr stream and registers it into
/// a sink repository so that it can be later retrieved in other parts of the
/// application. Different stderr sinks can be created by providing different
/// names. Returns a pointer to the newly created sink or nullptr if a sink with
/// the same name was already registered.
sink* create_stderr_sink(const std::string& name = "stderr");

/// Creates a new sink that writes into the a file in the specified path and
/// registers it into a sink repository so that it can be later retrieved in
/// other parts of the application. Returns a pointer to the newly created sink
/// or nullptr if a sink with the same path was already registered.
/// Specifying a max_size value different to zero will make the sink create a
/// new file each time the current file exceeds this value. The units of
/// max_size are bytes.
sink* create_file_sink(const std::string& path, size_t max_size = 0);

///
/// Framework configuration and control functions.
///

/// This function initializes the logging framework. It must be called before
/// any log entry is generated.
/// NOTE: Calling this function more than once has no side effects.
void init();

/// Installs the specified error handler to receive any error messages generated
/// by the framework.
/// NOTE: This function should be called before init().
void set_error_handler(error_handler handler);

} // namespace srslog

#endif // SRSLOG_SRSLOG_H
