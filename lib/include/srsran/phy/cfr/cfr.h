/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_CFR_H
#define SRSRAN_CFR_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft.h"

#define CFR_EMA_INIT_AVG_PWR 0.1

/**
 * @brief CFR manual threshold or PAPR limiting with CMA or EMA power averaging
 */
typedef enum SRSRAN_API {
  SRSRAN_CFR_THR_INVALID,
  SRSRAN_CFR_THR_MANUAL,
  SRSRAN_CFR_THR_AUTO_CMA,
  SRSRAN_CFR_THR_AUTO_EMA,
  SRSRAN_CFR_NOF_MODES
} srsran_cfr_mode_t;

/**
 * @brief CFR module configuration arguments
 */
typedef struct SRSRAN_API {
  bool              cfr_enable;
  srsran_cfr_mode_t cfr_mode;
  // always used (mandatory)
  uint32_t symbol_bw; ///< OFDM symbol bandwidth, in FFT bins
  uint32_t symbol_sz; ///< OFDM symbol size (in samples). This is the FFT size
  float    alpha;     ///< Alpha parameter of the clipping algorithm
  bool     dc_sc;     ///< Take into account the DC subcarrier for the filter BW

  // SRSRAN_CFR_THR_MANUAL mode parameters
  float manual_thr; ///< Fixed threshold used in SRSRAN_CFR_THR_MANUAL mode

  // SRSRAN_CFR_THR_AUTO_CMA and SRSRAN_CFR_THR_AUTO_EMA mode parameters
  bool  measure_out_papr; ///< Enable / disable output PAPR measurement
  float max_papr_db;      ///< Input PAPR threshold used in SRSRAN_CFR_THR_AUTO_CMA and SRSRAN_CFR_THR_AUTO_EMA modes
  float ema_alpha;        ///< EMA alpha parameter for avg power calculation, used in SRSRAN_CFR_THR_AUTO_EMA mode
} srsran_cfr_cfg_t;

typedef struct SRSRAN_API {
  srsran_cfr_cfg_t cfg;
  float            max_papr_lin;

  srsran_dft_plan_t fft_plan;
  srsran_dft_plan_t ifft_plan;
  float*            lpf_spectrum; ///< FFT filter spectrum
  uint32_t          lpf_bw;       ///< Bandwidth of the LPF

  float* abs_buffer_in;  ///< Store the input absolute value
  float* abs_buffer_out; ///< Store the output absolute value
  cf_t*  peak_buffer;

  float pwr_avg_in;  ///< store the avg. input power with MA or EMA averaging
  float pwr_avg_out; ///< store the avg. output power with MA or EMA averaging

  // Power average buffers, used in SRSRAN_CFR_THR_AUTO_CMA mode
  uint64_t cma_n;
} srsran_cfr_t;

SRSRAN_API int srsran_cfr_init(srsran_cfr_t* q, srsran_cfr_cfg_t* cfg);

/**
 * @brief Applies the CFR algorithm to the time domain OFDM symbols
 *
 * @attention This function must be called once per symbol, and it will process q->symbol_sz samples
 *
 * @param[in]  q    The CFR object and configuration
 * @param[in]  in   Input buffer containing the time domain OFDM symbol without CP
 * @param[out] out  Output buffer with the processed OFDM symbol
 * @return SRSRAN_SUCCESS if the CFR object is initialised, otherwise SRSRAN_ERROR
 */
SRSRAN_API void srsran_cfr_process(srsran_cfr_t* q, cf_t* in, cf_t* out);

SRSRAN_API void srsran_cfr_free(srsran_cfr_t* q);

/**
 * @brief Checks the validity of the CFR algorithm parameters.
 *
 * @attention Does not check symbol size and bandwidth
 *
 * @param[in] cfr_conf the CFR configuration
 * @return true if the configuration is valid, false otherwise
 */
SRSRAN_API bool srsran_cfr_params_valid(srsran_cfr_cfg_t* cfr_conf);

/**
 * @brief Sets the manual threshold of the CFR (used in manual mode).
 *
 * @attention this is not thread-safe
 *
 * @param[in] q the CFR object
 * @return SRSRAN_SUCCESS if successful, SRSRAN_ERROR or SRSRAN_ERROR_INVALID_INPUTS otherwise
 */
SRSRAN_API int srsran_cfr_set_threshold(srsran_cfr_t* q, float thres);

/**
 * @brief Sets the papr target of the CFR (used in auto modes).
 *
 * @attention this is not thread-safe
 *
 * @param[in] q the CFR object
 * @return SRSRAN_SUCCESS if successful, SRSRAN_ERROR or SRSRAN_ERROR_INVALID_INPUTS otherwise
 */
SRSRAN_API int srsran_cfr_set_papr(srsran_cfr_t* q, float papr);

/**
 * @brief Converts a string representing a CFR mode from the config files into srsran_cfr_mode_t type
 *
 * @param[in]  mode_str   the cfr.mode string coming from the config file
 * @return SRSRAN_CFR_THR_INVALID if mode_str is empty,
 * SRSRAN_CFR_THR_INVALID if mode_str is not recognised,
 * otherwise it returns the corresponding srsran_cfr_mode_t value.
 */
SRSRAN_API srsran_cfr_mode_t srsran_cfr_str2mode(const char* mode_str);

#endif // SRSRAN_CFR_H
