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

#ifndef SRSLOG_JSON_FORMATTER_H
#define SRSLOG_JSON_FORMATTER_H

#include "srsran/srslog/formatter.h"

namespace srslog {

/// JSON formatter class implementation.
/// Formats each log entry and context into its own JSON object making the
/// formatter stateless so that new entries do not depend on the state of
/// previous ones. The output is ready for JSON streaming following the
/// "Concatenated JSON" style.
class json_formatter : public log_formatter
{
public:
  json_formatter() { scope_stack.reserve(16); }

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

  void format_list_end(fmt::string_view list_name, unsigned level, fmt::memory_buffer& buffer) override;

  void format_metric(fmt::string_view    metric_name,
                     fmt::string_view    metric_value,
                     fmt::string_view    metric_units,
                     metric_kind         kind,
                     unsigned            level,
                     fmt::memory_buffer& buffer) override;

  /// Pushes a new entry in the scope stack.
  void push_scope(unsigned size) { scope_stack.emplace_back(size, false); }

  /// Pushes a new list entry in the scope stack.
  void push_list_scope(unsigned size) { scope_stack.emplace_back(size, true); }

  /// Pops the topmost entry in the scope stack.
  void pop_scope()
  {
    assert(!scope_stack.empty() && "Popping scope in empty stack");
    scope_stack.pop_back();
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

  /// Returns true if the current scope is a list.
  bool in_list_scope() const
  {
    assert(!scope_stack.empty() && "No scope exists");
    return scope_stack.back().inside_list;
  }

  /// Increments the nesting level by one.
  void increment_nest_level() { ++nest_level; }

  /// Decrements the nesting level by one.
  void decrement_nest_level()
  {
    assert(nest_level && "Expected the nesting level to greater than 0");
    --nest_level;
  }

  /// Return the number of space chars to indent the specified level.
  unsigned indents(unsigned level) const { return (nest_level + level) * 2; }

private:
  /// Keeps track of some information about a JSON scope.
  struct scope {
    scope(unsigned size, bool inside_list) : size(size), inside_list(inside_list) {}
    /// Number of elements this scope holds.
    unsigned size;
    /// If true, indicates this scope belongs to a list.
    const bool inside_list;
  };

private:
  unsigned           nest_level = 0;
  std::vector<scope> scope_stack;
};

} // namespace srslog

#endif // SRSLOG_JSON_FORMATTER_H
