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

#include "srsran/phy/common/timestamp.h"
#include "math.h"

int srsran_timestamp_init(srsran_timestamp_t* t, time_t full_secs, double frac_secs)
{
  int ret = SRSRAN_ERROR;
  if (t != NULL && frac_secs >= 0.0) {
    t->full_secs = full_secs;
    t->frac_secs = frac_secs;
    ret          = SRSRAN_SUCCESS;
  }
  return ret;
}

void srsran_timestamp_init_uint64(srsran_timestamp_t* ts_time, uint64_t ts_count, double base_srate)
{
  uint64_t seconds      = ts_count / (uint64_t)base_srate;
  uint64_t frac_samples = (uint64_t)(seconds * (uint64_t)base_srate);
  double   frac_seconds = (double)(ts_count - frac_samples) / base_srate;

  if (ts_time) {
    srsran_timestamp_init(ts_time, seconds, frac_seconds);
  }
}

int srsran_timestamp_copy(srsran_timestamp_t* dest, srsran_timestamp_t* src)
{
  int ret = SRSRAN_ERROR;
  if (dest != NULL && src != NULL) {
    dest->full_secs = src->full_secs;
    dest->frac_secs = src->frac_secs;
    ret             = SRSRAN_SUCCESS;
  }
  return ret;
}

int srsran_timestamp_compare(const srsran_timestamp_t* a, const srsran_timestamp_t* b)
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

int srsran_timestamp_add(srsran_timestamp_t* t, time_t full_secs, double frac_secs)
{
  int ret = SRSRAN_ERROR;
  if (t != NULL && frac_secs >= 0.0) {
    t->frac_secs += frac_secs;
    t->full_secs += full_secs;
    double r = floor(t->frac_secs);
    t->full_secs += r;
    t->frac_secs -= r;
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

int srsran_timestamp_sub(srsran_timestamp_t* t, time_t full_secs, double frac_secs)
{
  int ret = SRSRAN_ERROR;
  if (t != NULL && frac_secs >= 0.0) {
    t->frac_secs -= frac_secs;
    t->full_secs -= full_secs;
    if (t->frac_secs < 0) {
      t->frac_secs = t->frac_secs + 1;
      t->full_secs--;
    }
    if (t->full_secs < 0)
      return SRSRAN_ERROR;
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

double srsran_timestamp_real(const srsran_timestamp_t* t)
{
  return t->frac_secs + t->full_secs;
}

bool srsran_timestamp_iszero(const srsran_timestamp_t* t)
{
  return ((t->full_secs == 0) && (t->frac_secs == 0));
}

uint32_t srsran_timestamp_uint32(srsran_timestamp_t* t)
{
  uint32_t x = t->full_secs * 1e6 + (uint32_t)(t->frac_secs * 1e6);
  return x;
}

uint64_t srsran_timestamp_uint64(const srsran_timestamp_t* t, double srate)
{
  return (uint64_t)(t->full_secs * (uint64_t)srate) + (uint64_t)round(t->frac_secs * srate);
}
