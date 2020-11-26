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
 *  File:         mod.h
 *
 *  Description:  Modulation.
 *                Supports BPSK, QPSK, 16QAM, 64QAM and 256QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef SRSLTE_MOD_H
#define SRSLTE_MOD_H

#include <stdint.h>

#include "modem_table.h"
#include "srslte/config.h"

SRSLTE_API int srslte_mod_modulate(const srslte_modem_table_t* table, uint8_t* bits, cf_t* symbols, uint32_t nbits);

SRSLTE_API int
srslte_mod_modulate_bytes(const srslte_modem_table_t* q, const uint8_t* bits, cf_t* symbols, uint32_t nbits);

#endif // SRSLTE_MOD_H
