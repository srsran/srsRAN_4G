/**
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

#ifndef SRSLOG_TEXT_FORMATTER_H
#define SRSLOG_TEXT_FORMATTER_H

#include "srslte/srslog/formatter.h"

namespace srslog {

/// Plain text formatter implementation class.
//:TODO: this class needs refactoring to be compatible with multiple nesting of
// metrics.
class text_formatter : public log_formatter
{
public:
  std::unique_ptr<log_formatter> clone() const override;

  void format(detail::log_entry_metadata&& metadata,
              fmt::memory_buffer& buffer) override;

private:
  void format_context_begin(const detail::log_entry_metadata& md,
                            const std::string& ctx_name,
                            unsigned size,
                            fmt::memory_buffer& buffer) override;

  void format_context_end(const detail::log_entry_metadata& md,
                          const std::string& ctx_name,
                          fmt::memory_buffer& buffer) override;

  void format_metric_set_begin(const std::string& set_name,
                               unsigned size,
                               unsigned level,
                               fmt::memory_buffer& buffer) override;

  void format_metric_set_end(const std::string& set_name,
                             unsigned level,
                             fmt::memory_buffer& buffer) override;

  void format_list_begin(const std::string& list_name,
                         unsigned size,
                         unsigned level,
                         fmt::memory_buffer& buffer) override
  {
    //:TODO: implement me
  }

  void format_list_end(const std::string& list_name,
                       unsigned level,
                       fmt::memory_buffer& buffer) override
  {
    //:TODO: implement me
  }

  void format_metric(const std::string& metric_name,
                     const std::string& metric_value,
                     const std::string& metric_units,
                     metric_kind kind,
                     unsigned level,
                     fmt::memory_buffer& buffer) override;

private:
  /// Flags that the formatting should take place into a single line.
  bool do_one_line_ctx_format = false;
};

} // namespace srslog

#endif // SRSLOG_TEXT_FORMATTER_H
