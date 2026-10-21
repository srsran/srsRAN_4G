// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

/******************************************************************************
 *  File:         demod_soft.h
 *
 *  Description:  Soft demodulator.
 *                Supports BPSK, QPSK, 16QAM and 64QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef SRSRAN_DEMOD_SOFT_H
#define SRSRAN_DEMOD_SOFT_H

#include <stdint.h>

#include "modem_table.h"
#include "srsran/config.h"

SRSRAN_API int srsran_demod_soft_demodulate(srsran_mod_t modulation, const cf_t* symbols, float* llr, int nsymbols);

SRSRAN_API int srsran_demod_soft_demodulate_s(srsran_mod_t modulation, const cf_t* symbols, short* llr, int nsymbols);

SRSRAN_API int srsran_demod_soft_demodulate_b(srsran_mod_t modulation, const cf_t* symbols, int8_t* llr, int nsymbols);

#endif // SRSRAN_DEMOD_SOFT_H
