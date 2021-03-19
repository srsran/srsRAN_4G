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

#include "json_formatter.h"
#include "srsran/srslog/detail/log_entry_metadata.h"

using namespace srslog;

std::unique_ptr<log_formatter> json_formatter::clone() const
{
  return std::unique_ptr<log_formatter>(new json_formatter);
}

void json_formatter::format(detail::log_entry_metadata&& metadata,
                            fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer,
                 "{{\n"
                 "  \"log_entry\": \"{}\"",
                 fmt::vsprintf(metadata.fmtstring, std::move(metadata.store)));

  if (!metadata.hex_dump.empty()) {
    fmt::format_to(
        buffer,
        ",\n  \"hex_dump\": \"{:02x}\"",
        fmt::join(metadata.hex_dump.cbegin(), metadata.hex_dump.cend(), " "));
  }

  fmt::format_to(buffer, "\n}}\n");
}

void json_formatter::format_context_begin(const detail::log_entry_metadata& md,
                                          const std::string& ctx_name,
                                          unsigned size,
                                          fmt::memory_buffer& buffer)
{
  assert(scope_stack.empty() && "Stack should be empty");
  assert(nest_level == 0 && "Nesting level should be 0");

  fmt::format_to(buffer, "{{\n");
  push_scope(size);

  if (md.fmtstring) {
    fmt::format_to(buffer,
                   "  \"log_entry\": \"{}\",\n",
                   fmt::vsprintf(md.fmtstring, std::move(md.store)));
  }
}

void json_formatter::format_context_end(const detail::log_entry_metadata& md,
                                        const std::string& ctx_name,
                                        fmt::memory_buffer& buffer)
{
  pop_scope();
  fmt::format_to(buffer, "}}\n");

  assert(scope_stack.empty() && "Stack should be empty");
  assert(nest_level == 0 && "Nesting level should be 0");
}

void json_formatter::format_metric_set_begin(const std::string& set_name,
                                             unsigned size,
                                             unsigned level,
                                             fmt::memory_buffer& buffer)
{
  // Arrays in JSON require an additional nesting level before inserting the
  // object.
  // array: [
  //   {
  //     "obj: {}
  //   }
  // ]
  if (in_list_scope()) {
    fmt::format_to(buffer, "{: <{}}{{\n", ' ', indents(level));
    increment_nest_level();
  }

  consume_element();

  fmt::format_to(buffer, "{: <{}}\"{}\": {{\n", ' ', indents(level), set_name);
  push_scope(size);
}

void json_formatter::format_metric_set_end(const std::string& set_name,
                                           unsigned level,
                                           fmt::memory_buffer& buffer)
{
  pop_scope();
  fmt::format_to(buffer,
                 "{: <{}}}}{}\n",
                 ' ',
                 indents(level),
                 needs_comma() && !in_list_scope() ? "," : "");

  if (in_list_scope()) {
    decrement_nest_level();
    fmt::format_to(
        buffer, "{: <{}}}}{}\n", ' ', indents(level), needs_comma() ? "," : "");
  }
}

void json_formatter::format_metric(const std::string& metric_name,
                                   const std::string& metric_value,
                                   const std::string& metric_units,
                                   metric_kind kind,
                                   unsigned level,
                                   fmt::memory_buffer& buffer)
{
  consume_element();

  fmt::format_to(buffer,
                 "{: <{}}\"{}\": {}{}{}{}\n",
                 ' ',
                 indents(level),
                 metric_name,
                 kind == metric_kind::string ? "\"" : "",
                 metric_value,
                 kind == metric_kind::string ? "\"" : "",
                 needs_comma() ? "," : "");
}

void json_formatter::format_list_begin(const std::string& list_name,
                                       unsigned size,
                                       unsigned level,
                                       fmt::memory_buffer& buffer)
{
  consume_element();

  fmt::format_to(buffer, "{: <{}}\"{}\": [\n", ' ', indents(level), list_name);
  push_list_scope(size);
}

void json_formatter::format_list_end(const std::string& list_name,
                                     unsigned level,
                                     fmt::memory_buffer& buffer)
{
  pop_scope();
  fmt::format_to(
      buffer, "{: <{}}]{}\n", ' ', indents(level), needs_comma() ? "," : "");
}
