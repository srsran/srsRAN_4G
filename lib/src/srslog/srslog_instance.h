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

#ifndef SRSLOG_SRSLOG_INSTANCE_H
#define SRSLOG_SRSLOG_INSTANCE_H

#include "log_backend_impl.h"
#include "sink_repository.h"
#include "srslte/srslog/detail/support/any.h"
#include "srslte/srslog/log_channel.h"

namespace srslog {

/// Singleton of the framework containing all the required classes.
class srslog_instance
{
  srslog_instance() { default_sink = &sink_repo.get_stdout_sink(); }

public:
  srslog_instance(const srslog_instance& other) = delete;
  srslog_instance& operator=(const srslog_instance& other) = delete;

  /// Access function to the singleton instance.
  static srslog_instance& get()
  {
    static srslog_instance instance;
    return instance;
  }

  /// Logger repository accessor.
  using logger_repo_type = object_repository<std::string, detail::any>;
  logger_repo_type& get_logger_repo() { return logger_repo; }
  const logger_repo_type& get_logger_repo() const { return logger_repo; }

  /// Log channel repository accessor.
  using channel_repo_type = object_repository<std::string, log_channel>;
  channel_repo_type& get_channel_repo() { return channel_repo; }
  const channel_repo_type& get_channel_repo() const { return channel_repo; }

  /// Sink repository accessor.
  sink_repository& get_sink_repo() { return sink_repo; }
  const sink_repository& get_sink_repo() const { return sink_repo; }

  /// Backend accessor.
  detail::log_backend& get_backend() { return backend; }
  const detail::log_backend& get_backend() const { return backend; }

  /// Installs the specified error handler into the backend.
  void set_error_handler(error_handler callback)
  {
    backend.set_error_handler(std::move(callback));
  }

  /// Set the specified sink as the default one.
  void set_default_sink(sink& s) { default_sink = &s; }

  /// Returns the default sink.
  sink& get_default_sink() { return *default_sink; }

private:
  /// NOTE: The order of declaration of each member is important here for proper
  /// destruction.
  sink_repository sink_repo;
  log_backend_impl backend;
  channel_repo_type channel_repo;
  logger_repo_type logger_repo;
  detail::shared_variable<sink*> default_sink{nullptr};
};

} // namespace srslog

#endif // SRSLOG_SRSLOG_INSTANCE_H
