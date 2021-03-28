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

#ifndef SRSLOG_FORMATTER_H
#define SRSLOG_FORMATTER_H

#include "srsran/srslog/bundled/fmt/format.h"
#include "srsran/srslog/context.h"

namespace srslog {

namespace detail {
struct log_entry_metadata;
}

/// The generic metric value formatter.
template <typename T>
struct metric_value_formatter {
  metric_value_formatter() = delete;
  /// All specializations should implement the following method with signature:
  /// template <typename T>
  /// void format(const T& v, fmt::memory_buffer& buffer)
};

/// Default metric value formatter. Users that want to override this behaviour
/// should add an specialization of the metric they want to customize.
template <typename Ty, typename Name, typename Units>
struct metric_value_formatter<metric<Ty, Name, Units> > {
  template <typename T>
  void format(const T& v, fmt::memory_buffer& buffer)
  {
    fmt::format_to(buffer, "{}", v);
  }
};

/// This is the base class that provides a common framework to format log
/// entries to different kinds of formats. User should implement two different
/// kinds of formats:
///  a) Basic log entry formatting.
///  b) Generic context formatting.
///
/// For context formatting, callbacks are provided so that derived classes
/// handle specific formatting rules.
class log_formatter
{
public:
  virtual ~log_formatter() = default;

  /// Returns a copy of the formatter.
  virtual std::unique_ptr<log_formatter> clone() const = 0;

  /// Formats the log entry into the input buffer.
  virtual void format(detail::log_entry_metadata&& metadata, fmt::memory_buffer& buffer) = 0;

  /// Formats the context and log entry into the input buffer.
  template <typename... Ts>
  void format_ctx(const srslog::context<Ts...>& ctx, detail::log_entry_metadata&& metadata, fmt::memory_buffer& buffer)
  {
    format_context_begin(metadata, ctx.name(), sizeof...(Ts), buffer);
    iterate_tuple(ctx.contents(), 1, buffer, detail::make_index_sequence<sizeof...(Ts)>{});
    format_context_end(metadata, ctx.name(), buffer);
  }

private:
  /// Processes all elements in a tuple.
  template <typename... Ts, std::size_t... Is>
  void
  iterate_tuple(const std::tuple<Ts...>& t, unsigned level, fmt::memory_buffer& buffer, detail::index_sequence<Is...>)
  {
    (void)std::initializer_list<int>{(process_element(std::get<Is>(t), level, buffer), 0)...};
  }

  /// Processes the input metric set.
  template <typename Name, typename... Ts>
  void process_element(const metric_set<Name, Ts...>& ms, unsigned level, fmt::memory_buffer& buffer)
  {
    format_metric_set_begin(ms.name(), sizeof...(Ts), level, buffer);
    iterate_tuple(ms.contents(), level + 1, buffer, detail::make_index_sequence<sizeof...(Ts)>{});
    format_metric_set_end(ms.name(), level, buffer);
  }

  /// Processes the input metric list.
  template <typename Name, typename T>
  void process_element(const metric_list<Name, T>& list, unsigned level, fmt::memory_buffer& buffer)
  {
    format_list_begin(list.name(), list.size(), level, buffer);
    for (const auto& elem : list) {
      process_element(elem, level + 1, buffer);
    }
    format_list_end(list.name(), level, buffer);
  }

  /// Processes the input metric.
  template <typename Ty, typename Name, typename Units>
  void process_element(const metric<Ty, Name, Units>& t, unsigned level, fmt::memory_buffer& buffer)
  {
    fmt::memory_buffer value;
    metric_value_formatter<typename std::decay<decltype(t)>::type>{}.format(t.value, value);
    value.push_back('\0');

    format_metric(t.name(), value.data(), t.units(), t.kind(), level, buffer);
  }

private:
  /// Derived classes should implement the following callbacks to format metric
  /// objects. Each callback is invoked at a different place of the formatting
  /// algorithm.

  /// This callback gets called at the beginning of the context formatting
  /// algorithm.
  virtual void format_context_begin(const detail::log_entry_metadata& md,
                                    fmt::string_view                  ctx_name,
                                    unsigned                          size,
                                    fmt::memory_buffer&               buffer) = 0;

  /// This callback gets called at the end of the context formatting algorithm.
  virtual void
  format_context_end(const detail::log_entry_metadata& md, fmt::string_view ctx_name, fmt::memory_buffer& buffer) = 0;

  /// This callback gets called at the beginning of a metric set formatting
  /// procedure.
  virtual void
  format_metric_set_begin(fmt::string_view set_name, unsigned size, unsigned level, fmt::memory_buffer& buffer) = 0;

  /// This callback gets called at the beginning of a metric set formatting end.
  virtual void format_metric_set_end(fmt::string_view set_name, unsigned level, fmt::memory_buffer& buffer) = 0;

  /// This callback gets called at the beginning of a metric list formatting
  /// procedure.
  virtual void
  format_list_begin(fmt::string_view list_name, unsigned size, unsigned level, fmt::memory_buffer& buffer) = 0;

  /// This callback gets called at the end of a metric list formatting
  /// procedure.
  virtual void format_list_end(fmt::string_view list_name, unsigned level, fmt::memory_buffer& buffer) = 0;

  /// This callback gets called for each metric.
  virtual void format_metric(fmt::string_view    metric_name,
                             fmt::string_view    metric_value,
                             fmt::string_view    metric_units,
                             metric_kind         kind,
                             unsigned            level,
                             fmt::memory_buffer& buffer) = 0;
};

} // namespace srslog

#endif // SRSLOG_FORMATTER_H
