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

#ifndef ENBUL_H
#define ENBUL_H

#include <stdbool.h>

#include "srslte/common/phy_common.h"
#include "srslte/dft/ofdm.h"
#include "srslte/ch_estimation/refsignal_ul.h"
#include "srslte/phch/prach.h"
#include "srslte/phch/pusch.h"
#include "srslte/phch/pusch_cfg.h"
#include "srslte/phch/ra.h"

#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

#include "srslte/config.h"

typedef struct SRSLTE_API {
  srslte_cell_t cell;

  cf_t *sf_symbols[SRSLTE_MAX_PORTS]; 
  
  srslte_ofdm_t   fft;
  srslte_pusch_t  pusch;
  srslte_prach_t  prach;
  
  srslte_refsignal_ul_t ul_refs;
  srslte_pusch_cfg_t pusch_cfg; 
  srslte_softbuffer_rx_t softbuffer;
      
} srslte_enb_ul_t;

typedef struct {
  srslte_ra_ul_dci_t    grant;
  uint32_t              rnti_idx; 
  uint32_t              rv_idx; 
  uint8_t              *data; 
} srslte_enb_ul_pusch_t; 

/* This function shall be called just after the initial synchronization */
SRSLTE_API int srslte_enb_ul_init(srslte_enb_ul_t *q, 
                                  srslte_cell_t cell, 
                                  uint32_t nof_rntis);

SRSLTE_API void srslte_enb_ul_free(srslte_enb_ul_t *q);

SRSLTE_API void srslte_enb_ul_set_cfi(srslte_enb_ul_t *q, 
                                      uint32_t cfi);

SRSLTE_API void srslte_enb_ul_rx_signal(srslte_enb_ul_t *q, 
                                        cf_t *signal_buffer); 

SRSLTE_API int srslte_enb_ul_add_rnti(srslte_enb_ul_t *q, 
                                      uint32_t idx, 
                                      uint16_t rnti); 

SRSLTE_API int srslte_enb_ul_get_pusch(srslte_enb_ul_t *q, 
                                       srslte_ra_ul_dci_t *grant, 
                                       uint32_t rnti_idx,
                                       uint32_t rv_idx,
                                       uint8_t *data,
                                       uint32_t sf_idx); 

SRSLTE_API int srslte_enb_ul_get_pusch_multi(srslte_enb_ul_t *q, 
                                             srslte_enb_ul_pusch_t *grants, 
                                             uint32_t nof_pusch,
                                             uint32_t sf_idx); 

SRSLTE_API int srslte_enb_ul_get_prach(srslte_enb_ul_t *q, 
                                       uint32_t sf_idx);


#endif
