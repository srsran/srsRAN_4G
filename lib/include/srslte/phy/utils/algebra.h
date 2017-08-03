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

#ifndef SRSLTE_ALGEBRA_H
#define SRSLTE_ALGEBRA_H

#include "srslte/config.h"

#ifdef LV_HAVE_SSE

#define _MM_MULJ_PS(X) _mm_permute_ps(_MM_CONJ_PS(X), 0b10110001)
#define _MM_CONJ_PS(X) (_mm_xor_ps(X, (__m128){0.0f, -0.0f, 0.0f, -0.0f}))
#define _MM_PROD_PS(a, b) _mm_addsub_ps(_mm_mul_ps(a,_mm_moveldup_ps(b)),_mm_mul_ps(\
                            _mm_shuffle_ps(a,a,0xB1),_mm_movehdup_ps(b)))

SRSLTE_API void srslte_algebra_2x2_zf_sse(__m128 y0,
                                          __m128 y1,
                                          __m128 h00,
                                          __m128 h01,
                                          __m128 h10,
                                          __m128 h11,
                                          __m128 *x0,
                                          __m128 *x1,
                                          float norm);

#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX

#define _MM256_MULJ_PS(X) _mm256_permute_ps(_MM256_CONJ_PS(X), 0b10110001)
#define _MM256_CONJ_PS(X) (_mm256_xor_ps(X, (__m256){0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f}))
#define _MM256_PROD_PS(a, b) _mm256_addsub_ps(_mm256_mul_ps(a,_mm256_moveldup_ps(b)),\
                              _mm256_mul_ps(_mm256_shuffle_ps(a,a,0xB1),_mm256_movehdup_ps(b)))

SRSLTE_API void srslte_algebra_2x2_zf_avx(__m256 y0,
                                          __m256 y1,
                                          __m256 h00,
                                          __m256 h01,
                                          __m256 h10,
                                          __m256 h11,
                                          __m256 *x0,
                                          __m256 *x1,
                                          float norm);

#endif /* LV_HAVE_AVX */

#endif //SRSLTE_ALGEBRA_H
