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

/******************************************************************************
 *  File:         demod_hard.h
 *
 *  Description:  Hard demodulator.
 *                Supports BPSK, QPSK, 16QAM and 64QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef SRSLTE_DEMOD_HARD_H
#define SRSLTE_DEMOD_HARD_H

#include <complex.h>
#include <stdint.h>

#include "srslte/config.h"
#include "modem_table.h"

typedef struct SRSLTE_API {
  srslte_mod_t mod; /* In this implementation, mapping table is hard-coded */
}srslte_demod_hard_t;


SRSLTE_API void srslte_demod_hard_init(srslte_demod_hard_t* q);

SRSLTE_API void srslte_demod_hard_table_set(srslte_demod_hard_t* q, 
                                            srslte_mod_t mod);

SRSLTE_API int srslte_demod_hard_demodulate(srslte_demod_hard_t* q, 
                                            cf_t* symbols, 
                                            uint8_t *bits, 
                                            uint32_t nsymbols);

#endif // SRSLTE_DEMOD_HARD_H
