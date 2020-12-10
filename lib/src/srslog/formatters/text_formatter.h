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
