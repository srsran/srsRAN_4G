/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#ifndef SRSLTE_TURBODECODER_H
#define SRSLTE_TURBODECODER_H

#include "srslte/config.h"
#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/tc_interl.h"

#define SRSLTE_TCOD_RATE 3
#define SRSLTE_TCOD_TOTALTAIL 12

#define SRSLTE_TCOD_MAX_LEN_CB 6144

// Expect the input to be aligned for sub-block window processing.
#define SRSLTE_TDEC_EXPECT_INPUT_SB 1

// Include interfaces for 8 and 16 bit decoder implementations
#define LLR_IS_8BIT
#include "srslte/phy/fec/turbodecoder_impl.h"
#undef LLR_IS_8BIT

#define LLR_IS_16BIT
#include "srslte/phy/fec/turbodecoder_impl.h"
#undef LLR_IS_16BIT

#define SRSLTE_TDEC_NOF_AUTO_MODES_8 2
#define SRSLTE_TDEC_NOF_AUTO_MODES_16 3

typedef enum { SRSLTE_TDEC_8, SRSLTE_TDEC_16 } srslte_tdec_llr_type_t;

typedef struct SRSLTE_API {
  uint32_t max_long_cb;

  void*                     dec8_hdlr[SRSLTE_TDEC_NOF_AUTO_MODES_8];
  void*                     dec16_hdlr[SRSLTE_TDEC_NOF_AUTO_MODES_16];
  srslte_tdec_8bit_impl_t*  dec8[SRSLTE_TDEC_NOF_AUTO_MODES_8];
  srslte_tdec_16bit_impl_t* dec16[SRSLTE_TDEC_NOF_AUTO_MODES_16];
  int                       nof_blocks8[SRSLTE_TDEC_NOF_AUTO_MODES_8];
  int                       nof_blocks16[SRSLTE_TDEC_NOF_AUTO_MODES_16];

  // Declare as void types as can be int8 or int16
  void* app1;
  void* app2;
  void* ext1;
  void* ext2;
  void* syst0;
  void* parity0;
  void* parity1;

  void* input_conv;

  bool force_not_sb;

  srslte_tdec_impl_type_t dec_type;

  srslte_tdec_llr_type_t current_llr_type;
  uint32_t               current_dec;
  uint32_t               current_long_cb;
  uint32_t               current_inter_idx;
  int                    current_cbidx;
  srslte_tc_interl_t     interleaver[4][SRSLTE_NOF_TC_CB_SIZES];
  int                    n_iter;
} srslte_tdec_t;

SRSLTE_API int srslte_tdec_init(srslte_tdec_t* h, uint32_t max_long_cb);

SRSLTE_API int srslte_tdec_init_manual(srslte_tdec_t* h, uint32_t max_long_cb, srslte_tdec_impl_type_t dec_type);

SRSLTE_API void srslte_tdec_free(srslte_tdec_t* h);

SRSLTE_API void srslte_tdec_force_not_sb(srslte_tdec_t* h);

SRSLTE_API int srslte_tdec_new_cb(srslte_tdec_t* h, uint32_t long_cb);

SRSLTE_API int srslte_tdec_get_nof_iterations(srslte_tdec_t* h);

SRSLTE_API uint32_t srslte_tdec_autoimp_get_subblocks(uint32_t long_cb);

SRSLTE_API uint32_t srslte_tdec_autoimp_get_subblocks_8bit(uint32_t long_cb);

SRSLTE_API void srslte_tdec_iteration(srslte_tdec_t* h, int16_t* input, uint8_t* output);

SRSLTE_API int
srslte_tdec_run_all(srslte_tdec_t* h, int16_t* input, uint8_t* output, uint32_t nof_iterations, uint32_t long_cb);

SRSLTE_API void srslte_tdec_iteration_8bit(srslte_tdec_t* h, int8_t* input, uint8_t* output);

SRSLTE_API int
srslte_tdec_run_all_8bit(srslte_tdec_t* h, int8_t* input, uint8_t* output, uint32_t nof_iterations, uint32_t long_cb);

#endif // SRSLTE_TURBODECODER_H
