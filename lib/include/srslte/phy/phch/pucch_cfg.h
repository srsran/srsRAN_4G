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

#ifndef SRSLTE_PUCCH_CFG_H
#define SRSLTE_PUCCH_CFG_H

#include "srslte/phy/phch/cqi.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/phch/uci_cfg.h"

#define SRSLTE_PUCCH_SIZE_AN_CS 4
#define SRSLTE_PUCCH_SIZE_AN_N3 4
#define SRSLTE_PUCCH_NOF_AN_CS 2
#define SRSLTE_PUCCH_MAX_BITS SRSLTE_CQI_MAX_BITS

typedef enum SRSLTE_API {
  SRSLTE_PUCCH_FORMAT_1 = 0,
  SRSLTE_PUCCH_FORMAT_1A,
  SRSLTE_PUCCH_FORMAT_1B,
  SRSLTE_PUCCH_FORMAT_2,
  SRSLTE_PUCCH_FORMAT_2A,
  SRSLTE_PUCCH_FORMAT_2B,
  SRSLTE_PUCCH_FORMAT_3,
  SRSLTE_PUCCH_FORMAT_ERROR,
} srslte_pucch_format_t;

typedef struct SRSLTE_API {
  // Input configuration for this subframe
  uint16_t rnti;

  // UCI configuration
  srslte_uci_cfg_t uci_cfg;

  // Common configuration
  uint32_t delta_pucch_shift;
  uint32_t n_rb_2;
  uint32_t N_cs;
  uint32_t N_pucch_1;
  bool     group_hopping_en; // common pusch config

  // Dedicated PUCCH configuration
  uint32_t I_sr;
  bool     sr_configured;
  uint32_t n_pucch_1[4]; // 4 n_pucch resources specified by RRC
  uint32_t n_pucch_2;
  uint32_t n_pucch_sr;
  bool     simul_cqi_ack;
  bool     tdd_ack_multiplex; // if false, bundle
  bool     sps_enabled;

  // Release 10 CA specific
  srslte_ack_nack_feedback_mode_t ack_nack_feedback_mode;
  uint32_t                        n1_pucch_an_cs[SRSLTE_PUCCH_SIZE_AN_CS][SRSLTE_PUCCH_NOF_AN_CS];
  uint32_t                        n3_pucch_an_list[SRSLTE_PUCCH_SIZE_AN_N3];

  // Other configuration
  float threshold_format1;
  float threshold_data_valid_format1a;
  float threshold_data_valid_format2;
  float threshold_data_valid_format3;
  float threshold_dmrs_detection;
  bool  meas_ta_en;

  // PUCCH configuration generated during a call to encode/decode
  srslte_pucch_format_t format;
  uint16_t              n_pucch;
  uint8_t               pucch2_drs_bits[SRSLTE_PUCCH_MAX_BITS];

} srslte_pucch_cfg_t;

#endif // SRSLTE_PUCCH_CFG_H
