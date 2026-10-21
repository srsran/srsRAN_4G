// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSLOG_DETAIL_LOG_ENTRY_METADATA_H
#define SRSLOG_DETAIL_LOG_ENTRY_METADATA_H

#include "srsran/srslog/bundled/fmt/printf.h"
#include <chrono>

namespace srslog {

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
  std::vector<uint8_t>                                hex_dump;
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_LOG_ENTRY_METADATA_H
