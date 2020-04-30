/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <complex.h>
#include <math.h>
#include <memory.h>
#include <srslte/phy/utils/vector.h>
#include <srslte/phy/utils/vector_simd.h>
#include <stdlib.h>

#include "srslte/phy/utils/mat.h"

/* Generic implementation for complex reciprocal */
inline cf_t srslte_mat_cf_recip_gen(cf_t a)
{
  return conjf(a) / (crealf(a) * crealf(a) + cimagf(a) * cimagf(a));
}

/* Generic implementation for 2x2 determinant */
inline cf_t srslte_mat_2x2_det_gen(cf_t a00, cf_t a01, cf_t a10, cf_t a11)
{
  return a00 * a11 - a01 * a10;
}

/* 2x2 Matrix inversion, generic implementation */
inline void srslte_mat_2x2_inv_gen(cf_t a00, cf_t a01, cf_t a10, cf_t a11, cf_t* r00, cf_t* r01, cf_t* r10, cf_t* r11)
{
  cf_t div = srslte_mat_cf_recip_gen(srslte_mat_2x2_det_gen(a00, a01, a10, a11));
  *r00     = a11 * div;
  *r01     = -a01 * div;
  *r10     = -a10 * div;
  *r11     = a00 * div;
}

/* Generic implementation for Zero Forcing (ZF) solver */
inline void
srslte_mat_2x2_zf_gen(cf_t y0, cf_t y1, cf_t h00, cf_t h01, cf_t h10, cf_t h11, cf_t* x0, cf_t* x1, float norm)
{
  cf_t _norm = srslte_mat_cf_recip_gen(srslte_mat_2x2_det_gen(h00, h01, h10, h11)) * norm;
  *x0        = (y0 * h11 - h01 * y1) * _norm;
  *x1        = (y1 * h00 - h10 * y0) * _norm;
}

/* Generic implementation for Minimum Mean Squared Error (MMSE) solver */
inline void srslte_mat_2x2_mmse_csi_gen(cf_t   y0,
                                        cf_t   y1,
                                        cf_t   h00,
                                        cf_t   h01,
                                        cf_t   h10,
                                        cf_t   h11,
                                        cf_t*  x0,
                                        cf_t*  x1,
                                        float* csi0,
                                        float* csi1,
                                        float  noise_estimate,
                                        float  norm)
{
  /* Create conjugated matrix */
  cf_t _h00 = conjf(h00);
  cf_t _h01 = conjf(h01);
  cf_t _h10 = conjf(h10);
  cf_t _h11 = conjf(h11);

  /* 1. A = H' x H + No*/
  cf_t a00       = _h00 * h00 + _h10 * h10 + noise_estimate;
  cf_t a01       = _h00 * h01 + _h10 * h11;
  cf_t a10       = _h01 * h00 + _h11 * h10;
  cf_t a11       = _h01 * h01 + _h11 * h11 + noise_estimate;
  cf_t a_det_rcp = srslte_mat_cf_recip_gen(srslte_mat_2x2_det_gen(a00, a01, a10, a11));

  /* 2. B = inv(H' x H + No) = inv(A) */
  cf_t _norm = norm * a_det_rcp;
  cf_t b00   = a11 * _norm;
  cf_t b01   = -a01 * _norm;
  cf_t b10   = -a10 * _norm;
  cf_t b11   = a00 * _norm;

  /* 3. W = inv(H' x H + No) x H' = B x H' */
  cf_t w00 = b00 * _h00 + b01 * _h01;
  cf_t w01 = b00 * _h10 + b01 * _h11;
  cf_t w10 = b10 * _h00 + b11 * _h01;
  cf_t w11 = b10 * _h10 + b11 * _h11;

  /* 4. X = W x Y */
  *x0 = (y0 * w00 + y1 * w01);
  *x1 = (y0 * w10 + y1 * w11);

  /* 5. Set CSI */
  *csi0 = 1.0f / crealf(b00);
  *csi1 = 1.0f / crealf(b11);
}

/* Generic implementation for Minimum Mean Squared Error (MMSE) solver */
void srslte_mat_2x2_mmse_gen(cf_t  y0,
                             cf_t  y1,
                             cf_t  h00,
                             cf_t  h01,
                             cf_t  h10,
                             cf_t  h11,
                             cf_t* x0,
                             cf_t* x1,
                             float noise_estimate,
                             float norm)
{
  float csi0, csi1;
  srslte_mat_2x2_mmse_csi_gen(y0, y1, h00, h01, h10, h11, x0, x1, &csi0, &csi1, noise_estimate, norm);
}

inline float srslte_mat_2x2_cn(cf_t h00, cf_t h01, cf_t h10, cf_t h11)
{
  // 1. A = H * H' (A = A')
  float a00 =
      crealf(h00) * crealf(h00) + crealf(h01) * crealf(h01) + cimagf(h00) * cimagf(h00) + cimagf(h01) * cimagf(h01);
  cf_t  a01 = h00 * conjf(h10) + h01 * conjf(h11);
  float a11 =
      crealf(h10) * crealf(h10) + crealf(h11) * crealf(h11) + cimagf(h10) * cimagf(h10) + cimagf(h11) * cimagf(h11);

  // 2. |H * H' - {λ0, λ1}| = 0 -> aλ² + bλ + c = 0
  float b = a00 + a11;
  float c = a00 * a11 - (crealf(a01) * crealf(a01) + cimagf(a01) * cimagf(a01));

  // 3. λ = (-b ± sqrt(b² - 4 * c))/2
  float sqr  = sqrtf(b * b - 4.0f * c);
  float xmax = b + sqr;
  float xmin = b - sqr;

  // 4. Bound xmin and xmax
  if (!isnormal(xmin) || xmin < 1e-9) {
    xmin = 1e-9;
  }
  if (!isnormal(xmax) || xmax > 1e+9) {
    xmax = 1e+9;
  }

  // 5. κ = sqrt(λ_max / λ_min)
  return 10.0f * log10f(xmax / xmin);
}

#ifdef LV_HAVE_SSE
#include <smmintrin.h>

/* SSE implementation for complex reciprocal */
inline __m128 srslte_mat_cf_recip_sse(__m128 a)
{
  __m128 conj  = _MM_CONJ_PS(a);
  __m128 sqabs = _mm_mul_ps(a, a);
  sqabs        = _mm_add_ps(_mm_movehdup_ps(sqabs), _mm_moveldup_ps(sqabs));

  __m128 recp = _mm_rcp_ps(sqabs);

  return _mm_mul_ps(recp, conj);
}

/* SSE implementation for 2x2 determinant */
inline __m128 srslte_mat_2x2_det_sse(__m128 a00, __m128 a01, __m128 a10, __m128 a11)
{
  return _mm_sub_ps(_MM_PROD_PS(a00, a11), _MM_PROD_PS(a01, a10));
}

/* SSE implementation for Zero Forcing (ZF) solver */
inline void srslte_mat_2x2_zf_sse(__m128  y0,
                                  __m128  y1,
                                  __m128  h00,
                                  __m128  h01,
                                  __m128  h10,
                                  __m128  h11,
                                  __m128* x0,
                                  __m128* x1,
                                  float   norm)
{
  __m128 detmult1 = _MM_PROD_PS(h00, h11);
  __m128 detmult2 = _MM_PROD_PS(h01, h10);

  __m128 det    = _mm_sub_ps(detmult1, detmult2);
  __m128 detrec = _mm_mul_ps(srslte_mat_cf_recip_sse(det), _mm_set1_ps(norm));

  *x0 = _MM_PROD_PS(_mm_sub_ps(_MM_PROD_PS(h11, y0), _MM_PROD_PS(h01, y1)), detrec);
  *x1 = _MM_PROD_PS(_mm_sub_ps(_MM_PROD_PS(h00, y1), _MM_PROD_PS(h10, y0)), detrec);
}

/* SSE implementation for Minimum Mean Squared Error (MMSE) solver */
inline void srslte_mat_2x2_mmse_sse(__m128  y0,
                                    __m128  y1,
                                    __m128  h00,
                                    __m128  h01,
                                    __m128  h10,
                                    __m128  h11,
                                    __m128* x0,
                                    __m128* x1,
                                    float   noise_estimate,
                                    float   norm)
{
  __m128 _noise_estimate = _mm_set_ps(0.0f, noise_estimate, 0.0f, noise_estimate);
  __m128 _norm           = _mm_set1_ps(norm);

  /* Create conjugated matrix */
  __m128 _h00 = _MM_CONJ_PS(h00);
  __m128 _h01 = _MM_CONJ_PS(h01);
  __m128 _h10 = _MM_CONJ_PS(h10);
  __m128 _h11 = _MM_CONJ_PS(h11);

  /* 1. A = H' x H + No*/
  __m128 a00 = _mm_add_ps(_mm_add_ps(_MM_SQMOD_PS(h00), _MM_SQMOD_PS(h10)), _noise_estimate);
  __m128 a01 = _mm_add_ps(_MM_PROD_PS(_h00, h01), _MM_PROD_PS(_h10, h11));
  __m128 a10 = _mm_add_ps(_MM_PROD_PS(_h01, h00), _MM_PROD_PS(_h11, h10));
  __m128 a11 = _mm_add_ps(_mm_add_ps(_MM_SQMOD_PS(h01), _MM_SQMOD_PS(h11)), _noise_estimate);

  /* 2. B = inv(H' x H + No) = inv(A) */
  __m128 b00 = a11;
  __m128 b01 = _mm_xor_ps(a01, _mm_set1_ps(-0.0f));
  __m128 b10 = _mm_xor_ps(a10, _mm_set1_ps(-0.0f));
  __m128 b11 = a00;
  _norm      = _mm_mul_ps(_norm, srslte_mat_cf_recip_sse(srslte_mat_2x2_det_sse(a00, a01, a10, a11)));

  /* 3. W = inv(H' x H + No) x H' = B x H' */
  __m128 w00 = _mm_add_ps(_MM_PROD_PS(b00, _h00), _MM_PROD_PS(b01, _h01));
  __m128 w01 = _mm_add_ps(_MM_PROD_PS(b00, _h10), _MM_PROD_PS(b01, _h11));
  __m128 w10 = _mm_add_ps(_MM_PROD_PS(b10, _h00), _MM_PROD_PS(b11, _h01));
  __m128 w11 = _mm_add_ps(_MM_PROD_PS(b10, _h10), _MM_PROD_PS(b11, _h11));

  /* 4. X = W x Y */
  *x0 = _MM_PROD_PS(_mm_add_ps(_MM_PROD_PS(y0, w00), _MM_PROD_PS(y1, w01)), _norm);
  *x1 = _MM_PROD_PS(_mm_add_ps(_MM_PROD_PS(y0, w10), _MM_PROD_PS(y1, w11)), _norm);
}

#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
#include <srslte/phy/utils/vector.h>

/* AVX implementation for complex reciprocal */
inline __m256 srslte_mat_cf_recip_avx(__m256 a)
{
  __m256 conj  = _MM256_CONJ_PS(a);
  __m256 sqabs = _mm256_mul_ps(a, a);
  sqabs        = _mm256_add_ps(_mm256_movehdup_ps(sqabs), _mm256_moveldup_ps(sqabs));

  __m256 recp = _mm256_rcp_ps(sqabs);

  return _mm256_mul_ps(recp, conj);
}

/* AVX implementation for 2x2 determinant */
inline __m256 srslte_mat_2x2_det_avx(__m256 a00, __m256 a01, __m256 a10, __m256 a11)
{
#ifdef LV_HAVE_FMA
  return _MM256_PROD_SUB_PS(a00, a11, _MM256_PROD_PS(a01, a10));
#else
  return _mm256_sub_ps(_MM256_PROD_PS(a00, a11), _MM256_PROD_PS(a01, a10));
#endif /* LV_HAVE_FMA */
}

/* AVX implementation for Zero Forcing (ZF) solver */
inline void srslte_mat_2x2_zf_avx(__m256  y0,
                                  __m256  y1,
                                  __m256  h00,
                                  __m256  h01,
                                  __m256  h10,
                                  __m256  h11,
                                  __m256* x0,
                                  __m256* x1,
                                  float   norm)
{

  __m256 det    = srslte_mat_2x2_det_avx(h00, h01, h10, h11);
  __m256 detrec = _mm256_mul_ps(srslte_mat_cf_recip_avx(det), _mm256_set1_ps(norm));

#ifdef LV_HAVE_FMA
  *x0 = _MM256_PROD_PS(_MM256_PROD_SUB_PS(h11, y0, _MM256_PROD_PS(h01, y1)), detrec);
  *x1 = _MM256_PROD_PS(_MM256_PROD_SUB_PS(h00, y1, _MM256_PROD_PS(h10, y0)), detrec);
#else
  *x0        = _MM256_PROD_PS(_mm256_sub_ps(_MM256_PROD_PS(h11, y0), _MM256_PROD_PS(h01, y1)), detrec);
  *x1        = _MM256_PROD_PS(_mm256_sub_ps(_MM256_PROD_PS(h00, y1), _MM256_PROD_PS(h10, y0)), detrec);
#endif /* LV_HAVE_FMA */
}

/* AVX implementation for Minimum Mean Squared Error (MMSE) solver */
inline void srslte_mat_2x2_mmse_avx(__m256  y0,
                                    __m256  y1,
                                    __m256  h00,
                                    __m256  h01,
                                    __m256  h10,
                                    __m256  h11,
                                    __m256* x0,
                                    __m256* x1,
                                    float   noise_estimate,
                                    float   norm)
{
  __m256 _noise_estimate =
      _mm256_set_ps(0.0f, noise_estimate, 0.0f, noise_estimate, 0.0f, noise_estimate, 0.0f, noise_estimate);
  __m256 _norm = _mm256_set1_ps(norm);

  /* Create conjugated matrix */
  __m256 _h00 = _MM256_CONJ_PS(h00);
  __m256 _h01 = _MM256_CONJ_PS(h01);
  __m256 _h10 = _MM256_CONJ_PS(h10);
  __m256 _h11 = _MM256_CONJ_PS(h11);

  /* 1. A = H' x H + No*/
#ifdef LV_HAVE_FMA
  __m256 a00 = _MM256_SQMOD_ADD_PS(h00, h10, _noise_estimate);
  __m256 a01 = _MM256_PROD_ADD_PS(_h00, h01, _MM256_PROD_PS(_h10, h11));
  __m256 a10 = _MM256_PROD_ADD_PS(_h01, h00, _MM256_PROD_PS(_h11, h10));
  __m256 a11 = _MM256_SQMOD_ADD_PS(h01, h11, _noise_estimate);
#else
  __m256 a00 = _mm256_add_ps(_MM256_SQMOD_PS(h00, h10), _noise_estimate);
  __m256 a01 = _mm256_add_ps(_MM256_PROD_PS(_h00, h01), _MM256_PROD_PS(_h10, h11));
  __m256 a10 = _mm256_add_ps(_MM256_PROD_PS(_h01, h00), _MM256_PROD_PS(_h11, h10));
  __m256 a11 = _mm256_add_ps(_MM256_SQMOD_PS(h01, h11), _noise_estimate);
#endif /* LV_HAVE_FMA */

  /* 2. B = inv(H' x H + No) = inv(A) */
  __m256 b00 = a11;
  __m256 b01 = _mm256_xor_ps(a01, _mm256_set1_ps(-0.0f));
  __m256 b10 = _mm256_xor_ps(a10, _mm256_set1_ps(-0.0f));
  __m256 b11 = a00;
  _norm      = _mm256_mul_ps(_norm, srslte_mat_cf_recip_avx(srslte_mat_2x2_det_avx(a00, a01, a10, a11)));

  /* 3. W = inv(H' x H + No) x H' = B x H' */
#ifdef LV_HAVE_FMA
  __m256 w00 = _MM256_PROD_ADD_PS(b00, _h00, _MM256_PROD_PS(b01, _h01));
  __m256 w01 = _MM256_PROD_ADD_PS(b00, _h10, _MM256_PROD_PS(b01, _h11));
  __m256 w10 = _MM256_PROD_ADD_PS(b10, _h00, _MM256_PROD_PS(b11, _h01));
  __m256 w11 = _MM256_PROD_ADD_PS(b10, _h10, _MM256_PROD_PS(b11, _h11));
#else
  __m256 w00 = _mm256_add_ps(_MM256_PROD_PS(b00, _h00), _MM256_PROD_PS(b01, _h01));
  __m256 w01 = _mm256_add_ps(_MM256_PROD_PS(b00, _h10), _MM256_PROD_PS(b01, _h11));
  __m256 w10 = _mm256_add_ps(_MM256_PROD_PS(b10, _h00), _MM256_PROD_PS(b11, _h01));
  __m256 w11 = _mm256_add_ps(_MM256_PROD_PS(b10, _h10), _MM256_PROD_PS(b11, _h11));
#endif /* LV_HAVE_FMA */

  /* 4. X = W x Y */
#ifdef LV_HAVE_FMA
  *x0 = _MM256_PROD_PS(_MM256_PROD_ADD_PS(y0, w00, _MM256_PROD_PS(y1, w01)), _norm);
  *x1 = _MM256_PROD_PS(_MM256_PROD_ADD_PS(y0, w10, _MM256_PROD_PS(y1, w11)), _norm);
#else
  *x0        = _MM256_PROD_PS(_mm256_add_ps(_MM256_PROD_PS(y0, w00), _MM256_PROD_PS(y1, w01)), _norm);
  *x1        = _MM256_PROD_PS(_mm256_add_ps(_MM256_PROD_PS(y0, w10), _MM256_PROD_PS(y1, w11)), _norm);
#endif /* LV_HAVE_FMA */
}

#endif /* LV_HAVE_AVX */

int srslte_matrix_NxN_inv_init(srslte_matrix_NxN_inv_t* q, uint32_t N)
{
  int ret = SRSLTE_SUCCESS;

  if (q && N) {
    // Set all to zero
    bzero(q, sizeof(srslte_matrix_NxN_inv_t));

    q->N = N;

    q->row_buffer = srslte_vec_cf_malloc(N * 2);
    if (!q->row_buffer) {
      perror("malloc");
      ret = SRSLTE_ERROR;
    }

    if (!ret) {
      q->matrix = srslte_vec_cf_malloc(N * N * 2);
      if (!q->matrix) {
        perror("malloc");
        ret = SRSLTE_ERROR;
      }
    }
  }

  return ret;
}

static inline void srslte_vec_sc_prod_ccc_simd_inline(const cf_t* x, const cf_t h, cf_t* z, const int len)
{
  int i = 0;

#if SRSLTE_SIMD_F_SIZE

#ifdef HAVE_NEON
  i = srslte_vec_sc_prod_ccc_simd2(x, h, z, len);
#else
  const simd_f_t hre = srslte_simd_f_set1(__real__ h);
  const simd_f_t him = srslte_simd_f_set1(__imag__ h);

  for (; i < len - SRSLTE_SIMD_F_SIZE / 2 + 1; i += SRSLTE_SIMD_F_SIZE / 2) {
    simd_f_t temp = srslte_simd_f_load((float*)&x[i]);

    simd_f_t m1 = srslte_simd_f_mul(hre, temp);
    simd_f_t sw = srslte_simd_f_swap(temp);
    simd_f_t m2 = srslte_simd_f_mul(him, sw);
    simd_f_t r  = srslte_simd_f_addsub(m1, m2);
    srslte_simd_f_store((float*)&z[i], r);
  }

#endif
#endif
  for (; i < len; i++) {
    __real__ z[i] = __real__ x[i] * __real__ h - __imag__ x[i] * __imag__ h;
    __imag__ z[i] = __real__ x[i] * __imag__ h + __imag__ x[i] * __real__ h;
  }
}

static inline void srslte_vec_sub_fff_simd_inline(const float* x, const float* y, float* z, const int len)
{
  int i = 0;

#if SRSLTE_SIMD_F_SIZE

  for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
    simd_f_t a = srslte_simd_f_loadu(&x[i]);
    simd_f_t b = srslte_simd_f_loadu(&y[i]);

    simd_f_t r = srslte_simd_f_sub(a, b);

    srslte_simd_f_storeu(&z[i], r);
  }
#endif

  for (; i < len; i++) {
    z[i] = x[i] - y[i];
  }
}

static inline void srslte_vec_sub_ccc_simd_inline(const cf_t* x, const cf_t* y, cf_t* z, const int len)
{
  srslte_vec_sub_fff_simd_inline((float*)x, (float*)y, (float*)z, 2 * len);
}

static inline cf_t reciprocal(cf_t x)
{
  cf_t y = 0.0f;

  float mod  = __real__ x * __real__ x + __imag__ x * __imag__ x;

  if (isnormal(mod)) {
    __real__ y = (__real__ x) / mod;
    __imag__ y = -(__imag__ x) / mod;
  }

  return y;
}

static inline float _cabs2(cf_t x)
{
  return __real__ x * __real__ x + __imag__ x * __imag__ x;
}

void srslte_matrix_NxN_inv_run(srslte_matrix_NxN_inv_t* q, cf_t* in, cf_t* out)
{
  if (q && in && out) {
    int   N = q->N;
    cf_t* tmp_src_ptr;
    cf_t* tmp_dst_ptr;

    // 0) Copy the input vector in the matrix
    tmp_src_ptr = in;
    tmp_dst_ptr = q->matrix;
    for (int i = 0; i < N; i++) {
      // Populate first half with input matrix
      memcpy(tmp_dst_ptr, tmp_src_ptr, sizeof(cf_t) * N);
      tmp_src_ptr += N;
      tmp_dst_ptr += N;

      // Populate second half with identity matrix
      bzero(tmp_dst_ptr, sizeof(cf_t) * N);
      tmp_dst_ptr[i] = 1.0f;
      tmp_dst_ptr += N;
    }

    // 1) Forward elimination
    for (int i = 0; i < N - 1; i++) {
      uint32_t row_i = N - i - 1;
      uint32_t col_i = N - i - 1;
      float    max_v = 0.0f;
      uint32_t max_i = 0;

      // Find Row with highest value in the column to reduce
      for (int j = 0; j < N - i; j++) {
        float v = _cabs2(q->matrix[(j + 1) * 2 * N - 1 - i]);
        if (v > max_v) {
          max_i = j;
          max_v = v;
        }
      }

      // Swap rows
      if (max_i != row_i) {
        memcpy(q->row_buffer, &q->matrix[row_i * N * 2], sizeof(cf_t) * N * 2);
        memcpy(&q->matrix[row_i * N * 2], &q->matrix[max_i * N * 2], sizeof(cf_t) * N * 2);
        memcpy(&q->matrix[max_i * N * 2], q->row_buffer, sizeof(cf_t) * N * 2);
      }

      tmp_src_ptr = &q->matrix[N * 2 * row_i]; // Select row
      cf_t b      = tmp_src_ptr[col_i];
      srslte_vec_sc_prod_ccc_simd_inline(tmp_src_ptr, reciprocal(b), tmp_src_ptr, 2 * N);

      for (int j = 0; j < N - i - 1; j++) {
        cf_t a = q->matrix[N * (2 * j + 1) - 1 - i];

        if (a != 0.0f && b != 0.0f) {
          tmp_dst_ptr = &q->matrix[N * 2 * j];
          srslte_vec_sc_prod_ccc_simd_inline(tmp_dst_ptr, reciprocal(a), tmp_dst_ptr, 2 * N);
          srslte_vec_sub_ccc_simd_inline(tmp_dst_ptr, tmp_src_ptr, tmp_dst_ptr, 2 * N);
        }
      }
    }
    srslte_vec_sc_prod_ccc_simd_inline(q->matrix, reciprocal(q->matrix[0]), q->matrix, 2 * N);

    // 2) Backward elimination
    for (int i = 0; i < N - 1; i++) {
      tmp_src_ptr = &q->matrix[N * 2 * i];
      cf_t b      = tmp_src_ptr[i];
      srslte_vec_sc_prod_ccc_simd_inline(tmp_src_ptr, reciprocal(b), tmp_src_ptr, 2 * N);

      for (int j = N - 1; j > i; j--) {
        cf_t a = q->matrix[N * 2 * j + i];

        tmp_dst_ptr = &q->matrix[N * 2 * j];
        srslte_vec_sc_prod_ccc_simd_inline(tmp_dst_ptr, reciprocal(a), tmp_dst_ptr, 2 * N);
        srslte_vec_sub_ccc_simd_inline(tmp_dst_ptr, tmp_src_ptr, tmp_dst_ptr, 2 * N);
      }
    }
    srslte_vec_sc_prod_ccc_simd_inline(&q->matrix[2 * N * (N - 1)],
                                       reciprocal(q->matrix[2 * N * (N - 1) + N - 1]),
                                       &q->matrix[2 * N * (N - 1)],
                                       2 * N);

    // 4) Copy result
    tmp_src_ptr = &q->matrix[N];
    tmp_dst_ptr = out;
    for (int i = 0; i < N; i++) {
      memcpy(tmp_dst_ptr, tmp_src_ptr, sizeof(cf_t) * N);
      tmp_src_ptr += 2 * N;
      tmp_dst_ptr += N;
    }

#if 0
    printf("tmp = [...\n");
    for (int i = 0; i < N; i++) {
      printf("\t");
      for (int j = 0; j < 2 * N; j++) {
        printf("%c %+.3f%+.3fi", j ==0 ?  ' ' : ',', __real__ q->matrix[2 * N * i  + j], __imag__ q->matrix[2 * N  * i + j]);
      }
      printf("; ...\n");
    }
    printf("];\n");

    printf("in = [...\n");
    for (int i = 0; i < N; i++) {
      printf("\t");
      for (int j = 0; j < N; j++) {
        printf("%c %+.3f%+.3fi", j ==0 ?  ' ' : ',', __real__ in[N * i  + j], __imag__ in[N  * i + j]);
      }
      printf("; ...\n");
    }
    printf("];\n");

    printf("out = [...\n");
    for (int i = 0; i < N; i++) {
      printf("\t");
      for (int j = 0; j < N; j++) {
        printf("%c %+.3f%+.3fi", j ==0 ?  ' ' : ',', __real__ out[N * i  + j], __imag__ out[N  * i + j]);
      }
      printf("; ...\n");
    }
    printf("];\n");
#endif
  }
}

void srslte_matrix_NxN_inv_free(srslte_matrix_NxN_inv_t* q)
{
  if (q) {

    if (q->matrix) {
      free(q->matrix);
    }

    if (q->row_buffer) {
      free(q->row_buffer);
    }

    // Default all to zero
    bzero(q, sizeof(srslte_matrix_NxN_inv_t));
  }
}
