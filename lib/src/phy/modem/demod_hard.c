/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <stdlib.h>
#include <strings.h>

#include "hard_demod_lte.h"
#include "srslte/phy/modem/demod_hard.h"

void srslte_demod_hard_init(srslte_demod_hard_t* q)
{
  bzero((void*)q, sizeof(srslte_demod_hard_t));
}

void srslte_demod_hard_table_set(srslte_demod_hard_t* q, srslte_mod_t mod)
{
  q->mod = mod;
}

int srslte_demod_hard_demodulate(srslte_demod_hard_t* q, cf_t* symbols, uint8_t* bits, uint32_t nsymbols)
{

  int nbits = -1;
  switch (q->mod) {
    case SRSLTE_MOD_BPSK:
      hard_bpsk_demod(symbols, bits, nsymbols);
      nbits = nsymbols;
      break;
    case SRSLTE_MOD_QPSK:
      hard_qpsk_demod(symbols, bits, nsymbols);
      nbits = nsymbols * 2;
      break;
    case SRSLTE_MOD_16QAM:
      hard_qam16_demod(symbols, bits, nsymbols);
      nbits = nsymbols * 4;
      break;
    case SRSLTE_MOD_64QAM:
      hard_qam64_demod(symbols, bits, nsymbols);
      nbits = nsymbols * 6;
      break;
    case SRSLTE_MOD_256QAM:
      hard_qam256_demod(symbols, bits, nsymbols);
      nbits = nsymbols * 8;
      break;
    case SRSLTE_MOD_NITEMS:
    default:; // Do nothing
  }
  return nbits;
}
