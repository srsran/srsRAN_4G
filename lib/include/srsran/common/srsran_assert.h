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

#ifndef SRSRAN_ASSERT_H
#define SRSRAN_ASSERT_H

#include "srsran/srslog/srslog.h"
#include <cstdio>

#define srsran_unlikely(expr) __builtin_expect(!!(expr), 0)

#define srsran_terminate(fmt, ...)                                                                                     \
  std::fprintf(stderr, "%s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);                                         \
  srslog::flush();                                                                                                     \
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

#define srsran_expect(condition, fmt, ...) srsran_assert(condition, fmt, ##__VA_ARGS__)

#endif

#endif // SRSRAN_ASSERT_H
