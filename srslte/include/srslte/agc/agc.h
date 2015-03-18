/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#include "srslte/config.h"

/* Automatic Gain Control 
 *
 */
typedef _Complex float cf_t;

#define SRSLTE_AGC_DEFAULT_BW  (5e-2)

typedef struct SRSLTE_API{
  float bandwidth;
  float gain; 
  float y_out;
  bool lock;
  bool isfirst; 
} srslte_agc_t;

SRSLTE_API int srslte_agc_init (srslte_agc_t *q);

SRSLTE_API void srslte_agc_free(srslte_agc_t *q);

SRSLTE_API void srslte_agc_reset(srslte_agc_t *q);

SRSLTE_API void srslte_agc_set_bandwidth(srslte_agc_t *q, 
                                  float bandwidth);

SRSLTE_API float srslte_agc_get_rssi(srslte_agc_t *q);

SRSLTE_API float srslte_agc_get_output_level(srslte_agc_t *q); 

SRSLTE_API float srslte_agc_get_gain(srslte_agc_t *q);

SRSLTE_API void srslte_agc_lock(srslte_agc_t *q, 
                                bool enable);

SRSLTE_API void srslte_agc_process(srslte_agc_t *q, 
                                   cf_t *input,
                                   cf_t *output, 
                                   uint32_t len);

#endif // AGC_
