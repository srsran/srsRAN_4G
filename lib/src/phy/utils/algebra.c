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


#include <immintrin.h>

#include "srslte/phy/utils/algebra.h"

#ifdef LV_HAVE_SSE

inline void srslte_algebra_2x2_zf_sse(__m128 y0, __m128 y1, __m128 h00, __m128 h01, __m128 h10, __m128 h11,
                                      __m128 *x0, __m128 *x1, float norm) {
  __m128 detmult1 = _MM_PROD_PS(h00, h11);
  __m128 detmult2 = _MM_PROD_PS(h01, h10);

  __m128 det = _mm_sub_ps(detmult1, detmult2);
  __m128 detconj = _MM_CONJ_PS(det);
  __m128 detabs2 = _MM_PROD_PS(det, detconj);
  __m128 detabs2rec = _mm_rcp_ps(detabs2);
  detabs2rec = _mm_moveldup_ps(detabs2rec);
  __m128 detrec = _mm_mul_ps(_mm_mul_ps(detconj, detabs2rec),
                                (__m128) {norm, norm, norm, norm});

  *x0 = _MM_PROD_PS(_mm_sub_ps(_MM_PROD_PS(h11, y0), _MM_PROD_PS(h01, y1)), detrec);
  *x1 = _MM_PROD_PS(_mm_sub_ps(_MM_PROD_PS(h00, y1), _MM_PROD_PS(h10, y0)), detrec);
}

#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX

inline void srslte_algebra_2x2_zf_avx(__m256 y0, __m256 y1, __m256 h00, __m256 h01, __m256 h10, __m256 h11,
                                 __m256 *x0, __m256 *x1, float norm) {
  __m256 detmult1 = _MM256_PROD_PS(h00, h11);
  __m256 detmult2 = _MM256_PROD_PS(h01, h10);

  __m256 det = _mm256_sub_ps(detmult1, detmult2);
  __m256 detconj = _MM256_CONJ_PS(det);
  __m256 sqdet = _mm256_mul_ps(det, det);
  __m256 detabs2 = _mm256_add_ps(_mm256_movehdup_ps(sqdet), _mm256_moveldup_ps(sqdet));
  __m256 detabs2rec = _mm256_rcp_ps(detabs2);
  __m256 detrec = _mm256_mul_ps(_mm256_mul_ps(detconj, detabs2rec),
                                (__m256) {norm, norm, norm, norm, norm, norm, norm, norm});

  *x0 = _MM256_PROD_PS(_mm256_sub_ps(_MM256_PROD_PS(h11, y0), _MM256_PROD_PS(h01, y1)), detrec);
  *x1 = _MM256_PROD_PS(_mm256_sub_ps(_MM256_PROD_PS(h00, y1), _MM256_PROD_PS(h10, y0)), detrec);
}

#endif /* LV_HAVE_AVX */
