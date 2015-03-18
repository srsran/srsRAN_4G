/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "liblte/phy/common/timestamp.h"
#include "math.h"

int timestamp_init(timestamp_t *t, time_t full_secs, double frac_secs){
  int ret = LIBLTE_ERROR;
  if(t != NULL && frac_secs >= 0.0){
    t->full_secs = full_secs;
    t->frac_secs = frac_secs;
    ret = LIBLTE_SUCCESS;
  }
  return ret;
}

int timestamp_copy(timestamp_t *dest, timestamp_t *src){
  int ret = LIBLTE_ERROR;
  if(dest != NULL && src != NULL){
    dest->full_secs = src->full_secs;
    dest->frac_secs = src->frac_secs;
    ret = LIBLTE_SUCCESS;
  }
  return ret;
}

int timestamp_add(timestamp_t *t, time_t full_secs, double frac_secs){
  int ret = LIBLTE_ERROR;
  if(t != NULL && frac_secs >= 0.0){
    t->frac_secs += frac_secs;
    t->full_secs += full_secs;
    double r = floor(t->frac_secs);
    t->full_secs += r;
    t->frac_secs -= r;
    ret = LIBLTE_SUCCESS;
  }
  return ret;
}

int timestamp_sub(timestamp_t *t, time_t full_secs, double frac_secs){
  int ret = LIBLTE_ERROR;
  if(t != NULL && frac_secs >= 0.0){
    t->frac_secs -= frac_secs;
    t->full_secs -= full_secs;
    if(t->frac_secs < 0){
      t->frac_secs = 1-t->frac_secs;
      t->full_secs--;
    }
    if(t->full_secs < 0)
      return LIBLTE_ERROR;
    ret = LIBLTE_SUCCESS;
  }
  return ret;
}

double timestamp_real(timestamp_t *t){
 return t->frac_secs + t->full_secs;
}
