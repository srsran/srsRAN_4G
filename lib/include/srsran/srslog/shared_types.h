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

#ifndef SRSLOG_SHARED_TYPES_H
#define SRSLOG_SHARED_TYPES_H

#include <functional>
#include <string>

namespace srslog {

/// Generic error handler callback.
using error_handler = std::function<void(const std::string&)>;

/// Backend priority levels.
enum class backend_priority {
  /// Default priority of the operating system.
  normal,
  /// Thread will be given a high priority.
  high,
  /// Thread will be given a very high priority.
  very_high
};

/// syslog log local types
enum class syslog_local_type {
  local0,
  local1,
  local2,
  local3,
  local4,
  local5,
  local6,
  local7,
};

} // namespace srslog

#endif // SRSLOG_SHARED_TYPES_H
