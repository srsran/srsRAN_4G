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

#ifndef SRSLTE_UCI_CFG_NR_H
#define SRSLTE_UCI_CFG_NR_H

#include <stdbool.h>
#include <stdint.h>

#define SRSLTE_UCI_NR_MAX_ACK_BITS 360
#define SRSLTE_UCI_NR_MAX_SR_BITS 10
#define SRSLTE_UCI_NR_MAX_CSI1_BITS 10
#define SRSLTE_UCI_NR_MAX_CSI2_BITS 10

typedef struct SRSLTE_API {
  uint32_t     o_ack;      ///< Number of HARQ-ACK bits
  uint32_t     o_sr;       ///< Number of SR bits
  uint32_t     o_csi1;     ///< Number of CSI1 report number of bits
  uint32_t     o_csi2;     ///< Number of CSI2 report number of bits
  srslte_mod_t modulation; ///< Modulation
} srslte_uci_cfg_nr_t;

typedef struct SRSLTE_API {
  uint8_t ack[SRSLTE_UCI_NR_MAX_ACK_BITS];
  uint8_t sr[SRSLTE_UCI_NR_MAX_SR_BITS];
  uint8_t csi1[SRSLTE_UCI_NR_MAX_CSI1_BITS];
  uint8_t csi2[SRSLTE_UCI_NR_MAX_CSI2_BITS];
  bool    valid;
} srslte_uci_value_nr_t;

#endif // SRSLTE_UCI_CFG_NR_H
