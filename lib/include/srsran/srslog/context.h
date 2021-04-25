/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLOG_CONTEXT_H
#define SRSLOG_CONTEXT_H

#include "srsran/srslog/detail/support/tmp_utils.h"
#include <cassert>
#include <string>

namespace srslog {

/// Metric formatting kinds for textual conversion.
enum class metric_kind {
  numeric, /// Metric represents a numeric value.
  string   /// Metric represents a string.
};

namespace detail {

/// This metrics container class is a wrapper to simplify access to the elements
/// of the underlying tuple that stores metrics and metric sets.
template <typename... Ts>
struct metrics_container {
  /// Writes the arg value to metric T.
  template <typename T, typename Arg>
  void write(Arg&& arg)
  {
    constexpr std::size_t index    = detail::get_type_index_in_tuple<T, Ts...>();
    std::get<index>(metrics).value = std::forward<Arg>(arg);
  }

  /// Returns the value of metric T.
  template <typename T>
  auto read() const -> const decltype(T::value) &
  {
    constexpr std::size_t index = detail::get_type_index_in_tuple<T, Ts...>();
    return std::get<index>(metrics).value;
  }

  /// Returns the element of type T.
  template <typename T>
  T& get()
  {
    constexpr std::size_t index = detail::get_type_index_in_tuple<T, Ts...>();
    return std::get<index>(metrics);
  }

  /// Returns the element of type T.
  template <typename T>
  const T& get() const
  {
    constexpr std::size_t index = detail::get_type_index_in_tuple<T, Ts...>();
    return std::get<index>(metrics);
  }

  /// Returns the element in the specified index of list T.
  /// NOTE: T must have implemented the T operator.
  template <typename T>
  auto at(std::size_t i) -> typename T::value_type&
  {
    constexpr std::size_t index = detail::get_type_index_in_tuple<T, Ts...>();
    auto&                 elem  = std::get<index>(metrics);
    assert(i < elem.size() && "Invalid index");
    return elem[i];
  }

  /// Returns the element in the specified index of list T.
  /// NOTE: T must have implemented the T operator.
  template <typename T>
  auto at(std::size_t i) const -> const typename T::value_type&
  {
    constexpr std::size_t index = detail::get_type_index_in_tuple<T, Ts...>();
    const auto&           elem  = std::get<index>(metrics);
    assert(i < elem.size() && "Invalid index");
    return elem[i];
  }

  /// Returns the raw contents of the metric set as a tuple.
  const std::tuple<Ts...>& contents() const { return metrics; }

private:
  std::tuple<Ts...> metrics;
};

} // namespace detail

/// A generic list to store metric sets of the same type.
template <typename Name, typename T>
struct metric_list : public T {
  /// Returns the name of the list.
  static const char* name() { return Name::name(); }
};

/// Template specializations of this struct allow configuring what formatting
/// kind should be used for a concrete metric.
/// By default treat all metrics as strings.
template <typename T, typename = void>
struct metric_kind_selector {
  static const metric_kind kind = metric_kind::string;
};

/// A metric is the most basic object that composes a context. It is generally
/// used to represent any kind of state of a program.
/// It stores a value of type T associated with a name and the units.
template <typename Ty, typename Name, typename Units>
struct metric {
  /// Value of the metric.
  Ty value{};

  /// Returns the name of the metric.
  static const char* name() { return Name::name(); }

  /// Returns the units of the metric.
  static const char* units() { return Units::units(); }

  /// Returns the formatting kind of the metric.
  static metric_kind kind() { return metric_kind_selector<metric<Ty, Name, Units> >::kind; }
};

/// Template specialization that tags metrics with arithmetic values (integers
/// and floating point) as numeric.
template <typename Ty, typename Name, typename Units>
struct metric_kind_selector<metric<Ty, Name, Units>, typename std::enable_if<std::is_arithmetic<Ty>::value>::type> {
  static const metric_kind kind = metric_kind::numeric;
};

/// A metric set is a group of metrics that share a logical relation. Allows
/// storing and mixing other metric sets and metrics for building complex
/// structures.
template <typename Name, typename... Ts>
struct metric_set : public detail::metrics_container<Ts...> {
  /// Name of the metric set.
  static const char* name() { return Name::name(); }
};

/// A context captures the state of different parts of a program grouping metric
/// sets. It is the root element of the metrics structure and allows mixing and
/// storing other metric sets and metrics.
template <typename... Ts>
struct context : public detail::metrics_container<Ts...> {
  explicit context(std::string n) : name_str(std::move(n)) {}

  /// Name of the context.
  const std::string& name() const { return name_str; }

private:
  const std::string name_str;
};

namespace detail {

/// Builds a metric set type using a list of metric, metric sets or list types.
/// eg: using my_metric_t = srslog::build_metric_set_type<m1_t, set1_t, m2_t>;
/// NOTE: Adding duplicated types into the list is not allowed.
template <typename Name, typename... Ts>
using build_metric_set_type = metric_set<Name, typename std::decay<Ts>::type...>;

} // namespace detail

/// Builds a context type using a list of metric set types.
/// eg: using my_context_t = srslog::build_context_type<set1_t, set2_t>;
/// NOTE: Adding duplicated types into the list is not allowed.
template <typename... Ts>
using build_context_type = context<typename std::decay<Ts>::type...>;

/// This macro defines a new metric type using the following attributes:
///   a) name: encoded as a string.
///   b) Metric type: type identifier to create objects for this metric.
///   c) Value type: type of the underlying metric value.
///   d) Units: encoded as a string, leave as empty string for no units.
///
/// The following example declares a metric with the following attributes:
///   a) metric type: my_metric_t
///   b) metric value type: float
///   c) units: MB/s
///   d) name: Throughput
/// DECLARE_METRIC("Throughput", my_metric_t, float, "MB/s");
#define DECLARE_METRIC(_name_rep, _type, _value_type, _units)                                                          \
  namespace metric_info {                                                                                              \
  struct _type##__units {                                                                                              \
    static const char* units() { return _units; }                                                                      \
  };                                                                                                                   \
  struct _type##__name_rep {                                                                                           \
    static const char* name() { return _name_rep; }                                                                    \
  };                                                                                                                   \
  }                                                                                                                    \
  using _type = srslog::                                                                                               \
      metric<typename std::decay<_value_type>::type, metric_info::_type##__name_rep, metric_info::_type##__units>

/// This macro defines a new metric set type using the following attributes:
///   a) name: encoded as a string.
///   b) Metric set type: type identifier to create objects for this metric set.
///   c) Type list: list of types this set will hold (other sets, metrics,
///   lists).
///
/// The following example declares a metric set of three elements (two metrics
/// and one set) with the following attributes:
///   a) metric type: my_set_t
///   b) name: my_set
///   b) type list: metric1_t, metric2_t, set2_t
/// DECLARE_METRIC_SET("my_set", my_set_t, metric1_t, metric2_t, set2_t);
#define DECLARE_METRIC_SET(_name_rep, _type, ...)                                                                      \
  namespace metric_set_info {                                                                                          \
  struct _type##__name_rep {                                                                                           \
    static const char* name() { return _name_rep; }                                                                    \
  };                                                                                                                   \
  }                                                                                                                    \
  using _type = srslog::detail::build_metric_set_type<metric_set_info::_type##__name_rep, __VA_ARGS__>

/// This macro defines a list of metric sets of the same type:
///   a) name: encoded as a string.
///   b) List type: type identifier to create objects for this list.
///   c) Underlying type: type of the underlying list (vector, array, ...).
///
/// The following example declares a list of metrics sets of type set1_t with
/// the following attributes:
///   a) list type: my_list_t
///   b) name: my_list
///   b) underlying type: std::vector<set1_t>
/// DECLARE_METRIC_LIST("my_list", my_list_t, std::vector<set1_t>);
#define DECLARE_METRIC_LIST(_name_rep, _type, _list_type)                                                              \
  namespace list_info {                                                                                                \
  struct _type##__name_rep {                                                                                           \
    static const char* name() { return _name_rep; }                                                                    \
  };                                                                                                                   \
  }                                                                                                                    \
  using _type = srslog::metric_list<list_info::_type##__name_rep, typename std::decay<_list_type>::type>

} // namespace srslog

#endif // SRSLOG_CONTEXT_H
