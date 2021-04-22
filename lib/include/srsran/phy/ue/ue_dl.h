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

/******************************************************************************
 *  File:         ue_dl.h
 *
 *  Description:  UE downlink object.
 *
 *                This module is a frontend to all the downlink data and control
 *                channel processing modules.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_UE_DL_H
#define SRSRAN_UE_DL_H

#include <stdbool.h>

#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/ofdm.h"

#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/pcfich.h"
#include "srsran/phy/phch/pdcch.h"
#include "srsran/phy/phch/pdsch.h"
#include "srsran/phy/phch/pdsch_cfg.h"
#include "srsran/phy/phch/phich.h"
#include "srsran/phy/phch/pmch.h"
#include "srsran/phy/phch/ra.h"
#include "srsran/phy/phch/regs.h"

#include "srsran/phy/sync/cfo.h"

#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#include "srsran/config.h"

#define SRSRAN_MAX_CANDIDATES_UE 16 // From 36.213 Table 9.1.1-1
#define SRSRAN_MAX_CANDIDATES_COM 6 // From 36.213 Table 9.1.1-1
#define SRSRAN_MAX_CANDIDATES (SRSRAN_MAX_CANDIDATES_UE + SRSRAN_MAX_CANDIDATES_COM)

#define SRSRAN_MAX_FORMATS 4

#define SRSRAN_MI_NOF_REGS ((q->cell.frame_type == SRSRAN_FDD) ? 1 : 6)
#define SRSRAN_MI_MAX_REGS 6

#define SRSRAN_MAX_DCI_MSG SRSRAN_MAX_CARRIERS

typedef struct SRSRAN_API {
  srsran_dci_format_t   formats[SRSRAN_MAX_FORMATS];
  srsran_dci_location_t loc[SRSRAN_MAX_CANDIDATES];
  uint32_t              nof_locations;
  uint32_t              nof_formats;
} dci_blind_search_t;

typedef struct SRSRAN_API {
  // Cell configuration
  srsran_cell_t cell;
  uint32_t      nof_rx_antennas;
  uint16_t      current_mbsfn_area_id;

  // Objects for all DL Physical Channels
  srsran_pcfich_t pcfich;
  srsran_pdcch_t  pdcch;
  srsran_pdsch_t  pdsch;
  srsran_pmch_t   pmch;
  srsran_phich_t  phich;

  // Control region
  srsran_regs_t regs[SRSRAN_MI_MAX_REGS];
  uint32_t      mi_manual_index;
  bool          mi_auto;

  // Channel estimation and OFDM demodulation
  srsran_chest_dl_t     chest;
  srsran_chest_dl_res_t chest_res;
  srsran_ofdm_t         fft[SRSRAN_MAX_PORTS];
  srsran_ofdm_t         fft_mbsfn;

  // Buffers to store channel symbols after demodulation
  cf_t*              sf_symbols[SRSRAN_MAX_PORTS];
  dci_blind_search_t current_ss_common;

  srsran_dci_msg_t pending_ul_dci_msg[SRSRAN_MAX_DCI_MSG];
  uint32_t         pending_ul_dci_count;

  srsran_dci_location_t allocated_locations[SRSRAN_MAX_DCI_MSG];
  uint32_t              nof_allocated_locations;
} srsran_ue_dl_t;

// Downlink config (includes common and dedicated variables)
typedef struct SRSRAN_API {
  srsran_cqi_report_cfg_t cqi_report;
  srsran_pdsch_cfg_t      pdsch;
  srsran_dci_cfg_t        dci;
  srsran_tm_t             tm;
  bool                    dci_common_ss;
} srsran_dl_cfg_t;

typedef struct SRSRAN_API {
  srsran_dl_cfg_t       cfg;
  srsran_chest_dl_cfg_t chest_cfg;
  uint32_t              last_ri;
  float                 snr_to_cqi_offset;
} srsran_ue_dl_cfg_t;

typedef struct {
  uint32_t v_dai_dl;
  uint32_t n_cce;
  uint32_t grant_cc_idx;
  uint32_t tpc_for_pucch;
} srsran_pdsch_ack_resource_t;

typedef struct {
  srsran_pdsch_ack_resource_t resource;
  uint32_t                    k;
  uint8_t                     value[SRSRAN_MAX_CODEWORDS]; // 0/1 or 2 for DTX
  bool                        present;
} srsran_pdsch_ack_m_t;

typedef struct {
  uint32_t             M;
  srsran_pdsch_ack_m_t m[SRSRAN_UCI_MAX_M];
} srsran_pdsch_ack_cc_t;

typedef struct {
  srsran_pdsch_ack_cc_t           cc[SRSRAN_MAX_CARRIERS];
  uint32_t                        nof_cc;
  uint32_t                        V_dai_ul;
  srsran_tm_t                     transmission_mode;
  srsran_ack_nack_feedback_mode_t ack_nack_feedback_mode;
  bool                            is_grant_available;
  bool                            is_pusch_available;
  bool                            tdd_ack_multiplex;
  bool                            simul_cqi_ack;
  bool                            simul_cqi_ack_pucch3;
} srsran_pdsch_ack_t;

SRSRAN_API int
srsran_ue_dl_init(srsran_ue_dl_t* q, cf_t* input[SRSRAN_MAX_PORTS], uint32_t max_prb, uint32_t nof_rx_antennas);

SRSRAN_API void srsran_ue_dl_free(srsran_ue_dl_t* q);

SRSRAN_API int srsran_ue_dl_set_cell(srsran_ue_dl_t* q, srsran_cell_t cell);

SRSRAN_API int srsran_ue_dl_set_mbsfn_area_id(srsran_ue_dl_t* q, uint16_t mbsfn_area_id);

SRSRAN_API void srsran_ue_dl_set_non_mbsfn_region(srsran_ue_dl_t* q, uint8_t non_mbsfn_region_length);

SRSRAN_API void srsran_ue_dl_set_mi_manual(srsran_ue_dl_t* q, uint32_t mi_idx);

SRSRAN_API void srsran_ue_dl_set_mi_auto(srsran_ue_dl_t* q);

/* Perform signal demodulation and channel estimation and store signals in the object */
SRSRAN_API int srsran_ue_dl_decode_fft_estimate(srsran_ue_dl_t* q, srsran_dl_sf_cfg_t* sf, srsran_ue_dl_cfg_t* cfg);

SRSRAN_API int srsran_ue_dl_decode_fft_estimate_noguru(srsran_ue_dl_t*     q,
                                                       srsran_dl_sf_cfg_t* sf,
                                                       srsran_ue_dl_cfg_t* cfg,
                                                       cf_t*               input[SRSRAN_MAX_PORTS]);

/* Finds UL/DL DCI in the signal processed in a previous call to decode_fft_estimate() */
SRSRAN_API int srsran_ue_dl_find_ul_dci(srsran_ue_dl_t*     q,
                                        srsran_dl_sf_cfg_t* sf,
                                        srsran_ue_dl_cfg_t* dl_cfg,
                                        uint16_t            rnti,
                                        srsran_dci_ul_t     dci_msg[SRSRAN_MAX_DCI_MSG]);

SRSRAN_API int srsran_ue_dl_find_dl_dci(srsran_ue_dl_t*     q,
                                        srsran_dl_sf_cfg_t* sf,
                                        srsran_ue_dl_cfg_t* dl_cfg,
                                        uint16_t            rnti,
                                        srsran_dci_dl_t     dci_msg[SRSRAN_MAX_DCI_MSG]);

SRSRAN_API int srsran_ue_dl_dci_to_pdsch_grant(srsran_ue_dl_t*       q,
                                               srsran_dl_sf_cfg_t*   sf,
                                               srsran_ue_dl_cfg_t*   cfg,
                                               srsran_dci_dl_t*      dci,
                                               srsran_pdsch_grant_t* grant);

/* Decodes PDSCH and PHICH in the signal processed in a previous call to decode_fft_estimate() */
SRSRAN_API int srsran_ue_dl_decode_pdsch(srsran_ue_dl_t*     q,
                                         srsran_dl_sf_cfg_t* sf,
                                         srsran_pdsch_cfg_t* pdsch_cfg,
                                         srsran_pdsch_res_t  data[SRSRAN_MAX_CODEWORDS]);

SRSRAN_API int srsran_ue_dl_decode_pmch(srsran_ue_dl_t*     q,
                                        srsran_dl_sf_cfg_t* sf,
                                        srsran_pmch_cfg_t*  pmch_cfg,
                                        srsran_pdsch_res_t* data);

SRSRAN_API int srsran_ue_dl_decode_phich(srsran_ue_dl_t*       q,
                                         srsran_dl_sf_cfg_t*   sf,
                                         srsran_ue_dl_cfg_t*   cfg,
                                         srsran_phich_grant_t* grant,
                                         srsran_phich_res_t*   result);

SRSRAN_API int srsran_ue_dl_select_ri(srsran_ue_dl_t* q, uint32_t* ri, float* cn);

SRSRAN_API void srsran_ue_dl_gen_cqi_periodic(srsran_ue_dl_t*     q,
                                              srsran_ue_dl_cfg_t* cfg,
                                              uint32_t            wideband_value,
                                              uint32_t            tti,
                                              srsran_uci_data_t*  uci_data);

SRSRAN_API void srsran_ue_dl_gen_cqi_aperiodic(srsran_ue_dl_t*     q,
                                               srsran_ue_dl_cfg_t* cfg,
                                               uint32_t            wideband_value,
                                               srsran_uci_data_t*  uci_data);

SRSRAN_API void srsran_ue_dl_gen_ack(const srsran_cell_t*      cell,
                                     const srsran_dl_sf_cfg_t* sf,
                                     const srsran_pdsch_ack_t* ack_info,
                                     srsran_uci_data_t*        uci_data);

/* Functions used for testing purposes */
SRSRAN_API int srsran_ue_dl_find_and_decode(srsran_ue_dl_t*     q,
                                            srsran_dl_sf_cfg_t* sf,
                                            srsran_ue_dl_cfg_t* cfg,
                                            srsran_pdsch_cfg_t* pdsch_cfg,
                                            uint8_t*            data[SRSRAN_MAX_CODEWORDS],
                                            bool                acks[SRSRAN_MAX_CODEWORDS]);

SRSRAN_API void srsran_ue_dl_save_signal(srsran_ue_dl_t* q, srsran_dl_sf_cfg_t* sf, srsran_pdsch_cfg_t* pdsch_cfg);

#endif // SRSRAN_UE_DL_H
