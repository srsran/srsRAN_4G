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

SRSLTE_API void srslte_mat_2x2_mmse_csi_gen(cf_t y0, cf_t y1,
                                            cf_t h00, cf_t h01, cf_t h10, cf_t h11,
                                            cf_t *x0, cf_t *x1, float *csi0, float *csi1,
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

#if SRSLTE_SIMD_CF_SIZE != 0

/* Generic SIMD implementation for 2x2 determinant */
static inline simd_cf_t srslte_mat_2x2_det_simd(simd_cf_t a00, simd_cf_t a01, simd_cf_t a10, simd_cf_t a11) {
  return srslte_simd_cf_sub(srslte_simd_cf_prod(a00, a11), srslte_simd_cf_prod(a01, a10));
}

/* Generic SIMD implementation for Zero Forcing (ZF) solver */
static inline void srslte_mat_2x2_zf_csi_simd(simd_cf_t y0,
                                              simd_cf_t y1,
                                              simd_cf_t h00,
                                              simd_cf_t h01,
                                              simd_cf_t h10,
                                              simd_cf_t h11,
                                              simd_cf_t *x0,
                                              simd_cf_t *x1,
                                              simd_f_t *csi0,
                                              simd_f_t *csi1,
                                              float norm) {
  simd_cf_t det = srslte_mat_2x2_det_simd(h00, h01, h10, h11);
  simd_cf_t detrec = srslte_simd_cf_mul(srslte_simd_cf_rcp(det), srslte_simd_f_set1(norm));

  *x0 = srslte_simd_cf_prod(srslte_simd_cf_sub(srslte_simd_cf_prod(h11, y0), srslte_simd_cf_prod(h01, y1)), detrec);
  *x1 = srslte_simd_cf_prod(srslte_simd_cf_sub(srslte_simd_cf_prod(h00, y1), srslte_simd_cf_prod(h10, y0)), detrec);

  *csi0 = srslte_simd_f_set1(1.0f);
  *csi1 = srslte_simd_f_set1(1.0f);
}

static inline void srslte_mat_2x2_zf_simd(simd_cf_t y0,
                                          simd_cf_t y1,
                                          simd_cf_t h00,
                                          simd_cf_t h01,
                                          simd_cf_t h10,
                                          simd_cf_t h11,
                                          simd_cf_t *x0,
                                          simd_cf_t *x1,
                                          float norm) {
  simd_f_t csi1, csi2;
  srslte_mat_2x2_zf_csi_simd(y0, y1, h00, h01, h10, h11, x0, x1, &csi1, &csi2, norm);
}

/* Generic SIMD implementation for Minimum Mean Squared Error (MMSE) solver */
static inline void srslte_mat_2x2_mmse_csi_simd(simd_cf_t y0,
                                                simd_cf_t y1,
                                                simd_cf_t h00,
                                                simd_cf_t h01,
                                                simd_cf_t h10,
                                                simd_cf_t h11,
                                                simd_cf_t *x0,
                                                simd_cf_t *x1,
                                                simd_f_t *csi0,
                                                simd_f_t *csi1,
                                                float noise_estimate,
                                                float norm) {
  simd_cf_t _noise_estimate;
  simd_f_t _norm = srslte_simd_f_set1(norm);

  _noise_estimate.re = srslte_simd_f_set1(noise_estimate);
  _noise_estimate.im = srslte_simd_f_zero();

  /* 1. A = H' x H + No*/
  simd_cf_t a00 =
      srslte_simd_cf_add(srslte_simd_cf_add(srslte_simd_cf_conjprod(h00, h00), srslte_simd_cf_conjprod(h10, h10)),
                         _noise_estimate);
  simd_cf_t a01 = srslte_simd_cf_add(srslte_simd_cf_conjprod(h01, h00), srslte_simd_cf_conjprod(h11, h10));
  simd_cf_t a10 = srslte_simd_cf_add(srslte_simd_cf_conjprod(h00, h01), srslte_simd_cf_conjprod(h10, h11));
  simd_cf_t a11 =
      srslte_simd_cf_add(srslte_simd_cf_add(srslte_simd_cf_conjprod(h01, h01), srslte_simd_cf_conjprod(h11, h11)),
                         _noise_estimate);
  simd_cf_t a_det_rcp = srslte_simd_cf_rcp(srslte_mat_2x2_det_simd(a00, a01, a10, a11));

  /* 2. B = inv(H' x H + No) = inv(A) */
  simd_cf_t _norm2 = srslte_simd_cf_mul(a_det_rcp, _norm);
  simd_cf_t b00 = srslte_simd_cf_prod(a11, _norm2);
  simd_cf_t b01 = srslte_simd_cf_prod(srslte_simd_cf_neg(a01), _norm2);
  simd_cf_t b10 = srslte_simd_cf_prod(srslte_simd_cf_neg(a10), _norm2);
  simd_cf_t b11 = srslte_simd_cf_prod(a00, _norm2);


  /* 3. W = inv(H' x H + No) x H' = B x H' */
  simd_cf_t w00 = srslte_simd_cf_add(srslte_simd_cf_conjprod(b00, h00), srslte_simd_cf_conjprod(b01, h01));
  simd_cf_t w01 = srslte_simd_cf_add(srslte_simd_cf_conjprod(b00, h10), srslte_simd_cf_conjprod(b01, h11));
  simd_cf_t w10 = srslte_simd_cf_add(srslte_simd_cf_conjprod(b10, h00), srslte_simd_cf_conjprod(b11, h01));
  simd_cf_t w11 = srslte_simd_cf_add(srslte_simd_cf_conjprod(b10, h10), srslte_simd_cf_conjprod(b11, h11));

  /* 4. X = W x Y */
  *x0 = srslte_simd_cf_add(srslte_simd_cf_prod(y0, w00), srslte_simd_cf_prod(y1, w01));
  *x1 = srslte_simd_cf_add(srslte_simd_cf_prod(y0, w10), srslte_simd_cf_prod(y1, w11));

  /* 5. Extract CSI */
  *csi0 = srslte_simd_f_rcp(srslte_simd_cf_re(b00));
  *csi1 = srslte_simd_f_rcp(srslte_simd_cf_re(b11));
}

static inline void srslte_mat_2x2_mmse_simd(simd_cf_t y0,
                                            simd_cf_t y1,
                                            simd_cf_t h00,
                                            simd_cf_t h01,
                                            simd_cf_t h10,
                                            simd_cf_t h11,
                                            simd_cf_t *x0,
                                            simd_cf_t *x1,
                                            float noise_estimate,
                                            float norm) {
  simd_f_t csi0, csi1;
  srslte_mat_2x2_mmse_csi_simd(y0, y1, h00, h01, h10, h11, x0, x1, &csi0, &csi1, noise_estimate, norm);
}

#endif /* SRSLTE_SIMD_CF_SIZE != 0 */
#endif /* SRSLTE_MAT_H */
