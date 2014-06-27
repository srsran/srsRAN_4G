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


#ifndef PBCH_
#define PBCH_

#include "liblte/config.h"
#include "liblte/phy/common/base.h"
#include "liblte/phy/mimo/precoding.h"
#include "liblte/phy/mimo/layermap.h"
#include "liblte/phy/modem/mod.h"
#include "liblte/phy/modem/demod_soft.h"
#include "liblte/phy/scrambling/scrambling.h"
#include "liblte/phy/fec/rm_conv.h"
#include "liblte/phy/fec/convcoder.h"
#include "liblte/phy/fec/viterbi.h"
#include "liblte/phy/fec/crc.h"

#define PBCH_RE_CPNORM    240
#define PBCH_RE_CPEXT    216

typedef _Complex float cf_t;

typedef struct LIBLTE_API {
  int nof_ports;
  int nof_prb;
  int sfn;
  phich_length_t phich_length;
  phich_resources_t phich_resources;
}pbch_mib_t;

/* PBCH object */
typedef struct LIBLTE_API {
  int cell_id;
  lte_cp_t cp;
  int nof_prb;
  int nof_symbols;

  /* buffers */
  cf_t *ce[MAX_PORTS];
  cf_t *pbch_symbols[MAX_PORTS];
  cf_t *pbch_x[MAX_PORTS];
  cf_t *pbch_d;
  float *pbch_llr;
  float *temp;
  float *pbch_rm_f;
  char *pbch_rm_b;
  char *data;
  char *data_enc;

  int frame_idx;

  /* tx & rx objects */
  modem_table_t mod;
  demod_soft_t demod;
  sequence_t seq_pbch;
  viterbi_t decoder;
  crc_t crc;
  convcoder_t encoder;

}pbch_t;

LIBLTE_API int pbch_init(pbch_t *q, int nof_prb, int cell_id, lte_cp_t cp);
LIBLTE_API void pbch_free(pbch_t *q);
LIBLTE_API int pbch_decode(pbch_t *q, cf_t *slot1_symbols, cf_t *ce[MAX_PORTS], pbch_mib_t *mib);
LIBLTE_API void pbch_encode(pbch_t *q, pbch_mib_t *mib, cf_t *slot1_symbols[MAX_PORTS], int nof_ports);
LIBLTE_API void pbch_decode_reset(pbch_t *q);

LIBLTE_API void pbch_mib_fprint(FILE *stream, pbch_mib_t *mib);

#endif // PBCH_
