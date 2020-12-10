/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLOG_DETAIL_LOG_BACKEND_H
#define SRSLOG_DETAIL_LOG_BACKEND_H

namespace srslog {

namespace detail {

struct log_entry;

/// The log backend receives generated log entries from the application. Each
/// entry gets distributed to the corresponding sinks.
/// NOTE: Thread safe class.
class log_backend
{
public:
  virtual ~log_backend() = default;

  /// Starts the processing of incoming log entries.
  /// NOTE: Calling this function more than once has no side effects.
  virtual void start() = 0;

  /// Pushes a log entry into the backend.
  virtual void push(log_entry&& entry) = 0;

  /// Returns true when the backend has been started, otherwise false.
  virtual bool is_running() const = 0;
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_LOG_BACKEND_H
