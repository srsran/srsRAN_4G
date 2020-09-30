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
/// NOTE: All functions are thread safe unless otherwise specified.
///

///
/// Log channel management functions.
///

/// Finds a log channel with the specified id string in the repository. On
/// success returns a pointer to the requested log channel, otherwise nullptr.
log_channel* find_log_channel(const std::string& id);

/// Returns an instance of a log_channel with the specified id that writes to
/// the default sink using the default log channel configuration.
/// NOTE: Any '#' characters in the id will get removed.
log_channel& fetch_log_channel(const std::string& id);

/// Returns an instance of a log_channel with the specified id that writes to
/// the specified sink.
/// NOTE: Any '#' characters in the id will get removed.
log_channel&
fetch_log_channel(const std::string& id, sink& s, log_channel_config config);

/// Creates a new log channel instance with the specified id string and sink,
/// then registers it in the log channel repository so that it can be later
/// retrieved in other parts of the application. Returns a pointer to the
/// newly created channel, otherwise when a channel is already registered with
/// the same id it returns nullptr.
/// NOTE: The input id string should not contain any '#' characters otherwise
/// nullptr is returned.
/// NOTE: Deprecated, use fetch_log_channel instead.
log_channel* create_log_channel(const std::string& id, sink& s);

namespace detail {

/// Internal helper functions.
detail::any* fetch_logger(const std::string& id, detail::any&& logger);
detail::any* find_logger(const std::string& id);

} // namespace detail

///
/// Logger management functions.
///

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

/// Returns an instance of a basic logger (see basic_logger type) with the
/// specified id string. All logger channels will write into the default sink.
/// The context value of the logger can be printed on each log entry by setting
/// to true the should_print_context argument.
basic_logger& fetch_basic_logger(const std::string& id,
                                 bool should_print_context = true);

/// Returns an instance of a basic logger (see basic_logger type) with the
/// specified id string. All logger channels will write into the specified sink.
/// The context value of the logger can be printed on each log entry by setting
/// to true the should_print_context argument.
basic_logger& fetch_basic_logger(const std::string& id,
                                 sink& s,
                                 bool should_print_context = true);

/// Returns a logger instance with the specified id string, type and channel
/// references.
/// NOTE: T should be a type that is a logger.
template <typename T, typename... Args>
inline T& fetch_logger(const std::string& id, Args&&... args)
{
  static_assert(detail::is_logger<T>::value, "T should be a logger type");
  auto logger = detail::make_any<T>(id, std::forward<Args>(args)...);
  detail::any* p = detail::fetch_logger(id, std::move(logger));

  return *detail::any_cast<T>(p);
}

/// Creates a new basic logger instance (see basic_logger type) with the
/// specified id string and sink, registering it into the logger repository so
/// that it can be later retrieved in other parts of the application. The
/// context value of the logger can be printed on each log entry by setting to
/// true the should_print_context argument. All logger channels will write into
/// the specified sink. Returns a pointer to the newly created logger, otherwise
/// when a logger is already registered with the same id it returns nullptr.
/// NOTE: Deprecated, use fetch_basic_logger instead.
basic_logger* create_basic_logger(const std::string& id,
                                  sink& s,
                                  bool should_print_context = true);

/// Creates a new logger instance with the specified id string, type and channel
/// references, registering it into the logger repository so that it can be
/// later retrieved in other parts of the application. Returns a pointer to the
/// newly created logger, otherwise when a logger is already registered with the
/// same id it returns nullptr.
/// NOTE: T should be a type that is a logger.
/// NOTE: Deprecated, use fetch_logger instead.
template <typename T, typename... Args>
inline T* create_logger(const std::string& id, Args&&... args)
{
  static_assert(detail::is_logger<T>::value, "T should be a logger type");
  auto logger = detail::make_any<T>(id, std::forward<Args>(args)...);
  detail::any* p = detail::fetch_logger(id, std::move(logger));

  return detail::any_cast<T>(p);
}

///
/// Sink management functions.
///

/// Installs the specified sink to be used as the default one by new log
/// channels and loggers.
/// The initial default sink writes to stdout.
void set_default_sink(sink& s);

/// Returns the instance of the default sink being used.
sink& get_default_sink();

/// Finds a sink with the specified id string in the repository. On
/// success returns a pointer to the requested sink, otherwise nullptr.
sink* find_sink(const std::string& id);

/// Returns an instance of a sink that writes to the stdout stream.
sink& fetch_stdout_sink();

/// Returns an instance of a sink that writes to the stderr stream.
sink& fetch_stderr_sink();

/// Returns an instance of a sink that writes into a file in the specified path.
/// Specifying a max_size value different to zero will make the sink create a
/// new file each time the current file exceeds this value. The units of
/// max_size are bytes.
/// NOTE: Any '#' characters in the id will get removed.
sink& fetch_file_sink(const std::string& path, size_t max_size = 0);

/// Creates a new sink that writes into the a file in the specified path and
/// registers it into a sink repository so that it can be later retrieved in
/// other parts of the application. Returns a pointer to the newly created sink
/// or nullptr if a sink with the same path was already registered.
/// Specifying a max_size value different to zero will make the sink create a
/// new file each time the current file exceeds this value. The units of
/// max_size are bytes.
/// NOTE: Deprecated, use fetch_file_sink instead.
sink* create_file_sink(const std::string& path, size_t max_size = 0);

/// Creates a new sink that writes into the stdout stream and registers it into
/// a sink repository so that it can be later retrieved in other parts of the
/// application. Different stdout sinks can be created by providing different
/// names. Returns a pointer to the newly created sink or nullptr if a sink with
/// the same name was already registered.
/// NOTE: Deprecated, use get_stdout_sink instead.
sink* create_stdout_sink(const std::string& name = "stdout");

/// Creates a new sink that writes into the stderr stream and registers it into
/// a sink repository so that it can be later retrieved in other parts of the
/// application. Different stderr sinks can be created by providing different
/// names. Returns a pointer to the newly created sink or nullptr if a sink with
/// the same name was already registered.
/// NOTE: Deprecated, use get_stderr_sink instead.
sink* create_stderr_sink(const std::string& name = "stderr");

///
/// Framework configuration and control functions.
///

/// This function initializes the logging framework. It must be called before
/// any log entry is generated.
/// NOTE: Calling this function more than once has no side effects.
void init();

/// Flushes the contents of all the registered sinks. The caller thread will
/// block until the operation is completed.
/// NOTE: This function does nothing if init() has not been called.
void flush();

/// Installs the specified error handler to receive any error messages generated
/// by the framework.
/// NOTE: This function should be called before init() and is NOT thread safe.
void set_error_handler(error_handler handler);

} // namespace srslog

#endif // SRSLOG_SRSLOG_H
