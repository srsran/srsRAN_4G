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
