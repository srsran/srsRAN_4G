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

#include "srsran/common/standard_streams.h"
#include "srsran/srslog/srslog.h"

#ifdef ASSERTS_ENABLED

#define srsran_unlikely(expr) __builtin_expect(!!(expr), 0)

#define srsran_assert(condition, fmt, ...)                                                                             \
  do {                                                                                                                 \
    if (srsran_unlikely(not(condition))) {                                                                             \
      srslog::fetch_basic_logger("ALL").error("%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__);                       \
      srsran::console_stderr("%s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);                                   \
      srslog::flush();                                                                                                 \
      std::abort();                                                                                                    \
    }                                                                                                                  \
  } while (0)

#ifdef STOP_ON_WARNING

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

#define srslte_assert(condition, fmt, ...)                                                                             \
  do {                                                                                                                 \
  } while (0)

#define srsran_expect(condition, fmt, ...)                                                                             \
  do {                                                                                                                 \
  } while (0)

#endif

#endif // SRSRAN_ASSERT_H
