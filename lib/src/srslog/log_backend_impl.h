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

#ifndef SRSLOG_LOG_BACKEND_IMPL_H
#define SRSLOG_LOG_BACKEND_IMPL_H

#include "backend_worker.h"
#include "srslte/srslog/detail/log_backend.h"

namespace srslog {

/// This class implements the log backend interface. It internally manages a
/// worker thread to process incoming log entries.
/// NOTE: Thread safe class.
class log_backend_impl : public detail::log_backend
{
public:
  log_backend_impl() = default;

  log_backend_impl(const log_backend_impl& other) = delete;
  log_backend_impl& operator=(const log_backend_impl& other) = delete;

  void start() override { worker.start(); }

  bool push(detail::log_entry&& entry) override
  {
    return queue.push(std::move(entry));
  }

  bool is_running() const override { return worker.is_running(); }

  /// Installs the specified error handler into the backend worker.
  void set_error_handler(error_handler err_handler)
  {
    worker.set_error_handler(std::move(err_handler));
  }

  /// Stops the backend worker thread.
  void stop() { worker.stop(); }

private:
  detail::work_queue<detail::log_entry> queue;
  backend_worker worker{queue};
};

} // namespace srslog

#endif // SRSLOG_LOG_BACKEND_IMPL_H
