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

#ifndef SRSRAN_ASSERT_H
#define SRSRAN_ASSERT_H

#ifdef __cplusplus
#include "srsran/srslog/srslog.h"
#include <cstdio>
#include <stdarg.h>

#define srsran_unlikely(expr) __builtin_expect(!!(expr), 0)

/**
 * Command to terminate srsRAN application with an error message, ensuring first that the log is flushed
 */
[[gnu::noinline, noreturn]] inline bool srsran_terminate(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  srslog::flush();
  vfprintf(stderr, fmt, args);
  va_end(args);
  std::abort();
}

#define srsran_assertion_failure(fmt, ...)                                                                             \
  srsran_terminate("%s:%d: Assertion Failure: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * Macro that asserts condition is true. If false, it logs the remaining macro args, flushes the log,
 * prints the backtrace (if it was activated) and closes the application.
 */
#define srsran_always_assert(condition, fmt, ...) (void)((condition) || srsran_assertion_failure(fmt, ##__VA_ARGS__))

#define SRSRAN_IS_DEFINED(x) SRSRAN_IS_DEFINED2(x)
#define SRSRAN_IS_DEFINED2(x) (#x[0] == 0 || (#x[0] >= '1' && #x[0] <= '9'))

/**
 * Same as "srsran_always_assert" but it is only active when "enable_check" flag is defined
 */
#define srsran_assert_ifdef(enable_check, condition, fmt, ...)                                                         \
  (void)((not SRSRAN_IS_DEFINED(enable_check)) || (srsran_always_assert(condition, fmt, ##__VA_ARGS__), 0))

/**
 * Specialization of "srsran_assert_ifdef" for the ASSERTS_ENABLED flag
 */
#define srsran_assert(condition, fmt, ...) srsran_assert_ifdef(ASSERTS_ENABLED, condition, fmt, ##__VA_ARGS__)

/**
 * Specialization of "srsran_assert_ifdef" for the SANITY_CHECKS_ENABLED flag
 */
#ifndef NDEBUG
#define SANITY_CHECKS_ENABLED
#endif
#define srsran_sanity_check(condition, fmt, ...)                                                                       \
  srsran_assert_ifdef(SANITY_CHECKS_ENABLED, condition, fmt, ##__VA_ARGS__)

#ifdef STOP_ON_WARNING

#define srsran_expect(condition, fmt, ...) srsran_assert(condition, fmt, ##__VA_ARGS__)

#else

#define srsran_expect(condition, fmt, ...)                                                                             \
  do {                                                                                                                 \
    if (srsran_unlikely(not(condition))) {                                                                             \
      srslog::fetch_basic_logger("ALL").warning("%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__);                     \
    }                                                                                                                  \
  } while (0)

#endif

#else // __ifcplusplus

#include <cassert>

#ifdef ASSERTS_ENABLED
#define srsran_assert(condition, fmt, ...) (void)((condition) || (__assert(#condition, __FILE__, __FLAG__), 0))
#else
#define srsran_assert(condition, fmt, ...)                                                                             \
  do {                                                                                                                 \
  } while (0)
#endif

#endif // __ifcplusplus

#endif // SRSRAN_ASSERT_H
