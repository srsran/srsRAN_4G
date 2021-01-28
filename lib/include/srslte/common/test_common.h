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

#ifndef SRSLTE_TEST_COMMON_H
#define SRSLTE_TEST_COMMON_H

#include "srslte/config.h"

#ifdef __cplusplus

#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/logmap.h"
#include "srslte/srslog/srslog.h"
#include <atomic>
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
