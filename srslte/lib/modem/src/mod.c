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


#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include "srslte/utils/bit.h"
#include "srslte/modem/mod.h"

/** Low-level API */

int srslte_mod_modulate(srslte_modem_table_t* q, uint8_t *bits, cf_t* symbols, uint32_t nbits) {
  uint32_t i,j,idx;
  uint8_t *b_ptr=(uint8_t*) bits;
  j=0;
  for (i=0;i<nbits;i+=q->nbits_x_symbol) {
    idx = srslte_bit_pack(&b_ptr,q->nbits_x_symbol);
    if (idx < q->nsymbols) {
      symbols[j] = q->symbol_table[idx];      
    } else {
      return SRSLTE_ERROR;
    }
    j++;
  }
  return j;
}

/* Assumes packet bits as input */
int srslte_mod_modulate_bytes(srslte_modem_table_t* q, uint8_t *bits, cf_t* symbols, uint32_t nbits) {
  if (nbits%8) {
    fprintf(stderr, "Warning: srslte_mod_modulate_bytes() accepts byte-aligned inputs only "
                    "(nbits=%d, bits_x_symbol=%d)\n", nbits, q->nbits_x_symbol);
  }
  if (!q->byte_tables_init) {
    fprintf(stderr, "Error need to initiated modem tables for packeted bits before calling srslte_mod_modulate_bytes()\n");
    return -1; 
  }
  switch(q->nbits_x_symbol) {
    case 1:
      for (int i=0;i<nbits/8;i++) {
        memcpy(&symbols[8*i], &q->symbol_table_bpsk[bits[i]], sizeof(bpsk_packed_t));
      }
      break;
    case 2:
      for (int i=0;i<nbits/8;i++) {
        memcpy(&symbols[4*i], &q->symbol_table_qpsk[bits[i]], sizeof(qpsk_packed_t));
      }
      break;
    case 4:
      for (int i=0;i<nbits/8;i++) {
        memcpy(&symbols[2*i], &q->symbol_table_16qam[bits[i]], sizeof(qam16_packed_t));
      }
      break;
    case 6:
      if (nbits%24) {
        fprintf(stderr, "Warning: for 64QAM srslte_mod_modulate_bytes() accepts 24-bit aligned inputs only\n");
      }

      for (int i=0;i<nbits/24;i++) {
        uint32_t in80 = bits[3*i+0];
        uint32_t in81 = bits[3*i+1];
        uint32_t in82 = bits[3*i+2];
        
        uint8_t in0 = (in80&0xfc)>>2;
        uint8_t in1 = (in80&0x03)<<4 | ((in81&0xf0)>>4);
        uint8_t in2 = (in81&0x0f)<<2 | ((in82&0xc0)>>6);
        uint8_t in3 = in82&0x3f;
        
        symbols[i*4+0] = q->symbol_table[in0];
        symbols[i*4+1] = q->symbol_table[in1];
        symbols[i*4+2] = q->symbol_table[in2];
        symbols[i*4+3] = q->symbol_table[in3];
      }
      break;      
    default:
      fprintf(stderr, "srslte_mod_modulate_bytes() accepts QPSK/16QAM/64QAM modulations only\n");
      return -1; 
  }
  return nbits/q->nbits_x_symbol;
}


/* High-Level API */
int mod_initialize(srslte_mod_hl* hl) {
  srslte_modem_table_init(&hl->obj);
  if (srslte_modem_table_lte(&hl->obj,hl->init.std,false)) {
    return -1;
  }

  return 0;
}

int mod_work(srslte_mod_hl* hl) {
  int ret = srslte_mod_modulate(&hl->obj,hl->input,hl->output,hl->in_len);
  hl->out_len = ret;
  return 0;
}

int mod_stop(srslte_mod_hl* hl) {
  srslte_modem_table_free(&hl->obj);
  return 0;
}


