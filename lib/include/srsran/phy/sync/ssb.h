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

#ifndef SRSRAN_SSB_H
#define SRSRAN_SSB_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/dft/dft.h"
#include "srsran/phy/phch/pbch_nr.h"
#include <inttypes.h>

/**
 * @brief Default SSB maximum sampling rate
 */
#define SRSRAN_SSB_DEFAULT_MAX_SRATE_HZ 61.44e6

/**
 * @brief Default SSB minimum subcarrier spacing
 */
#define SRSRAN_SSB_DEFAULT_MIN_SCS srsran_subcarrier_spacing_15kHz

/**
 * @brief Default beta value, used in case they are set to zero
 */
#define SRSRAN_SSB_DEFAULT_BETA 1.0f

/**
 * @brief Maximum number of SSB positions in burst. Defined in TS 38.331 ServingCellConfigCommon, ssb-PositionsInBurst
 */
#define SRSRAN_SSB_NOF_CANDIDATES 64

/**
 * @brief Describes SSB object initialization arguments
 */
typedef struct SRSRAN_API {
  double                      max_srate_hz;       ///< Maximum sampling rate in Hz, set to zero to use default
  srsran_subcarrier_spacing_t min_scs;            ///< Minimum subcarrier spacing
  bool                        enable_search;      ///< Enables PSS/SSS blind search
  bool                        enable_measure;     ///< Enables PSS/SSS CSI measurements and frequency domain search
  bool                        enable_encode;      ///< Enables PBCH Encoder
  bool                        enable_decode;      ///< Enables PBCH Decoder
  bool                        disable_polar_simd; ///< Disables polar encoder/decoder SIMD acceleration
  float                       pbch_dmrs_thr;      ///< NR-PBCH DMRS threshold for blind decoding, set to 0 for default
} srsran_ssb_args_t;

/**
 * @brief Describes SSB configuration arguments
 */
typedef struct SRSRAN_API {
  double                      srate_hz;       ///< Current sampling rate in Hz
  double                      center_freq_hz; ///< Base-band center frequency in Hz
  double                      ssb_freq_hz;    ///< SSB center frequency
  srsran_subcarrier_spacing_t scs;            ///< SSB configured Subcarrier spacing
  srsran_ssb_patern_t         pattern;        ///< SSB pattern as defined in TS 38.313 section 4.1 Cell search
  srsran_duplex_mode_t        duplex_mode;    ///< Set to true if the spectrum is paired (FDD)
  uint32_t                    periodicity_ms; ///< SSB periodicity in ms
  float                       beta_pss;       ///< PSS power allocation
  float                       beta_sss;       ///< SSS power allocation
  float                       beta_pbch;      ///< PBCH power allocation
  float                       beta_pbch_dmrs; ///< PBCH DMRS power allocation
  float                       scaling;        ///< IFFT scaling (used for modulation), set to 0 for default
} srsran_ssb_cfg_t;

/**
 * @brief Describes SSB object
 */
typedef struct SRSRAN_API {
  srsran_ssb_args_t args; ///< Stores initialization arguments
  srsran_ssb_cfg_t  cfg;  ///< Stores last configuration

  /// Sampling rate dependent parameters
  float    scs_hz;        ///< Subcarrier spacing in Hz
  uint32_t max_sf_sz;     ///< Maximum subframe size at the specified sampling rate
  uint32_t max_symbol_sz; ///< Maximum symbol size given the minimum supported SCS and sampling rate
  uint32_t max_corr_sz;   ///< Maximum correlation size
  uint32_t max_ssb_sz;    ///< Maximum SSB size in samples at the configured sampling rate
  uint32_t sf_sz;         ///< Current subframe size at the specified sampling rate
  uint32_t symbol_sz;     ///< Current SSB symbol size (for the given base-band sampling rate)
  uint32_t corr_sz;       ///< Correlation size
  uint32_t corr_window;   ///< Correlation window length
  uint32_t ssb_sz;        ///< SSB size in samples at the configured sampling rate
  int32_t  f_offset;      ///< Current SSB integer frequency offset (multiple of SCS)
  uint32_t cp_sz;         ///< CP length for the given symbol size

  /// Other parameters
  uint32_t l_first[SRSRAN_SSB_NOF_CANDIDATES]; ///< Start symbol for each SSB candidate in half radio frame
  uint32_t Lmax;                               ///< Number of SSB candidates

  /// Internal Objects
  srsran_dft_plan_t ifft;      ///< IFFT object for modulating the SSB
  srsran_dft_plan_t fft;       ///< FFT object for demodulate the SSB.
  srsran_dft_plan_t fft_corr;  ///< FFT for correlation
  srsran_dft_plan_t ifft_corr; ///< IFFT for correlation
  srsran_pbch_nr_t  pbch;      ///< PBCH encoder and decoder

  /// Frequency/Time domain temporal data
  cf_t* tmp_freq;                     ///< Temporal frequency domain buffer
  cf_t* tmp_time;                     ///< Temporal time domain buffer
  cf_t* tmp_corr;                     ///< Temporal correlation frequency domain buffer
  cf_t* sf_buffer;                    ///< subframe buffer
  cf_t* pss_seq[SRSRAN_NOF_NID_2_NR]; ///< Possible frequency domain PSS for find
} srsran_ssb_t;

/**
 * @brief Describes an SSB search result
 * @note if pbch.crc is true, SSB transmission is found and decoded. Otherwise, no SSB transmission has been decoded
 */
typedef struct {
  uint32_t             N_id;     ///< Most suitable physical cell identifier
  uint32_t             t_offset; ///< Time offset in the input samples
  srsran_pbch_msg_nr_t pbch_msg; ///< Physical broadcast channel message of the most suitable SSB candidate
} srsran_ssb_search_res_t;

/**
 * @brief Initialises configures NR SSB with the given arguments
 * @param q SSB object
 * @param args NR PSS initialization arguments
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ssb_init(srsran_ssb_t* q, const srsran_ssb_args_t* args);

/**
 * @brief Frees NR SSB object
 * @param q SSB object
 */
SRSRAN_API void srsran_ssb_free(srsran_ssb_t* q);

/**
 * @brief Sets SSB configuration with the current SSB configuration
 * @param q SSB object
 * @param cfg Current SSB configuration
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ssb_set_cfg(srsran_ssb_t* q, const srsran_ssb_cfg_t* cfg);
/**
 * @brief Decodes PBCH in the given time domain signal
 * @note It currently expects an input buffer of half radio frame
 * @param q SSB object
 * @param N_id Physical Cell Identifier
 * @param n_hf Number of hald radio frame, 0 or 1
 * @param ssb_idx SSB candidate index
 * @param in Input baseband buffer
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ssb_decode_pbch(srsran_ssb_t*         q,
                                      uint32_t              N_id,
                                      uint32_t              n_hf,
                                      uint32_t              ssb_idx,
                                      const cf_t*           in,
                                      srsran_pbch_msg_nr_t* msg);

/**
 * @brief Searches for an SSB transmission and decodes the PBCH message
 * @param q SSB object
 * @param in Input baseband buffer
 * @param nof_samples Number of samples available in the buffer
 * @param res SSB Search result
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ssb_search(srsran_ssb_t* q, const cf_t* in, uint32_t nof_samples, srsran_ssb_search_res_t* res);

/**
 * @brief Decides if the SSB object is configured and a given subframe is configured for SSB transmission
 * @param q SSB object
 * @param sf_idx Subframe index within the radio frame
 * @return true if the SSB object is configured and SSB is transmitted, false otherwise
 */
SRSRAN_API bool srsran_ssb_send(srsran_ssb_t* q, uint32_t sf_idx);

/**
 * @brief Adds SSB to a given signal in time domain
 * @param q SSB object
 * @param N_id Physical Cell Identifier
 * @param msg NR PBCH message to transmit
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int
srsran_ssb_add(srsran_ssb_t* q, uint32_t N_id, const srsran_pbch_msg_nr_t* msg, const cf_t* in, cf_t* out);

/**
 * @brief Perform cell search and measurement
 * @note This function assumes the SSB transmission is aligned with the input base-band signal
 * @param q SSB object
 * @param in Base-band signal buffer
 * @param N_id Physical Cell Identifier of the most suitable cell identifier
 * @param meas SSB-based CSI measurement of the most suitable cell identifier
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ssb_csi_search(srsran_ssb_t*                  q,
                                     const cf_t*                    in,
                                     uint32_t                       nof_samples,
                                     uint32_t*                      N_id,
                                     srsran_csi_trs_measurements_t* meas);

/**
 * @brief Perform Channel State Information (CSI) measurement from the SSB
 * @param q SSB object
 * @param N_id Physical Cell Identifier
 * @param ssb_idx SSB candidate index
 * @param in Base-band signal
 * @param meas SSB-based CSI measurement
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ssb_csi_measure(srsran_ssb_t*                  q,
                                      uint32_t                       N_id,
                                      uint32_t                       ssb_idx,
                                      const cf_t*                    in,
                                      srsran_csi_trs_measurements_t* meas);

/**
 * @brief Find SSB signal in a given time domain subframe buffer
 * @param q SSB object
 * @param sf_buffer subframe buffer with 1ms worth of samples
 * @param N_id Physical cell identifier to find
 * @param meas Measurements performed on the found peak
 * @param pbch_msg PBCH decoded message
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ssb_find(srsran_ssb_t*                  q,
                               const cf_t*                    sf_buffer,
                               uint32_t                       N_id,
                               srsran_csi_trs_measurements_t* meas,
                               srsran_pbch_msg_nr_t*          pbch_msg);

/**
 * @brief Track SSB by performing measurements and decoding PBCH
 * @param q SSB object
 * @param sf_buffer subframe buffer with 1ms worth of samples
 * @param N_id Physical cell identifier to find
 * @param ssb_idx SSB candidate index
 * @param n_hf Number of half frame
 * @param meas Measurements perform
 * @param pbch_msg PBCH decoded message
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ssb_track(srsran_ssb_t*                  q,
                                const cf_t*                    sf_buffer,
                                uint32_t                       N_id,
                                uint32_t                       ssb_idx,
                                uint32_t                       n_hf,
                                srsran_csi_trs_measurements_t* meas,
                                srsran_pbch_msg_nr_t*          pbch_msg);

/**
 * @brief Calculates the subframe index within the radio frame of a given SSB candidate for the SSB object
 * @param q SSB object
 * @param ssb_idx SSB candidate index
 * @param half_frame Indicates whether it is half frame
 * @return The subframe index
 */
SRSRAN_API uint32_t srsran_ssb_candidate_sf_idx(const srsran_ssb_t* q, uint32_t ssb_idx, bool half_frame);

/**
 * @brief Calculates the SSB offset within the subframe of a given SSB candidate for the SSB object
 * @param q SSB object
 * @param ssb_idx SSB candidate index
 * @return The sample offset within the subframe
 */
SRSRAN_API uint32_t srsran_ssb_candidate_sf_offset(const srsran_ssb_t* q, uint32_t ssb_idx);

#endif // SRSRAN_SSB_H
