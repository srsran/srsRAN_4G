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
 *  File:         pdcch.h
 *
 *  Description:  Physical downlink control channel.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.8
 *****************************************************************************/

#ifndef SRSLTE_PDCCH_H
#define SRSLTE_PDCCH_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/scrambling/scrambling.h"
#include "srslte/phy/fec/rm_conv.h"
#include "srslte/phy/fec/convcoder.h"
#include "srslte/phy/fec/viterbi.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/regs.h"




typedef enum SRSLTE_API {
  SEARCH_UE, SEARCH_COMMON
} srslte_pdcch_search_mode_t;


/* PDCCH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;
  uint32_t nof_regs[3];
  uint32_t nof_cce[3];
  uint32_t max_bits;
  uint32_t nof_rx_antennas;
  bool     is_ue;
  
  srslte_regs_t *regs;

  /* buffers */
  cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  cf_t *symbols[SRSLTE_MAX_PORTS];
  cf_t *x[SRSLTE_MAX_PORTS];
  cf_t *d;
  uint8_t *e;
  float rm_f[3 * (SRSLTE_DCI_MAX_BITS + 16)];
  float *llr;

  /* tx & rx objects */
  srslte_modem_table_t mod;
  srslte_sequence_t seq[SRSLTE_NSUBFRAMES_X_FRAME];
  srslte_viterbi_t decoder;
  srslte_crc_t crc;
  
} srslte_pdcch_t;

SRSLTE_API int srslte_pdcch_init_ue(srslte_pdcch_t *q,
                                    uint32_t max_prb,
                                    uint32_t nof_rx_antennas);

SRSLTE_API int srslte_pdcch_init_enb(srslte_pdcch_t *q,
                                     uint32_t max_prb);

SRSLTE_API int srslte_pdcch_set_cell(srslte_pdcch_t *q,
                                     srslte_regs_t *regs,
                                     srslte_cell_t cell);

SRSLTE_API void srslte_pdcch_free(srslte_pdcch_t *q);


SRSLTE_API float srslte_pdcch_coderate(uint32_t nof_bits,
                                       uint32_t l); 

/* Encoding function */
SRSLTE_API int srslte_pdcch_encode(srslte_pdcch_t *q, 
                                   srslte_dci_msg_t *msg,
                                   srslte_dci_location_t location,
                                   uint16_t rnti,
                                   cf_t *sf_symbols[SRSLTE_MAX_PORTS],
                                   uint32_t nsubframe, 
                                   uint32_t cfi);

/* Decoding functions: Extract the LLRs and save them in the srslte_pdcch_t object */
SRSLTE_API int srslte_pdcch_extract_llr(srslte_pdcch_t *q, 
                                        cf_t *sf_symbols, 
                                        cf_t *ce[SRSLTE_MAX_PORTS],
                                        float noise_estimate, 
                                        uint32_t nsubframe, 
                                        uint32_t cfi);

SRSLTE_API int srslte_pdcch_extract_llr_multi(srslte_pdcch_t *q, 
                                              cf_t *sf_symbols[SRSLTE_MAX_PORTS], 
                                              cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                              float noise_estimate, 
                                              uint32_t nsubframe, 
                                              uint32_t cfi);

/* Decoding functions: Try to decode a DCI message after calling srslte_pdcch_extract_llr */
SRSLTE_API int srslte_pdcch_decode_msg(srslte_pdcch_t *q, 
                                       srslte_dci_msg_t *msg, 
                                       srslte_dci_location_t *location,
                                       srslte_dci_format_t format,
                                       uint32_t cfi,
                                       uint16_t *crc_rem);

SRSLTE_API int srslte_pdcch_dci_decode(srslte_pdcch_t *q, 
                                 float *e, 
                                 uint8_t *data, 
                                 uint32_t E, 
                                 uint32_t nof_bits, 
                                 uint16_t *crc); 

SRSLTE_API int srslte_pdcch_dci_encode(srslte_pdcch_t *q, 
                                       uint8_t *data, 
                                       uint8_t *e, 
                                       uint32_t nof_bits, 
                                       uint32_t E,
                                       uint16_t rnti); 

SRSLTE_API void srslte_pdcch_dci_encode_conv(srslte_pdcch_t *q, 
                                            uint8_t *data, 
                                            uint32_t nof_bits, 
                                            uint8_t *coded_data, 
                                            uint16_t rnti); 

/* Function for generation of UE-specific search space DCI locations */
SRSLTE_API uint32_t srslte_pdcch_ue_locations(srslte_pdcch_t *q, 
                                              srslte_dci_location_t *locations, 
                                              uint32_t max_locations,
                                              uint32_t nsubframe, 
                                              uint32_t cfi,
                                              uint16_t rnti);

SRSLTE_API uint32_t srslte_pdcch_ue_locations_ncce(uint32_t nof_cce, 
                                                   srslte_dci_location_t *c, 
                                                   uint32_t max_candidates, 
                                                   uint32_t nsubframe,
                                                   uint16_t rnti);

SRSLTE_API uint32_t srslte_pdcch_ue_locations_ncce_L(uint32_t nof_cce,
                                                     srslte_dci_location_t *c,
                                                     uint32_t max_candidates,
                                                     uint32_t nsubframe,
                                                     uint16_t rnti,
                                                     int L);

/* Function for generation of common search space DCI locations */
SRSLTE_API uint32_t srslte_pdcch_common_locations(srslte_pdcch_t *q, 
                                                  srslte_dci_location_t *locations, 
                                                  uint32_t max_locations,
                                                  uint32_t cfi);

SRSLTE_API uint32_t srslte_pdcch_common_locations_ncce(uint32_t nof_cce, 
                                                       srslte_dci_location_t *c, 
                                                       uint32_t max_candidates); 


#endif // SRSLTE_PDCCH_H
