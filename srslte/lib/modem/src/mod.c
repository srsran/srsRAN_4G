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
#include <assert.h>

#include "srslte/utils/bit.h"
#include "srslte/modem/mod.h"

/** Low-level API */

int mod_modulate(modem_table_t* q, const uint8_t *bits, cf_t* symbols, uint32_t nbits) {
  uint32_t i,j,idx;
  uint8_t *b_ptr=(uint8_t*) bits;
  j=0;
  for (i=0;i<nbits;i+=q->nbits_x_symbol) {
    idx = bit_unpack(&b_ptr,q->nbits_x_symbol);
    if (idx < q->nsymbols) {
      symbols[j] = q->symbol_table[idx];      
    } else {
      return SRSLTE_ERROR;
    }
    j++;
  }
  return j;
}


/* High-Level API */
int mod_initialize(mod_hl* hl) {
  modem_table_init(&hl->obj);
  if (modem_table_lte(&hl->obj,hl->init.std,false)) {
    return -1;
  }

  return 0;
}

int mod_work(mod_hl* hl) {
  int ret = mod_modulate(&hl->obj,hl->input,hl->output,hl->in_len);
  hl->out_len = ret;
  return 0;
}

int mod_stop(mod_hl* hl) {
  modem_table_free(&hl->obj);
  return 0;
}


