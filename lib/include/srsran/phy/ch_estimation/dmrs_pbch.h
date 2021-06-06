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

#ifndef SRSRAN_DMRS_PBCH_H
#define SRSRAN_DMRS_PBCH_H

#include "srsran/phy/common/phy_common_nr.h"

/**
 * @brief Describes the DeModulation Reference Signals (DMRS) for NR PBCH configuration
 */
typedef struct SRSRAN_API {
  uint32_t                    N_id;    ///< Physical cell identifier
  uint32_t                    n_hf;    ///< Number of half radio frame, 0 or 1
  uint32_t                    ssb_idx; ///< SSB candidate index
  uint32_t                    L_max;   ///< Number of SSB opportunities in half radio frame
  float                       beta;    ///< Power allocation specified in TS 38.213
  srsran_subcarrier_spacing_t scs;     ///< SSB configured subcarrier spacing
} srsran_dmrs_pbch_cfg_t;

/**
 * @brief Describes an NR PBCH DMRS based measurement
 */
typedef struct SRSRAN_API {
  float corr;         ///< Normalised correlation
  float epre;         ///< Linear energy per resource element
  float rsrp;         ///< Linear RSRP
  float cfo_hz;       ///< CFO in Hz
  float avg_delay_us; ///< Average delay in us
} srsran_dmrs_pbch_meas_t;

/**
 * @brief Put NR PBCH DMRS in the SSB resource grid
 * @param cfg PBCH DMRS configuration
 * @param[out] ssb_grid SSB resource grid
 * @return SRSRAN_SUCCESS if the inputs and configuration are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dmrs_pbch_put(const srsran_dmrs_pbch_cfg_t* cfg, cf_t ssb_grid[SRSRAN_SSB_NOF_RE]);

/**
 * @brief Estimates NR PBCH DMRS
 * @param cfg PBCH DMRS configuration
 * @param ssb_grid Demodulated SSB resource grid
 * @param[out] ce Estimated channel
 * @param[out] meas Estimated channel measurements
 * @return SRSRAN_SUCCESS if the inputs and configuration are valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dmrs_pbch_estimate(const srsran_dmrs_pbch_cfg_t* cfg,
                                         const cf_t                    ssb_grid[SRSRAN_SSB_NOF_RE],
                                         cf_t                          ce[SRSRAN_SSB_NOF_RE],
                                         srsran_dmrs_pbch_meas_t*      meas);

#endif // SRSRAN_DMRS_PBCH_H
