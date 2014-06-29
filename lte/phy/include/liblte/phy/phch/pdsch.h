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


#ifndef PDSCH_
#define PDSCH_

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/mimo/precoding.h"
#include "liblte/phy/mimo/layermap.h"
#include "liblte/phy/modem/mod.h"
#include "liblte/phy/modem/demod_soft.h"
#include "liblte/phy/scrambling/scrambling.h"
#include "liblte/phy/fec/rm_turbo.h"
#include "liblte/phy/fec/turbocoder.h"
#include "liblte/phy/fec/turbodecoder.h"
#include "liblte/phy/fec/crc.h"
#include "liblte/phy/phch/dci.h"
#include "liblte/phy/phch/regs.h"

#define TDEC_ITERATIONS	1

typedef _Complex float cf_t;

/* PDSCH object */
typedef struct LIBLTE_API {
  lte_cell_t cell;
  
  uint16_t max_symbols;
  uint16_t rnti;

  /* buffers */
  cf_t *ce[MAX_PORTS];
  cf_t *pdsch_symbols[MAX_PORTS];
  cf_t *pdsch_x[MAX_PORTS];
  cf_t *pdsch_d;
  char *pdsch_e_bits;
  char *cb_in_b;
  char *cb_out_b;
  float *pdsch_llr;
  float *pdsch_rm_f;

  /* tx & rx objects */
  modem_table_t mod[4];
  demod_soft_t demod;
  sequence_t seq_pdsch[NSUBFRAMES_X_FRAME];
  tcod_t encoder;
  tdec_t decoder;
  rm_turbo_t rm_turbo;
  crc_t crc_tb;
  crc_t crc_cb;
}pdsch_t;

LIBLTE_API int pdsch_init(pdsch_t *q, 
                          uint16_t user_rnti, 
                          lte_cell_t cell);

LIBLTE_API void pdsch_free(pdsch_t *q);

LIBLTE_API int pdsch_encode(pdsch_t *q, 
                            char *data, 
                            cf_t *sf_symbols[MAX_PORTS],
                            uint8_t nsubframe, 
                            ra_mcs_t mcs, 
                            ra_prb_t *prb_alloc);

LIBLTE_API int pdsch_decode(pdsch_t *q, 
                            cf_t *sf_symbols, 
                            cf_t *ce[MAX_PORTS],
                            char *data, 
                            uint8_t nsubframe, 
                            ra_mcs_t mcs, 
                            ra_prb_t *prb_alloc);

LIBLTE_API int pdsch_get(pdsch_t *q, 
                         cf_t *sf_symbols, 
                         cf_t *pdsch_symbols,
                         ra_prb_t *prb_alloc, 
                         uint8_t subframe);

#endif
