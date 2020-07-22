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

namespace srslog {

class sink;

namespace detail {

/// This structure packs all the required data required to create a log entry in
/// the backend.
struct log_entry {
  sink* s;
  std::string fmtstring;
  fmt::dynamic_format_arg_store<fmt::printf_context> store;
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_LOG_ENTRY_H
