/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSLTE_TEST_COMMON_H
#define SRSLTE_TEST_COMMON_H

#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/config.h"
#include <cstdio>

namespace srslte {

// logger that we can instantiate in a specific test scope
// useful if we want to define specific logging policies within a scope (e.g. null logger, count number of errors,
// exit on error, log special diagnostics on destruction). It restores the previous logger after exiting the scope
class scoped_tester_log : public srslte::log_filter
{
public:
  explicit scoped_tester_log(std::string layer) : srslte::log_filter(layer)
  {
    previous_log_test = current_log;
    current_log       = this;
    set_level(srslte::LOG_LEVEL_DEBUG);
  }
  scoped_tester_log(const scoped_tester_log&) = delete;
  scoped_tester_log(scoped_tester_log&&)      = delete;
  scoped_tester_log& operator=(const scoped_tester_log&) = delete;
  scoped_tester_log& operator=(scoped_tester_log&&) = delete;
  ~scoped_tester_log() override { current_log = previous_log_test; }

  void error(const char* message, ...) override __attribute__((format(printf, 2, 3)))
  {
    error_counter++;
    if (level >= srslte::LOG_LEVEL_ERROR) {
      char    args_msg[char_buff_size];
      va_list args;
      va_start(args, message);
      if (vsnprintf(args_msg, char_buff_size, message, args) > 0) {
        all_log(srslte::LOG_LEVEL_ERROR, tti, args_msg);
      }
      va_end(args);
    }
    if (exit_on_error) {
      exit(-1);
    }
  }

  void warning(const char* message, ...) override __attribute__((format(printf, 2, 3)))
  {
    warn_counter++;
    if (level >= srslte::LOG_LEVEL_WARNING) {
      char    args_msg[char_buff_size];
      va_list args;
      va_start(args, message);
      if (vsnprintf(args_msg, char_buff_size, message, args) > 0) {
        all_log(srslte::LOG_LEVEL_WARNING, tti, args_msg);
      }
      va_end(args);
    }
  }

  virtual void log_diagnostics()
  {
    if (error_counter > 0 or warn_counter > 0) {
      info("STATUS: counted %d errors and %d warnings.\n", error_counter, warn_counter);
    }
  }

  bool     exit_on_error = false;
  uint32_t error_counter = 0, warn_counter = 0;

  static srslte::log* get_instance() { return current_log; }

private:
  srslte::log*        previous_log_test = nullptr;
  static srslte::log* current_log;
};
srslte::log* scoped_tester_log::current_log = nullptr;

} // namespace srslte

#define TESTERROR(fmt, ...)                                                                                            \
  do {                                                                                                                 \
    if (srslte::scoped_tester_log::get_instance() == nullptr) {                                                        \
      printf(fmt, ##__VA_ARGS__);                                                                                      \
    } else {                                                                                                           \
      srslte::scoped_tester_log::get_instance()->error(fmt, ##__VA_ARGS__);                                            \
    }                                                                                                                  \
    return SRSLTE_ERROR;                                                                                               \
  } while (0)

#define TESTWARN(fmt, ...)                                                                                             \
  do {                                                                                                                 \
    if (srslte::scoped_tester_log::get_instance() == nullptr) {                                                        \
      printf(fmt, ##__VA_ARGS__);                                                                                      \
    } else {                                                                                                           \
      srslte::scoped_tester_log::get_instance()->warning(fmt, ##__VA_ARGS__);                                          \
    }                                                                                                                  \
  } while (0)

#define CONDERROR(cond, fmt, ...)                                                                                      \
  do {                                                                                                                 \
    if (cond) {                                                                                                        \
      TESTERROR(fmt, ##__VA_ARGS__);                                                                                   \
    }                                                                                                                  \
  } while (0)

#define TESTASSERT(cond) CONDERROR((not(cond)), "[%s][Line %d] Fail at \"%s\"\n", __FUNCTION__, __LINE__, (#cond))

#endif // SRSLTE_TEST_COMMON_H
