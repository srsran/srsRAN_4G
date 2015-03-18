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


#include <stdlib.h>
#include <strings.h>

#include "srslte/utils/vector.h"
#include "srslte/utils/bit.h"
#include "srslte/modem/demod_soft.h"
#include "soft_algs.h"


int demod_soft_init(demod_soft_t *q, uint32_t max_symbols) {
  int ret = SRSLTE_ERROR; 
  
  bzero((void*)q,sizeof(demod_soft_t));
  q->sigma = 1.0; 
  q->zones = vec_malloc(sizeof(uint32_t) * max_symbols);
  if (!q->zones) {
    perror("malloc");
    goto clean_exit;
  }
  q->dd = vec_malloc(sizeof(float*) * max_symbols * 7);
  if (!q->dd) {
    perror("malloc");
    goto clean_exit;
  }
  q->max_symbols = max_symbols;
  
  ret = SRSLTE_SUCCESS;
  
clean_exit:
  if (ret != SRSLTE_SUCCESS) {
    demod_soft_free(q);
  }
  return ret; 
}

void demod_soft_free(demod_soft_t *q) {
  if (q->zones) {
    free(q->zones);
  }
  if (q->dd) {
    free(q->dd);
  }
  bzero((void*)q,sizeof(demod_soft_t));
}

void demod_soft_table_set(demod_soft_t *q, modem_table_t *table) {
  q->table = table;
}

void demod_soft_alg_set(demod_soft_t *q, enum alg alg_type) {
  q->alg_type = alg_type;
}

void demod_soft_sigma_set(demod_soft_t *q, float sigma) {
  q->sigma = 2*sigma;
}

int demod_soft_demodulate(demod_soft_t *q, const cf_t* symbols, float* llr, int nsymbols) {
  switch(q->alg_type) {
  case EXACT:
    llr_exact(symbols, llr, nsymbols, q->table->nsymbols, q->table->nbits_x_symbol,
        q->table->symbol_table, q->table->soft_table.idx, q->sigma);
    break;
  case APPROX:
    if (nsymbols <= q->max_symbols) {
      llr_approx(symbols, llr, nsymbols, q->table->nsymbols, 
                q->table->nbits_x_symbol,
                q->table->symbol_table, q->table->soft_table.idx, 
                q->table->soft_table.d_idx, q->table->soft_table.min_idx, q->sigma, 
                q->zones, q->dd);
      
    } else {
      fprintf(stderr, "Too many symbols (%d>%d)\n", nsymbols, q->max_symbols);
      return -1; 
    }
    break;
  }
  return nsymbols*q->table->nbits_x_symbol;
}



/* High-Level API */
int demod_soft_initialize(demod_soft_hl* hl) {
  modem_table_init(&hl->table);
  if (modem_table_lte(&hl->table,hl->init.std,true)) {
    return -1;
  }
  demod_soft_init(&hl->obj, 10000);
  hl->obj.table = &hl->table;

  return 0;
}

int demod_soft_work(demod_soft_hl* hl) {
  hl->obj.sigma = hl->ctrl_in.sigma;
  hl->obj.alg_type = hl->ctrl_in.alg_type;
  int ret = demod_soft_demodulate(&hl->obj,hl->input,hl->output,hl->in_len);
  hl->out_len = ret;
  return 0;
}

int demod_soft_stop(demod_soft_hl* hl) {
  modem_table_free(&hl->table);
  return 0;
}
