/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLOG_SRSLOG_INSTANCE_H
#define SRSLOG_SRSLOG_INSTANCE_H

#include "formatters/text_formatter.h"
#include "log_backend_impl.h"
#include "object_repository.h"
#include "sinks/stream_sink.h"
#include "srsran/srslog/log_channel.h"

namespace srslog {

/// Singleton of the framework containing all the required classes.
class srslog_instance
{
  srslog_instance()
  {
    // stdout and stderr sinks are always present.
    auto& stdout_sink =
        sink_repo.emplace(std::piecewise_construct,
                          std::forward_as_tuple("stdout"),
                          std::forward_as_tuple(new stream_sink(sink_stream_type::stdout,
                                                                std::unique_ptr<log_formatter>(new text_formatter))));
    default_sink = stdout_sink.get();

    sink_repo.emplace(std::piecewise_construct,
                      std::forward_as_tuple("stderr"),
                      std::forward_as_tuple(new stream_sink(sink_stream_type::stderr,
                                                            std::unique_ptr<log_formatter>(new text_formatter))));

    // Initialize the default formatter pointer with a text formatter.
    {
      detail::scoped_lock lock(formatter_mutex);
      default_formatter = std::unique_ptr<log_formatter>(new text_formatter);
    }
  }

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
  logger_repo_type&       get_logger_repo() { return logger_repo; }
  const logger_repo_type& get_logger_repo() const { return logger_repo; }

  /// Log channel repository accessor.
  using channel_repo_type = object_repository<std::string, log_channel>;
  channel_repo_type&       get_channel_repo() { return channel_repo; }
  const channel_repo_type& get_channel_repo() const { return channel_repo; }

  /// Sink repository accessor.
  using sink_repo_type = object_repository<std::string, std::unique_ptr<sink> >;
  sink_repo_type&       get_sink_repo() { return sink_repo; }
  const sink_repo_type& get_sink_repo() const { return sink_repo; }

  /// Backend accessor.
  detail::log_backend&       get_backend() { return backend; }
  const detail::log_backend& get_backend() const { return backend; }

  /// Installs the specified error handler into the backend.
  void set_error_handler(error_handler callback) { backend.set_error_handler(std::move(callback)); }

  /// Set the specified sink as the default one.
  void set_default_sink(sink& s) { default_sink = &s; }

  /// Returns the default sink.
  sink& get_default_sink() const { return *default_sink; }

  /// Set the specified formatter as the default one.
  void set_default_formatter(std::unique_ptr<log_formatter> f)
  {
    detail::scoped_lock lock(formatter_mutex);
    default_formatter = std::move(f);
  }

  /// Returns the default formatter.
  std::unique_ptr<log_formatter> get_default_formatter() const
  {
    detail::scoped_lock lock(formatter_mutex);
    return default_formatter->clone();
  }

private:
  /// NOTE: The order of declaration of each member is important here for proper
  /// destruction.
  sink_repo_type                 sink_repo;
  log_backend_impl               backend;
  channel_repo_type              channel_repo;
  logger_repo_type               logger_repo;
  detail::shared_variable<sink*> default_sink{nullptr};
  mutable detail::mutex          formatter_mutex;
  std::unique_ptr<log_formatter> default_formatter;
};

} // namespace srslog

#endif // SRSLOG_SRSLOG_INSTANCE_H
