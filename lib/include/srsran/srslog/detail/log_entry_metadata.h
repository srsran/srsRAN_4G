/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSLOG_DETAIL_LOG_ENTRY_METADATA_H
#define SRSLOG_DETAIL_LOG_ENTRY_METADATA_H

#include "srsran/srslog/bundled/fmt/printf.h"
#include <chrono>

namespace srslog {

/// This type is used to store small strings without doing any memory allocation.
using small_str_buffer = fmt::basic_memory_buffer<char, 64>;

namespace detail {

/// This structure gives the user a way to log generic information as a context.
//: TODO: legacy struct, will get replaced by the new context framework.
struct log_context {
  /// Generic context value.
  uint32_t value;
  /// When true, the context value will be printed in the log entry.
  bool enabled;
};

/// Metadata fields carried for each log entry.
struct log_entry_metadata {
  std::chrono::high_resolution_clock::time_point      tp;
  log_context                                         context;
  const char*                                         fmtstring;
  fmt::dynamic_format_arg_store<fmt::printf_context>* store;
  std::string                                         log_name;
  char                                                log_tag;
  small_str_buffer                                    small_str;
  std::vector<uint8_t>                                hex_dump;
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_LOG_ENTRY_METADATA_H
