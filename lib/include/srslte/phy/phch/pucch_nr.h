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

#ifndef SRSLTE_PUCCH_NR_H
#define SRSLTE_PUCCH_NR_H

#include "srslte/config.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum SRSLTE_API {
  SRSLTE_PUCCH_NR_FORMAT_0 = 0,
  SRSLTE_PUCCH_NR_FORMAT_1,
  SRSLTE_PUCCH_NR_FORMAT_2,
  SRSLTE_PUCCH_NR_FORMAT_3,
  SRSLTE_PUCCH_NR_FORMAT_4,
  SRSLTE_PUCCH_NR_FORMAT_ERROR,
} srslte_pucch_nr_format_t;

/**
 * @brief Generic PUCCH Resource configuration
 */
typedef struct SRSLTE_API {
  // Common
  srslte_pucch_nr_format_t format;           ///< PUCCH format this configuration belongs
  uint32_t                 nof_symbols;      ///< Number of symbols
  uint32_t                 start_symbol_idx; ///< Starting symbol index
  double                   max_code_rate;    ///< Maximum code rate (0.08, 0.15, 0.25, 0.35, 0.45, 0.60, 0.80)
  bool                     enable_pi_bpsk;   ///< Enables PI-BPSK

  // Other parameters
  uint32_t initial_cyclic_shift; ///< Used by formats 0, 1
  uint32_t time_domain_occ;      ///< Used by format 1
  uint32_t nof_prb;              ///< Used by formats 2, 3
  uint32_t occ_lenth;            ///< Spreading factor, used by format 4
  uint32_t occ_index;            ///< Used by format 4

} srslte_pucch_nr_cfg_t;

#endif // SRSLTE_PUCCH_NR_H
