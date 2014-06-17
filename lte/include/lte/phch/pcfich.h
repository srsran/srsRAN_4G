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


#ifndef PCFICH_
#define PCFICH_

#include "lte/config.h"
#include "lte/common/base.h"
#include "lte/mimo/precoding.h"
#include "lte/mimo/layermap.h"
#include "lte/modem/mod.h"
#include "lte/modem/demod_hard.h"
#include "lte/scrambling/scrambling.h"
#include "lte/phch/regs.h"

#define PCFICH_CFI_LEN    32
#define PCFICH_RE      PCFICH_CFI_LEN/2
#define PCFICH_MAX_DISTANCE  5

typedef _Complex float cf_t;

/* PCFICH object */
typedef struct LIBLTE_API {
  int cell_id;
  lte_cp_t cp;
  int nof_symbols;
  int nof_prb;
  int nof_tx_ports;

  /* handler to REGs resource mapper */
  regs_t *regs;

  /* buffers */
  cf_t ce[MAX_PORTS_CTRL][PCFICH_RE];
  cf_t pcfich_symbols[MAX_PORTS_CTRL][PCFICH_RE];
  cf_t pcfich_x[MAX_PORTS_CTRL][PCFICH_RE];
  cf_t pcfich_d[PCFICH_RE];

  /* bit message */
  char data[PCFICH_CFI_LEN];

  /* tx & rx objects */
  modem_table_t mod;
  demod_hard_t demod;
  sequence_t seq_pcfich[NSUBFRAMES_X_FRAME];

}pcfich_t;

LIBLTE_API int pcfich_init(pcfich_t *q, regs_t *regs, int cell_id, int nof_prb, int nof_tx_ports, lte_cp_t cp);
LIBLTE_API void pcfich_free(pcfich_t *q);
LIBLTE_API int pcfich_decode(pcfich_t *q, cf_t *slot_symbols, cf_t *ce[MAX_PORTS_CTRL], int nsubframe, int *cfi, int *distance);
LIBLTE_API int pcfich_encode(pcfich_t *q, int cfi, cf_t *slot_symbols[MAX_PORTS_CTRL], int nsubframe);

LIBLTE_API bool pcfich_exists(int nframe, int nslot);
LIBLTE_API int pcfich_put(regs_t *h, cf_t *pcfich, cf_t *slot_data);
LIBLTE_API int pcfich_get(regs_t *h, cf_t *pcfich, cf_t *slot_data);

#endif // PCFICH_
