/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

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
