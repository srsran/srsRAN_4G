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
