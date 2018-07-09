/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 * File:        phy_logger.h
 * Description: Interface for logging output
 *****************************************************************************/

#ifndef SRSLTE_PHY_LOGGER_H
#define SRSLTE_PHY_LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus
typedef enum {LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_ERROR} phy_logger_level_t;

typedef void (*phy_log_handler_t)(phy_logger_level_t log_level, void *ctx, char *str);

void srslte_phy_log_register_handler(void *ctx, phy_log_handler_t handler); 

 void srslte_phy_log_print(phy_logger_level_t log_level, const char *format, ...);

#ifdef __cplusplus
}
#endif // C++ 

#endif // SRSLTE_PHY_LOGGER_H
