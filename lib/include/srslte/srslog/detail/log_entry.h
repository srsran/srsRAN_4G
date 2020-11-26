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
