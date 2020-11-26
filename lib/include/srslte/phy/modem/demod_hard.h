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
 *  File:         demod_hard.h
 *
 *  Description:  Hard demodulator.
 *                Supports BPSK, QPSK, 16QAM and 64QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef SRSLTE_DEMOD_HARD_H
#define SRSLTE_DEMOD_HARD_H

#include <stdint.h>

#include "modem_table.h"
#include "srslte/config.h"

typedef struct SRSLTE_API {
  srslte_mod_t mod; /* In this implementation, mapping table is hard-coded */
} srslte_demod_hard_t;

SRSLTE_API void srslte_demod_hard_init(srslte_demod_hard_t* q);

SRSLTE_API void srslte_demod_hard_table_set(srslte_demod_hard_t* q, srslte_mod_t mod);

SRSLTE_API int srslte_demod_hard_demodulate(srslte_demod_hard_t* q, cf_t* symbols, uint8_t* bits, uint32_t nsymbols);

#endif // SRSLTE_DEMOD_HARD_H
