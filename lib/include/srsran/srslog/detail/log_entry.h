// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSLOG_DETAIL_LOG_ENTRY_H
#define SRSLOG_DETAIL_LOG_ENTRY_H

#include "srsran/srslog/detail/log_entry_metadata.h"
#include "srsran/srslog/detail/support/thread_utils.h"

namespace srslog {

class sink;

namespace detail {

/// This command flushes all the messages pending in the backend.
struct flush_backend_cmd {
  shared_variable<bool>& completion_flag;
  std::vector<sink*>     sinks;
};

/// This structure packs all the required data required to create a log entry in
/// the backend.
//: TODO: replace this object using a real command pattern when we have a raw
// memory queue for passing entries.
struct log_entry {
  sink*                                                                          s;
  std::function<void(log_entry_metadata&& metadata, fmt::memory_buffer& buffer)> format_func;
  log_entry_metadata                                                             metadata;
  std::unique_ptr<flush_backend_cmd>                                             flush_cmd;
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_LOG_ENTRY_H
