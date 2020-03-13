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

#ifndef SRSLTE_UE_UL_H
#define SRSLTE_UE_UL_H

#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/ch_estimation/refsignal_ul.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/pusch.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#include "srslte/config.h"

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
} srslte_ue_ul_powerctrl_t;

typedef struct SRSLTE_API {
  // Uplink config (includes common and dedicated variables)
  srslte_pucch_cfg_t                pucch;
  srslte_pusch_cfg_t                pusch;
  srslte_pusch_hopping_cfg_t        hopping;
  srslte_ue_ul_powerctrl_t          power_ctrl;
  srslte_refsignal_dmrs_pusch_cfg_t dmrs;
  srslte_refsignal_srs_cfg_t        srs;
} srslte_ul_cfg_t;

typedef enum {
  SRSLTE_UE_UL_NORMALIZE_MODE_AUTO = 0,
  SRSLTE_UE_UL_NORMALIZE_MODE_FORCE_AMPLITUDE
} srslte_ue_ul_normalize_mode_t;

typedef struct SRSLTE_API {

  srslte_ul_cfg_t ul_cfg;
  bool            grant_available;
  uint32_t        cc_idx;

  srslte_ue_ul_normalize_mode_t normalize_mode;
  float                         force_peak_amplitude;
  bool                          cfo_en;
  float                         cfo_tol;
  float                         cfo_value;

} srslte_ue_ul_cfg_t;

typedef struct SRSLTE_API {
  srslte_cell_t cell;

  uint16_t current_rnti;
  bool     signals_pregenerated;

  srslte_ofdm_t fft;
  srslte_cfo_t  cfo;

  srslte_refsignal_ul_t             signals;
  srslte_refsignal_ul_dmrs_pregen_t pregen_dmrs;
  srslte_refsignal_srs_pregen_t     pregen_srs;

  srslte_pusch_t pusch;
  srslte_pucch_t pucch;

  srslte_ra_ul_pusch_hopping_t hopping;

  cf_t* out_buffer;
  cf_t* refsignal;
  cf_t* srs_signal;
  cf_t* sf_symbols;

} srslte_ue_ul_t;

SRSLTE_API int srslte_ue_ul_init(srslte_ue_ul_t* q, cf_t* out_buffer, uint32_t max_prb);

SRSLTE_API void srslte_ue_ul_free(srslte_ue_ul_t* q);

SRSLTE_API int srslte_ue_ul_set_cell(srslte_ue_ul_t* q, srslte_cell_t cell);

SRSLTE_API void srslte_ue_ul_set_rnti(srslte_ue_ul_t* q, uint16_t rnti);

SRSLTE_API int srslte_ue_ul_pregen_signals(srslte_ue_ul_t* q, srslte_ue_ul_cfg_t* cfg);

SRSLTE_API int srslte_ue_ul_dci_to_pusch_grant(srslte_ue_ul_t*       q,
                                               srslte_ul_sf_cfg_t*   sf,
                                               srslte_ue_ul_cfg_t*   cfg,
                                               srslte_dci_ul_t*      dci,
                                               srslte_pusch_grant_t* grant);

SRSLTE_API void srslte_ue_ul_pusch_hopping(srslte_ue_ul_t*       q,
                                           srslte_ul_sf_cfg_t*   sf,
                                           srslte_ue_ul_cfg_t*   cfg,
                                           srslte_pusch_grant_t* grant);

SRSLTE_API int
srslte_ue_ul_encode(srslte_ue_ul_t* q, srslte_ul_sf_cfg_t* sf, srslte_ue_ul_cfg_t* cfg, srslte_pusch_data_t* data);

SRSLTE_API int srslte_ue_ul_sr_send_tti(const srslte_pucch_cfg_t* cfg, uint32_t current_tti);

SRSLTE_API bool
srslte_ue_ul_gen_sr(srslte_ue_ul_cfg_t* cfg, srslte_ul_sf_cfg_t* sf, srslte_uci_data_t* uci_data, bool sr_request);

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
SRSLTE_API void srslte_ue_ul_pucch_resource_selection(const srslte_cell_t*      cell,
                                                      srslte_pucch_cfg_t*       cfg,
                                                      const srslte_uci_cfg_t*   uci_cfg,
                                                      const srslte_uci_value_t* uci_value,
                                                      uint8_t                   b[SRSLTE_UCI_MAX_ACK_BITS]);

SRSLTE_API bool srslte_ue_ul_info(srslte_ue_ul_cfg_t* cfg,
                                  srslte_ul_sf_cfg_t* sf,
                                  srslte_uci_value_t* uci_data,
                                  char*               str,
                                  uint32_t            str_len);

#endif // SRSLTE_UE_UL_H
