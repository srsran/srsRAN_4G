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

#ifndef UEDL_H
#define UEDL_H

/*******************************************************
 * 
 * This module is a frontend to all the data and control channels processing 
 * modules. 
 ********************************************************/



#include "srslte/ch_estimation/chest_dl.h"
#include "srslte/dft/ofdm.h"
#include "srslte/common/phy_common.h"

#include "srslte/phch/dci.h"
#include "srslte/phch/pcfich.h"
#include "srslte/phch/pdcch.h"
#include "srslte/phch/pdsch.h"
#include "srslte/phch/phich.h"
#include "srslte/phch/ra.h"
#include "srslte/phch/regs.h"

#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

#include "srslte/config.h"

#define SRSLTE_UE_UL_NOF_HARQ_PROCESSES 8

typedef struct SRSLTE_API {
  srslte_pcfich_t pcfich;
  srslte_pdcch_t pdcch;
  srslte_pdsch_t pdsch;
  srslte_harq_t harq_process[SRSLTE_UE_UL_NOF_HARQ_PROCESSES];
  srslte_regs_t regs;
  srslte_ofdm_t fft;
  srslte_chest_dl_t chest;
  
  srslte_ra_pdsch_t ra_dl;

  srslte_cell_t cell;

  cf_t *sf_symbols; 
  cf_t *ce[SRSLTE_MAX_PORTS];
  
  srslte_dci_format_t dci_format;
  uint32_t cfi;
  uint64_t pkt_errors; 
  uint64_t pkts_total;
  uint64_t nof_detected; 

  uint16_t current_rnti;
}srslte_ue_dl_t;

/* This function shall be called just after the initial synchronization */
SRSLTE_API int srslte_ue_dl_init(srslte_ue_dl_t *q, 
                                 srslte_cell_t cell);

SRSLTE_API void srslte_ue_dl_free(srslte_ue_dl_t *q);

SRSLTE_API int srslte_ue_dl_decode_fft_estimate(srslte_ue_dl_t *q, 
                                                cf_t *input, 
                                                uint32_t sf_idx, 
                                                uint32_t *cfi); 

SRSLTE_API int srslte_ue_dl_decode_rnti_rv_packet(srslte_ue_dl_t *q, 
                                                  srslte_dci_msg_t *dci_msg, 
                                                  uint8_t *data, 
                                                  uint32_t cfi, 
                                                  uint32_t sf_idx, 
                                                  uint16_t rnti, 
                                                  uint32_t rvidx); 

SRSLTE_API int srslte_ue_dl_find_ul_dci(srslte_ue_dl_t *q, 
                                        srslte_dci_msg_t *dci_msg, 
                                        uint32_t cfi, 
                                        uint32_t sf_idx, 
                                        uint16_t rnti); 

SRSLTE_API int srslte_ue_dl_find_dl_dci(srslte_ue_dl_t *q, 
                                        srslte_dci_msg_t *dci_msg, 
                                        uint32_t cfi, 
                                        uint32_t sf_idx, 
                                        uint16_t rnti); 

SRSLTE_API int srslte_ue_dl_decode(srslte_ue_dl_t * q, 
                                   cf_t *input, 
                                   uint8_t *data,
                                   uint32_t sf_idx);

SRSLTE_API int srslte_ue_dl_decode_rnti(srslte_ue_dl_t * q, 
                                        cf_t *input, 
                                        uint8_t *data,
                                        uint32_t sf_idx,
                                        uint16_t rnti);

SRSLTE_API int srslte_ue_dl_decode_rnti_rv(srslte_ue_dl_t * q, 
                                           cf_t *input, 
                                           uint8_t * data,
                                           uint32_t sf_idx, 
                                           uint16_t rnti, 
                                           uint32_t rvidx); 

SRSLTE_API void srslte_ue_dl_reset(srslte_ue_dl_t *q);

SRSLTE_API void srslte_ue_dl_set_rnti(srslte_ue_dl_t *q, 
                                      uint16_t rnti);

#endif