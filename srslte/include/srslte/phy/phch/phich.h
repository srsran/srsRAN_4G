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


#ifndef PHICH_
#define PHICH_

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/modem/demod_hard.h"
#include "srslte/phy/scrambling/scrambling.h"
#include "regs.h"

typedef _Complex float cf_t;

#define PHICH_NORM_NSEQUENCES  8
#define PHICH_EXT_NSEQUENCES  4
#define PHICH_MAX_SEQUENCES    PHICH_NORM_NSEQUENCES
#define PHICH_NBITS        3

#define PHICH_NORM_MSYMB    PHICH_NBITS * 4
#define PHICH_EXT_MSYMB      PHICH_NBITS * 2
#define PHICH_MAX_NSYMB      PHICH_NORM_MSYMB
#define PHICH_NORM_C      1
#define PHICH_EXT_C        2
#define PHICH_NORM_NSF      4
#define PHICH_EXT_NSF      2

/* phich object */
typedef struct LIBLTE_API {
  lte_cell_t cell;
  
  /* handler to REGs resource mapper */
  regs_t *regs;

  /* buffers */
  cf_t ce[MAX_PORTS][PHICH_MAX_NSYMB];
  cf_t phich_symbols[MAX_PORTS][PHICH_MAX_NSYMB];
  cf_t phich_x[MAX_PORTS][PHICH_MAX_NSYMB];
  cf_t phich_d[PHICH_MAX_NSYMB];
  cf_t phich_d0[PHICH_MAX_NSYMB];
  cf_t phich_z[PHICH_NBITS];

  /* bit message */
  uint8_t data[PHICH_NBITS];

  /* tx & rx objects */
  modem_table_t mod;
  demod_hard_t demod;
  sequence_t seq_phich[NSUBFRAMES_X_FRAME];
  precoding_t precoding; 

}phich_t;

LIBLTE_API int phich_init(phich_t *q, 
                          regs_t *regs, 
                          lte_cell_t cell);

LIBLTE_API void phich_free(phich_t *q);

LIBLTE_API int phich_decode(phich_t *q, 
                            cf_t *slot_symbols, 
                            cf_t *ce[MAX_PORTS],
                            float noise_estimate, 
                            uint32_t ngroup, 
                            uint32_t nseq, 
                            uint32_t nsubframe, 
                            uint8_t *ack, 
                            uint32_t *distance);

LIBLTE_API int phich_encode(phich_t *q, 
                            uint8_t ack, 
                            uint32_t ngroup, 
                            uint32_t nseq, 
                            uint32_t nsubframe,
                            cf_t *slot_symbols[MAX_PORTS]);

LIBLTE_API void phich_reset(phich_t *q, 
                            cf_t *slot_symbols[MAX_PORTS]);

LIBLTE_API uint32_t phich_ngroups(phich_t *q);

#endif // PHICH_
