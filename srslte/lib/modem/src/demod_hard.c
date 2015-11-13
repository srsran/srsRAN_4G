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
#include <strings.h>

#include "srslte/modem/demod_hard.h"
#include "hard_demod_lte.h"


void srslte_demod_hard_init(srslte_demod_hard_t* q) {
  bzero((void*) q, sizeof(srslte_demod_hard_t));
}

void srslte_demod_hard_table_set(srslte_demod_hard_t* q, srslte_mod_t mod) {
  q->mod = mod;
}

int srslte_demod_hard_demodulate(srslte_demod_hard_t* q, cf_t* symbols, uint8_t *bits, uint32_t nsymbols) {

  int nbits=-1;
  switch(q->mod) {
  case SRSLTE_MOD_BPSK:
    hard_bpsk_demod(symbols,bits,nsymbols);
    nbits=nsymbols;
    break;
  case SRSLTE_MOD_QPSK:
    hard_qpsk_demod(symbols,bits,nsymbols);
    nbits=nsymbols*2;
    break;
  case SRSLTE_MOD_16QAM:
    hard_qam16_demod(symbols,bits,nsymbols);
    nbits=nsymbols*4;
    break;
  case SRSLTE_MOD_64QAM:
    hard_qam64_demod(symbols,bits,nsymbols);
    nbits=nsymbols*6;
    break;
  }
  return nbits;
}


