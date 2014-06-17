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



#ifndef MODEM_TABLE_
#define MODEM_TABLE_

#include <stdbool.h>
#include <complex.h>
#include <stdint.h>

#include "lte/config.h"

typedef _Complex float cf_t;
typedef struct LIBLTE_API {
  int idx[2][6][32];
}soft_table_t;

typedef struct LIBLTE_API {
  cf_t* symbol_table;     // bit-to-symbol mapping
  soft_table_t soft_table;   // symbol-to-bit mapping (used in soft demodulating)
  int nsymbols;        // number of modulation symbols
  int nbits_x_symbol;      // number of bits per symbol
}modem_table_t;


// Modulation standards
enum modem_std {
  LTE_BPSK = 1, LTE_QPSK = 2, LTE_QAM16 = 4, LTE_QAM64 = 6
};

LIBLTE_API void modem_table_init(modem_table_t* q);
LIBLTE_API void modem_table_free(modem_table_t* q);
LIBLTE_API void modem_table_reset(modem_table_t* q);
LIBLTE_API int modem_table_set(modem_table_t* q, cf_t* table, soft_table_t *soft_table, int nsymbols, int nbits_x_symbol);
LIBLTE_API int modem_table_std(modem_table_t* q, enum modem_std table, bool compute_soft_demod);

#endif // MODEM_TABLE_
