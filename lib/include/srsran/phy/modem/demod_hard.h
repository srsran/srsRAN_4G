/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

#ifndef SRSRAN_DEMOD_HARD_H
#define SRSRAN_DEMOD_HARD_H

#include <stdint.h>

#include "modem_table.h"
#include "srsran/config.h"

typedef struct SRSRAN_API {
  srsran_mod_t mod; /* In this implementation, mapping table is hard-coded */
} srsran_demod_hard_t;

SRSRAN_API void srsran_demod_hard_init(srsran_demod_hard_t* q);

SRSRAN_API void srsran_demod_hard_table_set(srsran_demod_hard_t* q, srsran_mod_t mod);

SRSRAN_API int srsran_demod_hard_demodulate(srsran_demod_hard_t* q, cf_t* symbols, uint8_t* bits, uint32_t nsymbols);

#endif // SRSRAN_DEMOD_HARD_H
