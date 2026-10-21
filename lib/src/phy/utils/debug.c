// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "srsran/phy/utils/debug.h"
#include <stdlib.h>

static int  srsran_verbose     = 0;
static bool handler_registered = false;

int get_srsran_verbose_level(void)
{
  return srsran_verbose;
}

void set_srsran_verbose_level(int level)
{
  srsran_verbose = level;
}

void increase_srsran_verbose_level(void)
{
  srsran_verbose++;
}

bool is_handler_registered(void)
{
  return handler_registered;
}

void set_handler_enabled(bool enable)
{
  handler_registered = enable;
}

void get_time_interval(struct timeval* tdata)
{
  tdata[0].tv_sec  = tdata[2].tv_sec - tdata[1].tv_sec;
  tdata[0].tv_usec = tdata[2].tv_usec - tdata[1].tv_usec;
  if (tdata[0].tv_usec < 0) {
    tdata[0].tv_sec--;
    tdata[0].tv_usec += 1000000;
  }
}
