/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_TURBODECODER_H
#define SRSRAN_TURBODECODER_H

#include "srsran/config.h"
#include "srsran/phy/fec/cbsegm.h"
#include "srsran/phy/fec/turbo/tc_interl.h"

#define SRSRAN_TCOD_RATE 3
#define SRSRAN_TCOD_TOTALTAIL 12

#define SRSRAN_TCOD_MAX_LEN_CB 6144

// Expect the input to be aligned for sub-block window processing.
#define SRSRAN_TDEC_EXPECT_INPUT_SB 1

// Include interfaces for 8 and 16 bit decoder implementations
#define LLR_IS_8BIT
#include "srsran/phy/fec/turbo/turbodecoder_impl.h"
#undef LLR_IS_8BIT

#define LLR_IS_16BIT
#include "srsran/phy/fec/turbo/turbodecoder_impl.h"
#undef LLR_IS_16BIT

#define SRSRAN_TDEC_NOF_AUTO_MODES_8 2
#define SRSRAN_TDEC_NOF_AUTO_MODES_16 3

typedef enum { SRSRAN_TDEC_8, SRSRAN_TDEC_16 } srsran_tdec_llr_type_t;

typedef struct SRSRAN_API {
  uint32_t max_long_cb;

  void*                     dec8_hdlr[SRSRAN_TDEC_NOF_AUTO_MODES_8];
  void*                     dec16_hdlr[SRSRAN_TDEC_NOF_AUTO_MODES_16];
  srsran_tdec_8bit_impl_t*  dec8[SRSRAN_TDEC_NOF_AUTO_MODES_8];
  srsran_tdec_16bit_impl_t* dec16[SRSRAN_TDEC_NOF_AUTO_MODES_16];
  int                       nof_blocks8[SRSRAN_TDEC_NOF_AUTO_MODES_8];
  int                       nof_blocks16[SRSRAN_TDEC_NOF_AUTO_MODES_16];

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

  srsran_tdec_impl_type_t dec_type;

  srsran_tdec_llr_type_t current_llr_type;
  uint32_t               current_dec;
  uint32_t               current_long_cb;
  uint32_t               current_inter_idx;
  int                    current_cbidx;
  srsran_tc_interl_t     interleaver[4][SRSRAN_NOF_TC_CB_SIZES];
  int                    n_iter;
} srsran_tdec_t;

SRSRAN_API int srsran_tdec_init(srsran_tdec_t* h, uint32_t max_long_cb);

SRSRAN_API int srsran_tdec_init_manual(srsran_tdec_t* h, uint32_t max_long_cb, srsran_tdec_impl_type_t dec_type);

SRSRAN_API void srsran_tdec_free(srsran_tdec_t* h);

SRSRAN_API void srsran_tdec_force_not_sb(srsran_tdec_t* h);

SRSRAN_API int srsran_tdec_new_cb(srsran_tdec_t* h, uint32_t long_cb);

SRSRAN_API int srsran_tdec_get_nof_iterations(srsran_tdec_t* h);

SRSRAN_API uint32_t srsran_tdec_autoimp_get_subblocks(uint32_t long_cb);

SRSRAN_API uint32_t srsran_tdec_autoimp_get_subblocks_8bit(uint32_t long_cb);

SRSRAN_API void srsran_tdec_iteration(srsran_tdec_t* h, int16_t* input, uint8_t* output);

SRSRAN_API int
srsran_tdec_run_all(srsran_tdec_t* h, int16_t* input, uint8_t* output, uint32_t nof_iterations, uint32_t long_cb);

SRSRAN_API void srsran_tdec_iteration_8bit(srsran_tdec_t* h, int8_t* input, uint8_t* output);

SRSRAN_API int
srsran_tdec_run_all_8bit(srsran_tdec_t* h, int8_t* input, uint8_t* output, uint32_t nof_iterations, uint32_t long_cb);

#endif // SRSRAN_TURBODECODER_H
