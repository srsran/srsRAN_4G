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

#ifndef SRSLOG_LOGGER_H
#define SRSLOG_LOGGER_H

#include "srslte/srslog/log_channel.h"

namespace srslog {

namespace detail {

/// The logger_impl template class contains the common functionality to loggers.
/// Its main responsibility is to control the logging level by individually
/// enabling or disabling the logging channels that form the logger.
/// NOTE: Thread safe class.
template <typename T, typename Enum>
class logger_impl : public T
{
  static_assert(std::is_enum<Enum>::value, "Expected enum type");

  using enum_base_type = typename std::underlying_type<Enum>::type;
  static constexpr unsigned size = static_cast<enum_base_type>(Enum::LAST);

public:
  template <typename... Args>
  explicit logger_impl(std::string id, Args&&... args) :
    T{std::forward<Args>(args)...},
    logger_id(std::move(id)),
    channels{&args...}
  {
    static_assert(
        sizeof...(args) == size,
        "Number of levels in enum does not match number of log channels");
  }

  logger_impl(const logger_impl& other) = delete;
  logger_impl& operator=(const logger_impl& other) = delete;

  /// Returns the id string of the logger.
  const std::string& id() const { return logger_id; }

  /// Change the logging level.
  void set_level(Enum lvl)
  {
    detail::scoped_lock lock(m);
    for (unsigned i = 0, e = channels.size(); i != e; ++i) {
      channels[i]->set_enabled(static_cast<Enum>(i) <= lvl);
    }
  }

  /// Set the specified context value to all the channels of the logger.
  void set_context(uint32_t x)
  {
    detail::scoped_lock lock(m);
    for (auto channel : channels) {
      channel->set_context(x);
    }
  }

  /// Set the maximum number of bytes to can be printed in a hex dump to all the
  /// channels of the logger.
  void set_hex_dump_max_size(int x)
  {
    detail::scoped_lock lock(m);
    for (auto channel : channels) {
      channel->set_hex_dump_max_size(x);
    }
  }

private:
  /// Comparison operator for enum types, used by the set_level method.
  friend bool operator<=(Enum lhs, Enum rhs)
  {
    return static_cast<enum_base_type>(lhs) <= static_cast<enum_base_type>(rhs);
  }

private:
  const std::string logger_id;
  const std::array<log_channel*, size> channels;
  mutable detail::mutex m;
};

/// Type trait to detect if T is a logger.
template <typename T>
struct is_logger : std::false_type {};
template <typename T, typename Enum>
struct is_logger<logger_impl<T, Enum>> : std::true_type {};

} // namespace detail

/// The build_logger_type type alias allows developers to define new logger
/// types in an application.
///
/// To create a new logger type simply follow these steps:
///   1) Define an enum class where each element will represent a logging level.
///     Order the elements from highest to lowest logging level. The last
///     element should be called LAST as it is a sentinel value.
///   2) Define a struct composed by only log_channel references. Declare the
///     members in the same order as done in the enum.
///   3) Define the new logger type by using the build_logger_type alias. Pass
///     the previous defined types as template parameters.
///
/// Example to declare a logger with three logging levels: error, warning and
/// info, being error the highest logging level and info the lowest:
///   1) Define the logging level enum:
///     enum class three_level_logger_levels { error, warning, info, LAST };
///   2) Define the struct of three channels (same order as in the enum):
///     struct three_level_logger {
///       log_channel &error;
///       log_channel &warning;
///       log_channel &info;
///     };
///   3) Define the new logger type:
///     using my_new_logger =
///         build_logger_type<three_level_logger, three_level_logger_levels>;
template <typename T, typename Enum>
using build_logger_type = detail::logger_impl<T, Enum>;

///
/// Common logger types.
///

/// Basic logger with four levels.
enum class basic_levels { error, warning, info, debug, LAST };
struct basic_logger_channels {
  log_channel& error;
  log_channel& warning;
  log_channel& info;
  log_channel& debug;
};
using basic_logger = build_logger_type<basic_logger_channels, basic_levels>;

} // namespace srslog

#endif // SRSLOG_LOGGER_H
