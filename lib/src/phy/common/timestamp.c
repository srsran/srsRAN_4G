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

#include "srslte/phy/common/timestamp.h"
#include "math.h"

int srslte_timestamp_init(srslte_timestamp_t* t, time_t full_secs, double frac_secs)
{
  int ret = SRSLTE_ERROR;
  if (t != NULL && frac_secs >= 0.0) {
    t->full_secs = full_secs;
    t->frac_secs = frac_secs;
    ret          = SRSLTE_SUCCESS;
  }
  return ret;
}

void srslte_timestamp_init_uint64(srslte_timestamp_t* ts_time, uint64_t ts_count, double base_srate)
{
  uint64_t seconds      = ts_count / (uint64_t)base_srate;
  uint64_t frac_samples = (uint64_t)(seconds * (uint64_t)base_srate);
  double   frac_seconds = (double)(ts_count - frac_samples) / base_srate;

  if (ts_time) {
    srslte_timestamp_init(ts_time, seconds, frac_seconds);
  }
}

int srslte_timestamp_copy(srslte_timestamp_t* dest, srslte_timestamp_t* src)
{
  int ret = SRSLTE_ERROR;
  if (dest != NULL && src != NULL) {
    dest->full_secs = src->full_secs;
    dest->frac_secs = src->frac_secs;
    ret             = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_timestamp_compare(const srslte_timestamp_t* a, const srslte_timestamp_t* b)
{
  int ret = 0;

  if (a->full_secs > b->full_secs) {
    ret = +1;
  } else if (a->full_secs < b->full_secs) {
    ret = -1;
  } else if (a->frac_secs > b->frac_secs) {
    ret = +1;
  } else if (a->frac_secs < b->frac_secs) {
    ret = -1;
  }

  return ret;
}

int srslte_timestamp_add(srslte_timestamp_t* t, time_t full_secs, double frac_secs)
{
  int ret = SRSLTE_ERROR;
  if (t != NULL && frac_secs >= 0.0) {
    t->frac_secs += frac_secs;
    t->full_secs += full_secs;
    double r = floor(t->frac_secs);
    t->full_secs += r;
    t->frac_secs -= r;
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_timestamp_sub(srslte_timestamp_t* t, time_t full_secs, double frac_secs)
{
  int ret = SRSLTE_ERROR;
  if (t != NULL && frac_secs >= 0.0) {
    t->frac_secs -= frac_secs;
    t->full_secs -= full_secs;
    if (t->frac_secs < 0) {
      t->frac_secs = t->frac_secs + 1;
      t->full_secs--;
    }
    if (t->full_secs < 0)
      return SRSLTE_ERROR;
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

double srslte_timestamp_real(const srslte_timestamp_t* t)
{
  return t->frac_secs + t->full_secs;
}

bool srslte_timestamp_iszero(const srslte_timestamp_t* t)
{
  return ((t->full_secs == 0) && (t->frac_secs == 0));
}

uint32_t srslte_timestamp_uint32(srslte_timestamp_t* t)
{
  uint32_t x = t->full_secs * 1e6 + (uint32_t)(t->frac_secs * 1e6);
  return x;
}

uint64_t srslte_timestamp_uint64(const srslte_timestamp_t* t, double srate)
{
  return (uint64_t)(t->full_secs * (uint64_t)srate) + (uint64_t)round(t->frac_secs * srate);
}
