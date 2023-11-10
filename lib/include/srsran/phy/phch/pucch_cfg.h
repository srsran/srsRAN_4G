/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_PUCCH_CFG_H
#define SRSRAN_PUCCH_CFG_H

#include "srsran/phy/phch/cqi.h"
#include "srsran/phy/phch/ra.h"
#include "srsran/phy/phch/uci_cfg.h"

#define SRSRAN_PUCCH_SIZE_AN_CS 4
#define SRSRAN_PUCCH_SIZE_AN_N3 4
#define SRSRAN_PUCCH_NOF_AN_CS 2
#define SRSRAN_PUCCH_MAX_BITS SRSRAN_CQI_MAX_BITS

typedef enum SRSRAN_API {
  SRSRAN_PUCCH_FORMAT_1 = 0,
  SRSRAN_PUCCH_FORMAT_1A,
  SRSRAN_PUCCH_FORMAT_1B,
  SRSRAN_PUCCH_FORMAT_2,
  SRSRAN_PUCCH_FORMAT_2A,
  SRSRAN_PUCCH_FORMAT_2B,
  SRSRAN_PUCCH_FORMAT_3,
  SRSRAN_PUCCH_FORMAT_ERROR,
} srsran_pucch_format_t;

typedef struct SRSRAN_API {
  // Input configuration for this subframe
  uint16_t rnti;

  // UCI configuration
  srsran_uci_cfg_t uci_cfg;

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
  srsran_ack_nack_feedback_mode_t ack_nack_feedback_mode;
  uint32_t                        n1_pucch_an_cs[SRSRAN_PUCCH_SIZE_AN_CS][SRSRAN_PUCCH_NOF_AN_CS];
  uint32_t                        n3_pucch_an_list[SRSRAN_PUCCH_SIZE_AN_N3];

  // Other configuration
  float threshold_format1;
  float threshold_data_valid_format1a;
  float threshold_data_valid_format2;
  float threshold_data_valid_format3;
  float threshold_dmrs_detection;
  bool  meas_ta_en;
  bool  use_cedron_alg;

  // PUCCH configuration generated during a call to encode/decode
  srsran_pucch_format_t format;
  uint16_t              n_pucch;
  uint8_t               pucch2_drs_bits[SRSRAN_PUCCH_MAX_BITS];

} srsran_pucch_cfg_t;

#endif // SRSRAN_PUCCH_CFG_H
