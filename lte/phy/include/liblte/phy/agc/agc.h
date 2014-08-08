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



#ifndef AGC_
#define AGC_

#include <stdbool.h>
#include <stdint.h>
#include <complex.h>

#include "liblte/config.h"

/* Automatic Gain Control 
 *
 */
typedef _Complex float cf_t;

#define AGC_DEFAULT_BW  (5e-2)

typedef struct LIBLTE_API{
  float bandwidth;
  float gain; 
  float y_out;
  bool lock;
  bool isfirst; 
} agc_t;

LIBLTE_API int agc_init (agc_t *q);

LIBLTE_API void agc_free(agc_t *q);

LIBLTE_API void agc_reset(agc_t *q);

LIBLTE_API void agc_set_bandwidth(agc_t *q, 
                                  float bandwidth);

LIBLTE_API float agc_get_rssi(agc_t *q);

LIBLTE_API float agc_get_output_level(agc_t *q); 

LIBLTE_API float agc_get_gain(agc_t *q);

LIBLTE_API void agc_lock(agc_t *q, 
                         bool enable);

LIBLTE_API void agc_process(agc_t *q, 
                         cf_t *input,
                         cf_t *output, 
                         uint32_t len);

#endif // AGC_
