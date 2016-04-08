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


#include <stdbool.h>
#include <complex.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/common/phy_common.h"
#include "srslte/modem/modem_table.h"
#include "srslte/utils/vector.h"
#include "lte_tables.h"

/** Internal functions */
static int table_create(srslte_modem_table_t* q) {
  q->symbol_table = srslte_vec_malloc(q->nsymbols*sizeof(cf_t));
  return q->symbol_table==NULL;
}

void srslte_modem_table_init(srslte_modem_table_t* q) {
  bzero((void*)q,sizeof(srslte_modem_table_t));
}
void srslte_modem_table_free(srslte_modem_table_t* q) {
  if (q->symbol_table) {
    free(q->symbol_table);
  }
  if (q->symbol_table_bpsk) {
    free(q->symbol_table_bpsk);
  }
  if (q->symbol_table_qpsk) {
    free(q->symbol_table_qpsk);
  }
  if (q->symbol_table_16qam) {
    free(q->symbol_table_16qam);
  }
  bzero(q, sizeof(srslte_modem_table_t));
}
void srslte_modem_table_reset(srslte_modem_table_t* q) {
  srslte_modem_table_free(q);
  srslte_modem_table_init(q);
}

int srslte_modem_table_set(srslte_modem_table_t* q, cf_t* table, uint32_t nsymbols, uint32_t nbits_x_symbol) {
  if (q->nsymbols) {
    return SRSLTE_ERROR;
  }
  q->nsymbols = nsymbols;
  if (table_create(q)) {
    return SRSLTE_ERROR;
  }
  memcpy(q->symbol_table,table,q->nsymbols*sizeof(cf_t));
  q->nbits_x_symbol = nbits_x_symbol;
  return SRSLTE_SUCCESS;
}

int srslte_modem_table_lte(srslte_modem_table_t* q, srslte_mod_t modulation) {
  srslte_modem_table_init(q);
  switch(modulation) {
  case SRSLTE_MOD_BPSK:
    q->nbits_x_symbol = 1;
    q->nsymbols = 2;
    if (table_create(q)) {
      return SRSLTE_ERROR;
    }
    set_BPSKtable(q->symbol_table);
    break;
  case SRSLTE_MOD_QPSK:
    q->nbits_x_symbol = 2;
    q->nsymbols = 4;
    if (table_create(q)) {
      return SRSLTE_ERROR;
    }
    set_QPSKtable(q->symbol_table);
    break;
  case SRSLTE_MOD_16QAM:
    q->nbits_x_symbol = 4;
    q->nsymbols = 16;
    if (table_create(q)) {
      return SRSLTE_ERROR;
    }
    set_16QAMtable(q->symbol_table);
    break;
  case SRSLTE_MOD_64QAM:
    q->nbits_x_symbol = 6;
    q->nsymbols = 64;
    if (table_create(q)) {
      return SRSLTE_ERROR;
    }
    set_64QAMtable(q->symbol_table);
    break;
  }
  return SRSLTE_SUCCESS;
}

void srslte_modem_table_bytes(srslte_modem_table_t* q) {
  uint8_t mask_qpsk[4]  = {0xc0, 0x30, 0xc, 0x3}; 
  uint8_t mask_16qam[2] = {0xf0, 0xf}; 
  
  switch(q->nbits_x_symbol) {
    case 1:
      q->symbol_table_bpsk = srslte_vec_malloc(sizeof(bpsk_packed_t)*256);
      for (uint32_t i=0;i<256;i++) {
        for (int j=0;j<8;j++) {
          q->symbol_table_bpsk[i].symbol[j] = q->symbol_table[(i&(1<<(7-j)))>>(7-j)];
        }
      }
      q->byte_tables_init = true; 
      break;
    case 2:
      q->symbol_table_qpsk = srslte_vec_malloc(sizeof(qpsk_packed_t)*256);
      for (uint32_t i=0;i<256;i++) {
        for (int j=0;j<4;j++) {
          q->symbol_table_qpsk[i].symbol[j] = q->symbol_table[(i&mask_qpsk[j])>>(6-j*2)];
        }
      }
      q->byte_tables_init = true; 
      break;
    case 4:
      q->symbol_table_16qam = srslte_vec_malloc(sizeof(qam16_packed_t)*256);
      for (uint32_t i=0;i<256;i++) {
        for (int j=0;j<2;j++) {
          q->symbol_table_16qam[i].symbol[j] = q->symbol_table[(i&mask_16qam[j])>>(4-j*4)];
        }
      }
      q->byte_tables_init = true; 
      break;
    case 6:
      q->byte_tables_init = true; 
      break;
  }
}



