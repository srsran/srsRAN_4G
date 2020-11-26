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

#ifndef SRSLTE_LOG_HELPER_H
#define SRSLTE_LOG_HELPER_H

/**
 * @file log_helper.h
 *
 * Convenience macro to log formatted messages. It is checked if the log pointer is still valid before accessing it.
 */

namespace srslte {

#define Error(fmt, ...)                                                                                                \
  do {                                                                                                                 \
    if (log_h.get() != nullptr) {                                                                                      \
      log_h->error(fmt, ##__VA_ARGS__);                                                                                \
    }                                                                                                                  \
  } while (0)
#define Warning(fmt, ...)                                                                                              \
  do {                                                                                                                 \
    if (log_h.get() != nullptr) {                                                                                      \
      log_h->warning(fmt, ##__VA_ARGS__);                                                                              \
    }                                                                                                                  \
  } while (0)
#define Info(fmt, ...)                                                                                                 \
  do {                                                                                                                 \
    if (log_h.get() != nullptr) {                                                                                      \
      log_h->info(fmt, ##__VA_ARGS__);                                                                                 \
    }                                                                                                                  \
  } while (0)
#define Debug(fmt, ...)                                                                                                \
  do {                                                                                                                 \
    if (log_h.get() != nullptr) {                                                                                      \
      log_h->debug(fmt, ##__VA_ARGS__);                                                                                \
    }                                                                                                                  \
  } while (0)

#define Console(fmt, ...)                                                                                              \
  do {                                                                                                                 \
    if (log_h.get() != nullptr) {                                                                                      \
      srslte::console(fmt, ##__VA_ARGS__);                                                                          \
    }                                                                                                                  \
  } while (0)

} // namespace srslte

#endif // SRSLTE_LOG_HELPER_H
