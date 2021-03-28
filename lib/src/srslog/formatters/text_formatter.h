/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLOG_TEXT_FORMATTER_H
#define SRSLOG_TEXT_FORMATTER_H

#include "srsran/srslog/formatter.h"

namespace srslog {

/// Plain text formatter implementation class.
class text_formatter : public log_formatter
{
public:
  text_formatter() { scope_stack.reserve(16); }

  std::unique_ptr<log_formatter> clone() const override;

  void format(detail::log_entry_metadata&& metadata, fmt::memory_buffer& buffer) override;

private:
  void format_context_begin(const detail::log_entry_metadata& md,
                            fmt::string_view                  ctx_name,
                            unsigned                          size,
                            fmt::memory_buffer&               buffer) override;

  void format_context_end(const detail::log_entry_metadata& md,
                          fmt::string_view                  ctx_name,
                          fmt::memory_buffer&               buffer) override;

  void format_metric_set_begin(fmt::string_view    set_name,
                               unsigned            size,
                               unsigned            level,
                               fmt::memory_buffer& buffer) override;

  void format_metric_set_end(fmt::string_view set_name, unsigned level, fmt::memory_buffer& buffer) override;

  void
  format_list_begin(fmt::string_view list_name, unsigned size, unsigned level, fmt::memory_buffer& buffer) override;

  void format_list_end(fmt::string_view list_name, unsigned level, fmt::memory_buffer& buffer) override {}

  void format_metric(fmt::string_view    metric_name,
                     fmt::string_view    metric_value,
                     fmt::string_view    metric_units,
                     metric_kind         kind,
                     unsigned            level,
                     fmt::memory_buffer& buffer) override;

  /// Returns the set name of current scope.
  const std::string& get_current_set_name() const
  {
    assert(!scope_stack.empty() && "Empty scope stack");
    return scope_stack.back().set_name;
  }

  /// Consumes an element in the current scope.
  void consume_element()
  {
    assert(!scope_stack.empty() && "Consuming element in void scope");
    assert(scope_stack.back().size && "No more elements to consume");
    --scope_stack.back().size;
  }

  /// Returns true if the current element needs a comma.
  bool needs_comma() const
  {
    assert(!scope_stack.empty() && "No scope exists");
    return scope_stack.back().size;
  }

  /// Returns the number of indentations required for the input nesting level.
  unsigned get_indents(unsigned level) const { return level * 2; }

private:
  /// Keeps track of some state required for formatting.
  struct scope {
    scope(unsigned size, std::string set_name) : size(size), set_name(std::move(set_name)) {}
    /// Number of elements this scope holds.
    unsigned size;
    /// Set name in this scope.
    std::string set_name;
  };

private:
  /// Flags that the formatting should take place into a single line.
  bool               do_one_line_ctx_format = false;
  std::vector<scope> scope_stack;
};

} // namespace srslog

#endif // SRSLOG_TEXT_FORMATTER_H
