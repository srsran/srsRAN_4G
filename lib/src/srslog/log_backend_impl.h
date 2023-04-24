/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLOG_LOG_BACKEND_IMPL_H
#define SRSLOG_LOG_BACKEND_IMPL_H

#include "backend_worker.h"
#include "srsran/srslog/detail/log_backend.h"

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

  void start(backend_priority priority = backend_priority::normal) override { worker.start(priority); }

  bool push(detail::log_entry&& entry) override
  {
    auto* arg_store = entry.metadata.store;
    if (!queue.push(std::move(entry))) {
      arg_pool.dealloc(arg_store);
      return false;
    }
    return true;
  }

  fmt::dynamic_format_arg_store<fmt::printf_context>* alloc_arg_store() override { return arg_pool.alloc(); }

  bool is_running() const override { return worker.is_running(); }

  /// Installs the specified error handler into the backend worker.
  void set_error_handler(error_handler err_handler) { worker.set_error_handler(std::move(err_handler)); }

  /// Stops the backend worker thread.
  void stop() { worker.stop(); }

private:
  detail::work_queue<detail::log_entry> queue;
  detail::dyn_arg_store_pool            arg_pool;
  backend_worker                        worker{queue, arg_pool};
};

} // namespace srslog

#endif // SRSLOG_LOG_BACKEND_IMPL_H
