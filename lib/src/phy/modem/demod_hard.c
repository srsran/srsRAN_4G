/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <stdlib.h>
#include <strings.h>

#include "hard_demod_lte.h"
#include "srsran/phy/modem/demod_hard.h"

void srsran_demod_hard_init(srsran_demod_hard_t* q)
{
  bzero((void*)q, sizeof(srsran_demod_hard_t));
}

void srsran_demod_hard_table_set(srsran_demod_hard_t* q, srsran_mod_t mod)
{
  q->mod = mod;
}

int srsran_demod_hard_demodulate(srsran_demod_hard_t* q, cf_t* symbols, uint8_t* bits, uint32_t nsymbols)
{

  int nbits = -1;
  switch (q->mod) {
    case SRSRAN_MOD_BPSK:
      hard_bpsk_demod(symbols, bits, nsymbols);
      nbits = nsymbols;
      break;
    case SRSRAN_MOD_QPSK:
      hard_qpsk_demod(symbols, bits, nsymbols);
      nbits = nsymbols * 2;
      break;
    case SRSRAN_MOD_16QAM:
      hard_qam16_demod(symbols, bits, nsymbols);
      nbits = nsymbols * 4;
      break;
    case SRSRAN_MOD_64QAM:
      hard_qam64_demod(symbols, bits, nsymbols);
      nbits = nsymbols * 6;
      break;
    case SRSRAN_MOD_256QAM:
      hard_qam256_demod(symbols, bits, nsymbols);
      nbits = nsymbols * 8;
      break;
    case SRSRAN_MOD_NITEMS:
    default:; // Do nothing
  }
  return nbits;
}
