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

#ifndef SRSLTE_CHEST_COMMON_H
#define SRSLTE_CHEST_COMMON_H

#include "srslte/config.h"
#include <stdint.h>

#define SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN 64

typedef enum SRSLTE_API {
  SRSLTE_CHEST_FILTER_GAUSS = 0,
  SRSLTE_CHEST_FILTER_TRIANGLE,
  SRSLTE_CHEST_FILTER_NONE
} srslte_chest_filter_t;

SRSLTE_API void srslte_chest_average_pilots(cf_t*    input,
                                            cf_t*    output,
                                            float*   filter,
                                            uint32_t nof_ref,
                                            uint32_t nof_symbols,
                                            uint32_t filter_len);

SRSLTE_API uint32_t srslte_chest_set_smooth_filter3_coeff(float* smooth_filter, float w);

SRSLTE_API float srslte_chest_estimate_noise_pilots(cf_t* noisy, cf_t* noiseless, cf_t* noise_vec, uint32_t nof_pilots);

SRSLTE_API uint32_t srslte_chest_set_triangle_filter(float* fil, int filter_len);

SRSLTE_API uint32_t srslte_chest_set_smooth_filter_gauss(float* filter, uint32_t order, float std_dev);

#endif // SRSLTE_CHEST_COMMON_H
