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

#ifndef SRSLTE_TURBODECODER_SIMD_INTER_H
#define SRSLTE_TURBODECODER_SIMD_INTER_H


/** This is an simd inter-frame parallel turbo decoder. Parallizes 8 code-blocks using SSE
 *  This implementation is currently not functional and not used by the rest of the code 
 */

#include "srslte/config.h"
#include "srslte/phy/fec/tc_interl.h"
#include "srslte/phy/fec/cbsegm.h"

#if LV_HAVE_AVX2
  #define SRSLTE_TDEC_MAX_NPAR 16
#else
  #define SRSLTE_TDEC_MAX_NPAR 8
#endif


typedef struct SRSLTE_API {
  int max_long_cb;

  int16_t *syst0;
  int16_t *parity0;
  int16_t *syst1;
  int16_t *parity1;
  int16_t *llr1;
  int16_t *llr2;
  int16_t *w;
  int16_t *alpha;

  uint32_t max_par_cb;   
  int current_cbidx; 
  uint32_t current_long_cb;
  srslte_tc_interl_t interleaver[SRSLTE_NOF_TC_CB_SIZES];
  int n_iter[SRSLTE_TDEC_MAX_NPAR];
} srslte_tdec_simd_inter_t;

SRSLTE_API int srslte_tdec_simd_inter_init(srslte_tdec_simd_inter_t * h, 
                                           uint32_t max_par_cb, 
                                           uint32_t max_long_cb);

SRSLTE_API void srslte_tdec_simd_inter_free(srslte_tdec_simd_inter_t * h);

SRSLTE_API int srslte_tdec_simd_inter_reset(srslte_tdec_simd_inter_t * h, 
                                      uint32_t long_cb);

SRSLTE_API int srslte_tdec_simd_inter_get_nof_iterations_cb(srslte_tdec_simd_inter_t * h, 
                                                      uint32_t cb_idx);

SRSLTE_API int srslte_tdec_simd_inter_reset_cb(srslte_tdec_simd_inter_t * h, 
                                         uint32_t cb_idx);

SRSLTE_API void srslte_tdec_simd_inter_iteration(srslte_tdec_simd_inter_t * h, 
                                           int16_t * input[SRSLTE_TDEC_MAX_NPAR],
                                           uint32_t nof_cb,
                                           uint32_t long_cb);

SRSLTE_API void srslte_tdec_simd_inter_decision(srslte_tdec_simd_inter_t * h, 
                                          uint8_t *output[SRSLTE_TDEC_MAX_NPAR],
                                          uint32_t nof_cb,
                                          uint32_t long_cb);

SRSLTE_API void srslte_tdec_simd_inter_decision_byte(srslte_tdec_simd_inter_t * h, 
                                               uint8_t *output[SRSLTE_TDEC_MAX_NPAR],
                                               uint32_t nof_cb,
                                               uint32_t long_cb); 

SRSLTE_API void srslte_tdec_simd_inter_decision_byte_cb(srslte_tdec_simd_inter_t * h, 
                                                  uint8_t *output, 
                                                  uint32_t cbidx, 
                                                  uint32_t long_cb); 

SRSLTE_API int srslte_tdec_simd_inter_run_all(srslte_tdec_simd_inter_t * h, 
                                        int16_t *input[SRSLTE_TDEC_MAX_NPAR],
                                        uint8_t *output[SRSLTE_TDEC_MAX_NPAR],
                                        uint32_t nof_iterations, 
                                        uint32_t nof_cb,
                                        uint32_t long_cb);

#endif // SRSLTE_TURBODECODER_SIMD_INTER_H
