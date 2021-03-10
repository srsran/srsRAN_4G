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

#define Error(fmt, ...) logger.error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) logger.warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) logger.info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) logger.debug(fmt, ##__VA_ARGS__)
#define Console(fmt, ...) srslte::console(fmt, ##__VA_ARGS__)

} // namespace srslte

#endif // SRSLTE_LOG_HELPER_H
