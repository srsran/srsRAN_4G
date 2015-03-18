/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef UEUL_H
#define UEUL_H

/*******************************************************
 * 
 * This module is a frontend to all the data and control channels processing 
 * modules. 
 ********************************************************/


#include "srslte/common/phy_common.h"
#include "srslte/ch_estimation/chest_dl.h"
#include "srslte/common/fft.h"
#include "srslte/ch_estimation/refsignal_ul.h"
#include "srslte/phch/pusch.h"
#include "srslte/phch/ra.h"
#include "srslte/sync/cfo.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

#include "srslte/config.h"

#define NOF_HARQ_PROCESSES 8

typedef struct SRSLTE_API {
  srslte_fft_t fft;
  cfo_t cfo; 
  srslte_cell_t cell;
  
  bool normalize_en; 
  
  float current_cfo; 

  srslte_refsignal_drms_pusch_cfg_t pusch_drms_cfg;
  
  srslte_refsignal_ul_t drms; 
  harq_t harq_process[NOF_HARQ_PROCESSES];
  pusch_t pusch; 
  
  cf_t *refsignal; 
  cf_t *sf_symbols; 
  
  uint16_t current_rnti;  
}ue_ul_t;

/* This function shall be called just after the initial synchronization */
SRSLTE_API int ue_ul_init(ue_ul_t *q, 
                          srslte_cell_t cell);

SRSLTE_API void ue_ul_free(ue_ul_t *q);

SRSLTE_API void ue_ul_set_cfo(ue_ul_t *q, 
                              float cur_cfo); 

SRSLTE_API void ue_ul_set_normalization(ue_ul_t *q, 
                                        bool enabled); 

SRSLTE_API void ue_ul_set_pusch_cfg(ue_ul_t *q, 
                                    srslte_refsignal_drms_pusch_cfg_t *pusch_drms_cfg, 
                                    pusch_hopping_cfg_t *pusch_hopping_cfg); 

SRSLTE_API int ue_ul_pusch_encode(ue_ul_t *q,
                                  ra_pusch_t *ra_ul, 
                                  uint8_t *data, 
                                  uint32_t sf_idx, 
                                  cf_t *output_signal);

SRSLTE_API int ue_ul_pusch_encode_rnti(ue_ul_t *q,
                                       ra_pusch_t *ra_ul, 
                                       uint8_t *data, 
                                       uint32_t sf_idx, 
                                       uint16_t rnti, 
                                       cf_t *output_signal); 

SRSLTE_API int ue_ul_pusch_uci_encode(ue_ul_t *q,
                                      ra_pusch_t *ra_ul, 
                                      uint8_t *data, 
                                      uci_data_t uci_data, 
                                      uint32_t sf_idx, 
                                      cf_t *output_signal);

SRSLTE_API int ue_ul_pusch_uci_encode_rnti(ue_ul_t *q,
                                           ra_pusch_t *ra_ul, 
                                           uint8_t *data,
                                           uci_data_t uci_data, 
                                           uint32_t sf_idx, 
                                           uint16_t rnti, 
                                           cf_t *output_signal); 

SRSLTE_API void ue_ul_reset(ue_ul_t *q);

SRSLTE_API void ue_ul_set_rnti(ue_ul_t *q, 
                               uint16_t rnti);

#endif