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

#include "srslte/phch/regs.h"
#include "srslte/phch/pcfich.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/bit.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

// Table 5.3.4-1
static uint8_t cfi_table[4][PCFICH_CFI_LEN] = { 
  { 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1 }, 
  { 1, 0, 1,
    1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,
    0, 1, 1, 0 }, 
  { 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1,
    0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1 }, 
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } // reserved
};

bool pcfich_exists(int nframe, int nslot) {
  return true;
}

/** Initializes the pcfich channel receiver. 
 * On error, returns -1 and frees the structrure 
 */
int pcfich_init(pcfich_t *q, regs_t *regs, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q                         != NULL &&
      regs                      != NULL &&
      lte_cell_isvalid(&cell)) 
  {   
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(pcfich_t));
    q->cell = cell;
    q->regs = regs;
    q->nof_symbols = PCFICH_RE;
    
    if (precoding_init(&q->precoding, SF_LEN_RE(cell.nof_prb, cell.cp))) {
      fprintf(stderr, "Error initializing precoding\n");
    }

    if (modem_table_lte(&q->mod, LTE_QPSK, true)) {
      goto clean;
    }

    demod_soft_init(&q->demod, q->nof_symbols);
    demod_soft_table_set(&q->demod, &q->mod);
    demod_soft_alg_set(&q->demod, APPROX);

    for (int nsf = 0; nsf < SRSLTE_NSUBFRAMES_X_FRAME; nsf++) {
      if (sequence_pcfich(&q->seq_pcfich[nsf], 2 * nsf, q->cell.id)) {
        goto clean;
      }
    }
    
    /* convert cfi bit tables to floats for demodulation */
    for (int i=0;i<3;i++) {
      for (int j=0;j<PCFICH_CFI_LEN;j++) {
        q->cfi_table_float[i][j] = (float) 2.0*cfi_table[i][j]-1.0; 
      }
    }

    ret = SRSLTE_SUCCESS;
  }
  
  clean: 
  if (ret == SRSLTE_ERROR) {
    pcfich_free(q);
  }
  return ret;
}

void pcfich_free(pcfich_t *q) {
  for (int ns = 0; ns < SRSLTE_NSUBFRAMES_X_FRAME; ns++) {
    sequence_free(&q->seq_pcfich[ns]);
  }
  modem_table_free(&q->mod);
  precoding_free(&q->precoding); 
  demod_soft_free(&q->demod);

  bzero(q, sizeof(pcfich_t));
}

/** Finds the CFI with minimum distance with the vector of received 32 bits.
 * Saves the CFI value in the cfi pointer and returns the distance.
 */
float pcfich_cfi_decode(pcfich_t *q, uint32_t *cfi) {
  int i;
  int index = 0;
  float max_corr = 0;
  
  for (i = 0; i < 3; i++) {
    float corr = fabsf(vec_dot_prod_fff(q->cfi_table_float[i], q->data_f, PCFICH_CFI_LEN));
    if (corr > max_corr) {
      max_corr = corr; 
      index = i; 
    }
  }
  if (cfi) {
    *cfi = index + 1;
  }
  return max_corr;
}

/** Encodes the CFI producing a vector of 32 bits.
 *  36.211 10.3 section 5.3.4
 */
int pcfich_cfi_encode(int cfi, uint8_t bits[PCFICH_CFI_LEN]) {
  if (cfi < 1 || cfi > 3) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  } else{
    memcpy(bits, cfi_table[cfi - 1], PCFICH_CFI_LEN * sizeof(uint8_t));
    return SRSLTE_SUCCESS;    
  }
}

/* Decodes the PCFICH channel and saves the CFI in the cfi pointer.
 *
 * Returns 1 if successfully decoded the CFI, 0 if not and -1 on error
 */
int pcfich_decode(pcfich_t *q, cf_t *slot_symbols, cf_t *ce[SRSLTE_MAX_PORTS], float noise_estimate,
    uint32_t nsubframe, uint32_t *cfi, float *corr_result) 
{

  /* Set pointers for layermapping & precoding */
  int i;
  cf_t *x[MAX_LAYERS];
  cf_t *ce_precoding[SRSLTE_MAX_PORTS];

  if (q                 != NULL                 && 
      slot_symbols      != NULL                 && 
      nsubframe         <  SRSLTE_NSUBFRAMES_X_FRAME) 
  {

    /* number of layers equals number of ports */
    for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
      x[i] = q->pcfich_x[i];
    }
    for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
      ce_precoding[i] = q->ce[i];
    }

    /* extract symbols */
    if (q->nof_symbols
        != regs_pcfich_get(q->regs, slot_symbols, q->pcfich_symbols[0])) {
      fprintf(stderr, "There was an error getting the PCFICH symbols\n");
      return SRSLTE_ERROR;
    }

    /* extract channel estimates */
    for (i = 0; i < q->cell.nof_ports; i++) {
      if (q->nof_symbols != regs_pcfich_get(q->regs, ce[i], q->ce[i])) {
        fprintf(stderr, "There was an error getting the PCFICH symbols\n");
        return SRSLTE_ERROR;
      }
    }

    /* in control channels, only diversity is supported */
    if (q->cell.nof_ports == 1) {
      /* no need for layer demapping */
      predecoding_single(&q->precoding, q->pcfich_symbols[0], q->ce[0], q->pcfich_d,
          q->nof_symbols, noise_estimate);
    } else {
      predecoding_diversity(&q->precoding, q->pcfich_symbols[0], ce_precoding, x,
          q->cell.nof_ports, q->nof_symbols, noise_estimate);
      layerdemap_diversity(x, q->pcfich_d, q->cell.nof_ports,
          q->nof_symbols / q->cell.nof_ports);
    }

    /* demodulate symbols */
    demod_soft_sigma_set(&q->demod, 1.0);
    demod_soft_demodulate(&q->demod, q->pcfich_d, q->data_f, q->nof_symbols);

    /* Scramble with the sequence for slot nslot */
    scrambling_f(&q->seq_pcfich[nsubframe], q->data_f);

    /* decode CFI */
    float corr = pcfich_cfi_decode(q, cfi);
    if (corr_result) {
      *corr_result = corr;
    }
    return 1;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
  
}

/** Encodes CFI and maps symbols to the slot
 */
int pcfich_encode(pcfich_t *q, uint32_t cfi, cf_t *slot_symbols[SRSLTE_MAX_PORTS],
    uint32_t subframe) {
  int i;

  if (q                 != NULL                 && 
      cfi               <  3                    &&
      slot_symbols      != NULL                 && 
      subframe         <  SRSLTE_NSUBFRAMES_X_FRAME) 
  {

    /* Set pointers for layermapping & precoding */
    cf_t *x[MAX_LAYERS];
    cf_t *symbols_precoding[SRSLTE_MAX_PORTS];

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->pcfich_x[i];
    }
    for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
      symbols_precoding[i] = q->pcfich_symbols[i];
    }

    /* pack CFI */
    pcfich_cfi_encode(cfi, q->data);

    /* scramble for slot sequence nslot */
    scrambling_b(&q->seq_pcfich[subframe], q->data);

    mod_modulate(&q->mod, q->data, q->pcfich_d, PCFICH_CFI_LEN);

    /* layer mapping & precoding */
    if (q->cell.nof_ports > 1) {
      layermap_diversity(q->pcfich_d, x, q->cell.nof_ports, q->nof_symbols);
      precoding_diversity(&q->precoding, x, symbols_precoding, q->cell.nof_ports,
          q->nof_symbols / q->cell.nof_ports);
    } else {
      memcpy(q->pcfich_symbols[0], q->pcfich_d, q->nof_symbols * sizeof(cf_t));
    }

    /* mapping to resource elements */
    for (i = 0; i < q->cell.nof_ports; i++) {
      if (regs_pcfich_put(q->regs, q->pcfich_symbols[i], slot_symbols[i]) < 0) {
        fprintf(stderr, "Error putting PCHICH resource elements\n");
        return SRSLTE_ERROR;
      }
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}


