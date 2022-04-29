/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
 *  File:         modem_table.h
 *
 *  Description:  Modem tables used for modulation/demodulation.
 *                Supports BPSK, QPSK, 16QAM and 64QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef SRSRAN_MODEM_TABLE_H
#define SRSRAN_MODEM_TABLE_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"

typedef struct {
  cf_t symbol[8];
} bpsk_packed_t;

typedef struct {
  cf_t symbol[4];
} qpsk_packed_t;

typedef struct {
  cf_t symbol[2];
} qam16_packed_t;

typedef struct SRSRAN_API {
  cf_t*    symbol_table;   // bit-to-symbol mapping
  uint32_t nsymbols;       // number of modulation symbols
  uint32_t nbits_x_symbol; // number of bits per symbol

  bool            byte_tables_init;
  bpsk_packed_t*  symbol_table_bpsk;
  qpsk_packed_t*  symbol_table_qpsk;
  qam16_packed_t* symbol_table_16qam;
} srsran_modem_table_t;

SRSRAN_API void srsran_modem_table_init(srsran_modem_table_t* q);

SRSRAN_API void srsran_modem_table_free(srsran_modem_table_t* q);

SRSRAN_API void srsran_modem_table_reset(srsran_modem_table_t* q);

SRSRAN_API int srsran_modem_table_set(srsran_modem_table_t* q, cf_t* table, uint32_t nsymbols, uint32_t nbits_x_symbol);

SRSRAN_API int srsran_modem_table_lte(srsran_modem_table_t* q, srsran_mod_t modulation);

SRSRAN_API void srsran_modem_table_bytes(srsran_modem_table_t* q);

#endif // SRSRAN_MODEM_TABLE_H
