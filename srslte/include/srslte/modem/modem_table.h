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

/******************************************************************************
 *  File:         modem_table.h
 *
 *  Description:  Modem tables used for modulation/demodulation.
 *                Supports BPSK, QPSK, 16QAM and 64QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef MODEM_TABLE_
#define MODEM_TABLE_

#include <stdbool.h>
#include <complex.h>
#include <stdint.h>

#include "srslte/common/phy_common.h"
#include "srslte/config.h"

typedef struct SRSLTE_API {
  uint32_t idx[2][6][32];
  uint32_t min_idx[2][64][6];	/* NEW: for each constellation point zone (2, 4, 16, 64 for BPSK, QPSK, 16QAM, 64QAM) the 2x(1, 2, 4, and 6 closest constellation points) for each bit, respectively. */
  uint32_t d_idx[64][7];	/* NEW: for each constellation point zone (2, 4, 16, 64 for BPSK, QPSK, 16QAM, 64QAM) the 2, 3, 5 and 7 indices to constellation points that need to be computed for any recevied symbol modulated as BPSK, QPSK, 16QAM, and 64QAM, respectively. */

}srslte_soft_table_t;

typedef struct SRSLTE_API {
  cf_t* symbol_table;     	// bit-to-symbol mapping
  srslte_soft_table_t soft_table;   	// symbol-to-bit mapping (used in soft demodulating)
  uint32_t nsymbols;        	// number of modulation symbols
  uint32_t nbits_x_symbol;      // number of bits per symbol
}srslte_modem_table_t;


SRSLTE_API void srslte_modem_table_init(srslte_modem_table_t* q);

SRSLTE_API void srslte_modem_table_free(srslte_modem_table_t* q);

SRSLTE_API void srslte_modem_table_reset(srslte_modem_table_t* q);

SRSLTE_API int srslte_modem_table_set(srslte_modem_table_t* q, 
                                      cf_t* table, 
                                      srslte_soft_table_t *soft_table, 
                                      uint32_t nsymbols, 
                                      uint32_t nbits_x_symbol);

SRSLTE_API int srslte_modem_table_lte(srslte_modem_table_t* q, 
                                      srslte_mod_t modulation, 
                                      bool compute_soft_demod);

#endif // MODEM_TABLE_
