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

#ifndef SRSLOG_DETAIL_LOG_ENTRY_H
#define SRSLOG_DETAIL_LOG_ENTRY_H

#include "srslte/srslog/bundled/fmt/printf.h"
#include "srslte/srslog/detail/support/thread_utils.h"
#include <chrono>

namespace srslog {

class sink;

namespace detail {

/// This structure gives the user a way to log generic information as a context.
struct log_context {
  /// Generic contxt value.
  uint32_t value;
  /// When true, the context value will be printed in the log entry.
  bool enabled;
};

/// This command flushes all the messages pending in the backend.
struct flush_backend_cmd {
  shared_variable<bool>& completion_flag;
  std::vector<sink*> sinks;
};

/// This structure packs all the required data required to create a log entry in
/// the backend.
//:TODO: provide proper command objects when we have custom formatting.
struct log_entry {
  sink* s;
  std::chrono::high_resolution_clock::time_point tp;
  log_context context;
  std::string fmtstring;
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
  std::string log_name;
  char log_tag;
  std::vector<uint8_t> hex_dump;
  std::unique_ptr<flush_backend_cmd> flush_cmd;
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_LOG_ENTRY_H
