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

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/ch_estimation/refsignal_ul.h"
#include "srslte/phy/phch/pusch.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/debug.h"

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
  bool delta_mcs_based;
  bool acc_enabled;
  float p0_ue_pucch;
  float p_srs_offset;  
} srslte_ue_ul_powerctrl_t;

typedef struct SRSLTE_API {
  srslte_ofdm_t fft;
  srslte_cfo_t cfo; 
  srslte_cell_t cell;
  
  bool normalize_en; 
  bool cfo_en; 

  float current_cfo_tol;
  float current_cfo; 
  srslte_pucch_format_t last_pucch_format;
  
  srslte_pusch_cfg_t pusch_cfg; 
  srslte_refsignal_ul_t signals; 
  srslte_refsignal_ul_dmrs_pregen_t pregen_drms;
  srslte_refsignal_srs_pregen_t pregen_srs;
  
  srslte_softbuffer_tx_t softbuffer;
  
  srslte_pusch_t pusch; 
  srslte_pucch_t pucch; 
  
  srslte_pucch_sched_t              pucch_sched; 
  srslte_refsignal_srs_cfg_t        srs_cfg;
  srslte_uci_cfg_t                  uci_cfg;
  srslte_pusch_hopping_cfg_t        hopping_cfg;
  srslte_ue_ul_powerctrl_t          power_ctrl;
  
  cf_t *refsignal; 
  cf_t *srs_signal; 
  cf_t *sf_symbols; 
  
  uint16_t current_rnti;  
  bool signals_pregenerated;
}srslte_ue_ul_t;



/* This function shall be called just after the initial synchronization */
SRSLTE_API int srslte_ue_ul_init(srslte_ue_ul_t *q,
                                 cf_t *out_buffer,
                                 uint32_t max_prb);

SRSLTE_API void srslte_ue_ul_free(srslte_ue_ul_t *q);

SRSLTE_API int srslte_ue_ul_set_cell(srslte_ue_ul_t *q,
                                     srslte_cell_t cell);

SRSLTE_API void srslte_ue_ul_set_cfo_tol(srslte_ue_ul_t *q,
                                         float tol);

SRSLTE_API void srslte_ue_ul_set_cfo(srslte_ue_ul_t *q,
                                     float cur_cfo); 

SRSLTE_API void srslte_ue_ul_set_cfo_enable(srslte_ue_ul_t *q,
                                            bool enabled); 

SRSLTE_API void srslte_ue_ul_set_normalization(srslte_ue_ul_t *q, 
                                               bool enabled); 

SRSLTE_API void srslte_ue_ul_set_cfg(srslte_ue_ul_t *q, 
                                     srslte_refsignal_dmrs_pusch_cfg_t *dmrs_cfg, 
                                     srslte_refsignal_srs_cfg_t        *srs_cfg,
                                     srslte_pucch_cfg_t                *pucch_cfg, 
                                     srslte_pucch_sched_t              *pucch_sched, 
                                     srslte_uci_cfg_t                  *uci_cfg,
                                     srslte_pusch_hopping_cfg_t        *hopping_cfg, 
                                     srslte_ue_ul_powerctrl_t          *power_ctrl); 

SRSLTE_API int srslte_ue_ul_cfg_grant(srslte_ue_ul_t *q, 
                                      srslte_ra_ul_grant_t *grant,
                                      uint32_t tti, 
                                      uint32_t rvidx, 
                                      uint32_t current_tx_nb); 

SRSLTE_API int srslte_ue_ul_pucch_encode(srslte_ue_ul_t *q,
                                         srslte_uci_data_t uci_data, 
                                         uint32_t pdcch_n_cce, /* Ncce of the last PDCCH message received */
                                         uint32_t tti, 
                                         cf_t *output_signal);

SRSLTE_API int srslte_ue_ul_pusch_encode(srslte_ue_ul_t *q,
                                         uint8_t *data, 
                                         cf_t *output_signal);

SRSLTE_API int srslte_ue_ul_pusch_encode_rnti(srslte_ue_ul_t *q,
                                              uint8_t *data, 
                                              uint16_t rnti, 
                                              cf_t *output_signal); 

SRSLTE_API int srslte_ue_ul_pusch_uci_encode(srslte_ue_ul_t *q,
                                             uint8_t *data, 
                                             srslte_uci_data_t uci_data, 
                                             cf_t *output_signal);

SRSLTE_API int srslte_ue_ul_pusch_uci_encode_rnti(srslte_ue_ul_t *q,
                                                  uint8_t *data,
                                                  srslte_uci_data_t uci_data, 
                                                  uint16_t rnti, 
                                                  cf_t *output_signal); 

SRSLTE_API int srslte_ue_ul_pusch_encode_rnti_softbuffer(srslte_ue_ul_t *q, 
                                                         uint8_t *data, 
                                                         srslte_uci_data_t uci_data, 
                                                         srslte_softbuffer_tx_t *softbuffer,
                                                         uint16_t rnti, 
                                                         cf_t *output_signal);

SRSLTE_API int srslte_ue_ul_srs_encode(srslte_ue_ul_t *q, 
                                       uint32_t tti, 
                                       cf_t *output_signal); 

SRSLTE_API void srslte_ue_ul_reset(srslte_ue_ul_t *q);

SRSLTE_API int srslte_ue_ul_pregen_signals(srslte_ue_ul_t *q); 

SRSLTE_API void srslte_ue_ul_set_rnti(srslte_ue_ul_t *q, 
                                      uint16_t rnti);

/* Power control procedure */
SRSLTE_API float srslte_ue_ul_pusch_power(srslte_ue_ul_t *q, 
                                          float PL, 
                                          float p0_preamble);

SRSLTE_API float srslte_ue_ul_pucch_power(srslte_ue_ul_t *q, 
                                          float PL, 
                                          srslte_pucch_format_t format, 
                                          uint32_t n_cqi, 
                                          uint32_t n_harq);

SRSLTE_API float srslte_ue_ul_srs_power(srslte_ue_ul_t *q, 
                                          float PL);

/* Other static functions for UL PHY procedures defined in 36.213 */

SRSLTE_API int srslte_ue_ul_sr_send_tti(uint32_t I_sr, 
                                        uint32_t current_tti);

SRSLTE_API bool srslte_ue_ul_srs_tx_enabled(srslte_refsignal_srs_cfg_t *srs_cfg, 
                                            uint32_t tti); 


#endif // SRSLTE_UE_UL_H
