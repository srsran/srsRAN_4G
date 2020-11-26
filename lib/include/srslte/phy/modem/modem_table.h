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
 *  File:         modem_table.h
 *
 *  Description:  Modem tables used for modulation/demodulation.
 *                Supports BPSK, QPSK, 16QAM and 64QAM.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 7.1
 *****************************************************************************/

#ifndef SRSLTE_MODEM_TABLE_H
#define SRSLTE_MODEM_TABLE_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"

typedef struct {
  cf_t symbol[8];
} bpsk_packed_t;

typedef struct {
  cf_t symbol[4];
} qpsk_packed_t;

typedef struct {
  cf_t symbol[2];
} qam16_packed_t;

typedef struct SRSLTE_API {
  cf_t*    symbol_table;   // bit-to-symbol mapping
  uint32_t nsymbols;       // number of modulation symbols
  uint32_t nbits_x_symbol; // number of bits per symbol

  bool            byte_tables_init;
  bpsk_packed_t*  symbol_table_bpsk;
  qpsk_packed_t*  symbol_table_qpsk;
  qam16_packed_t* symbol_table_16qam;
} srslte_modem_table_t;

SRSLTE_API void srslte_modem_table_init(srslte_modem_table_t* q);

SRSLTE_API void srslte_modem_table_free(srslte_modem_table_t* q);

SRSLTE_API void srslte_modem_table_reset(srslte_modem_table_t* q);

SRSLTE_API int srslte_modem_table_set(srslte_modem_table_t* q, cf_t* table, uint32_t nsymbols, uint32_t nbits_x_symbol);

SRSLTE_API int srslte_modem_table_lte(srslte_modem_table_t* q, srslte_mod_t modulation);

SRSLTE_API void srslte_modem_table_bytes(srslte_modem_table_t* q);

#endif // SRSLTE_MODEM_TABLE_H
