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


#include "liblte/phy/fec/tc_interl.h"
#include "liblte/phy/fec/turbocoder.h"

#define NOF_REGS 3

int tcod_init(tcod_t *h, int long_cb) {

  if (tc_interl_LTE_init(&h->interl, long_cb)) {
    return -1;
  }
  h->long_cb = long_cb;
  return 0;
}

void tcod_free(tcod_t *h) {
  tc_interl_free(&h->interl);
  h->long_cb = 0;
}

void tcod_encode(tcod_t *h, char *input, char *output) {
  
  char reg1_0,reg1_1,reg1_2, reg2_0,reg2_1,reg2_2;
  int i,k=0,j;
  char bit;
  char in,out;
  int *per;

  per=h->interl.forward;
  
  reg1_0=0;
  reg1_1=0;
  reg1_2=0;
  
  reg2_0=0;
  reg2_1=0;
  reg2_2=0;
  
  k=0;
  for (i=0;i<h->long_cb;i++) {
    bit=input[i];
    
    output[k]=bit;
    k++;
    
    in=bit^(reg1_2^reg1_1);
    out=reg1_2^(reg1_0^in);

    reg1_2=reg1_1;
    reg1_1=reg1_0;
    reg1_0=in;
    
    output[k]=out;
    k++;
    
    bit=input[per[i]];
    
    in=bit^(reg2_2^reg2_1);
    out=reg2_2^(reg2_0^in);

    reg2_2=reg2_1;
    reg2_1=reg2_0;
    reg2_0=in;
    
    output[k]=out;
    k++;
  }
  
  k=3*h->long_cb;
  
  /* TAILING CODER #1 */
  for (j=0;j<NOF_REGS;j++) {
    bit=reg1_2^reg1_1;
    
    output[k]=bit;
    k++;
    
    in=bit^(reg1_2^reg1_1);
    out=reg1_2^(reg1_0^in);

    reg1_2=reg1_1;
    reg1_1=reg1_0;
    reg1_0=in;
    
    output[k]=out;
    k++;
  }
  
  /* TAILING CODER #2 */
  for (j=0;j<NOF_REGS;j++) {
    bit=reg2_2^reg2_1;
    
    output[k]=bit;
    k++;
      
    in=bit^(reg2_2^reg2_1);
    out=reg2_2^(reg2_0^in);

    reg2_2=reg2_1;
    reg2_1=reg2_0;
    reg2_0=in;
    
    output[k]=out;
    k++;
  }
}

