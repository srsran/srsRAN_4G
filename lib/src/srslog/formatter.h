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

#ifndef SRSLOG_FORMATTER_H
#define SRSLOG_FORMATTER_H

#include "srslte/srslog/bundled/fmt/chrono.h"
#include "srslte/srslog/bundled/fmt/ranges.h"
#include "srslte/srslog/detail/log_entry.h"

namespace srslog {

namespace detail {

/// Formats into a hex dump a range of elements, storing the result in the input
/// buffer.
inline void format_hex_dump(const std::vector<uint8_t>& v,
                            fmt::memory_buffer& buffer)
{
  if (v.empty()) {
    return;
  }

  const size_t elements_per_line = 16;

  for (auto i = v.cbegin(), e = v.cend(); i != e;) {
    auto num_elements =
        std::min<size_t>(elements_per_line, std::distance(i, e));

    fmt::format_to(buffer,
                   "    {:04x}: {:02x}\n",
                   std::distance(v.cbegin(), i),
                   fmt::join(i, i + num_elements, " "));

    std::advance(i, num_elements);
  }
}

} // namespace detail

/// Formats to text all the fields of a log entry,
inline std::string format_log_entry_to_text(detail::log_entry&& entry)
{
  fmt::memory_buffer buffer;

  // Time stamp data preparation.
  std::tm current_time =
      fmt::gmtime(std::chrono::high_resolution_clock::to_time_t(entry.tp));
  auto us_fraction = std::chrono::duration_cast<std::chrono::microseconds>(
                         entry.tp.time_since_epoch())
                         .count() %
                     1000000u;
  fmt::format_to(buffer, "{:%H:%M:%S}.{:06} ", current_time, us_fraction);

  // Format optional fields if present.
  if (!entry.log_name.empty()) {
    fmt::format_to(buffer, "[{: <4.4}] ", entry.log_name);
  }
  if (entry.log_tag != '\0') {
    fmt::format_to(buffer, "[{}] ", entry.log_tag);
  }
  if (entry.context.enabled) {
    fmt::format_to(buffer, "[{:5}] ", entry.context.value);
  }

  // Message formatting.
  fmt::format_to(
      buffer, "{}\n", fmt::vsprintf(entry.fmtstring, std::move(entry.store)));

  // Optional hex dump formatting.
  detail::format_hex_dump(entry.hex_dump, buffer);

  return fmt::to_string(buffer);
}

} // namespace srslog

#endif // SRSLOG_FORMATTER_H
