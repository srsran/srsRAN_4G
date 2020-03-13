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

#include "srslte/phy/utils/phy_logger.h"
#include "srslte/srslte.h"
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
/*********************************************************************
    Functions for external logging
*********************************************************************/
static phy_log_handler_t phy_log_handler;
static void*             callback_ctx = NULL;

void srslte_phy_log_register_handler(void* ctx, phy_log_handler_t handler)
{
  phy_log_handler = handler;
  callback_ctx    = ctx;
  handler_registered++;
}

void srslte_phy_log_print(phy_logger_level_t log_level, const char* format, ...)
{
  char    tmp[256];
  va_list args;
  va_start(args, format);
  if (phy_log_handler) {
    if (vsnprintf(tmp, 256, format, args) > 0) {
      phy_log_handler(log_level, callback_ctx, tmp);
    }
  }
  va_end(args);
}