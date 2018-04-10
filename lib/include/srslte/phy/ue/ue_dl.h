/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/common/phy_common.h"

#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/pcfich.h"
#include "srslte/phy/phch/pdcch.h"
#include "srslte/phy/phch/pdsch.h"
#include "srslte/phy/phch/pmch.h"
#include "srslte/phy/phch/pdsch_cfg.h"
#include "srslte/phy/phch/phich.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/phch/regs.h"

#include "srslte/phy/sync/cfo.h"

#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/debug.h"

#include "srslte/config.h"


#define MAX_CANDIDATES_UE  16 // From 36.213 Table 9.1.1-1
#define MAX_CANDIDATES_COM 6 // From 36.213 Table 9.1.1-1
#define MAX_CANDIDATES (MAX_CANDIDATES_UE + MAX_CANDIDATES_COM)   


typedef struct {
  srslte_dci_format_t format; 
  srslte_dci_location_t loc[MAX_CANDIDATES];
  uint32_t nof_locations; 
} dci_blind_search_t; 

typedef struct SRSLTE_API {
  srslte_pcfich_t pcfich;
  srslte_pdcch_t pdcch;
  srslte_pdsch_t pdsch;
  srslte_pmch_t  pmch;
  srslte_phich_t phich; 
  srslte_regs_t regs;
  srslte_ofdm_t fft[SRSLTE_MAX_PORTS];
  srslte_ofdm_t fft_mbsfn;
  srslte_chest_dl_t chest;
  
  srslte_pdsch_cfg_t pdsch_cfg;
  srslte_pdsch_cfg_t pmch_cfg;
  srslte_softbuffer_rx_t *softbuffers[SRSLTE_MAX_CODEWORDS];
  srslte_ra_dl_dci_t dl_dci;
  srslte_cell_t cell;

  uint32_t nof_rx_antennas;
  
  cf_t *sf_symbols;  // this is for backwards compatibility
  cf_t *sf_symbols_m[SRSLTE_MAX_PORTS]; 
  cf_t *ce[SRSLTE_MAX_PORTS]; // compatibility
  cf_t *ce_m[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];

  /* RI, PMI and SINR for MIMO statistics */
  float sinr[SRSLTE_MAX_LAYERS][SRSLTE_MAX_CODEBOOKS];
  uint32_t pmi[SRSLTE_MAX_LAYERS];
  uint32_t ri;

  /* Power allocation parameter 3GPP 36.213 Clause 5.2 Rho_b */
  float rho_b;

  srslte_dci_format_t dci_format;
  uint64_t pkt_errors; 
  uint64_t pkts_total;
  uint64_t pdsch_pkt_errors;
  uint64_t pdsch_pkts_total;
  uint64_t pmch_pkt_errors;
  uint64_t pmch_pkts_total;
  uint64_t nof_detected; 

  uint16_t current_rnti;
  uint16_t current_mbsfn_area_id;
  dci_blind_search_t current_ss_ue[3][10];
  dci_blind_search_t current_ss_common[3];
  srslte_dci_location_t last_location;
  srslte_dci_location_t last_location_ul;
  
  srslte_dci_msg_t pending_ul_dci_msg; 
  uint16_t pending_ul_dci_rnti; 
  
  float last_phich_corr;
}srslte_ue_dl_t;

/* This function shall be called just after the initial synchronization */
SRSLTE_API int srslte_ue_dl_init(srslte_ue_dl_t *q,
                                 cf_t *input[SRSLTE_MAX_PORTS],
                                 uint32_t max_prb,
                                 uint32_t nof_rx_antennas);

SRSLTE_API void srslte_ue_dl_free(srslte_ue_dl_t *q);

SRSLTE_API int srslte_ue_dl_set_cell(srslte_ue_dl_t *q,
                                          srslte_cell_t cell);

int srslte_ue_dl_decode_fft_estimate(srslte_ue_dl_t *q, 
                                     uint32_t sf_idx,
                                     uint32_t *cfi);

SRSLTE_API int srslte_ue_dl_decode_fft_estimate_mbsfn(srslte_ue_dl_t *q,
                                                uint32_t sf_idx,
                                                uint32_t *cfi,
                                                srslte_sf_t sf_type); 

SRSLTE_API int srslte_ue_dl_decode_fft_estimate_noguru(srslte_ue_dl_t *q,
                                                       cf_t *input[SRSLTE_MAX_PORTS],
                                                       uint32_t sf_idx,
                                                       uint32_t *cfi);

SRSLTE_API int srslte_ue_dl_decode_estimate(srslte_ue_dl_t *q,
                                            uint32_t sf_idx,
                                           uint32_t *cfi);

SRSLTE_API int srslte_ue_dl_decode_estimate_mbsfn(srslte_ue_dl_t *q, 
                                            uint32_t sf_idx, 
                                            uint32_t *cfi,
                                            srslte_sf_t sf_type);  

SRSLTE_API int srslte_ue_dl_cfg_grant(srslte_ue_dl_t *q,
                                      srslte_ra_dl_grant_t *grant,
                                      uint32_t cfi,
                                      uint32_t sf_idx,
                                      int rvidx[SRSLTE_MAX_CODEWORDS],
                                      srslte_mimo_type_t mimo_type);

SRSLTE_API int srslte_ue_dl_find_ul_dci(srslte_ue_dl_t *q, 
                                        uint32_t cfi, 
                                        uint32_t sf_idx, 
                                        uint16_t rnti, 
                                        srslte_dci_msg_t *dci_msg); 

SRSLTE_API int srslte_ue_dl_find_dl_dci(srslte_ue_dl_t *q, 
                                        uint32_t tm,
                                        uint32_t cfi, 
                                        uint32_t sf_idx, 
                                        uint16_t rnti, 
                                        srslte_dci_msg_t *dci_msg); 

SRSLTE_API int srslte_ue_dl_find_dl_dci_type(srslte_ue_dl_t *q, 
                                             uint32_t tm,
                                             uint32_t cfi, 
                                             uint32_t sf_idx, 
                                             uint16_t rnti, 
                                             srslte_rnti_type_t rnti_type, 
                                             srslte_dci_msg_t *dci_msg);

SRSLTE_API uint32_t srslte_ue_dl_get_ncce(srslte_ue_dl_t *q);

SRSLTE_API int srslte_ue_dl_decode(srslte_ue_dl_t *q,
                                   uint8_t *data[SRSLTE_MAX_CODEWORDS],
                                   uint32_t tm,
                                   uint32_t tti,
                                   bool acks[SRSLTE_MAX_CODEWORDS]);

SRSLTE_API int srslte_ue_dl_decode_rnti(srslte_ue_dl_t *q,
                                        uint8_t *data[SRSLTE_MAX_CODEWORDS],
                                        uint32_t tm,
                                        uint32_t tti,
                                        uint16_t rnti,
                                        bool acks[SRSLTE_MAX_CODEWORDS]);

/* Used by example applications - full PMCH decode for a given MBSFN area ID
 * srslte_ue_dl_decode_fft_estimate_multi,
 * srslte_chest_dl_get_noise_estimate,
 * srslte_ue_dl_cfg_grant,
 * srslte_pmch_decode_multi
 */
SRSLTE_API int srslte_ue_dl_decode_mbsfn(srslte_ue_dl_t * q,
                                         uint8_t *data,
                                         uint32_t tti);


SRSLTE_API int srslte_ue_dl_ri_pmi_select(srslte_ue_dl_t *q,
                                          uint8_t *ri,
                                          uint8_t *pmi,
                                          float *current_sinr);

SRSLTE_API int srslte_ue_dl_ri_select(srslte_ue_dl_t *q,
                                      uint8_t *ri,
                                      float *cn);

SRSLTE_API bool srslte_ue_dl_decode_phich(srslte_ue_dl_t *q, 
                                          uint32_t sf_idx, 
                                          uint32_t n_prb_lowest, 
                                          uint32_t n_dmrs); 

SRSLTE_API void srslte_ue_dl_reset(srslte_ue_dl_t *q);

SRSLTE_API void srslte_ue_dl_set_rnti(srslte_ue_dl_t *q, 
                                      uint16_t rnti);

/* Generate signals if required, store in q->current_mbsfn_area_id */
SRSLTE_API int srslte_ue_dl_set_mbsfn_area_id(srslte_ue_dl_t *q,
                                              uint16_t mbsfn_area_id);

SRSLTE_API void srslte_ue_dl_set_non_mbsfn_region(srslte_ue_dl_t *q,
                                                  uint8_t non_mbsfn_region_length);

SRSLTE_API void srslte_ue_dl_set_power_alloc(srslte_ue_dl_t *q,
                                              float rho_a,
                                              float rho_b);


SRSLTE_API void srslte_ue_dl_save_signal(srslte_ue_dl_t *q, 
                                         srslte_softbuffer_rx_t *softbuffer, 
                                         uint32_t tti, 
                                         uint32_t rv_idx, 
                                         uint16_t rnti, 
                                         uint32_t cfi); 


#endif // SRSLTE_UE_DL_H
