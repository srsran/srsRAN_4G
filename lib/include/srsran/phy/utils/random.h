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

#ifndef SRSRAN_RANDOM_H
#define SRSRAN_RANDOM_H

#include "srsran/config.h"

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* srsran_random_t;

SRSRAN_API srsran_random_t srsran_random_init(uint32_t seed);

SRSRAN_API int srsran_random_uniform_int_dist(srsran_random_t q, int min, int max);

SRSRAN_API float srsran_random_uniform_real_dist(srsran_random_t q, float min, float max);

SRSRAN_API cf_t srsran_random_uniform_complex_dist(srsran_random_t q, float min, float max);

SRSRAN_API void
srsran_random_uniform_complex_dist_vector(srsran_random_t q, cf_t* vector, uint32_t nsamples, float min, float max);

SRSRAN_API float srsran_random_gauss_dist(srsran_random_t q, float std_dev);

SRSRAN_API bool srsran_random_bool(srsran_random_t q, float prob_true);

SRSRAN_API void srsran_random_byte_vector(srsran_random_t q, uint8_t* c, uint32_t nsamples);

SRSRAN_API void srsran_random_bit_vector(srsran_random_t q, uint8_t* c, uint32_t nsamples);

SRSRAN_API void srsran_random_free(srsran_random_t q);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_RANDOM_H
