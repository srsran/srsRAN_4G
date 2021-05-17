/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#define SRSRAN_SSB_NOF_POSITION 64

/**
 * @brief Describes SSB object initialization arguments
 */
typedef struct SRSRAN_API {
  double                      max_srate_hz;   ///< Maximum sampling rate in Hz, set to zero to use default
  srsran_subcarrier_spacing_t min_scs;        ///< Minimum subcarrier spacing
  bool                        enable_search;  ///< Enables PSS/SSS blind search
  bool                        enable_measure; ///< Enables PSS/SSS CSI measurements and frequency domain search
  bool                        enable_encode;  ///< Enables PBCH Encoder
  bool                        enable_decode;  ///< Enables PBCH Decoder
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
  bool position[SRSRAN_SSB_NOF_POSITION];     ///< Indicates the time domain positions of the transmitted SS-blocks
  srsran_duplex_mode_t duplex_mode;           ///< Set to true if the spectrum is paired (FDD)
  uint32_t             periodicity_ms;        ///< SSB periodicity in ms
  float                beta_pss;              ////< PSS power allocation
  float                beta_sss;              ////< SSS power allocation
  float                beta_pbch;             ////< PBCH power allocation
  float                beta_pbch_dmrs;        ////< PBCH DMRS power allocation
} srsran_ssb_cfg_t;

/**
 * @brief Describes SSB object
 */
typedef struct SRSRAN_API {
  srsran_ssb_args_t args; ///< Stores initialization arguments
  srsran_ssb_cfg_t  cfg;  ///< Stores last configuration

  /// Sampling rate dependent parameters
  float    scs_hz;                           ///< Subcarrier spacing in Hz
  uint32_t max_symbol_sz;                    ///< Maximum symbol size given the minimum supported SCS and sampling rate
  uint32_t max_corr_sz;                      ///< Maximum correlation size
  uint32_t symbol_sz;                        ///< Current SSB symbol size (for the given base-band sampling rate)
  uint32_t corr_sz;                          ///< Correlation size
  uint32_t corr_window;                      ///< Correlation window length
  int32_t  f_offset;                         ///< Current SSB integer frequency offset (multiple of SCS)
  uint32_t t_offset;                         ///< Current SSB integer time offset (number of samples)
  uint32_t cp_sz[SRSRAN_SSB_DURATION_NSYMB]; ///< CP length for each SSB symbol

  /// Internal Objects
  srsran_dft_plan_t ifft;      ///< IFFT object for modulating the SSB
  srsran_dft_plan_t fft;       ///< FFT object for demodulate the SSB.
  srsran_dft_plan_t fft_corr;  ///< FFT for correlation
  srsran_dft_plan_t ifft_corr; ///< IFFT for correlation

  /// Frequency/Time domain temporal data
  cf_t* tmp_freq;                     ///< Temporal frequency domain buffer
  cf_t* tmp_time;                     ///< Temporal time domain buffer
  cf_t* tmp_corr;                     ///< Temporal correlation frequency domain buffer
  cf_t* pss_seq[SRSRAN_NOF_NID_2_NR]; ///< Possible frequency domain PSS for find
} srsran_ssb_t;

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
 * @param q SSB object
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ssb_decode_pbch(srsran_ssb_t* q, const cf_t* in, srsran_pbch_msg_nr_t* msg);

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
 * @param q NR PSS object
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
 * @param q NR PSS object
 * @param N_id Physical Cell Identifier
 * @param in Base-band signal
 * @param meas SSB-based CSI measurement
 * @return SRSRAN_SUCCESS if the parameters are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int
srsran_ssb_csi_measure(srsran_ssb_t* q, uint32_t N_id, const cf_t* in, srsran_csi_trs_measurements_t* meas);

#endif // SRSRAN_SSB_H
