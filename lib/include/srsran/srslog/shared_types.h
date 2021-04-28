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

#ifndef SRSLOG_SHARED_TYPES_H
#define SRSLOG_SHARED_TYPES_H

#include <functional>

namespace srslog {

/// Generic error handler callback.
using error_handler = std::function<void(const std::string&)>;

/// Backend priority levels.
enum class backend_priority
{
  /// Default priority of the operating system.
  normal,
  /// Thread will be given a high priority.
  high,
  /// Thread will be given a very high priority.
  very_high
};

} // namespace srslog

#endif // SRSLOG_SHARED_TYPES_H
