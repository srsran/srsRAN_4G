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
 *  File:         turbodecoder.h
 *
 *  Description:  Turbo Decoder.
 *                Parallel Concatenated Convolutional Code (PCCC) with two 8-state constituent
 *                encoders and one turbo code internal interleaver. The coding rate of turbo
 *                encoder is 1/3.
 *                MAP_GEN is the MAX-LOG-MAP generic implementation of the decoder.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.3.2
 *********************************************************************************************/

#ifndef TURBODECODER_
#define TURBODECODER_

#include "srslte/config.h"
#include "srslte/fec/tc_interl.h"
#include "srslte/fec/cbsegm.h"

#define SRSLTE_TCOD_RATE 3
#define SRSLTE_TCOD_TOTALTAIL 12

#define SRSLTE_TCOD_MAX_LEN_CB     6144
#define SRSLTE_TCOD_MAX_LEN_CODED  (SRSLTE_TCOD_RATE*SRSLTE_TCOD_MAX_LEN_CB+SRSLTE_TCOD_TOTALTAIL)

#include "srslte/fec/turbodecoder_gen.h"

#ifdef LV_HAVE_SSE
#include "srslte/fec/turbodecoder_sse.h"
#endif

typedef struct SRSLTE_API {
#ifdef LV_HAVE_SSE
  srslte_tdec_sse_t tdec_sse;
#else
  float *input_conv; 
  srslte_tdec_gen_t tdec_gen;
#endif  
} srslte_tdec_t;

SRSLTE_API int srslte_tdec_init(srslte_tdec_t * h, 
                                uint32_t max_long_cb);

SRSLTE_API void srslte_tdec_free(srslte_tdec_t * h);

SRSLTE_API int srslte_tdec_reset(srslte_tdec_t * h, uint32_t long_cb);

SRSLTE_API void srslte_tdec_iteration(srslte_tdec_t * h, 
                                      int16_t* input, 
                                      uint32_t long_cb);

SRSLTE_API void srslte_tdec_decision(srslte_tdec_t * h, 
                                     uint8_t *output, 
                                     uint32_t long_cb);

SRSLTE_API void srslte_tdec_decision_byte(srslte_tdec_t * h, 
                                          uint8_t *output, 
                                          uint32_t long_cb); 

SRSLTE_API int srslte_tdec_run_all(srslte_tdec_t * h, 
                                   int16_t * input, 
                                   uint8_t *output,
                                   uint32_t nof_iterations, 
                                   uint32_t long_cb);

#endif
