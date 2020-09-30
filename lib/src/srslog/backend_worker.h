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

#ifndef SRSLOG_BACKEND_WORKER_H
#define SRSLOG_BACKEND_WORKER_H

#include "srslte/srslog/detail/log_entry.h"
#include "srslte/srslog/detail/support/work_queue.h"
#include "srslte/srslog/shared_types.h"
#include <mutex>
#include <thread>

namespace srslog {

/// The backend worker runs in a secondary thread a routine that endlessly pops
/// log entries from a work queue and dispatches them to the selected sinks.
class backend_worker
{
  /// This period defines the maximum time the worker will sleep while waiting
  /// for new entries. This is required to check the termination variable
  /// periodically.
  static constexpr unsigned sleep_period_ms = 500;

public:
  explicit backend_worker(detail::work_queue<detail::log_entry>& queue) :
    queue(queue), running_flag(false)
  {}

  backend_worker(const backend_worker&) = delete;
  backend_worker& operator=(const backend_worker&) = delete;

  ~backend_worker() { stop(); }

  /// Starts the backend worker thread. After returning from this function the
  /// secondary thread is ensured to be running. Calling this function more than
  /// once has no effect.
  void start();

  /// Stops the backend worker thread if it is running, otherwise the call has
  /// no effect. After returning from this function the secondary thread is
  /// ensured to have terminated. Calling this function more than once has no
  /// effect.
  void stop();

  /// Returns true if the worker thread is currently running, otherwise
  /// returns false.
  bool is_running() const { return running_flag; }

  /// Uses the specified error handler to receive error notifications. Calls to
  /// this method when the worker is running will get ignored.
  void set_error_handler(error_handler new_err_handler)
  {
    // Ignore new handlers when the worker is running.
    if (is_running()) {
      return;
    }

    // Install a dummy callback if the input one is empty. This avoids having to
    // check for null at each call site.
    if (!new_err_handler) {
      err_handler = [](const std::string&) {};
      return;
    }

    err_handler = std::move(new_err_handler);
  }

private:
  /// Creates the worker thread.
  /// NOTE: This function should be only called once.
  void create_worker();

  /// Entry function used by the secondary thread.
  void do_work();

  /// Processes the log entry.
  void process_log_entry(detail::log_entry&& entry);

  /// Processes outstanding entries in the queue until it gets empty.
  void process_outstanding_entries();

  /// Checks the current size of the queue reporting an error message if it is
  /// about to reach its maximum capacity.
  /// Error message is only reported once to avoid spamming.
  void report_queue_on_full_once()
  {
    if (queue.is_almost_full()) {
      err_handler(
          fmt::format("The backend queue size is about to reach its maximum "
                      "capacity of {} elements, new log entries will get "
                      "discarded.\nConsider increasing the queue capacity.",
                      queue.get_capacity()));
      err_handler = [](const std::string&) {};
    }
  }

private:
  detail::work_queue<detail::log_entry>& queue;
  detail::shared_variable<bool> running_flag;
  error_handler err_handler = [](const std::string& error) {
    fmt::print(stderr, "srsLog error - {}\n", error);
  };
  std::once_flag start_once_flag;
  std::thread worker_thread;
};

} // namespace srslog

#endif // SRSLOG_BACKEND_WORKER_H
