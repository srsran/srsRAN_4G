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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "prb.h"
#include "liblte/phy/phch/regs.h"
#include "liblte/phy/phch/phich.h"
#include "liblte/phy/common/base.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

/** Table 6.9.1-2 */
const cf_t w_normal[PHICH_NORM_NSEQUENCES][4] = { { 1, 1, 1, 1 },
    { 1, -1, 1, -1 }, { 1, 1, -1, -1 }, { 1, -1, -1, 1 }, { I, I, I, I }, {
    I, -I, I, -I }, { I, I, -I, -I }, { I, -I, -I, I } };
const cf_t w_ext[PHICH_EXT_NSEQUENCES][2] = { { 1, 1 }, { 1, -1 }, { I, I }, {
I, -I } };

bool phich_exists(int nframe, int nslot) {
  return true;
}

int phich_ngroups(phich_t *q) {
  return regs_phich_ngroups(q->regs);
}

void phich_reset(phich_t *q, cf_t *slot_symbols[MAX_PORTS_CTRL]) {
  int i;
  for (i=0;i<MAX_PORTS_CTRL;i++) {
    regs_phich_reset(q->regs, slot_symbols[i]);
  }
}

/** Initializes the phich channel receiver */
int phich_init(phich_t *q, regs_t *regs, int cell_id, int nof_prb,
    int nof_tx_ports, lte_cp_t cp) {
  int ret = -1;
  bzero(q, sizeof(phich_t));
  q->cp = cp;
  q->regs = regs;
  q->nof_prb = nof_prb;
  q->nof_tx_ports = nof_tx_ports;

  if (modem_table_std(&q->mod, LTE_BPSK, false)) {
    goto clean;
  }

  demod_hard_init(&q->demod);
  demod_hard_table_set(&q->demod, LTE_BPSK);

  for (int nsf = 0; nsf < NSUBFRAMES_X_FRAME; nsf++) {
    if (sequence_phich(&q->seq_phich[nsf], 2 * nsf, cell_id)) {
      goto clean;
    }
  }

  ret = 0;
  clean: if (ret == -1) {
    phich_free(q);
  }
  return ret;
}

void phich_free(phich_t *q) {
  for (int ns = 0; ns < NSUBFRAMES_X_FRAME; ns++) {
    sequence_free(&q->seq_phich[ns]);
  }
  modem_table_free(&q->mod);
}

/* Decodes ACK
 *
 */
char phich_ack_decode(char bits[PHICH_NBITS], int *distance) {
  int i, n;

  n = 0;
  for (i = 0; i < PHICH_NBITS; i++) {
    n += bits[i];
  }
  INFO("PHICH decoder: %d, %d, %d\n", bits[0], bits[1], bits[2]);
  if (n >= 2) {
    if (distance) {
      *distance = 3-n;
    }
    return 1;
  } else {
    if (distance) {
      *distance = n;
    }
    return 0;
  }
}

/** Encodes the ACK
 *  36.212
 */
void phich_ack_encode(char ack, char bits[PHICH_NBITS]) {
  memset(bits, ack, 3 * sizeof(char));
}

/* Decodes the phich channel and saves the CFI in the cfi pointer.
 *
 * Returns 1 if successfully decoded the CFI, 0 if not and -1 on error
 */
int phich_decode(phich_t *q, cf_t *slot_symbols, cf_t *ce[MAX_PORTS_CTRL],
    int ngroup, int nseq, int nsubframe, char *ack, int *distance) {

  /* Set pointers for layermapping & precoding */
  int i, j;
  cf_t *x[MAX_LAYERS];
  cf_t *ce_precoding[MAX_PORTS];
  cf_t *symbols_precoding[MAX_PORTS];

  DEBUG("Decoding PHICH Ngroup: %d, Nseq: %d\n", ngroup, nseq);

  if (nsubframe < 0 || nsubframe > NSUBFRAMES_X_FRAME) {
    fprintf(stderr, "Invalid nslot %d\n", nsubframe);
    return -1;
  }

  if (CP_ISEXT(q->cp)) {
    if (nseq < 0 || nseq > PHICH_EXT_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return -1;
    }
  } else {
    if (nseq < 0 || nseq > PHICH_NORM_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return -1;
    }
  }
  if (ngroup >= regs_phich_ngroups(q->regs)) {
    fprintf(stderr, "Invalid ngroup %d\n", ngroup);
    return -1;
  }

  /* number of layers equals number of ports */
  for (i = 0; i < MAX_PORTS_CTRL; i++) {
    x[i] = q->phich_x[i];
  }
  for (i = 0; i < MAX_PORTS; i++) {
    ce_precoding[i] = q->ce[i];
    symbols_precoding[i] = q->phich_symbols[i];
  }

  /* extract symbols */
  if (PHICH_MAX_NSYMB
      != regs_phich_get(q->regs, slot_symbols, q->phich_symbols[0],
          ngroup)) {
    fprintf(stderr, "There was an error getting the phich symbols\n");
    return -1;
  }

  /* extract channel estimates */
  for (i = 0; i < q->nof_tx_ports; i++) {
    if (PHICH_MAX_NSYMB
        != regs_phich_get(q->regs, ce[i], q->ce[i], ngroup)) {
      fprintf(stderr, "There was an error getting the phich symbols\n");
      return -1;
    }
  }

  /* in control channels, only diversity is supported */
  if (q->nof_tx_ports == 1) {
    /* no need for layer demapping */
    predecoding_single_zf(q->phich_symbols[0], q->ce[0], q->phich_d0,
    PHICH_MAX_NSYMB);
  } else {
    predecoding_diversity_zf(symbols_precoding, ce_precoding, x,
        q->nof_tx_ports, PHICH_MAX_NSYMB);
    layerdemap_diversity(x, q->phich_d0, q->nof_tx_ports,
    PHICH_MAX_NSYMB / q->nof_tx_ports);
  }
  DEBUG("Recv!!: \n",0);
  DEBUG("d0: ",0);
  if (VERBOSE_ISDEBUG()) vec_fprint_c(stdout, q->phich_d0, PHICH_MAX_NSYMB);

  if (CP_ISEXT(q->cp)) {
    if (ngroup % 2) {
      for (i = 0; i < PHICH_EXT_MSYMB / 2; i++) {
        q->phich_d[2 * i + 0] = q->phich_d0[4 * i + 2];
        q->phich_d[2 * i + 1] = q->phich_d0[4 * i + 3];
      }
    } else {
      for (i = 0; i < PHICH_EXT_MSYMB / 2; i++) {
        q->phich_d[2 * i + 0] = q->phich_d0[4 * i];
        q->phich_d[2 * i + 1] = q->phich_d0[4 * i + 1];
      }
    }
  } else {
    memcpy(q->phich_d, q->phich_d0, PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  DEBUG("d: ",0);
  if (VERBOSE_ISDEBUG()) vec_fprint_c(stdout, q->phich_d, PHICH_EXT_MSYMB);

  scrambling_c(&q->seq_phich[nsubframe], q->phich_d);

  /* De-spreading */
  if (CP_ISEXT(q->cp)) {
    for (i=0;i<PHICH_NBITS;i++) {
      q->phich_z[i] = 0;
      for (j=0;j<PHICH_EXT_NSF;j++) {
        q->phich_z[i] += conjf(w_ext[nseq][j]) *
            q->phich_d[i*PHICH_EXT_NSF+j]/PHICH_EXT_NSF;
      }
    }
  } else {
    for (i=0;i<PHICH_NBITS;i++) {
      q->phich_z[i] = 0;
      for (j=0;j<PHICH_NORM_NSF;j++) {
        q->phich_z[i] += conjf(w_normal[nseq][j]) *
            q->phich_d[i*PHICH_NORM_NSF+j]/PHICH_NORM_NSF;
      }
    }
  }

  DEBUG("z: ",0);
  if (VERBOSE_ISDEBUG()) vec_fprint_c(stdout, q->phich_z, PHICH_NBITS);

  demod_hard_demodulate(&q->demod, q->phich_z, q->data, PHICH_NBITS);

  if (ack) {
    *ack = phich_ack_decode(q->data, distance);
  }

  return 0;
}

/** Encodes ACK/NACK bits, modulates and inserts into resource.
 * The parameter ack is an array of phich_ngroups() pointers to buffers of nof_sequences chars
 */
int phich_encode(phich_t *q, char ack, int ngroup, int nseq, int nsubframe,
    cf_t *slot_symbols[MAX_PORTS_CTRL]) {
  int i;

  if (nsubframe < 0 || nsubframe > NSUBFRAMES_X_FRAME) {
    fprintf(stderr, "Invalid nslot %d\n", nsubframe);
    return -1;
  }

  if (CP_ISEXT(q->cp)) {
    if (nseq < 0 || nseq > PHICH_EXT_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return -1;
    }
  } else {
    if (nseq < 0 || nseq > PHICH_NORM_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return -1;
    }
  }
  if (ngroup >= regs_phich_ngroups(q->regs)) {
    fprintf(stderr, "Invalid ngroup %d\n", ngroup);
    return -1;
  }

  /* Set pointers for layermapping & precoding */
  cf_t *x[MAX_LAYERS];
  cf_t *symbols_precoding[MAX_PORTS];

  /* number of layers equals number of ports */
  for (i = 0; i < q->nof_tx_ports; i++) {
    x[i] = q->phich_x[i];
  }
  for (i = 0; i < MAX_PORTS; i++) {
    symbols_precoding[i] = q->phich_symbols[i];
  }

  /* encode ACK/NACK bit */
  phich_ack_encode(ack, q->data);

  mod_modulate(&q->mod, q->data, q->phich_z, PHICH_NBITS);

  DEBUG("data: ",0);
  if (VERBOSE_ISDEBUG()) vec_fprint_c(stdout, q->phich_z, PHICH_NBITS);

  /* Spread with w */
  if (CP_ISEXT(q->cp)) {
    for (i = 0; i < PHICH_EXT_MSYMB; i++) {
      q->phich_d[i] = w_ext[nseq][i % PHICH_EXT_NSF]
          * q->phich_z[i / PHICH_EXT_NSF];
    }
  } else {
    for (i = 0; i < PHICH_NORM_MSYMB; i++) {
      q->phich_d[i] = w_normal[nseq][i % PHICH_NORM_NSF]
          * q->phich_z[i / PHICH_NORM_NSF];
    }
  }

  DEBUG("d: ",0);
  if (VERBOSE_ISDEBUG()) vec_fprint_c(stdout, q->phich_d, PHICH_EXT_MSYMB);

  scrambling_c(&q->seq_phich[nsubframe], q->phich_d);

  /* align to REG */
  if (CP_ISEXT(q->cp)) {
    if (ngroup % 2) {
      for (i = 0; i < PHICH_EXT_MSYMB / 2; i++) {
        q->phich_d0[4 * i + 0] = 0;
        q->phich_d0[4 * i + 1] = 0;
        q->phich_d0[4 * i + 2] = q->phich_d[2 * i];
        q->phich_d0[4 * i + 3] = q->phich_d[2 * i + 1];
      }
    } else {
      for (i = 0; i < PHICH_EXT_MSYMB / 2; i++) {
        q->phich_d0[4 * i + 0] = q->phich_d[2 * i];
        q->phich_d0[4 * i + 1] = q->phich_d[2 * i + 1];
        q->phich_d0[4 * i + 2] = 0;
        q->phich_d0[4 * i + 3] = 0;
      }
    }
  } else {
    memcpy(q->phich_d0, q->phich_d, PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  DEBUG("d0: ",0);
  if (VERBOSE_ISDEBUG()) vec_fprint_c(stdout, q->phich_d0, PHICH_MAX_NSYMB);


  /* layer mapping & precoding */
  if (q->nof_tx_ports > 1) {
    layermap_diversity(q->phich_d0, x, q->nof_tx_ports, PHICH_MAX_NSYMB);
    precoding_diversity(x, symbols_precoding, q->nof_tx_ports,
    PHICH_MAX_NSYMB / q->nof_tx_ports);
    /**FIXME: According to 6.9.2, Precoding for 4 tx ports is different! */
  } else {
    memcpy(q->phich_symbols[0], q->phich_d0, PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  /* mapping to resource elements */
  for (i = 0; i < q->nof_tx_ports; i++) {
    if (regs_phich_add(q->regs, q->phich_symbols[i], ngroup,
        slot_symbols[i]) < 0) {
      fprintf(stderr, "Error putting PCHICH resource elements\n");
      return -1;
    }
  }

  return 0;
}

