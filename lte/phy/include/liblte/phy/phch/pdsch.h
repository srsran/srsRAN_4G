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
#include "liblte/phy/phch/dci.h"
#include "liblte/phy/phch/regs.h"
#include "liblte/phy/phch/sch.h"
#include "liblte/phy/phch/harq.h"

#define TDEC_MAX_ITERATIONS         5

typedef _Complex float cf_t;

/* PDSCH object */
typedef struct LIBLTE_API {
  lte_cell_t cell;
  
  uint32_t max_re;
  bool rnti_is_set; 
  uint16_t rnti; 
  
  /* buffers */
  // void buffers are shared for tx and rx
  cf_t *ce[MAX_PORTS];
  cf_t *pdsch_symbols[MAX_PORTS];
  cf_t *pdsch_x[MAX_PORTS];
  cf_t *pdsch_d;
  void *pdsch_e;

  /* tx & rx objects */
  modem_table_t mod[4];
  demod_soft_t demod;
  sequence_t seq_pdsch[NSUBFRAMES_X_FRAME];
  precoding_t precoding; 

  sch_t dl_sch;
  
}pdsch_t;

LIBLTE_API int pdsch_init(pdsch_t *q, 
                          lte_cell_t cell);

LIBLTE_API void pdsch_free(pdsch_t *q);

LIBLTE_API int pdsch_set_rnti(pdsch_t *q, 
                               uint16_t rnti);

LIBLTE_API int pdsch_encode(pdsch_t *q,
                            harq_t *harq_process,
                            uint8_t *data, 
                            cf_t *sf_symbols[MAX_PORTS]);

LIBLTE_API int pdsch_encode_rnti(pdsch_t *q,
                                 harq_t *harq_process,
                                 uint8_t *data, 
                                 uint16_t rnti,
                                 cf_t *sf_symbols[MAX_PORTS]);

LIBLTE_API int pdsch_decode(pdsch_t *q, 
                            harq_t *harq_process, 
                            cf_t *sf_symbols, 
                            cf_t *ce[MAX_PORTS],
                            float noise_estimate, 
                            uint8_t *data);

LIBLTE_API int pdsch_decode_rnti(pdsch_t *q, 
                                 harq_t *harq_process, 
                                 cf_t *sf_symbols, 
                                 cf_t *ce[MAX_PORTS],
                                 float noise_estimate, 
                                 uint16_t rnti,
                                 uint8_t *data);

LIBLTE_API float pdsch_average_noi(pdsch_t *q); 

LIBLTE_API uint32_t pdsch_last_noi(pdsch_t *q); 

#endif
