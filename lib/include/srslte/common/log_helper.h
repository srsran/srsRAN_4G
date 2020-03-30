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
      log_h->console(fmt, ##__VA_ARGS__);                                                                              \
    }                                                                                                                  \
  } while (0)

} // namespace srslte

#endif // SRSLTE_LOG_HELPER_H
