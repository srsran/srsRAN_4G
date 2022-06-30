/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_OFDM_H
#define SRSRAN_OFDM_H

/**********************************************************************************************
 *  File:         ofdm.h
 *
 *  Description:  OFDM modulation object.
 *                Used in generation of downlink OFDM signals.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6
 *********************************************************************************************/

#include <strings.h>

#include "srsran/config.h"
#include "srsran/phy/cfr/cfr.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft.h"

/**
 * @struct srsran_ofdm_cfg_t
 * Contains the generic OFDM modulator configuration. The structure must be initialised to all zeros before being
 * filled. Only compulsory parameters need to be filled prior initialization.
 *
 * This structure must be used with init functions srsran_ofdm_rx_init_cfg and srsran_ofdm_tx_init_cfg. These provide
 * more flexible options.
 */
typedef struct SRSRAN_API {
  // Compulsory parameters
  uint32_t    nof_prb;    ///< Number of Resource Block
  cf_t*       in_buffer;  ///< Input buffer pointer
  cf_t*       out_buffer; ///< Output buffer pointer
  srsran_cp_t cp;         ///< Cyclic prefix type

  // Optional parameters
  srsran_sf_t      sf_type;          ///< Subframe type, normal or MBSFN
  bool             normalize;        ///< Normalization flag, it divides the output by square root of the symbol size
  float            freq_shift_f;     ///< Frequency shift, normalised by sampling rate (used in UL)
  float            rx_window_offset; ///< DFT Window offset in CP portion (0-1), RX only
  uint32_t         symbol_sz;        ///< Symbol size, forces a given symbol size for the number of PRB
  bool             keep_dc;          ///< If true, it does not remove the DC
  double           phase_compensation_hz; ///< Carrier frequency in Hz for phase compensation, set to 0 to disable
  srsran_cfr_cfg_t cfr_tx_cfg;            ///< Tx CFR configuration
} srsran_ofdm_cfg_t;

/**
 * @struct srsran_ofdm_t
 * OFDM object, common for Tx and Rx
 */
typedef struct SRSRAN_API {
  srsran_ofdm_cfg_t cfg;
  srsran_dft_plan_t fft_plan;
  srsran_dft_plan_t fft_plan_sf[2];
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
  cf_t              phase_compensation[SRSRAN_MAX_NSYMB * SRSRAN_NOF_SLOTS_PER_SF];
  srsran_cfr_t      tx_cfr; ///< Tx CFR object
} srsran_ofdm_t;

/**
 * @brief Initialises or reconfigures OFDM receiver
 *
 * @note The reconfiguration of the OFDM object considers only CP, number of PRB and optionally the FFT size
 * @attention The OFDM object must be zeroed externally prior calling the initialization for first time
 *
 * @param q OFDM object
 * @param cfg OFDM configuration
 * @return SRSRAN_SUCCESS if the initialization/reconfiguration is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ofdm_rx_init_cfg(srsran_ofdm_t* q, srsran_ofdm_cfg_t* cfg);

/**
 * @brief Initialises or reconfigures OFDM transmitter
 *
 * @note The reconfiguration of the OFDM object considers only CP, number of PRB and optionally the FFT size
 * @attention The OFDM object must be zeroed externally prior calling the initialization for first time
 *
 * @param q OFDM object
 * @param cfg OFDM configuration
 * @return SRSRAN_SUCCESS if the initialization/reconfiguration is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ofdm_tx_init_cfg(srsran_ofdm_t* q, srsran_ofdm_cfg_t* cfg);

SRSRAN_API int
srsran_ofdm_rx_init_mbsfn(srsran_ofdm_t* q, srsran_cp_t cp_type, cf_t* in_buffer, cf_t* out_buffer, uint32_t max_prb);

SRSRAN_API int
srsran_ofdm_rx_init(srsran_ofdm_t* q, srsran_cp_t cp_type, cf_t* in_buffer, cf_t* out_buffer, uint32_t max_prb);

SRSRAN_API int srsran_ofdm_tx_set_prb(srsran_ofdm_t* q, srsran_cp_t cp, uint32_t nof_prb);

SRSRAN_API int srsran_ofdm_rx_set_prb(srsran_ofdm_t* q, srsran_cp_t cp, uint32_t nof_prb);

SRSRAN_API void srsran_ofdm_rx_free(srsran_ofdm_t* q);

SRSRAN_API void srsran_ofdm_rx_sf(srsran_ofdm_t* q);

SRSRAN_API void srsran_ofdm_rx_sf_ng(srsran_ofdm_t* q, cf_t* input, cf_t* output);

SRSRAN_API int
srsran_ofdm_tx_init(srsran_ofdm_t* q, srsran_cp_t cp_type, cf_t* in_buffer, cf_t* out_buffer, uint32_t nof_prb);

SRSRAN_API int
srsran_ofdm_tx_init_mbsfn(srsran_ofdm_t* q, srsran_cp_t cp, cf_t* in_buffer, cf_t* out_buffer, uint32_t nof_prb);

SRSRAN_API void srsran_ofdm_tx_free(srsran_ofdm_t* q);

SRSRAN_API void srsran_ofdm_tx_sf(srsran_ofdm_t* q);

SRSRAN_API int srsran_ofdm_set_freq_shift(srsran_ofdm_t* q, float freq_shift);

SRSRAN_API void srsran_ofdm_set_normalize(srsran_ofdm_t* q, bool normalize_enable);

SRSRAN_API int srsran_ofdm_set_phase_compensation(srsran_ofdm_t* q, double center_freq_hz);

SRSRAN_API void srsran_ofdm_set_non_mbsfn_region(srsran_ofdm_t* q, uint8_t non_mbsfn_region);

SRSRAN_API int srsran_ofdm_set_cfr(srsran_ofdm_t* q, srsran_cfr_cfg_t* cfr);

#endif // SRSRAN_OFDM_H
