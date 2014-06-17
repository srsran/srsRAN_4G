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


#ifndef CFO_
#define CFO_

#include <complex.h>

#include "lte/config.h"

typedef _Complex float cf_t;

/** If the frequency is changed more than the tolerance, a new table is generated */
#define CFO_TOLERANCE 0.00001

#define CFO_CEXPTAB_SIZE 4096

typedef struct LIBLTE_API {
  float last_freq;
  float tol;
  int nsamples;
  cexptab_t tab;
  cf_t *cur_cexp;
}cfo_t;

LIBLTE_API int cfo_init(cfo_t *h, int nsamples);
LIBLTE_API void cfo_free(cfo_t *h);

LIBLTE_API void cfo_set_tol(cfo_t *h, float tol);
LIBLTE_API void cfo_correct(cfo_t *h, cf_t *x, float freq);

#endif // CFO_
