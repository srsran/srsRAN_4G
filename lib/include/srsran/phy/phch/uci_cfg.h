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

#ifndef SRSRAN_UCI_CFG_H
#define SRSRAN_UCI_CFG_H

#include "srsran/phy/phch/cqi.h"

#define SRSRAN_UCI_MAX_ACK_BITS 10
#define SRSRAN_UCI_MAX_ACK_SR_BITS (SRSRAN_UCI_MAX_ACK_BITS + 1)
#define SRSRAN_UCI_MAX_M 9

typedef struct SRSRAN_API {
  uint8_t ack_value[SRSRAN_UCI_MAX_ACK_BITS];
  bool    valid;
} srsran_uci_value_ack_t;

typedef struct SRSRAN_API {
  bool     pending_tb[SRSRAN_MAX_CODEWORDS]; //< Indicates whether there was a grant that requires an ACK/NACK
  uint32_t nof_acks;                         //< Number of transport blocks, deduced from transmission mode
  uint32_t ncce[SRSRAN_UCI_MAX_M];
  uint32_t N_bundle;
  uint32_t tdd_ack_M;
  uint32_t tdd_ack_m;
  bool     tdd_is_multiplex;
  uint32_t tpc_for_pucch;
  uint32_t grant_cc_idx;
} srsran_uci_cfg_ack_t;

typedef struct SRSRAN_API {
  srsran_uci_cfg_ack_t ack[SRSRAN_MAX_CARRIERS];
  srsran_cqi_cfg_t     cqi;
  bool                 is_scheduling_request_tti;
} srsran_uci_cfg_t;

typedef struct SRSRAN_API {
  bool                   scheduling_request;
  srsran_cqi_value_t     cqi;
  srsran_uci_value_ack_t ack;
  uint8_t                ri; // Only 1-bit supported for RI
} srsran_uci_value_t;

typedef struct SRSRAN_API {
  srsran_uci_cfg_t   cfg;
  srsran_uci_value_t value;
} srsran_uci_data_t;

typedef enum { UCI_BIT_0 = 0, UCI_BIT_1 = 1, UCI_BIT_REPETITION = 2, UCI_BIT_PLACEHOLDER = 3 } srsran_uci_bit_type_t;

typedef struct {
  uint32_t              position;
  srsran_uci_bit_type_t type;
} srsran_uci_bit_t;

#endif // SRSRAN_UCI_CFG_H
