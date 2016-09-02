/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
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

#include "srslte/phch/regs.h"
#include "srslte/phch/phich.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/bit.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

/** Table 6.9.1-2 */
const cf_t w_normal[SRSLTE_PHICH_NORM_NSEQUENCES][4] = { { 1, 1, 1, 1 },
    { 1, -1, 1, -1 }, { 1, 1, -1, -1 }, { 1, -1, -1, 1 }, { I, I, I, I }, {
    I, -I, I, -I }, { I, I, -I, -I }, { I, -I, -I, I } };
const cf_t w_ext[SRSLTE_PHICH_EXT_NSEQUENCES][2] = { { 1, 1 }, { 1, -1 }, { I, I }, {
I, -I } };


uint32_t srslte_phich_ngroups(srslte_phich_t *q) {
  return srslte_regs_phich_ngroups(q->regs);
}

uint32_t srslte_phich_nsf(srslte_phich_t *q) {
  if (SRSLTE_CP_ISNORM(q->cell.cp)) {
    return SRSLTE_PHICH_NORM_NSF;
  } else {
    return SRSLTE_PHICH_EXT_NSF; 
  }
}

void srslte_phich_reset(srslte_phich_t *q, cf_t *slot_symbols[SRSLTE_MAX_PORTS]) {
  int i;
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    srslte_regs_phich_reset(q->regs, slot_symbols[i]);
  }
}

/** Initializes the phich channel receiver */
int srslte_phich_init(srslte_phich_t *q, srslte_regs_t *regs, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q         != NULL &&
      regs      != NULL &&
      srslte_cell_isvalid(&cell)) 
  {

    bzero(q, sizeof(srslte_phich_t));
    ret = SRSLTE_ERROR;
    
    q->cell = cell;
    q->regs = regs;
    
    if (srslte_modem_table_lte(&q->mod, SRSLTE_MOD_BPSK)) {
      goto clean;
    }

    for (int nsf = 0; nsf < SRSLTE_NSUBFRAMES_X_FRAME; nsf++) {
      if (srslte_sequence_phich(&q->seq[nsf], 2 * nsf, q->cell.id)) {
        goto clean;
      }
    }
    ret = SRSLTE_SUCCESS;
  }
  clean: 
  if (ret == SRSLTE_ERROR) {
    srslte_phich_free(q);
  }
  return ret;
}

void srslte_phich_free(srslte_phich_t *q) {
  for (int ns = 0; ns < SRSLTE_NSUBFRAMES_X_FRAME; ns++) {
    srslte_sequence_free(&q->seq[ns]);
  }
  srslte_modem_table_free(&q->mod);

  bzero(q, sizeof(srslte_phich_t));

}

/* Computes n_group and n_seq according to Section 9.1.2 in 36.213 */
void srslte_phich_calc(srslte_phich_t *q, uint32_t n_prb_lowest, uint32_t n_dmrs, 
                       uint32_t *ngroup, uint32_t *nseq) 
{
  uint32_t Ngroups = srslte_phich_ngroups(q); 
  *ngroup = (n_prb_lowest+n_dmrs)%Ngroups;
  *nseq = ((n_prb_lowest/Ngroups)+n_dmrs)%(2*srslte_phich_nsf(q)); 
}


/* Decodes ACK
 *
 */
uint8_t srslte_phich_ack_decode(float bits[SRSLTE_PHICH_NBITS], float *distance) {
  int i;
  float ack_table[2][3] = {{-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0}}; 
  float max_corr = -9999; 
  uint8_t index=0; 
  
  if (SRSLTE_VERBOSE_ISINFO()) {
    INFO("Received bits: ", 0);
    srslte_vec_fprint_f(stdout, bits, SRSLTE_PHICH_NBITS);
  }
  
  for (i = 0; i < 2; i++) {
    float corr = srslte_vec_dot_prod_fff(ack_table[i], bits, SRSLTE_PHICH_NBITS);
    INFO("Corr%d=%f\n", i, corr);
    if (corr > max_corr) {
      max_corr = corr; 
      if (distance) {
        *distance = max_corr;
      }
      index = i; 
    }
  }  
  return index;
}

/** Encodes the ACK
 *  36.212
 */
void srslte_phich_ack_encode(uint8_t ack, uint8_t bits[SRSLTE_PHICH_NBITS]) {
  memset(bits, ack, 3 * sizeof(uint8_t));
}

/* Decodes the phich channel and saves the CFI in the cfi pointer.
 *
 * Returns 1 if successfully decoded the CFI, 0 if not and -1 on error
 */
int srslte_phich_decode(srslte_phich_t *q, cf_t *slot_symbols, cf_t *ce[SRSLTE_MAX_PORTS], float noise_estimate,
    uint32_t ngroup, uint32_t nseq, uint32_t subframe, uint8_t *ack, float *distance) {

  /* Set pointers for layermapping & precoding */
  int i, j;
  cf_t *x[SRSLTE_MAX_LAYERS];
  cf_t *ce_precoding[SRSLTE_MAX_PORTS];
  
  if (q == NULL || slot_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (subframe >= SRSLTE_NSUBFRAMES_X_FRAME) {
    fprintf(stderr, "Invalid nslot %d\n", subframe);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (SRSLTE_CP_ISEXT(q->cell.cp)) {
    if (nseq >= SRSLTE_PHICH_EXT_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }
  } else {
    if (nseq >= SRSLTE_PHICH_NORM_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }
  }
  if (ngroup >= srslte_regs_phich_ngroups(q->regs)) {
    fprintf(stderr, "Invalid ngroup %d\n", ngroup);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  DEBUG("Decoding PHICH Ngroup: %d, Nseq: %d\n", ngroup, nseq);

  /* number of layers equals number of ports */
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    x[i] = q->x[i];
  }
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    ce_precoding[i] = q->ce[i];
  }

  /* extract symbols */
  if (SRSLTE_PHICH_MAX_NSYMB
      != srslte_regs_phich_get(q->regs, slot_symbols, q->symbols[0], ngroup)) {
    fprintf(stderr, "There was an error getting the phich symbols\n");
    return SRSLTE_ERROR;
  }

  /* extract channel estimates */
  for (i = 0; i < q->cell.nof_ports; i++) {
    if (SRSLTE_PHICH_MAX_NSYMB != srslte_regs_phich_get(q->regs, ce[i], q->ce[i], ngroup)) {
      fprintf(stderr, "There was an error getting the phich symbols\n");
      return SRSLTE_ERROR;
    }
  }

  /* in control channels, only diversity is supported */
  if (q->cell.nof_ports == 1) {
    /* no need for layer demapping */
    srslte_predecoding_single(q->symbols[0], q->ce[0], q->d0, SRSLTE_PHICH_MAX_NSYMB, noise_estimate);
  } else {
    srslte_predecoding_diversity(q->symbols[0], ce_precoding, x,
        q->cell.nof_ports, SRSLTE_PHICH_MAX_NSYMB);
    srslte_layerdemap_diversity(x, q->d0, q->cell.nof_ports,
    SRSLTE_PHICH_MAX_NSYMB / q->cell.nof_ports);
  }
  DEBUG("Recv!!: \n", 0);
  DEBUG("d0: ", 0);
  if (SRSLTE_VERBOSE_ISDEBUG())
    srslte_vec_fprint_c(stdout, q->d0, SRSLTE_PHICH_MAX_NSYMB);

  if (SRSLTE_CP_ISEXT(q->cell.cp)) {
    if (ngroup % 2) {
      for (i = 0; i < SRSLTE_PHICH_EXT_MSYMB / 2; i++) {
        q->d[2 * i + 0] = q->d0[4 * i + 2];
        q->d[2 * i + 1] = q->d0[4 * i + 3];
      }
    } else {
      for (i = 0; i < SRSLTE_PHICH_EXT_MSYMB / 2; i++) {
        q->d[2 * i + 0] = q->d0[4 * i];
        q->d[2 * i + 1] = q->d0[4 * i + 1];
      }
    }
  } else {
    memcpy(q->d, q->d0, SRSLTE_PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  DEBUG("d: ", 0);
  if (SRSLTE_VERBOSE_ISDEBUG())
    srslte_vec_fprint_c(stdout, q->d, SRSLTE_PHICH_EXT_MSYMB);

  srslte_scrambling_c(&q->seq[subframe], q->d);

  /* De-spreading */
  if (SRSLTE_CP_ISEXT(q->cell.cp)) {
    for (i = 0; i < SRSLTE_PHICH_NBITS; i++) {
      q->z[i] = 0;
      for (j = 0; j < SRSLTE_PHICH_EXT_NSF; j++) {
        q->z[i] += conjf(w_ext[nseq][j])
            * q->d[i * SRSLTE_PHICH_EXT_NSF + j] / SRSLTE_PHICH_EXT_NSF;
      }
    }
  } else {
    for (i = 0; i < SRSLTE_PHICH_NBITS; i++) {
      q->z[i] = 0;
      for (j = 0; j < SRSLTE_PHICH_NORM_NSF; j++) {
        q->z[i] += conjf(w_normal[nseq][j])
            * q->d[i * SRSLTE_PHICH_NORM_NSF + j] / SRSLTE_PHICH_NORM_NSF;
      }
    }
  }

  DEBUG("z: ", 0);
  if (SRSLTE_VERBOSE_ISDEBUG())
    srslte_vec_fprint_c(stdout, q->z, SRSLTE_PHICH_NBITS);

  srslte_demod_soft_demodulate(SRSLTE_MOD_BPSK, q->z, q->data_rx, SRSLTE_PHICH_NBITS);

  if (ack) {
    *ack = srslte_phich_ack_decode(q->data_rx, distance);    
  }

  return SRSLTE_SUCCESS;
}

/** Encodes ACK/NACK bits, modulates and inserts into resource.
 * The parameter ack is an array of srslte_phich_ngroups() pointers to buffers of nof_sequences uint8_ts
 */
int srslte_phich_encode(srslte_phich_t *q, uint8_t ack, uint32_t ngroup, uint32_t nseq, uint32_t subframe,
    cf_t *slot_symbols[SRSLTE_MAX_PORTS]) {
  int i;

  if (q == NULL || slot_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (subframe >= SRSLTE_NSUBFRAMES_X_FRAME) {
    fprintf(stderr, "Invalid nslot %d\n", subframe);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (SRSLTE_CP_ISEXT(q->cell.cp)) {
    if (nseq >= SRSLTE_PHICH_EXT_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }
  } else {
    if (nseq >= SRSLTE_PHICH_NORM_NSEQUENCES) {
      fprintf(stderr, "Invalid nseq %d\n", nseq);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }
  }
  if (ngroup >= srslte_regs_phich_ngroups(q->regs)) {
    fprintf(stderr, "Invalid ngroup %d\n", ngroup);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }


  /* Set pointers for layermapping & precoding */
  cf_t *x[SRSLTE_MAX_LAYERS];
  cf_t *symbols_precoding[SRSLTE_MAX_PORTS];

  /* number of layers equals number of ports */
  for (i = 0; i < q->cell.nof_ports; i++) {
    x[i] = q->x[i];
  }
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    symbols_precoding[i] = q->symbols[i];
  }

  /* encode ACK/NACK bit */
  srslte_phich_ack_encode(ack, q->data);

  srslte_mod_modulate(&q->mod, q->data, q->z, SRSLTE_PHICH_NBITS);

  DEBUG("data: ", 0);
  if (SRSLTE_VERBOSE_ISDEBUG())
    srslte_vec_fprint_c(stdout, q->z, SRSLTE_PHICH_NBITS);

  /* Spread with w */
  if (SRSLTE_CP_ISEXT(q->cell.cp)) {
    for (i = 0; i < SRSLTE_PHICH_EXT_MSYMB; i++) {
      q->d[i] = w_ext[nseq][i % SRSLTE_PHICH_EXT_NSF]
          * q->z[i / SRSLTE_PHICH_EXT_NSF];
    }
  } else {
    for (i = 0; i < SRSLTE_PHICH_NORM_MSYMB; i++) {
      q->d[i] = w_normal[nseq][i % SRSLTE_PHICH_NORM_NSF]
          * q->z[i / SRSLTE_PHICH_NORM_NSF];
    }
  }

  DEBUG("d: ", 0);
  if (SRSLTE_VERBOSE_ISDEBUG())
    srslte_vec_fprint_c(stdout, q->d, SRSLTE_PHICH_EXT_MSYMB);

  srslte_scrambling_c(&q->seq[subframe], q->d);

  /* align to REG */
  if (SRSLTE_CP_ISEXT(q->cell.cp)) {
    if (ngroup % 2) {
      for (i = 0; i < SRSLTE_PHICH_EXT_MSYMB / 2; i++) {
        q->d0[4 * i + 0] = 0;
        q->d0[4 * i + 1] = 0;
        q->d0[4 * i + 2] = q->d[2 * i];
        q->d0[4 * i + 3] = q->d[2 * i + 1];
      }
    } else {
      for (i = 0; i < SRSLTE_PHICH_EXT_MSYMB / 2; i++) {
        q->d0[4 * i + 0] = q->d[2 * i];
        q->d0[4 * i + 1] = q->d[2 * i + 1];
        q->d0[4 * i + 2] = 0;
        q->d0[4 * i + 3] = 0;
      }
    }
  } else {
    memcpy(q->d0, q->d, SRSLTE_PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  DEBUG("d0: ", 0);
  if (SRSLTE_VERBOSE_ISDEBUG())
    srslte_vec_fprint_c(stdout, q->d0, SRSLTE_PHICH_MAX_NSYMB);

  /* layer mapping & precoding */
  if (q->cell.nof_ports > 1) {
    srslte_layermap_diversity(q->d0, x, q->cell.nof_ports, SRSLTE_PHICH_MAX_NSYMB);
    srslte_precoding_diversity(x, symbols_precoding, q->cell.nof_ports,
    SRSLTE_PHICH_MAX_NSYMB / q->cell.nof_ports);
    /**FIXME: According to 6.9.2, Precoding for 4 tx ports is different! */
  } else {
    memcpy(q->symbols[0], q->d0, SRSLTE_PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  /* mapping to resource elements */
  for (i = 0; i < q->cell.nof_ports; i++) {
    if (srslte_regs_phich_add(q->regs, q->symbols[i], ngroup, slot_symbols[i])
        < 0) {
      fprintf(stderr, "Error putting PCHICH resource elements\n");
      return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

