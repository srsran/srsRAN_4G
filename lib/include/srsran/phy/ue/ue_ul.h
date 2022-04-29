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

/******************************************************************************
 *  File:         ue_ul.h
 *
 *  Description:  UE uplink object.
 *
 *                This module is a frontend to all the uplink data and control
 *                channel processing modules.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_UE_UL_H
#define SRSRAN_UE_UL_H

#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/ch_estimation/refsignal_ul.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/pusch.h"
#include "srsran/phy/phch/ra.h"
#include "srsran/phy/sync/cfo.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#include "srsran/config.h"

/* UE UL power control */
typedef struct {
  // Common configuration
  float p0_nominal_pusch;
  float alpha;
  float p0_nominal_pucch;
  float delta_f_pucch[5];
  float delta_preamble_msg3;

  // Dedicated configuration
  float p0_ue_pusch;
  bool  delta_mcs_based;
  bool  acc_enabled;
  float p0_ue_pucch;
  float p_srs_offset;
} srsran_ue_ul_powerctrl_t;

typedef struct SRSRAN_API {
  // Uplink config (includes common and dedicated variables)
  srsran_pucch_cfg_t                pucch;
  srsran_pusch_cfg_t                pusch;
  srsran_pusch_hopping_cfg_t        hopping;
  srsran_ue_ul_powerctrl_t          power_ctrl;
  srsran_refsignal_dmrs_pusch_cfg_t dmrs;
  srsran_refsignal_srs_cfg_t        srs;
} srsran_ul_cfg_t;

typedef enum {
  SRSRAN_UE_UL_NORMALIZE_MODE_AUTO = 0,
  SRSRAN_UE_UL_NORMALIZE_MODE_FORCE_AMPLITUDE
} srsran_ue_ul_normalize_mode_t;

typedef struct SRSRAN_API {
  srsran_ul_cfg_t ul_cfg;
  bool            grant_available;
  uint32_t        cc_idx;

  srsran_ue_ul_normalize_mode_t normalize_mode;
  float                         force_peak_amplitude;
  bool                          cfo_en;
  float                         cfo_tol;
  float                         cfo_value;

} srsran_ue_ul_cfg_t;

typedef struct SRSRAN_API {
  srsran_cell_t cell;

  bool signals_pregenerated;

  srsran_ofdm_t fft;
  srsran_cfo_t  cfo;

  srsran_refsignal_ul_t             signals;
  srsran_refsignal_ul_dmrs_pregen_t pregen_dmrs;
  srsran_refsignal_srs_pregen_t     pregen_srs;

  srsran_pusch_t pusch;
  srsran_pucch_t pucch;

  srsran_ra_ul_pusch_hopping_t hopping;

  srsran_cfr_cfg_t cfr_config;

  cf_t* out_buffer;
  cf_t* refsignal;
  cf_t* srs_signal;
  cf_t* sf_symbols;

} srsran_ue_ul_t;

SRSRAN_API int srsran_ue_ul_init(srsran_ue_ul_t* q, cf_t* out_buffer, uint32_t max_prb);

SRSRAN_API void srsran_ue_ul_free(srsran_ue_ul_t* q);

SRSRAN_API int srsran_ue_ul_set_cell(srsran_ue_ul_t* q, srsran_cell_t cell);

SRSRAN_API int srsran_ue_ul_set_cfr(srsran_ue_ul_t* q, const srsran_cfr_cfg_t* cfr);

SRSRAN_API int srsran_ue_ul_pregen_signals(srsran_ue_ul_t* q, srsran_ue_ul_cfg_t* cfg);

SRSRAN_API int srsran_ue_ul_dci_to_pusch_grant(srsran_ue_ul_t*       q,
                                               srsran_ul_sf_cfg_t*   sf,
                                               srsran_ue_ul_cfg_t*   cfg,
                                               srsran_dci_ul_t*      dci,
                                               srsran_pusch_grant_t* grant);

SRSRAN_API void srsran_ue_ul_pusch_hopping(srsran_ue_ul_t*       q,
                                           srsran_ul_sf_cfg_t*   sf,
                                           srsran_ue_ul_cfg_t*   cfg,
                                           srsran_pusch_grant_t* grant);

SRSRAN_API int
srsran_ue_ul_encode(srsran_ue_ul_t* q, srsran_ul_sf_cfg_t* sf, srsran_ue_ul_cfg_t* cfg, srsran_pusch_data_t* data);

SRSRAN_API int srsran_ue_ul_sr_send_tti(const srsran_pucch_cfg_t* cfg, uint32_t current_tti);

SRSRAN_API bool
srsran_ue_ul_gen_sr(srsran_ue_ul_cfg_t* cfg, srsran_ul_sf_cfg_t* sf, srsran_uci_data_t* uci_data, bool sr_request);

/**
 * Determines the PUCCH resource selection according to 3GPP 36.213 R10 Section 10.1. The PUCCH format and resource are
 * saved in cfg->format and cfg->n_pucch. Also, HARQ-ACK
 *
 * @param cell Cell parameter, non-modifiable
 * @param cfg PUCCH configuration and contains function results
 * @param uci_cfg UCI configuration
 * @param uci_data UCI data
 * @param b Modified bits after applying HARQ-ACK feedback mode "encoding"
 */
SRSRAN_API void srsran_ue_ul_pucch_resource_selection(const srsran_cell_t*      cell,
                                                      srsran_pucch_cfg_t*       cfg,
                                                      const srsran_uci_cfg_t*   uci_cfg,
                                                      const srsran_uci_value_t* uci_value,
                                                      uint8_t                   b[SRSRAN_UCI_MAX_ACK_BITS]);

SRSRAN_API bool srsran_ue_ul_info(srsran_ue_ul_cfg_t* cfg,
                                  srsran_ul_sf_cfg_t* sf,
                                  srsran_uci_value_t* uci_data,
                                  char*               str,
                                  uint32_t            str_len);

#endif // SRSRAN_UE_UL_H
