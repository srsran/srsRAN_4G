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

#include "liblte/phy/modem/demod_hard.h"
#include "hard_demod_lte.h"


void demod_hard_init(demod_hard_t* q) {
  bzero((void*) q, sizeof(demod_hard_t));
}

void demod_hard_table_set(demod_hard_t* q, lte_mod_t mod) {
  q->mod = mod;
}

int demod_hard_demodulate(demod_hard_t* q, cf_t* symbols, uint8_t *bits, uint32_t nsymbols) {

  int nbits=-1;
  switch(q->mod) {
  case LTE_BPSK:
    hard_bpsk_demod(symbols,bits,nsymbols);
    nbits=nsymbols;
    break;
  case LTE_QPSK:
    hard_qpsk_demod(symbols,bits,nsymbols);
    nbits=nsymbols*2;
    break;
  case LTE_QAM16:
    hard_qam16_demod(symbols,bits,nsymbols);
    nbits=nsymbols*4;
    break;
  case LTE_QAM64:
    hard_qam64_demod(symbols,bits,nsymbols);
    nbits=nsymbols*6;
    break;
  }
  return nbits;
}


int demod_hard_initialize(demod_hard_hl* hl) {
  demod_hard_init(&hl->obj);
  demod_hard_table_set(&hl->obj,hl->init.std);

  return 0;
}

int demod_hard_work(demod_hard_hl* hl) {
  int ret = demod_hard_demodulate(&hl->obj,hl->input,hl->output,hl->in_len);
  hl->out_len = ret;
  return 0;
}

int demod_hard_stop(demod_hard_hl* hl) {
  return 0;
}


