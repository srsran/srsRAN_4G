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
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

/** Table 6.9.1-2 */
const cf_t w_normal[PHICH_NORM_NSEQUENCES][4] = { { 1, 1, 1, 1 },
    { 1, -1, 1, -1 }, { 1, 1, -1, -1 }, { 1, -1, -1, 1 }, { I, I, I, I }, {
    I, -I, I, -I }, { I, I, -I, -I }, { I, -I, -I, I } };
const cf_t w_ext[PHICH_EXT_NSEQUENCES][2] = { { 1, 1 }, { 1, -1 }, { I, I }, {
I, -I } };


uint32_t phich_ngroups(phich_t *q) {
  return regs_phich_ngroups(q->regs);
}

void phich_reset(phich_t *q, cf_t *slot_symbols[MAX_PORTS]) {
  int i;
  for (i = 0; i < MAX_PORTS; i++) {
    regs_phich_reset(q->regs, slot_symbols[i]);
  }
}

/** Initializes the phich channel receiver */
int phich_init(phich_t *q, regs_t *regs, lte_cell_t cell) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q         != NULL &&
      regs      != NULL &&
      lte_cell_isvalid(&cell)) 
  {

    bzero(q, sizeof(phich_t));
    ret = LIBLTE_ERROR;
    
    q->cell = cell;
    q->regs = regs;
    
    if (precoding_init(&q->precoding, SF_LEN_RE(cell.nof_prb, cell.cp))) {
      fprintf(stderr, "Error initializing precoding\n");
    }

    if (modem_table_lte(&q->mod, LTE_BPSK, false)) {
      goto clean;
    }

    demod_hard_init(&q->demod);
    demod_hard_table_set(&q->demod, LTE_BPSK);

    for (int nsf = 0; nsf < NSUBFRAMES_X_FRAME; nsf++) {
      if (sequence_phich(&q->seq_phich[nsf], 2 * nsf, q->cell.id)) {
        goto clean;
      }
    }
    ret = LIBLTE_SUCCESS;
  }
  clean: 
  if (ret == LIBLTE_ERROR) {
    phich_free(q);
  }
  return ret;
}

void phich_free(phich_t *q) {
  for (int ns = 0; ns < NSUBFRAMES_X_FRAME; ns++) {
    sequence_free(&q->seq_phich[ns]);
  }
  modem_table_free(&q->mod);
  precoding_free(&q->precoding);

  bzero(q, sizeof(phich_t));

}

/* Decodes ACK
 *
 */
uint8_t phich_ack_decode(uint8_t bits[PHICH_NBITS], uint32_t *distance) {
  int i, n;

  n = 0;
  for (i = 0; i < PHICH_NBITS; i++) {
    n += bits[i];
  }
  INFO("PHICH decoder: %d, %d, %d\n", bits[0], bits[1], bits[2]);
  if (n >= 2) {
    if (distance) {
      *distance = 3 - n;
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
void phich_ack_encode(uint8_t ack, uint8_t bits[PHICH_NBITS]) {
  memset(bits, ack, 3 * sizeof(uint8_t));
}

/* Decodes the phich channel and saves the CFI in the cfi pointer.
 *
 * Returns 1 if successfully decoded the CFI, 0 if not and -1 on error
 */
int phich_decode(phich_t *q, cf_t *slot_symbols, cf_t *ce[MAX_PORTS], float noise_estimate,
    uint32_t ngroup, uint32_t nseq, uint32_t subframe, uint8_t *ack, uint32_t *distance) {

  /* Set pointers for layermapping & precoding */
  int i, j;
  cf_t *x[MAX_LAYERS];
  cf_t *ce_precoding[MAX_PORTS];
  
  if (q == NULL || slot_symbols == NULL) {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }

  if (subframe >= NSUBFRAMES_X_FRAME) {
    fprintf(stderr, "Invalid nslot %d\n", subframe);
    return LIBLTE_ERROR_INVALID_INPUTS;
  }

  if (CP_ISEXT(q->cell.cp)) {
    if (nseq >= PHICH_EXT_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return LIBLTE_ERROR_INVALID_INPUTS;
    }
  } else {
    if (nseq >= PHICH_NORM_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return LIBLTE_ERROR_INVALID_INPUTS;
    }
  }
  if (ngroup >= regs_phich_ngroups(q->regs)) {
    fprintf(stderr, "Invalid ngroup %d\n", ngroup);
    return LIBLTE_ERROR_INVALID_INPUTS;
  }

  DEBUG("Decoding PHICH Ngroup: %d, Nseq: %d\n", ngroup, nseq);

  /* number of layers equals number of ports */
  for (i = 0; i < MAX_PORTS; i++) {
    x[i] = q->phich_x[i];
  }
  for (i = 0; i < MAX_PORTS; i++) {
    ce_precoding[i] = q->ce[i];
  }

  /* extract symbols */
  if (PHICH_MAX_NSYMB
      != regs_phich_get(q->regs, slot_symbols, q->phich_symbols[0], ngroup)) {
    fprintf(stderr, "There was an error getting the phich symbols\n");
    return LIBLTE_ERROR;
  }

  /* extract channel estimates */
  for (i = 0; i < q->cell.nof_ports; i++) {
    if (PHICH_MAX_NSYMB != regs_phich_get(q->regs, ce[i], q->ce[i], ngroup)) {
      fprintf(stderr, "There was an error getting the phich symbols\n");
      return LIBLTE_ERROR;
    }
  }

  /* in control channels, only diversity is supported */
  if (q->cell.nof_ports == 1) {
    /* no need for layer demapping */
    predecoding_single(&q->precoding, q->phich_symbols[0], q->ce[0], q->phich_d0,
    PHICH_MAX_NSYMB, noise_estimate);
  } else {
    predecoding_diversity(&q->precoding, q->phich_symbols[0], ce_precoding, x,
        q->cell.nof_ports, PHICH_MAX_NSYMB, noise_estimate);
    layerdemap_diversity(x, q->phich_d0, q->cell.nof_ports,
    PHICH_MAX_NSYMB / q->cell.nof_ports);
  }
  DEBUG("Recv!!: \n", 0);
  DEBUG("d0: ", 0);
  if (VERBOSE_ISDEBUG())
    vec_fprint_c(stdout, q->phich_d0, PHICH_MAX_NSYMB);

  if (CP_ISEXT(q->cell.cp)) {
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

  DEBUG("d: ", 0);
  if (VERBOSE_ISDEBUG())
    vec_fprint_c(stdout, q->phich_d, PHICH_EXT_MSYMB);

  scrambling_c(&q->seq_phich[subframe], q->phich_d);

  /* De-spreading */
  if (CP_ISEXT(q->cell.cp)) {
    for (i = 0; i < PHICH_NBITS; i++) {
      q->phich_z[i] = 0;
      for (j = 0; j < PHICH_EXT_NSF; j++) {
        q->phich_z[i] += conjf(w_ext[nseq][j])
            * q->phich_d[i * PHICH_EXT_NSF + j] / PHICH_EXT_NSF;
      }
    }
  } else {
    for (i = 0; i < PHICH_NBITS; i++) {
      q->phich_z[i] = 0;
      for (j = 0; j < PHICH_NORM_NSF; j++) {
        q->phich_z[i] += conjf(w_normal[nseq][j])
            * q->phich_d[i * PHICH_NORM_NSF + j] / PHICH_NORM_NSF;
      }
    }
  }

  DEBUG("z: ", 0);
  if (VERBOSE_ISDEBUG())
    vec_fprint_c(stdout, q->phich_z, PHICH_NBITS);

  demod_hard_demodulate(&q->demod, q->phich_z, q->data, PHICH_NBITS);

  if (ack) {
    *ack = phich_ack_decode(q->data, distance);
  }

  return LIBLTE_SUCCESS;
}

/** Encodes ACK/NACK bits, modulates and inserts into resource.
 * The parameter ack is an array of phich_ngroups() pointers to buffers of nof_sequences uint8_ts
 */
int phich_encode(phich_t *q, uint8_t ack, uint32_t ngroup, uint32_t nseq, uint32_t subframe,
    cf_t *slot_symbols[MAX_PORTS]) {
  int i;

  if (q == NULL || slot_symbols == NULL) {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }

  if (subframe >= NSUBFRAMES_X_FRAME) {
    fprintf(stderr, "Invalid nslot %d\n", subframe);
    return LIBLTE_ERROR_INVALID_INPUTS;
  }

  if (CP_ISEXT(q->cell.cp)) {
    if (nseq >= PHICH_EXT_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return LIBLTE_ERROR_INVALID_INPUTS;
    }
  } else {
    if (nseq >= PHICH_NORM_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return LIBLTE_ERROR_INVALID_INPUTS;
    }
  }
  if (ngroup >= regs_phich_ngroups(q->regs)) {
    fprintf(stderr, "Invalid ngroup %d\n", ngroup);
    return LIBLTE_ERROR_INVALID_INPUTS;
  }


  /* Set pointers for layermapping & precoding */
  cf_t *x[MAX_LAYERS];
  cf_t *symbols_precoding[MAX_PORTS];

  /* number of layers equals number of ports */
  for (i = 0; i < q->cell.nof_ports; i++) {
    x[i] = q->phich_x[i];
  }
  for (i = 0; i < MAX_PORTS; i++) {
    symbols_precoding[i] = q->phich_symbols[i];
  }

  /* encode ACK/NACK bit */
  phich_ack_encode(ack, q->data);

  mod_modulate(&q->mod, q->data, q->phich_z, PHICH_NBITS);

  DEBUG("data: ", 0);
  if (VERBOSE_ISDEBUG())
    vec_fprint_c(stdout, q->phich_z, PHICH_NBITS);

  /* Spread with w */
  if (CP_ISEXT(q->cell.cp)) {
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

  DEBUG("d: ", 0);
  if (VERBOSE_ISDEBUG())
    vec_fprint_c(stdout, q->phich_d, PHICH_EXT_MSYMB);

  scrambling_c(&q->seq_phich[subframe], q->phich_d);

  /* align to REG */
  if (CP_ISEXT(q->cell.cp)) {
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

  DEBUG("d0: ", 0);
  if (VERBOSE_ISDEBUG())
    vec_fprint_c(stdout, q->phich_d0, PHICH_MAX_NSYMB);

  /* layer mapping & precoding */
  if (q->cell.nof_ports > 1) {
    layermap_diversity(q->phich_d0, x, q->cell.nof_ports, PHICH_MAX_NSYMB);
    precoding_diversity(&q->precoding, x, symbols_precoding, q->cell.nof_ports,
    PHICH_MAX_NSYMB / q->cell.nof_ports);
    /**FIXME: According to 6.9.2, Precoding for 4 tx ports is different! */
  } else {
    memcpy(q->phich_symbols[0], q->phich_d0, PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  /* mapping to resource elements */
  for (i = 0; i < q->cell.nof_ports; i++) {
    if (regs_phich_add(q->regs, q->phich_symbols[i], ngroup, slot_symbols[i])
        < 0) {
      fprintf(stderr, "Error putting PCHICH resource elements\n");
      return LIBLTE_ERROR;
    }
  }

  return LIBLTE_SUCCESS;
}

