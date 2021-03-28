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

/******************************************************************************
 * File:        phy_logger.h
 * Description: Interface for logging output
 *****************************************************************************/

#ifndef SRSRAN_PHY_LOGGER_H
#define SRSRAN_PHY_LOGGER_H

#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
typedef enum { LOG_LEVEL_INFO_S, LOG_LEVEL_DEBUG_S, LOG_LEVEL_ERROR_S } phy_logger_level_t;

typedef void (*phy_log_handler_t)(phy_logger_level_t log_level, void* ctx, char* str);

void srsran_phy_log_register_handler(void* ctx, phy_log_handler_t handler);

void srsran_phy_log_print(phy_logger_level_t log_level, const char* format, ...);

#ifdef __cplusplus
}
#endif // C++

#endif // SRSRAN_PHY_LOGGER_H
