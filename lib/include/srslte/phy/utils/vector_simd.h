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

#ifndef VECTORSIMD_
#define VECTORSIMD_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include "srslte/config.h"

#ifdef LV_HAVE_AVX512
#define SRSLTE_IS_ALIGNED(PTR) (((size_t)(PTR) & 0x3F) == 0)
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX
#define SRSLTE_IS_ALIGNED(PTR) (((size_t)(PTR) & 0x1F) == 0)
#else /* LV_HAVE_AVX */
#ifdef LV_HAVE_SSE
#define SRSLTE_IS_ALIGNED(PTR) (((size_t)(PTR) & 0x0F) == 0)
#else /* LV_HAVE_SSE */
#define SRSLTE_IS_ALIGNED(PTR) (true)
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX */
#endif /* LV_HAVE_AVX512 */

SRSLTE_API int srslte_vec_dot_prod_sss_simd(int16_t *x, int16_t *y, int len);

SRSLTE_API void srslte_vec_sum_sss_simd(int16_t *x, int16_t *y, int16_t *z, int len);

SRSLTE_API void srslte_vec_sub_sss_simd(int16_t *x, int16_t *y, int16_t *z, int len);

SRSLTE_API void srslte_vec_sub_sss_avx2(short *x, short *y, short *z, uint32_t len);

SRSLTE_API cf_t srslte_vec_acc_cc_simd(cf_t *x, int len);

SRSLTE_API void srslte_vec_add_fff_simd(float *x, float *y, float *z, int len);

SRSLTE_API void srslte_vec_sub_fff_simd(float *x, float *y, float *z, int len);

SRSLTE_API void srslte_vec_sc_prod_fff_simd(float *x, float h, float *z, int len);

SRSLTE_API void srslte_vec_sc_prod_ccc_simd(cf_t *x, cf_t h, cf_t *z, int len);

SRSLTE_API void srslte_vec_prod_fff_simd(float *x, float *y, float *z, int len);

SRSLTE_API void srslte_vec_prod_ccc_simd(cf_t *x,cf_t *y, cf_t *z, int len);

SRSLTE_API void srslte_vec_prod_conj_ccc_simd(cf_t *x,cf_t *y, cf_t *z, int len);

SRSLTE_API void srslte_vec_prod_ccc_cf_simd(float *a_re, float *a_im, float *b_re, float *b_im, float *r_re, float *r_im, int len);

SRSLTE_API void srslte_vec_prod_ccc_c16_simd(int16_t *a_re, int16_t *a_im, int16_t *b_re, int16_t *b_im, int16_t *r_re,
                                             int16_t *r_im, int len);

SRSLTE_API void srslte_vec_prod_sss_simd(int16_t *x, int16_t *y, int16_t *z, int len);

SRSLTE_API cf_t srslte_vec_dot_prod_conj_ccc_simd(cf_t *x, cf_t *y, int len);

SRSLTE_API cf_t srslte_vec_dot_prod_ccc_simd(cf_t *x, cf_t *y, int len);

SRSLTE_API cf_t srslte_vec_dot_prod_ccc_sse(cf_t *x, cf_t *y, uint32_t len);

SRSLTE_API c16_t srslte_vec_dot_prod_ccc_c16i_simd(c16_t *x, c16_t *y, int len);

SRSLTE_API  void srslte_vec_sc_div2_sss_avx2(short *x, int k, short *z, uint32_t len);

SRSLTE_API void srslte_vec_abs_cf_simd(cf_t *x, float *z, int len);

SRSLTE_API void srslte_vec_abs_square_cf_simd(cf_t *x, float *z, int len);

SRSLTE_API void srslte_vec_prod_sss_sse(short *x, short *y, short *z, uint32_t len);

SRSLTE_API void srslte_vec_prod_sss_avx(short *x, short *y, short *z, uint32_t len);

SRSLTE_API void srslte_vec_sc_div2_sss_sse(short *x, int n_rightshift, short *z, uint32_t len); 

SRSLTE_API  void srslte_vec_sc_div2_sss_avx(short *x, int k, short *z, uint32_t len);

SRSLTE_API void srslte_vec_lut_sss_sse(short *x, unsigned short *lut, short *y, uint32_t len); 

SRSLTE_API void srslte_vec_convert_fi_sse(float *x, int16_t *z, float scale, uint32_t len); 

SRSLTE_API void srslte_vec_mult_scalar_cf_f_avx( cf_t *z,const cf_t *x,const float h,const uint32_t len);

SRSLTE_API void srslte_vec_lut_sss_sse(short *x, unsigned short *lut, short *y, uint32_t len);

SRSLTE_API void srslte_vec_convert_fi_sse(float *x, int16_t *z, float scale, uint32_t len);

SRSLTE_API void srslte_vec_sc_prod_cfc_simd(const cf_t *x,const float h,cf_t *y,const int len);

SRSLTE_API void srslte_vec_cp_simd(cf_t *src, cf_t *dst, int len);

#ifdef __cplusplus
}
#endif

#endif
