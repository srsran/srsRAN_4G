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

#ifndef SRSLTE_UCI_CFG_H
#define SRSLTE_UCI_CFG_H

#include "srslte/phy/phch/cqi.h"

#define SRSLTE_UCI_MAX_ACK_BITS 10
#define SRSLTE_UCI_MAX_ACK_SR_BITS (SRSLTE_UCI_MAX_ACK_BITS + 1)
#define SRSLTE_UCI_MAX_M 9

typedef struct SRSLTE_API {
  uint8_t ack_value[SRSLTE_UCI_MAX_ACK_BITS];
  bool    valid;
} srslte_uci_value_ack_t;

typedef struct SRSLTE_API {
  bool     pending_tb[SRSLTE_MAX_CODEWORDS]; //< Indicates whether there was a grant that requires an ACK/NACK
  uint32_t nof_acks;                         //< Number of transport blocks, deduced from transmission mode
  uint32_t ncce[SRSLTE_UCI_MAX_M];
  uint32_t N_bundle;
  uint32_t tdd_ack_M;
  uint32_t tdd_ack_m;
  bool     tdd_is_multiplex;
  uint32_t tpc_for_pucch;
  uint32_t grant_cc_idx;
} srslte_uci_cfg_ack_t;

typedef struct SRSLTE_API {
  srslte_uci_cfg_ack_t ack[SRSLTE_MAX_CARRIERS];
  srslte_cqi_cfg_t     cqi;
  bool                 is_scheduling_request_tti;
} srslte_uci_cfg_t;

typedef struct SRSLTE_API {
  bool                   scheduling_request;
  srslte_cqi_value_t     cqi;
  srslte_uci_value_ack_t ack;
  uint8_t                ri; // Only 1-bit supported for RI
} srslte_uci_value_t;

typedef struct SRSLTE_API {
  srslte_uci_cfg_t   cfg;
  srslte_uci_value_t value;
} srslte_uci_data_t;

typedef enum { UCI_BIT_0 = 0, UCI_BIT_1 = 1, UCI_BIT_REPETITION = 2, UCI_BIT_PLACEHOLDER = 3 } srslte_uci_bit_type_t;

typedef struct {
  uint32_t              position;
  srslte_uci_bit_type_t type;
} srslte_uci_bit_t;

#endif // SRSLTE_UCI_CFG_H
