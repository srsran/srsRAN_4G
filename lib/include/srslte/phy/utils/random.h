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

#ifndef SRSLTE_RANDOM_H
#define SRSLTE_RANDOM_H

#include "srslte/config.h"

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* srslte_random_t;

SRSLTE_API srslte_random_t srslte_random_init(uint32_t seed);

SRSLTE_API int srslte_random_uniform_int_dist(srslte_random_t q, int min, int max);

SRSLTE_API float srslte_random_uniform_real_dist(srslte_random_t q, float min, float max);

SRSLTE_API cf_t srslte_random_uniform_complex_dist(srslte_random_t q, float min, float max);

SRSLTE_API void
srslte_random_uniform_complex_dist_vector(srslte_random_t q, cf_t* vector, uint32_t nsamples, float min, float max);

SRSLTE_API float srslte_random_gauss_dist(srslte_random_t q, float std_dev);

SRSLTE_API bool srslte_random_bool(srslte_random_t q, float prob_true);

SRSLTE_API void srslte_random_free(srslte_random_t q);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_RANDOM_H
