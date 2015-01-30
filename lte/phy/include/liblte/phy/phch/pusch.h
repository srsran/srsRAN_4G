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


#ifndef PUCH_
#define PUSCH_

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/mimo/precoding.h"
#include "liblte/phy/mimo/layermap.h"
#include "liblte/phy/modem/mod.h"
#include "liblte/phy/modem/demod_soft.h"
#include "liblte/phy/scrambling/scrambling.h"
#include "liblte/phy/phch/regs.h"
#include "liblte/phy/phch/sch.h"
#include "liblte/phy/phch/harq.h"

#define TDEC_MAX_ITERATIONS         5

typedef _Complex float cf_t;

/* PDSCH object */
typedef struct LIBLTE_API {
  lte_cell_t cell;
  
  uint32_t max_symbols;
  bool rnti_is_set; 
  uint16_t rnti; 
  
  /* buffers */
  // void buffers are shared for tx and rx
  cf_t *ce[MAX_PORTS];
  cf_t *pusch_symbols[MAX_PORTS];
  cf_t *pusch_x[MAX_PORTS];
  cf_t *pusch_d;
  void *pusch_e;

  uint8_t *pusch_q_ri;
  uint8_t *pusch_q_ack;

  /* tx & rx objects */
  modem_table_t mod[4];
  demod_soft_t demod;
  sequence_t seq_pusch[NSUBFRAMES_X_FRAME];

  sch_t dl_sch;
  
}pusch_t;




LIBLTE_API int pusch_init(pusch_t *q, 
                          lte_cell_t cell);

LIBLTE_API void pusch_free(pusch_t *q);

LIBLTE_API int pusch_set_rnti(pusch_t *q, 
                               uint16_t rnti);

LIBLTE_API int pusch_encode(pusch_t *q, 
                            uint8_t *data, 
                            cf_t *sf_symbols[MAX_PORTS],
                            uint32_t nsubframe,
                            harq_t *harq_process, 
                            uint32_t rv_idx);

LIBLTE_API int pusch_uci_encode(pusch_t *q, 
                                uint8_t *data, 
                                uci_data_t uci_data, 
                                cf_t *sf_symbols[MAX_PORTS], 
                                uint32_t subframe, 
                                harq_t *harq_process, 
                                uint32_t rv_idx);

LIBLTE_API int pusch_decode(pusch_t *q, 
                            cf_t *sf_symbols, 
                            cf_t *ce[MAX_PORTS],
                            float noise_estimate, 
                            uint8_t *data, 
                            uint32_t nsubframe,
                            harq_t *harq_process, 
                            uint32_t rv_idx);

LIBLTE_API float pusch_average_noi(pusch_t *q); 

LIBLTE_API uint32_t pusch_last_noi(pusch_t *q); 

LIBLTE_API int pusch_get(pusch_t *q, 
                         cf_t *sf_symbols, 
                         cf_t *pusch_symbols,
                         ra_prb_t *prb_alloc, 
                         uint32_t subframe);

#endif
