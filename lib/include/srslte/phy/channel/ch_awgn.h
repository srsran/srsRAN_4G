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


#endif
