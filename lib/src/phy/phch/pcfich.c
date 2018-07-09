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

#include "srslte/phy/phch/regs.h"
#include "srslte/phy/phch/pcfich.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/debug.h"

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

bool srslte_pcfich_exists(int nframe, int nslot) {
  return true;
}

/** Initializes the pcfich channel receiver. 
 * On error, returns -1 and frees the structrure 
 */
int srslte_pcfich_init(srslte_pcfich_t *q, uint32_t nof_rx_antennas) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q != NULL)
  {   
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(srslte_pcfich_t));
    q->nof_rx_antennas = nof_rx_antennas;
    q->nof_symbols = PCFICH_RE;

    if (srslte_modem_table_lte(&q->mod, SRSLTE_MOD_QPSK)) {
      goto clean;
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
    srslte_pcfich_free(q);
  }
  return ret;
}

void srslte_pcfich_free(srslte_pcfich_t *q) {
  for (int ns = 0; ns < SRSLTE_NSUBFRAMES_X_FRAME; ns++) {
    srslte_sequence_free(&q->seq[ns]);
  }
  srslte_modem_table_free(&q->mod);

  bzero(q, sizeof(srslte_pcfich_t));
}

int srslte_pcfich_set_cell(srslte_pcfich_t *q, srslte_regs_t *regs, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q                         != NULL &&
      regs                      != NULL &&
      srslte_cell_isvalid(&cell))
  {
    q->regs = regs;
    if (cell.id != q->cell.id || q->cell.nof_prb == 0) {
      memcpy(&q->cell, &cell, sizeof(srslte_cell_t));
      for (int nsf = 0; nsf < SRSLTE_NSUBFRAMES_X_FRAME; nsf++) {
        if (srslte_sequence_pcfich(&q->seq[nsf], 2 * nsf, q->cell.id)) {
          return SRSLTE_ERROR;
        }
      }
    }
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}


/** Finds the CFI with minimum distance with the vector of received 32 bits.
 * Saves the CFI value in the cfi pointer and returns the distance.
 */
float srslte_pcfich_cfi_decode(srslte_pcfich_t *q, uint32_t *cfi) {
  int i;
  int index = 0;
  float max_corr = 0;
  float corr[3];
  
  for (i = 0; i < 3; i++) {
    corr[i] = srslte_vec_dot_prod_fff(q->cfi_table_float[i], q->data_f, PCFICH_CFI_LEN);
    if (corr[i] > max_corr) {
      max_corr = corr[i]; 
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
int srslte_pcfich_cfi_encode(uint32_t cfi, uint8_t bits[PCFICH_CFI_LEN]) {
  if (cfi < 1 || cfi > 3) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  } else{
    memcpy(bits, cfi_table[cfi - 1], PCFICH_CFI_LEN * sizeof(uint8_t));
    return SRSLTE_SUCCESS;    
  }
}

int srslte_pcfich_decode(srslte_pcfich_t *q, cf_t *sf_symbols, cf_t *ce[SRSLTE_MAX_PORTS], float noise_estimate,
    uint32_t nsubframe, uint32_t *cfi, float *corr_result) 
{
  cf_t *_sf_symbols[SRSLTE_MAX_PORTS]; 
  cf_t *_ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  
  _sf_symbols[0] = sf_symbols; 
  for (int i=0;i<q->cell.nof_ports;i++) {
    _ce[i][0] = ce[i]; 
  }
  return srslte_pcfich_decode_multi(q, _sf_symbols, _ce, noise_estimate, nsubframe, cfi, corr_result);
}

/* Decodes the PCFICH channel and saves the CFI in the cfi pointer.
 *
 * Returns 1 if successfully decoded the CFI, 0 if not and -1 on error
 */
int srslte_pcfich_decode_multi(srslte_pcfich_t *q, cf_t *sf_symbols[SRSLTE_MAX_PORTS], cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], float noise_estimate,
                               uint32_t nsubframe, uint32_t *cfi, float *corr_result) 
{

  /* Set pointers for layermapping & precoding */
  int i;
  cf_t *x[SRSLTE_MAX_LAYERS];
  
  if (q                 != NULL                 && 
      sf_symbols      != NULL                 && 
      nsubframe         <  SRSLTE_NSUBFRAMES_X_FRAME) 
  {

    /* number of layers equals number of ports */
    for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
      x[i] = q->x[i];
    }
    
    cf_t *q_symbols[SRSLTE_MAX_PORTS];
    cf_t *q_ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
    
    /* extract symbols */
    for (int j=0;j<q->nof_rx_antennas;j++) {
      if (q->nof_symbols
          != srslte_regs_pcfich_get(q->regs, sf_symbols[j], q->symbols[j])) {
        fprintf(stderr, "There was an error getting the PCFICH symbols\n");
        return SRSLTE_ERROR;
      }

      q_symbols[j] = q->symbols[j];

      /* extract channel estimates */
      for (i = 0; i < q->cell.nof_ports; i++) {
        if (q->nof_symbols != srslte_regs_pcfich_get(q->regs, ce[i][j], q->ce[i][j])) {
          fprintf(stderr, "There was an error getting the PCFICH symbols\n");
          return SRSLTE_ERROR;
        }
        q_ce[i][j] = q->ce[i][j];
      }
    }
    
    /* in control channels, only diversity is supported */
    if (q->cell.nof_ports == 1) {
      /* no need for layer demapping */
      srslte_predecoding_single_multi(q_symbols, q_ce[0], q->d, NULL, q->nof_rx_antennas, q->nof_symbols, 1.0f, noise_estimate);
    } else {
      srslte_predecoding_diversity_multi(q_symbols, q_ce, x, NULL, q->nof_rx_antennas, q->cell.nof_ports, q->nof_symbols, 1.0f);
      srslte_layerdemap_diversity(x, q->d, q->cell.nof_ports, q->nof_symbols / q->cell.nof_ports);
    }

    /* demodulate symbols */
    srslte_demod_soft_demodulate(SRSLTE_MOD_QPSK, q->d, q->data_f, q->nof_symbols);

    /* Scramble with the sequence for slot nslot */
    srslte_scrambling_f(&q->seq[nsubframe], q->data_f);

    /* decode CFI */
    float corr = srslte_pcfich_cfi_decode(q, cfi);
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
int srslte_pcfich_encode(srslte_pcfich_t *q, uint32_t cfi, cf_t *slot_symbols[SRSLTE_MAX_PORTS],
    uint32_t subframe) {
  int i;

  if (q                 != NULL                 && 
      cfi               <=  3                    &&
      slot_symbols      != NULL                 && 
      subframe         <  SRSLTE_NSUBFRAMES_X_FRAME) 
  {

    /* Set pointers for layermapping & precoding */
    cf_t *x[SRSLTE_MAX_LAYERS];
    cf_t *q_symbols[SRSLTE_MAX_PORTS];

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
      q_symbols[i] = q->symbols[i];
    }

    /* pack CFI */
    srslte_pcfich_cfi_encode(cfi, q->data);

    /* scramble for slot sequence nslot */
    srslte_scrambling_b(&q->seq[subframe], q->data);

    srslte_mod_modulate(&q->mod, q->data, q->d, PCFICH_CFI_LEN);

    /* layer mapping & precoding */
    if (q->cell.nof_ports > 1) {
      srslte_layermap_diversity(q->d, x, q->cell.nof_ports, q->nof_symbols);
      srslte_precoding_diversity(x, q_symbols, q->cell.nof_ports, q->nof_symbols / q->cell.nof_ports, 1.0f);
    } else {
      memcpy(q->symbols[0], q->d, q->nof_symbols * sizeof(cf_t));
    }

    /* mapping to resource elements */
    for (i = 0; i < q->cell.nof_ports; i++) {
      if (srslte_regs_pcfich_put(q->regs, q->symbols[i], slot_symbols[i]) < 0) {
        fprintf(stderr, "Error putting PCHICH resource elements\n");
        return SRSLTE_ERROR;
      }
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}


