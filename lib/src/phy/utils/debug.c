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

#include "srsran/phy/utils/debug.h"
#include <stdlib.h>

int         srsran_verbose     = 0;
static bool handler_registered = false;

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
