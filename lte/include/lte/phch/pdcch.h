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

#include "lte/config.h"
#include "lte/common/base.h"
#include "lte/mimo/precoding.h"
#include "lte/mimo/layermap.h"
#include "lte/modem/mod.h"
#include "lte/modem/demod_soft.h"
#include "lte/scrambling/scrambling.h"
#include "lte/fec/rm_conv.h"
#include "lte/fec/convcoder.h"
#include "lte/fec/viterbi.h"
#include "lte/fec/crc.h"
#include "lte/phch/dci.h"
#include "lte/phch/regs.h"

typedef _Complex float cf_t;

#define PDCCH_NOF_SEARCH_MODES  3

typedef enum {
  SEARCH_NONE=3, SEARCH_SI=0, SEARCH_RA=1, SEARCH_UE=2
}pdcch_search_mode_t;

/*
 * A search mode is indicated by higher layers to look for SI/C/RA-RNTI
 * DCI messages as defined in Section 7.1 of 36.213
 */
typedef struct LIBLTE_API {
  int nof_candidates;
  dci_candidate_t *candidates[NSUBFRAMES_X_FRAME];
}pdcch_search_t;

/* PDCCH object */
typedef struct LIBLTE_API {
  int cell_id;
  lte_cp_t cp;
  int nof_prb;
  int nof_bits;
  int nof_symbols;
  int nof_ports;
  int nof_regs;
  int nof_cce;

  pdcch_search_t search_mode[PDCCH_NOF_SEARCH_MODES];
  pdcch_search_mode_t current_search_mode;

  regs_t *regs;

  /* buffers */
  cf_t *ce[MAX_PORTS_CTRL];
  cf_t *pdcch_symbols[MAX_PORTS_CTRL];
  cf_t *pdcch_x[MAX_PORTS_CTRL];
  cf_t *pdcch_d;
  char *pdcch_e;
  float *pdcch_llr;

  /* tx & rx objects */
  modem_table_t mod;
  demod_soft_t demod;
  sequence_t seq_pdcch[NSUBFRAMES_X_FRAME];
  viterbi_t decoder;
  crc_t crc;
}pdcch_t;

LIBLTE_API int pdcch_init(pdcch_t *q, regs_t *regs, int nof_prb, int nof_ports, int cell_id, lte_cp_t cp);
LIBLTE_API void pdcch_free(pdcch_t *q);

/* Encoding functions */
LIBLTE_API int pdcch_encode(pdcch_t *q, dci_t *dci, cf_t *slot1_symbols[MAX_PORTS_CTRL], int nsubframe);


/* Decoding functions */

/* There are two ways to decode the DCI messages:
 * a) call pdcch_set_search_si/ue/ra and then call pdcch_decode()
 * b) call pdcch_extract_llr() and then call pdcch_decode_si/ue/ra
 */

LIBLTE_API int pdcch_decode(pdcch_t *q, cf_t *slot1_symbols, cf_t *ce[MAX_PORTS_CTRL],
    dci_t *dci, int nsubframe, float ebno);
LIBLTE_API int pdcch_extract_llr(pdcch_t *q, cf_t *slot1_symbols, cf_t *ce[MAX_PORTS_CTRL], float *llr,
    int nsubframe, float ebno);

LIBLTE_API void pdcch_init_search_si(pdcch_t *q);
LIBLTE_API void pdcch_set_search_si(pdcch_t *q);
LIBLTE_API int pdcch_decode_si(pdcch_t *q, float *llr, dci_t *dci);

LIBLTE_API void pdcch_init_search_ue(pdcch_t *q, unsigned short c_rnti);
LIBLTE_API void pdcch_set_search_ue(pdcch_t *q);
LIBLTE_API int pdcch_decode_ue(pdcch_t *q, float *llr, dci_t *dci, int nsubframe);

LIBLTE_API void pdcch_init_search_ra(pdcch_t *q, unsigned short ra_rnti);
LIBLTE_API void pdcch_set_search_ra(pdcch_t *q);
LIBLTE_API int pdcch_decode_ra(pdcch_t *q, float *llr, dci_t *dci);


#endif
