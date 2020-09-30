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

#ifndef SRSLOG_DETAIL_LOG_BACKEND_H
#define SRSLOG_DETAIL_LOG_BACKEND_H

#include "srslte/srslog/detail/log_entry.h"

namespace srslog {

namespace detail {

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
  virtual void push(detail::log_entry&& entry) = 0;

  /// Returns true when the backend has been started, otherwise false.
  virtual bool is_running() const = 0;
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_LOG_BACKEND_H
