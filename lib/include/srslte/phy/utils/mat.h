/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_MAT_H
#define SRSLTE_MAT_H

#include "srslte/config.h"
#include "srslte/phy/utils/simd.h"

/* Generic implementation for complex reciprocal */
SRSLTE_API cf_t srslte_mat_cf_recip_gen(cf_t a);

/* Generic implementation for 2x2 determinant */
SRSLTE_API cf_t srslte_mat_2x2_det_gen(cf_t a00, cf_t a01, cf_t a10, cf_t a11);

/* Generic implementation for 2x2 Matrix Inversion */
SRSLTE_API void srslte_mat_2x2_inv_gen(cf_t a00, cf_t a01, cf_t a10, cf_t a11,
                                       cf_t *r00, cf_t *r01, cf_t *r10, cf_t *r11);

/* Generic implementation for Zero Forcing (ZF) solver */
SRSLTE_API void srslte_mat_2x2_zf_gen(cf_t y0, cf_t y1,
                                      cf_t h00, cf_t h01, cf_t h10, cf_t h11,
                                      cf_t *x0, cf_t *x1,
                                      float norm);

/* Generic implementation for Minimum Mean Squared Error (MMSE) solver */
SRSLTE_API void srslte_mat_2x2_mmse_gen(cf_t y0, cf_t y1,
                                        cf_t h00, cf_t h01, cf_t h10, cf_t h11,
                                        cf_t *x0, cf_t *x1,
                                        float noise_estimate,
                                        float norm);

SRSLTE_API float srslte_mat_2x2_cn(cf_t h00,
                                   cf_t h01,
                                   cf_t h10,
                                   cf_t h11);


#ifdef LV_HAVE_SSE

/* SSE implementation for complex reciprocal */
SRSLTE_API __m128 srslte_mat_cf_recip_sse(__m128 a);

/* SSE implementation for 2x2 determinant */
SRSLTE_API __m128 srslte_mat_2x2_det_sse(__m128 a00, __m128 a01, __m128 a10, __m128 a11);

/* SSE implementation for Zero Forcing (ZF) solver */
SRSLTE_API void srslte_mat_2x2_zf_sse(__m128 y0, __m128 y1,
                                      __m128 h00, __m128 h01, __m128 h10, __m128 h11,
                                      __m128 *x0, __m128 *x1,
                                      float norm);

/* SSE implementation for Minimum Mean Squared Error (MMSE) solver */
SRSLTE_API void srslte_mat_2x2_mmse_sse(__m128 y0, __m128 y1,
                                        __m128 h00, __m128 h01, __m128 h10, __m128 h11,
                                        __m128 *x0, __m128 *x1,
                                        float noise_estimate, float norm);

#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX

/* AVX implementation for complex reciprocal */
SRSLTE_API __m256 srslte_mat_cf_recip_avx(__m256 a);

/* AVX implementation for 2x2 determinant */
SRSLTE_API __m256 srslte_mat_2x2_det_avx(__m256 a00, __m256 a01, __m256 a10, __m256 a11);

/* AVX implementation for Zero Forcing (ZF) solver */
SRSLTE_API void srslte_mat_2x2_zf_avx(__m256 y0, __m256 y1,
                                      __m256 h00, __m256 h01, __m256 h10, __m256 h11,
                                      __m256 *x0, __m256 *x1,
                                      float norm);

/* AVX implementation for Minimum Mean Squared Error (MMSE) solver */
SRSLTE_API void srslte_mat_2x2_mmse_avx(__m256 y0, __m256 y1,
                                        __m256 h00, __m256 h01, __m256 h10, __m256 h11,
                                        __m256 *x0, __m256 *x1,
                                        float noise_estimate, float norm);

#endif /* LV_HAVE_AVX */

#endif // SRSLTE_MAT_H 
