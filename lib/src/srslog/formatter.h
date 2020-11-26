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
