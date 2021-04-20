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

#include "srsran/srslog/srslog.h"
#include <cstdio>

#define srsran_unlikely(expr) __builtin_expect(!!(expr), 0)

#define srsran_terminate(fmt, ...)                                                                                     \
  std::fprintf(stderr, "%s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);                                         \
  std::abort()

#ifdef ASSERTS_ENABLED

/**
 * Macro that asserts condition is true. If false, it logs the remaining parameters, prints the backtrace and closes
 * the application
 */
#define srsran_assert(condition, fmt, ...)                                                                             \
  do {                                                                                                                 \
    if (srsran_unlikely(not(condition))) {                                                                             \
      srsran_terminate(fmt, ##__VA_ARGS__);                                                                            \
    }                                                                                                                  \
  } while (0)

#ifdef STOP_ON_WARNING

/**
 * Macro that verifies if condition is true. If false, and STOP_ON_WARNING is true, it behaves like srsran_assert.
 * If STOP_ON_WARNING is false, it logs a warning.
 */
#define srsran_expect(condition, fmt, ...) srsran_assert(condition, fmt, ##__VA_ARGS__)

#else // STOP_ON_WARNING

#define srsran_expect(condition, fmt, ...)                                                                             \
  do {                                                                                                                 \
    if (srsran_unlikely(not(condition))) {                                                                             \
      srslog::fetch_basic_logger("ALL").warning("%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__);                     \
    }                                                                                                                  \
  } while (0)

#endif // STOP_ON_WARNING

#else // ASSERTS_ENABLED

#define srsran_assert(condition, fmt, ...)                                                                             \
  do {                                                                                                                 \
  } while (0)

#define srsran_expect(condition, fmt, ...)                                                                             \
  do {                                                                                                                 \
  } while (0)

#endif

#endif // SRSRAN_ASSERT_H
