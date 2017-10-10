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
#define SRSLTE_SIMD_BIT_ALIGN 512
#define SRSLTE_IS_ALIGNED(PTR) (((size_t)(PTR) & 0x3F) == 0)
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX
#define SRSLTE_SIMD_BIT_ALIGN 256
#define SRSLTE_IS_ALIGNED(PTR) (((size_t)(PTR) & 0x1F) == 0)
#else /* LV_HAVE_AVX */
#ifdef LV_HAVE_SSE
#define SRSLTE_SIMD_BIT_ALIGN 128
#define SRSLTE_IS_ALIGNED(PTR) (((size_t)(PTR) & 0x0F) == 0)
#else /* LV_HAVE_SSE */
#define SRSLTE_SIMD_BIT_ALIGN 64
#define SRSLTE_IS_ALIGNED(PTR) (1)
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX */
#endif /* LV_HAVE_AVX512 */

/* SIMD Basic vector math */
SRSLTE_API void srslte_vec_sum_sss_simd(int16_t *x, int16_t *y, int16_t *z, int len);

SRSLTE_API void srslte_vec_sub_sss_simd(int16_t *x, int16_t *y, int16_t *z, int len);

SRSLTE_API float srslte_vec_acc_ff_simd(float *x, int len);

SRSLTE_API cf_t srslte_vec_acc_cc_simd(cf_t *x, int len);

SRSLTE_API void srslte_vec_add_fff_simd(float *x, float *y, float *z, int len);

SRSLTE_API void srslte_vec_sub_fff_simd(float *x, float *y, float *z, int len);

/* SIMD Vector Scalar Product */
SRSLTE_API void srslte_vec_sc_prod_cfc_simd(const cf_t *x,const float h,cf_t *y,const int len);

SRSLTE_API void srslte_vec_sc_prod_fff_simd(float *x, float h, float *z, int len);

SRSLTE_API void srslte_vec_sc_prod_ccc_simd(cf_t *x, cf_t h, cf_t *z, int len);

/* SIMD Vector Product */
SRSLTE_API void srslte_vec_prod_ccc_split_simd(float *a_re, float *a_im, float *b_re, float *b_im, float *r_re, float *r_im, int len);

SRSLTE_API void srslte_vec_prod_ccc_c16_simd(int16_t *a_re, int16_t *a_im, int16_t *b_re, int16_t *b_im, int16_t *r_re,
                                             int16_t *r_im, int len);

SRSLTE_API void srslte_vec_prod_sss_simd(int16_t *x, int16_t *y, int16_t *z, int len);

SRSLTE_API void srslte_vec_prod_cfc_simd(cf_t *x, float *y, cf_t *z, int len);

SRSLTE_API void srslte_vec_prod_fff_simd(float *x, float *y, float *z, int len);

SRSLTE_API void srslte_vec_prod_ccc_simd(cf_t *x,cf_t *y, cf_t *z, int len);

SRSLTE_API void srslte_vec_prod_conj_ccc_simd(cf_t *x,cf_t *y, cf_t *z, int len);

/* SIMD Division */
SRSLTE_API void srslte_vec_div_ccc_simd(cf_t *x,cf_t *y, cf_t *z, int len);

SRSLTE_API void srslte_vec_div_cfc_simd(cf_t *x, float *y, cf_t *z, int len);

SRSLTE_API void srslte_vec_div_fff_simd(float *x, float *y, float *z, int len);

/* SIMD Dot product */
SRSLTE_API cf_t srslte_vec_dot_prod_conj_ccc_simd(cf_t *x, cf_t *y, int len);

SRSLTE_API cf_t srslte_vec_dot_prod_ccc_simd(cf_t *x, cf_t *y, int len);

SRSLTE_API c16_t srslte_vec_dot_prod_ccc_c16i_simd(c16_t *x, c16_t *y, int len);

SRSLTE_API int srslte_vec_dot_prod_sss_simd(int16_t *x, int16_t *y, int len);

/* SIMD Modulus functions */
SRSLTE_API void srslte_vec_abs_cf_simd(cf_t *x, float *z, int len);

SRSLTE_API void srslte_vec_abs_square_cf_simd(cf_t *x, float *z, int len);

/* Other Functions */
SRSLTE_API void srslte_vec_lut_sss_simd(short *x, unsigned short *lut, short *y, int len);

SRSLTE_API void srslte_vec_convert_fi_simd(float *x, int16_t *z, float scale, int len);

SRSLTE_API void srslte_vec_cp_simd(cf_t *src, cf_t *dst, int len);


/* SIMD Find Max functions */
SRSLTE_API uint32_t srslte_vec_max_fi_simd(float *x, int len);

SRSLTE_API uint32_t srslte_vec_max_ci_simd(cf_t *x, int len);

#ifdef __cplusplus
}
#endif

#endif
