/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

/******************************************************************************
 *  File:         debug.h
 *
 *  Description:  Debug output utilities.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_FILTER_H
#define SRSRAN_FILTER_H

#include "srsran/config.h"
#include "srsran/phy/utils/vector.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct SRSRAN_API {
  cf_t*  filter_input;
  cf_t*  downsampled_input;
  cf_t*  filter_output;
  bool   is_decimator;
  int    factor;
  int    num_taps;
  float* taps;

} srsran_filt_cc_t;

void srsran_filt_decim_cc_init(srsran_filt_cc_t* q, int factor, int order);

void srsran_filt_decim_cc_free(srsran_filt_cc_t* q);

void srsran_filt_decim_cc_execute(srsran_filt_cc_t* q, cf_t* input, cf_t* downsampled_input, cf_t* output, int size);

void srsran_downsample_cc(cf_t* input, cf_t* output, int M, int size);
#endif // SRSRAN_FILTER_H