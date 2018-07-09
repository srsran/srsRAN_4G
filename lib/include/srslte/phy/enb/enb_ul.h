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
 *  File:         enb_ul.h
 *
 *  Description:  ENB uplink object.
 *
 *                This module is a frontend to all the uplink data and control
 *                channel processing modules for the ENB receiver side.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_ENB_UL_H
#define SRSLTE_ENB_UL_H

#include <stdbool.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/ch_estimation/chest_ul.h"
#include "srslte/phy/phch/prach.h"
#include "srslte/phy/phch/pusch.h"
#include "srslte/phy/phch/pusch_cfg.h"
#include "srslte/phy/phch/ra.h"

#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/debug.h"

#include "srslte/config.h"

typedef struct {
  uint32_t n_prb_lowest;
  uint32_t n_dmrs;  
} srslte_enb_ul_phich_info_t; 

typedef struct {
  bool uci_cfg_en;
  bool srs_cfg_en;
  srslte_uci_cfg_t uci_cfg;
  srslte_refsignal_srs_cfg_t srs_cfg;
  srslte_pucch_sched_t pucch_sched;  
} srslte_enb_ul_user_t; 

typedef struct SRSLTE_API {
  srslte_cell_t cell;
  
  cf_t *sf_symbols; 
  cf_t *ce; 
  
  srslte_ofdm_t     fft;
  srslte_chest_ul_t chest;
  
  srslte_pusch_t  pusch;
  srslte_pucch_t  pucch;
  srslte_prach_t  prach;
  
  srslte_pusch_cfg_t     pusch_cfg; 
  
  srslte_pusch_hopping_cfg_t hopping_cfg;
  
  // Configuration for each user
  srslte_enb_ul_user_t **users; 
  
} srslte_enb_ul_t;

typedef struct {
  uint16_t                rnti; 
  srslte_ra_ul_dci_t      grant;
  srslte_dci_location_t   location; 
  uint32_t                rv_idx; 
  uint32_t                current_tx_nb; 
  uint8_t                *data; 
  srslte_softbuffer_rx_t *softbuffer;
  bool                    needs_pdcch; 
} srslte_enb_ul_pusch_t; 

/* This function shall be called just after the initial synchronization */
SRSLTE_API int srslte_enb_ul_init(srslte_enb_ul_t *q,
                                  cf_t *in_buffer,
                                  uint32_t max_prb);

SRSLTE_API void srslte_enb_ul_free(srslte_enb_ul_t *q);

SRSLTE_API int srslte_enb_ul_set_cell(srslte_enb_ul_t *q,
                                      srslte_cell_t cell,
                                      srslte_prach_cfg_t* prach_cfg,
                                      srslte_refsignal_dmrs_pusch_cfg_t *pusch_cfg,
                                      srslte_pusch_hopping_cfg_t *hopping_cfg,
                                      srslte_pucch_cfg_t *pucch_cfg);

SRSLTE_API int srslte_enb_ul_add_rnti(srslte_enb_ul_t *q, 
                                      uint16_t rnti); 

SRSLTE_API void srslte_enb_ul_rem_rnti(srslte_enb_ul_t *q, 
                                      uint16_t rnti); 

SRSLTE_API int srslte_enb_ul_cfg_ue(srslte_enb_ul_t *q, uint16_t rnti, 
                                    srslte_uci_cfg_t *uci_cfg, 
                                    srslte_pucch_sched_t *pucch_sched,
                                    srslte_refsignal_srs_cfg_t *srs_cfg);


SRSLTE_API void srslte_enb_ul_fft(srslte_enb_ul_t *q);

SRSLTE_API int srslte_enb_ul_get_pucch(srslte_enb_ul_t *q, 
                                       uint16_t rnti, 
                                       uint32_t pdcch_n_cce, 
                                       uint32_t sf_rx, 
                                       srslte_uci_data_t *uci_data); 

SRSLTE_API int srslte_enb_ul_get_pusch(srslte_enb_ul_t *q, 
                                       srslte_ra_ul_grant_t *grant, 
                                       srslte_softbuffer_rx_t *softbuffer,
                                       uint16_t rnti, 
                                       uint32_t rv_idx, 
                                       uint32_t current_tx_nb,
                                       uint8_t *data, 
                                       srslte_cqi_value_t *cqi_value,
                                       srslte_uci_data_t *uci_data,
                                       uint32_t tti); 

SRSLTE_API int srslte_enb_ul_detect_prach(srslte_enb_ul_t *q, 
                                          uint32_t tti, 
                                          uint32_t freq_offset, 
                                          cf_t *signal, 
                                          uint32_t *indices, 
                                          float *offsets, 
                                          float *peak2avg);


#endif // SRSLTE_ENB_UL_H
