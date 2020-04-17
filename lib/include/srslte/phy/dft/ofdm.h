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

#ifndef SRSLTE_OFDM_H
#define SRSLTE_OFDM_H

/**********************************************************************************************
 *  File:         ofdm.h
 *
 *  Description:  OFDM modulation object.
 *                Used in generation of downlink OFDM signals.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6
 *********************************************************************************************/

#include <strings.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/dft.h"

/**
 * @struct srslte_ofdm_cfg_t
 * Contains the generic OFDM modulator configuration. The structure must be initialised to all zeros before being
 * filled. Only compulsory parameters need to be filled prior initialization.
 *
 * This structure must be used with init functions srslte_ofdm_rx_init_cfg and srslte_ofdm_tx_init_cfg. These provide
 * more flexible options.
 */
typedef struct SRSLTE_API {
  // Compulsory parameters
  uint32_t    nof_prb;    //< Number of Resource Block
  cf_t*       in_buffer;  //< Input bnuffer pointer
  cf_t*       out_buffer; //< Output buffer pointer
  srslte_cp_t cp;         //< Cyclic prefix type

  // Optional parameters
  srslte_sf_t sf_type;          //< Subframe type, normal or MBSFN
  bool        normalize;        //< Normalization flag, it divides the output by square root of the symbol size
  float       freq_shift_f;     //< Frequency shift, normalised by sampling rate (used in UL)
  float       rx_window_offset; //< DFT Window offset in CP portion (0-1), RX only
  uint32_t    symbol_sz;        //< Symbol size, forces a given symbol size for the number of PRB
} srslte_ofdm_cfg_t;

/**
 * @struct srslte_ofdm_t
 * OFDM object, common for Tx and Rx
 */
typedef struct SRSLTE_API {
  srslte_ofdm_cfg_t cfg;
  srslte_dft_plan_t fft_plan;
  srslte_dft_plan_t fft_plan_sf[2];
  uint32_t          max_prb;
  uint32_t          nof_symbols;
  uint32_t          nof_guards;
  uint32_t          nof_re;
  uint32_t          slot_sz;
  uint32_t          sf_sz;
  cf_t*             tmp; // for removing zero padding
  bool              mbsfn_subframe;
  uint32_t          mbsfn_guard_len;
  uint32_t          nof_symbols_mbsfn;
  uint8_t           non_mbsfn_region;
  uint32_t          window_offset_n;
  cf_t*             shift_buffer;
  cf_t*             window_offset_buffer;
} srslte_ofdm_t;

SRSLTE_API int srslte_ofdm_rx_init_cfg(srslte_ofdm_t* q, srslte_ofdm_cfg_t* cfg);

SRSLTE_API int srslte_ofdm_tx_init_cfg(srslte_ofdm_t* q, srslte_ofdm_cfg_t* cfg);

SRSLTE_API int
srslte_ofdm_rx_init_mbsfn(srslte_ofdm_t* q, srslte_cp_t cp_type, cf_t* in_buffer, cf_t* out_buffer, uint32_t max_prb);

SRSLTE_API int
srslte_ofdm_rx_init(srslte_ofdm_t* q, srslte_cp_t cp_type, cf_t* in_buffer, cf_t* out_buffer, uint32_t max_prb);

SRSLTE_API int srslte_ofdm_tx_set_prb(srslte_ofdm_t* q, srslte_cp_t cp, uint32_t nof_prb);

SRSLTE_API int srslte_ofdm_rx_set_prb(srslte_ofdm_t* q, srslte_cp_t cp, uint32_t nof_prb);

SRSLTE_API void srslte_ofdm_rx_free(srslte_ofdm_t* q);

SRSLTE_API void srslte_ofdm_rx_sf(srslte_ofdm_t* q);

SRSLTE_API void srslte_ofdm_rx_sf_ng(srslte_ofdm_t* q, cf_t* input, cf_t* output);

SRSLTE_API int
srslte_ofdm_tx_init(srslte_ofdm_t* q, srslte_cp_t cp_type, cf_t* in_buffer, cf_t* out_buffer, uint32_t nof_prb);

SRSLTE_API int
srslte_ofdm_tx_init_mbsfn(srslte_ofdm_t* q, srslte_cp_t cp, cf_t* in_buffer, cf_t* out_buffer, uint32_t nof_prb);

SRSLTE_API void srslte_ofdm_tx_free(srslte_ofdm_t* q);

SRSLTE_API void srslte_ofdm_tx_sf(srslte_ofdm_t* q);

SRSLTE_API int srslte_ofdm_set_freq_shift(srslte_ofdm_t* q, float freq_shift);

SRSLTE_API void srslte_ofdm_set_normalize(srslte_ofdm_t* q, bool normalize_enable);

SRSLTE_API void srslte_ofdm_set_non_mbsfn_region(srslte_ofdm_t* q, uint8_t non_mbsfn_region);

#endif // SRSLTE_OFDM_H
