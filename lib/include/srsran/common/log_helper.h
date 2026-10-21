// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_LOG_HELPER_H
#define SRSRAN_LOG_HELPER_H

/**
 * @file log_helper.h
 *
 * Convenience macro to log formatted messages. It is checked if the log pointer is still valid before accessing it.
 */

namespace srsran {

#define Error(fmt, ...) logger.error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) logger.warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) logger.info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) logger.debug(fmt, ##__VA_ARGS__)
#define Console(fmt, ...) srsran::console(fmt, ##__VA_ARGS__)

} // namespace srsran

#endif // SRSRAN_LOG_HELPER_H
