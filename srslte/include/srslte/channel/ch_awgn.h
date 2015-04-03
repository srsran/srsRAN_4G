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

/**********************************************************************************************
 *  File:         ch_awgn.h
 *
 *  Description:  Additive white gaussian noise channel object
 *
 *  Reference:
 *********************************************************************************************/

#include <complex.h>
#include <stdint.h>

#include "srslte/config.h"

#ifndef CH_AWGN_
#define CH_AWGN_

SRSLTE_API void srslte_ch_awgn_c(const cf_t* input, 
                                 cf_t* output, 
                                 float variance, 
                                 uint32_t len);

SRSLTE_API void srslte_ch_awgn_f(const float* x, 
                                 float* y, 
                                 float variance, 
                                 uint32_t len);

SRSLTE_API float srslte_ch_awgn_get_variance(float ebno_db, 
                                             float rate);

/* High-level API */

typedef struct SRSLTE_API{
  const cf_t* input;
  int in_len;
  struct srslte_ch_awgn_ctrl_in {
    float variance;      // Noise variance
  } ctrl_in;

  cf_t* output;
  int out_len;
}srslte_ch_awgn_hl;

SRSLTE_API int srslte_ch_awgn_initialize(srslte_ch_awgn_hl* hl);
SRSLTE_API int srslte_ch_awgn_work(srslte_ch_awgn_hl* hl);
SRSLTE_API int srslte_ch_awgn_stop(srslte_ch_awgn_hl* hl);

#endif
