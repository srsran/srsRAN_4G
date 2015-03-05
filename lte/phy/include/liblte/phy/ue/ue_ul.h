/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
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


#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/ch_estimation/chest_dl.h"
#include "liblte/phy/common/fft.h"
#include "liblte/phy/ch_estimation/refsignal_ul.h"
#include "liblte/phy/phch/pusch.h"
#include "liblte/phy/phch/ra.h"
#include "liblte/phy/sync/cfo.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

#include "liblte/config.h"

#define NOF_HARQ_PROCESSES 8

typedef struct LIBLTE_API {
  lte_fft_t fft;
  cfo_t cfo; 
  lte_cell_t cell;
  
  float current_cfo; 

  refsignal_drms_pusch_cfg_t pusch_drms_cfg;
  
  refsignal_ul_t drms; 
  harq_t harq_process[NOF_HARQ_PROCESSES];
  pusch_t pusch; 
  
  cf_t *refsignal; 
  cf_t *sf_symbols; 
  
  uint16_t current_rnti;  
}ue_ul_t;

/* This function shall be called just after the initial synchronization */
LIBLTE_API int ue_ul_init(ue_ul_t *q, 
                          lte_cell_t cell);

LIBLTE_API void ue_ul_free(ue_ul_t *q);

LIBLTE_API void ue_ul_set_cfo(ue_ul_t *q, 
                              float cur_cfo); 

LIBLTE_API void ue_ul_set_pusch_cfg(ue_ul_t *q, 
                                    refsignal_drms_pusch_cfg_t *pusch_drms_cfg, 
                                    pusch_hopping_cfg_t *pusch_hopping_cfg); 

LIBLTE_API int ue_ul_pusch_encode(ue_ul_t *q,
                                  ra_pusch_t *ra_ul, 
                                  uint8_t *data, 
                                  uint32_t sf_idx, 
                                  cf_t *output_signal);

LIBLTE_API int ue_ul_pusch_encode_rnti(ue_ul_t *q,
                                       ra_pusch_t *ra_ul, 
                                       uint8_t *data, 
                                       uint32_t sf_idx, 
                                       uint16_t rnti, 
                                       cf_t *output_signal); 

LIBLTE_API int ue_ul_pusch_uci_encode(ue_ul_t *q,
                                      ra_pusch_t *ra_ul, 
                                      uint8_t *data, 
                                      uci_data_t uci_data, 
                                      uint32_t sf_idx, 
                                      cf_t *output_signal);

LIBLTE_API int ue_ul_pusch_uci_encode_rnti(ue_ul_t *q,
                                           ra_pusch_t *ra_ul, 
                                           uint8_t *data,
                                           uci_data_t uci_data, 
                                           uint32_t sf_idx, 
                                           uint16_t rnti, 
                                           cf_t *output_signal); 

LIBLTE_API void ue_ul_reset(ue_ul_t *q);

LIBLTE_API void ue_ul_set_rnti(ue_ul_t *q, 
                               uint16_t rnti);

#endif