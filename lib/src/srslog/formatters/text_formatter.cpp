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

#include "text_formatter.h"
#include "srslte/srslog/bundled/fmt/chrono.h"
#include "srslte/srslog/detail/log_entry_metadata.h"

using namespace srslog;

std::unique_ptr<log_formatter> text_formatter::clone() const
{
  return std::unique_ptr<log_formatter>(new text_formatter(*this));
}

/// Formats into a hex dump a range of elements, storing the result in the input
/// buffer.
static void format_hex_dump(const std::vector<uint8_t>& v,
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

/// Format the log metadata into the input buffer.
static void format_metadata(const detail::log_entry_metadata& metadata,
                            fmt::memory_buffer& buffer)
{
  // Time stamp data preparation.
  std::tm current_time =
      fmt::gmtime(std::chrono::high_resolution_clock::to_time_t(metadata.tp));
  auto us_fraction = std::chrono::duration_cast<std::chrono::microseconds>(
                         metadata.tp.time_since_epoch())
                         .count() %
                     1000000u;
  fmt::format_to(buffer, "{:%H:%M:%S}.{:06} ", current_time, us_fraction);

  // Format optional fields if present.
  if (!metadata.log_name.empty()) {
    fmt::format_to(buffer, "[{: <4.4}] ", metadata.log_name);
  }
  if (metadata.log_tag != '\0') {
    fmt::format_to(buffer, "[{}] ", metadata.log_tag);
  }
  if (metadata.context.enabled) {
    fmt::format_to(buffer, "[{:5}] ", metadata.context.value);
  }
}

void text_formatter::format(detail::log_entry_metadata&& metadata,
                            fmt::memory_buffer& buffer)
{
  // Prefix first.
  format_metadata(metadata, buffer);

  // Message formatting.
  fmt::format_to(buffer,
                 "{}\n",
                 fmt::vsprintf(metadata.fmtstring, std::move(metadata.store)));

  // Optional hex dump formatting.
  format_hex_dump(metadata.hex_dump, buffer);
}

void text_formatter::format_context_begin(const detail::log_entry_metadata& md,
                                          const std::string& ctx_name,
                                          unsigned size,
                                          fmt::memory_buffer& buffer)
{
  do_one_line_ctx_format = !md.fmtstring.empty();

  format_metadata(md, buffer);
  if (do_one_line_ctx_format) {
    fmt::format_to(buffer, "[");
    return;
  }
  fmt::format_to(buffer, "Context dump for \"{}\"\n", ctx_name);
}

void text_formatter::format_context_end(const detail::log_entry_metadata& md,
                                        const std::string& ctx_name,
                                        fmt::memory_buffer& buffer)
{
  if (do_one_line_ctx_format) {
    fmt::format_to(buffer, "]: {}\n", fmt::vsprintf(md.fmtstring, md.store));
    return;
  }
}

void text_formatter::format_metric_set_begin(const std::string& set_name,
                                             unsigned size,
                                             unsigned level,
                                             fmt::memory_buffer& buffer)
{
  /*if (do_one_line_ctx_format) {
    fmt::format_to(buffer, "{}", is_first ? "[" : " [");
    return;
  }
  fmt::format_to(buffer, "  {}\n", set_name);*/
}

void text_formatter::format_metric_set_end(const std::string& set_name,
                                           unsigned level,
                                           fmt::memory_buffer& buffer)
{
  if (do_one_line_ctx_format) {
    fmt::format_to(buffer, "]");
    return;
  }
}

void text_formatter::format_metric(const std::string& metric_name,
                                   const std::string& metric_value,
                                   const std::string& metric_units,
                                   metric_kind kind,
                                   unsigned level,
                                   fmt::memory_buffer& buffer)
{
  //:TODO: re-enable
  /*if (do_one_line_ctx_format) {
    fmt::format_to(buffer,
                   "{}{}_{}: {}{}{}",
                   ctx.is_first_metric ? "" : ", ",
                   ctx.set_name,
                   ctx.metric_name,
                   ctx.metric_value,
                   ctx.metric_units.empty() ? "" : " ",
                   ctx.metric_units);
    return;
  }
  fmt::format_to(buffer,
                 "    {}: {}{}{}\n",
                 ctx.metric_name,
                 ctx.metric_value,
                 ctx.metric_units.empty() ? "" : " ",
                 ctx.metric_units);*/
}
