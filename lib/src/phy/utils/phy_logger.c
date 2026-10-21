// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "srsran/phy/utils/phy_logger.h"
#include "srsran/srsran.h"
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

void srsran_phy_log_register_handler(void* ctx, phy_log_handler_t handler)
{
  phy_log_handler = handler;
  callback_ctx    = ctx;
  set_handler_enabled(true);
}

void srsran_phy_log_print(phy_logger_level_t log_level, const char* format, ...)
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