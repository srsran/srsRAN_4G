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
  
void mod_bpsk_bytes(srslte_modem_table_t* q, uint8_t *bits, cf_t* symbols, uint32_t nbits) {
  uint8_t mask_bpsk[8]  = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
  uint8_t shift_bpsk[8] = {7, 6, 5, 4, 3, 2, 1, 0};

  for (int i=0;i<nbits/8;i++) {
    memcpy(&symbols[8*i], &q->symbol_table_bpsk[bits[i]], sizeof(bpsk_packed_t));
  }
  for (int i=0;i<nbits%8;i++) {
    symbols[8*(nbits/8)+i] = q->symbol_table[(bits[8*(nbits/8)+i]&mask_bpsk[i])>>shift_bpsk[i]];
  }    
}

void mod_qpsk_bytes(srslte_modem_table_t* q, uint8_t *bits, cf_t* symbols, uint32_t nbits) {
  uint8_t mask_qpsk[4]  = {0xc0, 0x30, 0x0c, 0x03};
  uint8_t shift_qpsk[4]  = {6, 4, 2, 0};
  for (int i=0;i<nbits/8;i++) {
    memcpy(&symbols[4*i], &q->symbol_table_qpsk[bits[i]], sizeof(qpsk_packed_t));
  }
  // Encode last 1, 2 or 3 bit pairs if not multiple of 8
  for (int i=0;i<(nbits%8)/2;i++) {
    symbols[4*(nbits/8)+i] = q->symbol_table[(bits[nbits/8]&mask_qpsk[i])>>shift_qpsk[i]];
  }
}

void mod_16qam_bytes(srslte_modem_table_t* q, uint8_t *bits, cf_t* symbols, uint32_t nbits) {
  for (int i=0;i<nbits/8;i++) {
    memcpy(&symbols[2*i], &q->symbol_table_16qam[bits[i]], sizeof(qam16_packed_t));
  }
  // Encode last 4 bits if not multiple of 8
  if (nbits%8) {
    symbols[2*(nbits/8)] = q->symbol_table[(bits[nbits/8]&0xf0)>>4];
  }
}

void mod_64qam_bytes(srslte_modem_table_t* q, uint8_t *bits, cf_t* symbols, uint32_t nbits) {
  uint8_t in0, in1, in2, in3; 
  uint32_t in80, in81, in82;
  
  for (int i=0;i<nbits/24;i++) {
    in80 = bits[3*i+0];
    in81 = bits[3*i+1];
    in82 = bits[3*i+2];
    
    in0 = (in80&0xfc)>>2;
    in1 = (in80&0x03)<<4 | ((in81&0xf0)>>4);
    in2 = (in81&0x0f)<<2 | ((in82&0xc0)>>6);
    in3 = in82&0x3f;
    
    symbols[i*4+0] = q->symbol_table[in0];
    symbols[i*4+1] = q->symbol_table[in1];
    symbols[i*4+2] = q->symbol_table[in2];
    symbols[i*4+3] = q->symbol_table[in3];
  }
  if (nbits%24 >= 6) {
    in80 = bits[3*(nbits/24)+0];
    in0 = (in80&0xfc)>>2;
    
    symbols[4*(nbits/24)+0] = q->symbol_table[in0];
  }
  if (nbits%24 >= 12) {
    in81 = bits[3*(nbits/24)+1];
    in1 = (in80&0x03)<<4 | ((in81&0xf0)>>4);
    
    symbols[4*(nbits/24)+1] = q->symbol_table[in1];
  }
  if (nbits%24 >= 18) {
    in82 = bits[3*(nbits/24)+2];
    in2 = (in81&0x0f)<<2 | ((in82&0xc0)>>6);

    symbols[4*(nbits/24)+2] = q->symbol_table[in2];
  }
}

/* Assumes packet bits as input */
int srslte_mod_modulate_bytes(srslte_modem_table_t* q, uint8_t *bits, cf_t* symbols, uint32_t nbits) 
{

  if (!q->byte_tables_init) {
    fprintf(stderr, "Error need to initiated modem tables for packeted bits before calling srslte_mod_modulate_bytes()\n");
    return -1; 
  }
  if (nbits % q->nbits_x_symbol) {
    fprintf(stderr, "Error modulator expects number of bits (%d) to be multiple of %d\n", nbits, q->nbits_x_symbol);
    return -1; 
  }
  switch(q->nbits_x_symbol) {
    case 1:
      mod_bpsk_bytes(q, bits, symbols, nbits);
      break;
    case 2:
      mod_qpsk_bytes(q, bits, symbols, nbits);
      break;
    case 4:
      mod_16qam_bytes(q, bits, symbols, nbits);
      break;
    case 6:
      mod_64qam_bytes(q, bits, symbols, nbits);
      break;      
    default:
      fprintf(stderr, "srslte_mod_modulate_bytes() accepts QPSK/16QAM/64QAM modulations only\n");
      return -1; 
  }
  return nbits/q->nbits_x_symbol;
}
