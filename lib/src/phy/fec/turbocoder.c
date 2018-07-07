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


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/turbocoder.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/vector.h"

#define NOF_REGS 3

#define RATE 3
#define TOTALTAIL 12

uint8_t tcod_lut_next_state[188][8][256];
uint8_t tcod_lut_output[188][8][256];
uint16_t tcod_per_fw[188][6144];
static srslte_bit_interleaver_t tcod_interleavers[188];

static bool table_initiated = false; 

int srslte_tcod_init(srslte_tcod_t *h, uint32_t max_long_cb) {

  h->max_long_cb = max_long_cb;
  h->temp = srslte_vec_malloc(max_long_cb/8);
  
  if (!table_initiated) {
    table_initiated = true; 
    srslte_tcod_gentable();
  }
  return 0;
}

void srslte_tcod_free(srslte_tcod_t *h) {
  if (table_initiated) {
    h->max_long_cb = 0;
    if (h->temp) {
      free(h->temp);
    }
    for (int i = 0; i < 188; i++) {
      srslte_bit_interleaver_free(&tcod_interleavers[i]);
    }
    table_initiated = false;
  }
}

/* Expects bits (1 byte = 1 bit) and produces bits. The systematic and parity bits are interlaced in the output */
int srslte_tcod_encode(srslte_tcod_t *h, uint8_t *input, uint8_t *output, uint32_t long_cb) 
{

  uint8_t reg1_0, reg1_1, reg1_2, reg2_0, reg2_1, reg2_2;
  uint32_t i, k = 0, j;
  uint8_t bit;
  uint8_t in, out;
  uint16_t *per;

  if (long_cb > h->max_long_cb) {
    fprintf(stderr, "Turbo coder initiated for max_long_cb=%d\n",
        h->max_long_cb);
    return -1;
  }

  int longcb_idx = srslte_cbsegm_cbindex(long_cb);
  if (longcb_idx < 0) {
    fprintf(stderr, "Invalid CB size %d\n", long_cb);
    return -1;
  }
 
  per = tcod_per_fw[longcb_idx];

  reg1_0 = 0;
  reg1_1 = 0;
  reg1_2 = 0;

  reg2_0 = 0;
  reg2_1 = 0;
  reg2_2 = 0;

  k = 0;
  for (i = 0; i < long_cb; i++) {
    if (input[i] == SRSLTE_TX_NULL) {
      bit = 0;
    } else {
      bit = input[i];
    }
    output[k] = input[i];
    
    k++;

    in = bit ^ (reg1_2 ^ reg1_1);
    out = reg1_2 ^ (reg1_0 ^ in);

    reg1_2 = reg1_1;
    reg1_1 = reg1_0;
    reg1_0 = in;
    
    if (input[i] == SRSLTE_TX_NULL) {
      output[k] = SRSLTE_TX_NULL;
    } else {
      output[k] = out;
    }
    k++;

    bit = input[per[i]];
    if (bit == SRSLTE_TX_NULL) {
      bit = 0; 
    }

    in = bit ^ (reg2_2 ^ reg2_1);
    out = reg2_2 ^ (reg2_0 ^ in);

    reg2_2 = reg2_1;
    reg2_1 = reg2_0;
    reg2_0 = in;

    output[k] = out;
    k++;

  }

  k = 3 * long_cb;

  /* TAILING CODER #1 */
  for (j = 0; j < NOF_REGS; j++) {
    bit = reg1_2 ^ reg1_1;

    output[k] = bit;
    k++;

    in = bit ^ (reg1_2 ^ reg1_1);
    out = reg1_2 ^ (reg1_0 ^ in);

    reg1_2 = reg1_1;
    reg1_1 = reg1_0;
    reg1_0 = in;

    output[k] = out;
    k++;
  }

  /* TAILING CODER #2 */
  for (j = 0; j < NOF_REGS; j++) {
    bit = reg2_2 ^ reg2_1;

    output[k] = bit;
    k++;

    in = bit ^ (reg2_2 ^ reg2_1);
    out = reg2_2 ^ (reg2_0 ^ in);

    reg2_2 = reg2_1;
    reg2_1 = reg2_0;
    reg2_0 = in;

    output[k] = out;
    k++;
  }
  return 0;
}

/* Expects bytes and produces bytes. The systematic and parity bits are interlaced in the output */
int srslte_tcod_encode_lut(srslte_tcod_t *h, uint8_t *input, uint8_t *parity, uint32_t cblen_idx) 
{
  if (cblen_idx < 188) {
    uint32_t long_cb = srslte_cbsegm_cbsize(cblen_idx);
    
    if (long_cb % 8) {
      fprintf(stderr, "Turbo coder LUT implementation long_cb must be multiple of 8\n");
      return -1; 
    }
    
    /* Parity bits for the 1st constituent encoders */
    uint8_t state0 = 0;   
    for (uint32_t i=0;i<long_cb/8;i++) {
      parity[i] = tcod_lut_output[cblen_idx][state0][input[i]];    
      state0 = tcod_lut_next_state[cblen_idx][state0][input[i]] % 8;
    }
    parity[long_cb/8] = 0;  // will put tail here later
    
    /* Interleave input */
    srslte_bit_interleaver_run(&tcod_interleavers[cblen_idx], input, h->temp, 0);
    //srslte_bit_interleave(input, h->temp, tcod_per_fw[cblen_idx], long_cb);

    /* Parity bits for the 2nd constituent encoders */
    uint8_t state1 = 0;
    for (uint32_t i=0;i<long_cb/8;i++) {
      uint8_t out = tcod_lut_output[cblen_idx][state1][h->temp[i]];    
      parity[long_cb/8+i] |= (out&0xf0)>>4;
      parity[long_cb/8+i+1] = (out&0xf)<<4; 
      state1 = tcod_lut_next_state[cblen_idx][state1][h->temp[i]] % 8;
    }

    /* Tail bits */
    uint8_t reg1_0, reg1_1, reg1_2, reg2_0, reg2_1, reg2_2;
    uint8_t bit, in, out; 
    uint8_t k=0;
    uint8_t tail[12]; 
    
    reg2_0 = (state1&4)>>2;
    reg2_1 = (state1&2)>>1;
    reg2_2 = state1&1;
    
    reg1_0 = (state0&4)>>2;
    reg1_1 = (state0&2)>>1;
    reg1_2 = state0&1;
      
    /* TAILING CODER #1 */
    for (uint32_t j = 0; j < NOF_REGS; j++) {
      bit = reg1_2 ^ reg1_1;

      tail[k] = bit;
      k++;

      in = bit ^ (reg1_2 ^ reg1_1);
      out = reg1_2 ^ (reg1_0 ^ in);

      reg1_2 = reg1_1;
      reg1_1 = reg1_0;
      reg1_0 = in;

      tail[k] = out;
      k++;
    }

    /* TAILING CODER #2 */
    for (uint32_t j = 0; j < NOF_REGS; j++) {
      bit = reg2_2 ^ reg2_1;

      tail[k] = bit;
      k++;

      in = bit ^ (reg2_2 ^ reg2_1);
      out = reg2_2 ^ (reg2_0 ^ in);

      reg2_2 = reg2_1;
      reg2_1 = reg2_0;
      reg2_0 = in;

      tail[k] = out;
      k++;
    }
    
    uint8_t tailv[3][4];
    for (int i=0;i<4;i++) {
      for (int j=0;j<3;j++) {
        tailv[j][i] = tail[3*i+j];
      }
    }
    uint8_t *x = tailv[0];
    input[long_cb/8] = (srslte_bit_pack(&x, 4)<<4);
    x = tailv[1];
    parity[long_cb/8] |= (srslte_bit_pack(&x, 4)<<4);
    x = tailv[2];
    parity[2*long_cb/8] |= (srslte_bit_pack(&x, 4)&0xf);
    
    return 3*long_cb+TOTALTAIL;
  } else {
    return -1; 
  }
}

void srslte_tcod_gentable() {
  srslte_tc_interl_t interl; 

  if (srslte_tc_interl_init(&interl, 6144)) {
    fprintf(stderr, "Error initiating interleave\n");
    return;
  }
  
  for (uint32_t len=0;len<188;len++) {
    uint32_t long_cb = srslte_cbsegm_cbsize(len);
    if (srslte_tc_interl_LTE_gen(&interl, long_cb)) {
      fprintf(stderr, "Error initiating TC interleaver for long_cb=%d\n", long_cb);
      return;
    }
    // Save fw/bw permutation tables
    for (uint32_t i=0;i<long_cb;i++) {
      tcod_per_fw[len][i] = interl.forward[i];
    }
    srslte_bit_interleaver_init(&tcod_interleavers[len], tcod_per_fw[len], long_cb);
    for (uint32_t i=long_cb;i<6144;i++) {
      tcod_per_fw[len][i] = 0;
    }
    // Compute state transitions
    for (uint32_t state=0;state<8;state++) {
      for (uint32_t data=0;data<256;data++) {
          
        uint8_t reg_0, reg_1, reg_2;
        reg_0 = (state&4)>>2;
        reg_1 = (state&2)>>1;
        reg_2 = state&1;
        
        tcod_lut_output[len][state][data] = 0;
        uint8_t bit, in, out; 
        for (uint32_t i = 0; i < 8; i++) {
          bit = (data&(1<<(7-i)))?1:0;

          in = bit ^ (reg_2 ^ reg_1);
          out = reg_2 ^ (reg_0 ^ in);

          reg_2 = reg_1;
          reg_1 = reg_0;
          reg_0 = in;

          tcod_lut_output[len][state][data] |= out<<(7-i);

        }
        tcod_lut_next_state[len][state][data] = reg_0<<2 | reg_1<<1 | reg_2;        
      }
    }  
  }

  srslte_tc_interl_free(&interl);
}
