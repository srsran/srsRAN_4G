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

#ifndef SRSLTE_TEST_COMMON_H
#define SRSLTE_TEST_COMMON_H

#include "srslte/config.h"

#ifdef __cplusplus

#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/logmap.h"
#include <cstdio>

namespace srslte {

// Description: log filter that we can instantiate in a specific test scope, and cleans itself on scope exit
// useful if we want to define specific logging policies within a scope (e.g. null logger, count number of errors,
// exit on error, log special diagnostics on destruction). It restores the previous logger after exiting the scope
class test_log_filter : public srslte::log_filter
{
public:
  explicit test_log_filter(std::string layer) : srslte::log_filter(std::move(layer))
  {
    set_level(srslte::LOG_LEVEL_DEBUG);
  }
  test_log_filter(const test_log_filter&) = delete;
  test_log_filter(test_log_filter&&)      = delete;
  test_log_filter& operator=(const test_log_filter&) = delete;
  test_log_filter& operator=(test_log_filter&&) = delete;
  ~test_log_filter() override                   = default;

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
};

// specialization of test_log_filter to store last logged message
class nullsink_log : public test_log_filter
{
public:
  explicit nullsink_log(std::string layer) : test_log_filter(std::move(layer)) {}

  void debug(const char* message, ...) override __attribute__((format(printf, 2, 3)))
  {
    va_list args;
    va_start(args, message);
    log_va_list(LOG_LEVEL_DEBUG, message, args);
    va_end(args);
  }

  void info(const char* message, ...) override __attribute__((format(printf, 2, 3)))
  {
    va_list args;
    va_start(args, message);
    log_va_list(LOG_LEVEL_INFO, message, args);
    va_end(args);
  }

  void warning(const char* message, ...) override __attribute__((format(printf, 2, 3)))
  {
    warn_counter++;
    va_list args;
    va_start(args, message);
    log_va_list(LOG_LEVEL_WARNING, message, args);
    va_end(args);
  }

  void error(const char* message, ...) override __attribute__((format(printf, 2, 3)))
  {
    error_counter++;
    va_list args;
    va_start(args, message);
    log_va_list(LOG_LEVEL_ERROR, message, args);
    va_end(args);
    if (exit_on_error) {
      exit(-1);
    }
  }

  srslte::LOG_LEVEL_ENUM last_log_level = LOG_LEVEL_NONE;
  std::string            last_log_msg;

private:
  void log_va_list(srslte::LOG_LEVEL_ENUM loglevel, const char* message, va_list argp)
  {
    last_log_level = loglevel;
    if (level >= loglevel) {
      char args_msg[char_buff_size];
      if (vsnprintf(args_msg, char_buff_size, message, argp) > 0) {
        last_log_msg = args_msg;
      }
    }
  }
};

template <typename Log>
class scoped_log
{
public:
  template <typename... Args>
  explicit scoped_log(Args&&... args)
  {
    std::unique_ptr<Log> l{new Log{std::forward<Args>(args)...}};
    // store previous log, and register the newly created one
    prev_log    = srslte::logmap::deregister_log(l->get_service_name());
    current_log = l.get();
    srslte::logmap::register_log(std::move(l));
  }
  scoped_log(scoped_log<Log>&&) noexcept = default;
  ~scoped_log()
  {
    srslte::logmap::deregister_log(current_log->get_service_name());
    if (prev_log != nullptr) {
      srslte::logmap::register_log(std::move(prev_log));
    }
  }

  Log* operator->() { return current_log; }
  Log* get() { return current_log; }

private:
  Log*                         current_log = nullptr;
  std::unique_ptr<srslte::log> prev_log;
};

} // namespace srslte

#define TESTERROR(fmt, ...)                                                                                            \
  do {                                                                                                                 \
    srslte::logmap::get("TEST")->error(fmt, ##__VA_ARGS__);                                                            \
    return SRSLTE_ERROR;                                                                                               \
  } while (0)

#define TESTWARN(fmt, ...)                                                                                             \
  do {                                                                                                                 \
    srslte::logmap::get("TEST")->warning(fmt, ##__VA_ARGS__);                                                          \
  } while (0)

#define CONDERROR(cond, fmt, ...)                                                                                      \
  do {                                                                                                                 \
    if (cond) {                                                                                                        \
      TESTERROR(fmt, ##__VA_ARGS__);                                                                                   \
    }                                                                                                                  \
  } while (0)

#define TESTASSERT(cond) CONDERROR((not(cond)), "[%s][Line %d] Fail at \"%s\"\n", __FUNCTION__, __LINE__, (#cond))

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

#endif // SRSLTE_TEST_COMMON_H
