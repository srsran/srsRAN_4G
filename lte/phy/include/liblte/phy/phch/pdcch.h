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

#ifndef PDCCH_
#define PDCCH_

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/mimo/precoding.h"
#include "liblte/phy/mimo/layermap.h"
#include "liblte/phy/modem/mod.h"
#include "liblte/phy/modem/demod_soft.h"
#include "liblte/phy/scrambling/scrambling.h"
#include "liblte/phy/fec/rm_conv.h"
#include "liblte/phy/fec/convcoder.h"
#include "liblte/phy/fec/viterbi.h"
#include "liblte/phy/fec/crc.h"
#include "liblte/phy/phch/dci.h"
#include "liblte/phy/phch/regs.h"

typedef _Complex float cf_t;


typedef enum LIBLTE_API {
  SEARCH_UE, SEARCH_COMMON
} pdcch_search_mode_t;


/* PDCCH object */
typedef struct LIBLTE_API {
  lte_cell_t cell;
  uint32_t e_bits;
  uint32_t nof_regs;
  uint32_t nof_cce;
  uint32_t max_bits;

  regs_t *regs;

  /* buffers */
  cf_t *ce[MAX_PORTS];
  cf_t *pdcch_symbols[MAX_PORTS];
  cf_t *pdcch_x[MAX_PORTS];
  cf_t *pdcch_d;
  uint8_t *pdcch_e;
  float *pdcch_llr;

  /* tx & rx objects */
  modem_table_t mod;
  demod_soft_t demod;
  sequence_t seq_pdcch[NSUBFRAMES_X_FRAME];
  viterbi_t decoder;
  crc_t crc;
} pdcch_t;

LIBLTE_API int pdcch_init(pdcch_t *q, 
                          regs_t *regs, 
                          lte_cell_t cell);

LIBLTE_API void pdcch_free(pdcch_t *q);


/* Encoding function */
LIBLTE_API int pdcch_encode(pdcch_t *q, 
                                dci_msg_t *msg,
                                dci_location_t location,
                                uint16_t rnti,
                                cf_t *sf_symbols[MAX_PORTS],
                                uint32_t nsubframe, 
                                uint32_t cfi);

/* Decoding functions: Extract the LLRs and save them in the pdcch_t object */
LIBLTE_API int pdcch_extract_llr(pdcch_t *q, 
                                 cf_t *sf_symbols, 
                                 cf_t *ce[MAX_PORTS],
                                 dci_location_t location,
                                 uint32_t nsubframe, 
                                 uint32_t cfi);

/* Decoding functions: Try to decode a DCI message after calling pdcch_extract_llr */
LIBLTE_API int pdcch_decode_msg(pdcch_t *q, 
                                dci_msg_t *msg, 
                                dci_format_t format,
                                uint16_t *crc_rem);

/* Function for generation of UE-specific search space DCI locations */
LIBLTE_API uint32_t pdcch_ue_locations(pdcch_t *q, 
                                       dci_location_t *locations, 
                                       uint32_t max_locations,
                                       uint32_t nsubframe, 
                                       uint32_t cfi,
                                       uint16_t rnti);

/* Function for generation of common search space DCI locations */
LIBLTE_API uint32_t pdcch_common_locations(pdcch_t *q, 
                                           dci_location_t *locations, 
                                           uint32_t max_locations,
                                           uint32_t cfi);

#endif
