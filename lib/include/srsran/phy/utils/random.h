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

SRSRAN_API void srsran_random_free(srsran_random_t q);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_RANDOM_H
