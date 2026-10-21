// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

/******************************************************************************
 *  File:         mod.h
 *
 *  Description:  Modulation.
 *                Supports BPSK, QPSK, 16QAM, 64QAM and 256QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef SRSRAN_MOD_H
#define SRSRAN_MOD_H

#include <stdint.h>

#include "modem_table.h"
#include "srsran/config.h"

SRSRAN_API int srsran_mod_modulate(const srsran_modem_table_t* table, uint8_t* bits, cf_t* symbols, uint32_t nbits);

SRSRAN_API int
srsran_mod_modulate_bytes(const srsran_modem_table_t* q, const uint8_t* bits, cf_t* symbols, uint32_t nbits);

#endif // SRSRAN_MOD_H
