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

#include "srslte/phch/pucch.h"
#include "srslte/phch/uci.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/bit.h"
#include "srslte/utils/debug.h"
#include "srslte/utils/vector.h"
#include "srslte/dft/dft_precoding.h"

#define MAX_PUSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)

bool srslte_pucch_cfg_isvalid(srslte_pucch_cfg_t *cfg) {
  return true;
}


/* Generates n_cs_cell according to Sec 5.4 of 36.211 */
int srslte_generate_n_cs_cell(srslte_cell_t cell, uint32_t n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_SRSLTE_SRSLTE_CP_NORM_NSYMB]) 
{
  srslte_sequence_t seq; 
  bzero(&seq, sizeof(srslte_sequence_t));

  srslte_sequence_LTE_pr(&seq, 8*SRSLTE_CP_NSYMB(cell.cp)*SRSLTE_NSLOTS_X_FRAME, cell.id);

  for (uint32_t ns=0;ns<SRSLTE_NSLOTS_X_FRAME;ns++) {
    for (uint32_t l=0;l<SRSLTE_CP_NSYMB(cell.cp);l++) {
      n_cs_cell[ns][l] = 0; 
      for (uint32_t i=0;i<8;i++) {
        n_cs_cell[ns][l] += seq.c[8*SRSLTE_CP_NSYMB(cell.cp)*ns+8*l+i]<<i;
      }
    }
  }
  srslte_sequence_free(&seq);
  return SRSLTE_SUCCESS;
}


/* Calculates alpha according to 5.5.2.2.2 of 36.211 */
float srslte_pucch_get_alpha(uint32_t n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_SRSLTE_SRSLTE_CP_NORM_NSYMB], 
                      srslte_pucch_cfg_t *cfg, 
                      srslte_cp_t cp, bool is_drms,
                      uint32_t ns, uint32_t l, 
                      uint32_t *n_oc_ptr) 
{
  uint32_t c = SRSLTE_CP_ISNORM(cp)?3:2;
  uint32_t N_prime = (cfg->n_pucch < c*cfg->N_cs/cfg->delta_pucch_shift)?cfg->N_cs:12;

  uint32_t n_prime = cfg->n_pucch;
  if (cfg->n_pucch >= c*cfg->N_cs/cfg->delta_pucch_shift) {
    n_prime = (cfg->n_pucch-c*cfg->N_cs/cfg->delta_pucch_shift)%(cfg->N_cs/cfg->delta_pucch_shift);
  }
  
  uint32_t n_oc_div = (!is_drms && SRSLTE_CP_ISEXT(cp))?2:1;

  uint32_t n_oc = n_prime*cfg->delta_pucch_shift/N_prime;
  if (!is_drms && SRSLTE_CP_ISEXT(cp)) {
    n_oc *= 2; 
  }
  if (n_oc_ptr) {
    *n_oc_ptr = n_oc; 
  }
  uint32_t n_cs = 0; 
  if (SRSLTE_CP_ISNORM(cp)) {
    n_cs = (n_cs_cell[ns][l]+(n_prime*cfg->delta_pucch_shift+(n_oc%cfg->delta_pucch_shift))%N_prime)%12;
  } else {
    n_cs = (n_cs_cell[ns][l]+(n_prime*cfg->delta_pucch_shift+n_oc/n_oc_div)%N_prime)%12;    
  }
  
  return 2 * M_PI * (n_cs) / 12;
}

int pucch_cp(srslte_pucch_t *q, srslte_harq_t *harq, cf_t *input, cf_t *output, bool advance_input) 
{
  return SRSLTE_ERROR;
}

int pucch_put(srslte_pucch_t *q, srslte_harq_t *harq, cf_t *input, cf_t *output) {
  return pucch_cp(q, harq, input, output, true);
}

int pucch_get(srslte_pucch_t *q, srslte_harq_t *harq, cf_t *input, cf_t *output) {
  return pucch_cp(q, harq, input, output, false);
}


/** Initializes the PDCCH transmitter and receiver */
int srslte_pucch_init(srslte_pucch_t *q, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL && srslte_cell_isvalid(&cell)) {
    ret = SRSLTE_ERROR;
    bzero(q, sizeof(srslte_pucch_t));
    
    q->cell = cell; 
    
    if (srslte_generate_n_cs_cell(q->cell, q->n_cs_cell)) {
      return SRSLTE_ERROR;
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

void srslte_pucch_free(srslte_pucch_t *q) {
 bzero(q, sizeof(srslte_pucch_t));
}

/** Decodes the PUSCH from the received symbols
 */
int pucch_decode(srslte_pucch_t *q, srslte_harq_t *harq, cf_t *sf_symbols, cf_t *ce, float noise_estimate, uint8_t *data) 
{
  return SRSLTE_ERROR_INVALID_INPUTS;
}

static cf_t uci_encode_format1() {
  return 1.0;
}

static cf_t uci_encode_format1a(uint8_t bit) {
  return bit?1.0:-1.0;
}

static cf_t uci_encode_format1b(uint8_t bits[2]) {
  if (bits[0] == 0) {
    if (bits[1] == 0) {
      return 1;  
    } else {
      return -I; 
    }
  } else {
    if (bits[1] == 0) {
      return I;  
    } else {
      return -1.0; 
    }    
  }
}

static void uci_mod_bits(srslte_pucch_t *q, srslte_pucch_cfg_t *cfg, uint8_t bits[SRSLTE_PUCCH_MAX_BITS])
{  
  cf_t d_0 = 0;
  uint8_t tmp[2];
  switch(cfg->format) {
    case SRSLTE_PUCCH_FORMAT_1:
      d_0 = uci_encode_format1();
      break;
    case SRSLTE_PUCCH_FORMAT_1A:
      d_0 = uci_encode_format1a(bits[0]);
      break;
    case SRSLTE_PUCCH_FORMAT_1B:
      tmp[0] = bits[0];
      tmp[1] = bits[1];
      d_0 = uci_encode_format1b(tmp);
    default:
      fprintf(stderr, "PUCCH format 2 not supported\n");
      return;
  }
  /*
  for (uint32_t n=0;n<SRSLTE_PUCCH_N_SEQ;n++) {
    q->y[n] = d_0+
  }
  */
}

int srslte_pucch_encode(srslte_pucch_t *q, srslte_pucch_cfg_t *cfg, uint8_t bits[SRSLTE_PUCCH_MAX_BITS], cf_t *sf_symbols) 
{
  uci_mod_bits(q, cfg, bits);
  return SRSLTE_ERROR;     
}

  