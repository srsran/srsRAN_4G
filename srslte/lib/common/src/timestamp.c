/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include "srslte/common/timestamp.h"
#include "math.h"

int srslte_timestamp_init(srslte_timestamp_t *t, time_t full_secs, double frac_secs){
  int ret = SRSLTE_ERROR;
  if(t != NULL && frac_secs >= 0.0){
    t->full_secs = full_secs;
    t->frac_secs = frac_secs;
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_timestamp_copy(srslte_timestamp_t *dest, srslte_timestamp_t *src){
  int ret = SRSLTE_ERROR;
  if(dest != NULL && src != NULL){
    dest->full_secs = src->full_secs;
    dest->frac_secs = src->frac_secs;
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_timestamp_add(srslte_timestamp_t *t, time_t full_secs, double frac_secs){
  int ret = SRSLTE_ERROR;
  if(t != NULL && frac_secs >= 0.0){
    t->frac_secs += frac_secs;
    t->full_secs += full_secs;
    double r = floor(t->frac_secs);
    t->full_secs += r;
    t->frac_secs -= r;
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_timestamp_sub(srslte_timestamp_t *t, time_t full_secs, double frac_secs){
  int ret = SRSLTE_ERROR;
  if(t != NULL && frac_secs >= 0.0){
    t->frac_secs -= frac_secs;
    t->full_secs -= full_secs;
    if(t->frac_secs < 0){
      t->frac_secs = 1-t->frac_secs;
      t->full_secs--;
    }
    if(t->full_secs < 0)
      return SRSLTE_ERROR;
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

double srslte_timestamp_real(srslte_timestamp_t *t){
 return t->frac_secs + t->full_secs;
}

uint32_t srslte_timestamp_uint32(srslte_timestamp_t *t){
 uint32_t x = t->full_secs*1e6 + (uint32_t) (t->frac_secs*1e6);
 return x; 
}
