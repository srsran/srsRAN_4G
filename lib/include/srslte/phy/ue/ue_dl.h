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
 *  File:         ue_dl.h
 *
 *  Description:  UE downlink object.
 *
 *                This module is a frontend to all the downlink data and control
 *                channel processing modules.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_UE_DL_H
#define SRSLTE_UE_DL_H

#include <stdbool.h>

#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/ofdm.h"

#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/pcfich.h"
#include "srslte/phy/phch/pdcch.h"
#include "srslte/phy/phch/pdsch.h"
#include "srslte/phy/phch/pdsch_cfg.h"
#include "srslte/phy/phch/phich.h"
#include "srslte/phy/phch/pmch.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/phch/regs.h"

#include "srslte/phy/sync/cfo.h"

#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#include "srslte/config.h"

#define SRSLTE_MAX_CANDIDATES_UE 16 // From 36.213 Table 9.1.1-1
#define SRSLTE_MAX_CANDIDATES_COM 6 // From 36.213 Table 9.1.1-1
#define SRSLTE_MAX_CANDIDATES (SRSLTE_MAX_CANDIDATES_UE + SRSLTE_MAX_CANDIDATES_COM)

#define SRSLTE_MAX_FORMATS 4

#define SRSLTE_MI_NOF_REGS ((q->cell.frame_type == SRSLTE_FDD) ? 1 : 6)
#define SRSLTE_MI_MAX_REGS 6

#define SRSLTE_MAX_DCI_MSG SRSLTE_MAX_CARRIERS

typedef struct SRSLTE_API {
  srslte_dci_format_t   formats[SRSLTE_MAX_FORMATS];
  srslte_dci_location_t loc[SRSLTE_MAX_CANDIDATES];
  uint32_t              nof_locations;
  uint32_t              nof_formats;
} dci_blind_search_t;

typedef struct SRSLTE_API {
  // Cell configuration
  srslte_cell_t cell;
  uint32_t      nof_rx_antennas;
  uint16_t      current_mbsfn_area_id;
  uint16_t      pregen_rnti;

  // Objects for all DL Physical Channels
  srslte_pcfich_t pcfich;
  srslte_pdcch_t  pdcch;
  srslte_pdsch_t  pdsch;
  srslte_pmch_t   pmch;
  srslte_phich_t  phich;

  // Control region
  srslte_regs_t regs[SRSLTE_MI_MAX_REGS];
  uint32_t      mi_manual_index;
  bool          mi_auto;

  // Channel estimation and OFDM demodulation
  srslte_chest_dl_t     chest;
  srslte_chest_dl_res_t chest_res;
  srslte_ofdm_t         fft[SRSLTE_MAX_PORTS];
  srslte_ofdm_t         fft_mbsfn;

  // Buffers to store channel symbols after demodulation
  cf_t* sf_symbols[SRSLTE_MAX_PORTS];

  // Variables for blind DCI search
  dci_blind_search_t current_ss_ue[SRSLTE_MI_MAX_REGS][SRSLTE_NOF_CFI][SRSLTE_NOF_SF_X_FRAME];
  dci_blind_search_t current_ss_common[SRSLTE_MI_MAX_REGS][SRSLTE_NOF_CFI];
  srslte_dci_msg_t   pending_ul_dci_msg[SRSLTE_MAX_DCI_MSG];
  uint32_t           pending_ul_dci_count;

  srslte_dci_location_t allocated_locations[SRSLTE_MAX_DCI_MSG];
  uint32_t              nof_allocated_locations;
} srslte_ue_dl_t;

// Downlink config (includes common and dedicated variables)
typedef struct SRSLTE_API {
  srslte_cqi_report_cfg_t cqi_report;
  srslte_pdsch_cfg_t      pdsch;
  srslte_dci_cfg_t        dci;
  srslte_tm_t             tm;
  bool                    dci_common_ss;
} srslte_dl_cfg_t;

typedef struct SRSLTE_API {
  srslte_dl_cfg_t       cfg;
  srslte_chest_dl_cfg_t chest_cfg;
  uint32_t              last_ri;
  float                 snr_to_cqi_offset;
} srslte_ue_dl_cfg_t;

typedef struct {
  uint32_t v_dai_dl;
  uint32_t n_cce;
  uint32_t grant_cc_idx;
  uint32_t tpc_for_pucch;
} srslte_pdsch_ack_resource_t;

typedef struct {
  srslte_pdsch_ack_resource_t resource;
  uint32_t                    k;
  uint8_t                     value[SRSLTE_MAX_CODEWORDS]; // 0/1 or 2 for DTX
  bool                        present;
} srslte_pdsch_ack_m_t;

typedef struct {
  uint32_t             M;
  srslte_pdsch_ack_m_t m[SRSLTE_UCI_MAX_M];
} srslte_pdsch_ack_cc_t;

typedef struct {
  srslte_pdsch_ack_cc_t           cc[SRSLTE_MAX_CARRIERS];
  uint32_t                        nof_cc;
  uint32_t                        V_dai_ul;
  srslte_tm_t                     transmission_mode;
  srslte_ack_nack_feedback_mode_t ack_nack_feedback_mode;
  bool                            is_grant_available;
  bool                            is_pusch_available;
  bool                            tdd_ack_multiplex;
  bool                            simul_cqi_ack;
  bool                            simul_cqi_ack_pucch3;
} srslte_pdsch_ack_t;

SRSLTE_API int
srslte_ue_dl_init(srslte_ue_dl_t* q, cf_t* input[SRSLTE_MAX_PORTS], uint32_t max_prb, uint32_t nof_rx_antennas);

SRSLTE_API void srslte_ue_dl_free(srslte_ue_dl_t* q);

SRSLTE_API int srslte_ue_dl_set_cell(srslte_ue_dl_t* q, srslte_cell_t cell);

SRSLTE_API void srslte_ue_dl_set_rnti(srslte_ue_dl_t* q, uint16_t rnti);

SRSLTE_API int srslte_ue_dl_set_mbsfn_area_id(srslte_ue_dl_t* q, uint16_t mbsfn_area_id);

SRSLTE_API void srslte_ue_dl_set_non_mbsfn_region(srslte_ue_dl_t* q, uint8_t non_mbsfn_region_length);

SRSLTE_API void srslte_ue_dl_set_mi_manual(srslte_ue_dl_t* q, uint32_t mi_idx);

SRSLTE_API void srslte_ue_dl_set_mi_auto(srslte_ue_dl_t* q);

/* Perform signal demodulation and channel estimation and store signals in the object */
SRSLTE_API int srslte_ue_dl_decode_fft_estimate(srslte_ue_dl_t* q, srslte_dl_sf_cfg_t* sf, srslte_ue_dl_cfg_t* cfg);

SRSLTE_API int srslte_ue_dl_decode_fft_estimate_noguru(srslte_ue_dl_t*     q,
                                                       srslte_dl_sf_cfg_t* sf,
                                                       srslte_ue_dl_cfg_t* cfg,
                                                       cf_t*               input[SRSLTE_MAX_PORTS]);

/* Finds UL/DL DCI in the signal processed in a previous call to decode_fft_estimate() */
SRSLTE_API int srslte_ue_dl_find_ul_dci(srslte_ue_dl_t*     q,
                                        srslte_dl_sf_cfg_t* sf,
                                        srslte_ue_dl_cfg_t* dl_cfg,
                                        uint16_t            rnti,
                                        srslte_dci_ul_t     dci_msg[SRSLTE_MAX_DCI_MSG]);

SRSLTE_API int srslte_ue_dl_find_dl_dci(srslte_ue_dl_t*     q,
                                        srslte_dl_sf_cfg_t* sf,
                                        srslte_ue_dl_cfg_t* dl_cfg,
                                        uint16_t            rnti,
                                        srslte_dci_dl_t     dci_msg[SRSLTE_MAX_DCI_MSG]);

SRSLTE_API int srslte_ue_dl_dci_to_pdsch_grant(srslte_ue_dl_t*       q,
                                               srslte_dl_sf_cfg_t*   sf,
                                               srslte_ue_dl_cfg_t*   cfg,
                                               srslte_dci_dl_t*      dci,
                                               srslte_pdsch_grant_t* grant);

/* Decodes PDSCH and PHICH in the signal processed in a previous call to decode_fft_estimate() */
SRSLTE_API int srslte_ue_dl_decode_pdsch(srslte_ue_dl_t*     q,
                                         srslte_dl_sf_cfg_t* sf,
                                         srslte_pdsch_cfg_t* pdsch_cfg,
                                         srslte_pdsch_res_t  data[SRSLTE_MAX_CODEWORDS]);

SRSLTE_API int srslte_ue_dl_decode_pmch(srslte_ue_dl_t*     q,
                                        srslte_dl_sf_cfg_t* sf,
                                        srslte_pmch_cfg_t*  pmch_cfg,
                                        srslte_pdsch_res_t* data);

SRSLTE_API int srslte_ue_dl_decode_phich(srslte_ue_dl_t*       q,
                                         srslte_dl_sf_cfg_t*   sf,
                                         srslte_ue_dl_cfg_t*   cfg,
                                         srslte_phich_grant_t* grant,
                                         srslte_phich_res_t*   result);

SRSLTE_API int srslte_ue_dl_select_ri(srslte_ue_dl_t* q, uint32_t* ri, float* cn);

SRSLTE_API void srslte_ue_dl_gen_cqi_periodic(srslte_ue_dl_t*     q,
                                              srslte_ue_dl_cfg_t* cfg,
                                              uint32_t            wideband_value,
                                              uint32_t            tti,
                                              srslte_uci_data_t*  uci_data);

SRSLTE_API void srslte_ue_dl_gen_cqi_aperiodic(srslte_ue_dl_t*     q,
                                               srslte_ue_dl_cfg_t* cfg,
                                               uint32_t            wideband_value,
                                               srslte_uci_data_t*  uci_data);

SRSLTE_API void srslte_ue_dl_gen_ack(const srslte_cell_t*      cell,
                                     const srslte_dl_sf_cfg_t* sf,
                                     const srslte_pdsch_ack_t* ack_info,
                                     srslte_uci_data_t*        uci_data);

/* Functions used for testing purposes */
SRSLTE_API int srslte_ue_dl_find_and_decode(srslte_ue_dl_t*     q,
                                            srslte_dl_sf_cfg_t* sf,
                                            srslte_ue_dl_cfg_t* cfg,
                                            srslte_pdsch_cfg_t* pdsch_cfg,
                                            uint8_t*            data[SRSLTE_MAX_CODEWORDS],
                                            bool                acks[SRSLTE_MAX_CODEWORDS]);

SRSLTE_API void srslte_ue_dl_save_signal(srslte_ue_dl_t* q, srslte_dl_sf_cfg_t* sf, srslte_pdsch_cfg_t* pdsch_cfg);

#endif // SRSLTE_UE_DL_H
