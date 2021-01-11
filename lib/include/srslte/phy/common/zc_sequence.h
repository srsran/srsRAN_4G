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

#ifndef SRSLTE_ZC_SEQUENCE_H
#define SRSLTE_ZC_SEQUENCE_H

#include "srslte/config.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Generates ZC sequences given the required parameters used in the TS 36 series (LTE)
 *
 * @remark Implemented as defined in TS 36.211 section 5.5.1 Generation of the reference signal sequence
 *
 * @param[in] u Group number {0,1,...29}
 * @param[in] v Base sequence
 * @param[in] alpha Phase shift
 * @param[in] nof_prb Number of PRB
 * @param[out] sequence Output sequence
 * @return SRSLTE_SUCCESS if the generation is successful, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_zc_sequence_generate_lte(uint32_t u, uint32_t v, float alpha, uint32_t nof_prb, cf_t* sequence);

/**
 * @brief Generates ZC sequences given the required parameters used in the TS 38 series (NR)
 *
 * @remark Implemented as defined in TS 38.211 section 5.2.2 Low-PAPR sequence generation

 * @param u Group number {0,1,...29}
 * @param v base sequence
 * @param alpha Phase shift
 * @param m Number of PRB
 * @param delta Delta parameter described in specification
 * @param sequence Output sequence
 * @return SRSLTE_SUCCESS if the generation is successful, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int
srslte_zc_sequence_generate_nr(uint32_t u, uint32_t v, float alpha, uint32_t m, uint32_t delta, cf_t* sequence);

#endif // SRSLTE_ZC_SEQUENCE_H
