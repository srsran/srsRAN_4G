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

#ifndef SRSRAN_TEST_COMMON_H
#define SRSRAN_TEST_COMMON_H

#include "srsran/config.h"

#ifdef __cplusplus

#include "srsran/common/crash_handler.h"
#include "srsran/common/srsran_assert.h"
#include "srsran/common/standard_streams.h"
#include "srsran/srslog/srslog.h"
#include <atomic>
#include <cstdio>

namespace srsran {

/// This custom sink intercepts log messages to count error and warning log entries.
class log_sink_spy : public srslog::sink
{
public:
  explicit log_sink_spy(std::unique_ptr<srslog::log_formatter> f) :
    srslog::sink(std::move(f)), s(srslog::get_default_sink())
  {
    error_counter.store(0);
    warning_counter.store(0);
  }

  /// Identifier of this custom sink.
  static const char* name() { return "log_sink_spy"; }

  /// Returns the number of log entries tagged as errors.
  unsigned get_error_counter() const
  {
    // Flush to make sure all entries have been processed by the backend.
    srslog::flush();
    return error_counter.load();
  }

  /// Returns the number of log entries tagged as warnings.
  unsigned get_warning_counter() const
  {
    // Flush to make sure all entries have been processed by the backend.
    srslog::flush();
    return warning_counter.load();
  }

  /// Resets the counters back to 0.
  void reset_counters()
  {
    // Flush to make sure all entries have been processed by the backend.
    srslog::flush();
    error_counter.store(0);
    warning_counter.store(0);
  }

  srslog::detail::error_string write(srslog::detail::memory_buffer buffer) override
  {
    std::string entry(buffer.data(), buffer.size());
    if (entry.find("[E]") != std::string::npos) {
      error_counter.fetch_add(1);
    } else if (entry.find("[W]") != std::string::npos) {
      warning_counter.fetch_add(1);
    }

    return s.write(buffer);
  }

  srslog::detail::error_string flush() override { return s.flush(); }

private:
  srslog::sink&         s;
  std::atomic<unsigned> error_counter;
  std::atomic<unsigned> warning_counter;
};

/// This custom sink intercepts log messages allowing users to check if a certain log entry has been generated.
/// Calling spy.has_message("something") will return true if any log entries generated so far contain the string
/// "something".
/// The log entries history can be cleared with reset so old entries can be discarded.
class log_sink_message_spy : public srslog::sink
{
public:
  explicit log_sink_message_spy(std::unique_ptr<srslog::log_formatter> f) :
    srslog::sink(std::move(f)), s(srslog::get_default_sink())
  {}

  /// Identifier of this custom sink.
  static const char* name() { return "log_sink_message_spy"; }

  /// Discards all registered log entries.
  void reset()
  {
    // Flush to make sure all entries have been processed by the backend.
    srslog::flush();
    entries.clear();
  }

  /// Returns true if the string in msg is found in the registered log entries.
  bool has_message(const std::string& msg) const
  {
    srslog::flush();
    return std::find_if(entries.cbegin(), entries.cend(), [&](const std::string& entry) {
             return entry.find(msg) != std::string::npos;
           }) != entries.cend();
  }

  srslog::detail::error_string write(srslog::detail::memory_buffer buffer) override
  {
    entries.emplace_back(buffer.data(), buffer.size());

    return s.write(buffer);
  }

  srslog::detail::error_string flush() override { return s.flush(); }

private:
  srslog::sink&            s;
  std::vector<std::string> entries;
};

inline void test_init(int argc, char** argv)
{
  // Setup a backtrace pretty printer
  srsran_debug_handle_crash(argc, argv);

  srslog::fetch_basic_logger("ALL").set_level(srslog::basic_levels::info);

  // Start the log backend.
  srslog::init();
}

} // namespace srsran

#define CONDERROR(cond, fmt, ...) srsran_assert(not(cond), fmt, ##__VA_ARGS__)

#define TESTERROR(fmt, ...) CONDERROR(true, fmt, ##__VA_ARGS__)

#define TESTASSERT(cond) srsran_assert((cond), "Fail at \"%s\"", (#cond))

#else // if C

#include <stdio.h>

#define TESTASSERT(cond)                                                                                               \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      printf("[%s][Line %d] Fail at \"%s\"\n", __FUNCTION__, __LINE__, (#cond));                                       \
      return -1;                                                                                                       \
    }                                                                                                                  \
  } while (0)

#endif // __cplusplus

#endif // SRSRAN_TEST_COMMON_H
