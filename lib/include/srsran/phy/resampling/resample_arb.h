/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
 *  File:         resample_arb.h
 *
 *  Description:  Arbitrary rate resampler using a polyphase filter bank
 *                implementation.
 *
 *  Reference:    Multirate Signal Processing for Communication Systems
 *                fredric j. harris
 *****************************************************************************/

#ifndef SRSRAN_RESAMPLE_ARB_H
#define SRSRAN_RESAMPLE_ARB_H

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"

#define SRSRAN_RESAMPLE_ARB_N_35 35
#define SRSRAN_RESAMPLE_ARB_N 32 // Polyphase filter rows
#define SRSRAN_RESAMPLE_ARB_M 8  // Polyphase filter columns

typedef struct SRSRAN_API {
  float rate; // Resample rate
  float step; // Step increment through filter
  float acc;  // Index into filter
  bool  interpolate;
  cf_t  reg[SRSRAN_RESAMPLE_ARB_M]; // Our window of samples

} srsran_resample_arb_t;

SRSRAN_API void srsran_resample_arb_init(srsran_resample_arb_t* q, float rate, bool interpolate);

SRSRAN_API int srsran_resample_arb_compute(srsran_resample_arb_t* q, cf_t* input, cf_t* output, int n_in);

#endif // SRSRAN_RESAMPLE_ARB_
