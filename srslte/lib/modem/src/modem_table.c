/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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


#include <stdbool.h>
#include <complex.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/common/phy_common.h"
#include "srslte/modem/modem_table.h"
#include "lte_tables.h"

/** Internal functions */
static int table_create(srslte_modem_table_t* q) {
  q->symbol_table = malloc(q->nsymbols*sizeof(cf_t));
  return q->symbol_table==NULL;
}

void srslte_modem_table_init(srslte_modem_table_t* q) {
  bzero((void*)q,sizeof(srslte_modem_table_t));
}
void srslte_modem_table_free(srslte_modem_table_t* q) {
  if (q->symbol_table) {
    free(q->symbol_table);
  }
  bzero(q, sizeof(srslte_modem_table_t));
}
void srslte_modem_table_reset(srslte_modem_table_t* q) {
  srslte_modem_table_free(q);
  srslte_modem_table_init(q);
}

int srslte_modem_table_set(srslte_modem_table_t* q, cf_t* table, srslte_soft_table_t *soft_table, uint32_t nsymbols, uint32_t nbits_x_symbol) {
  if (q->nsymbols) {
    return SRSLTE_ERROR;
  }
  q->nsymbols = nsymbols;
  if (table_create(q)) {
    return SRSLTE_ERROR;
  }
  memcpy(q->symbol_table,table,q->nsymbols*sizeof(cf_t));
  memcpy(&q->soft_table,soft_table,sizeof(srslte_soft_table_t));
  q->nbits_x_symbol = nbits_x_symbol;
  return SRSLTE_SUCCESS;
}

int srslte_modem_table_lte(srslte_modem_table_t* q, srslte_mod_t modulation, bool compute_soft_demod) {
  switch(modulation) {
  case SRSLTE_MOD_BPSK:
    q->nbits_x_symbol = 1;
    q->nsymbols = 2;
    if (table_create(q)) {
      return SRSLTE_ERROR;
    }
    set_BPSKtable(q->symbol_table, &q->soft_table, compute_soft_demod);
    break;
  case SRSLTE_MOD_QPSK:
    q->nbits_x_symbol = 2;
    q->nsymbols = 4;
    if (table_create(q)) {
      return SRSLTE_ERROR;
    }
    set_QPSKtable(q->symbol_table, &q->soft_table, compute_soft_demod);
    break;
  case SRSLTE_MOD_16QAM:
    q->nbits_x_symbol = 4;
    q->nsymbols = 16;
    if (table_create(q)) {
      return SRSLTE_ERROR;
    }
    set_16QAMtable(q->symbol_table, &q->soft_table, compute_soft_demod);
    break;
  case SRSLTE_MOD_64QAM:
    q->nbits_x_symbol = 6;
    q->nsymbols = 64;
    if (table_create(q)) {
      return SRSLTE_ERROR;
    }
    set_64QAMtable(q->symbol_table, &q->soft_table, compute_soft_demod);
    break;
  }
  return SRSLTE_SUCCESS;
}
