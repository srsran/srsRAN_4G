/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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
 *  File:         demod_soft.h
 *
 *  Description:  Soft demodulator.
 *                Supports BPSK, QPSK, 16QAM and 64QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef SRSLTE_DEMOD_SOFT_H
#define SRSLTE_DEMOD_SOFT_H

#include <stdint.h>

#include "modem_table.h"
#include "srslte/config.h"

SRSLTE_API int srslte_demod_soft_demodulate(srslte_mod_t modulation, const cf_t* symbols, float* llr, int nsymbols);

SRSLTE_API int srslte_demod_soft_demodulate_s(srslte_mod_t modulation, const cf_t* symbols, short* llr, int nsymbols);

SRSLTE_API int srslte_demod_soft_demodulate_b(srslte_mod_t modulation, const cf_t* symbols, int8_t* llr, int nsymbols);

#endif // SRSLTE_DEMOD_SOFT_H
