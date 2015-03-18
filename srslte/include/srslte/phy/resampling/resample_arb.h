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

#ifndef RESAMPLE_ARB_
#define RESAMPLE_ARB_

#include <stdint.h>
#include <complex.h>

#include "srslte/config.h"

typedef _Complex float cf_t;

#define RESAMPLE_ARB_N    32  // Polyphase filter rows
#define RESAMPLE_ARB_M    8   // Polyphase filter columns

typedef struct LIBLTE_API {
  float rate;                // Resample rate
  float step;                // Step increment through filter
  float acc;                 // Index into filter
  cf_t reg[RESAMPLE_ARB_M];  // Our window of samples
}resample_arb_t;

LIBLTE_API void resample_arb_init(resample_arb_t *q, float rate);
LIBLTE_API int resample_arb_compute(resample_arb_t *q, cf_t *input, cf_t *output, int n_in);

#endif //RESAMPLE_ARB_
