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

#ifndef SRSLTE_VECTOR_SIMD_H
#define SRSLTE_VECTOR_SIMD_H

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


/*SIMD Logical operations*/
SRSLTE_API void srslte_vec_xor_bbb_simd(const int8_t *x, const int8_t *y, int8_t *z, int len);

/* SIMD Basic vector math */
SRSLTE_API void srslte_vec_sum_sss_simd(const int16_t *x, const int16_t *y, int16_t *z, int len);

SRSLTE_API void srslte_vec_sub_sss_simd(const int16_t *x, const int16_t *y, int16_t *z, int len);

SRSLTE_API float srslte_vec_acc_ff_simd(const float *x, int len);

SRSLTE_API cf_t srslte_vec_acc_cc_simd(const cf_t *x, int len);

SRSLTE_API void srslte_vec_add_fff_simd(const float *x, const float *y, float *z, int len);

SRSLTE_API void srslte_vec_sub_fff_simd(const float *x, const float *y, float *z, int len);

/* SIMD Vector Scalar Product */
SRSLTE_API void srslte_vec_sc_prod_cfc_simd(const cf_t *x, const float h,cf_t *y, const int len);

SRSLTE_API void srslte_vec_sc_prod_fff_simd(const float *x, const float h, float *z, const int len);

SRSLTE_API void srslte_vec_sc_prod_ccc_simd(const cf_t *x, const cf_t h, cf_t *z, const int len);

/* SIMD Vector Product */
SRSLTE_API void srslte_vec_prod_ccc_split_simd(const float *a_re, const float *a_im, const float *b_re, const float *b_im,
                                               float *r_re, float *r_im, const int len);

SRSLTE_API void srslte_vec_prod_ccc_c16_simd(const int16_t *a_re, const int16_t *a_im, const int16_t *b_re, const int16_t *b_im,
                                             int16_t *r_re, int16_t *r_im, const int len);

SRSLTE_API void srslte_vec_prod_sss_simd(const int16_t *x, const int16_t *y, int16_t *z, const int len);

SRSLTE_API void srslte_vec_prod_cfc_simd(const cf_t *x, const float *y, cf_t *z, const int len);

SRSLTE_API void srslte_vec_prod_fff_simd(const float *x, const float *y, float *z, const int len);

SRSLTE_API void srslte_vec_prod_ccc_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len);

SRSLTE_API void srslte_vec_prod_conj_ccc_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len);

/* SIMD Division */
SRSLTE_API void srslte_vec_div_ccc_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len);

SRSLTE_API void srslte_vec_div_cfc_simd(const cf_t *x, const float *y, cf_t *z, const int len);

SRSLTE_API void srslte_vec_div_fff_simd(const float *x, const float *y, float *z, const int len);

/* SIMD Dot product */
SRSLTE_API cf_t srslte_vec_dot_prod_conj_ccc_simd(const cf_t *x, const cf_t *y, const int len);

SRSLTE_API cf_t srslte_vec_dot_prod_ccc_simd(const cf_t *x, const cf_t *y, const int len);

#ifdef ENABLE_C16
SRSLTE_API c16_t srslte_vec_dot_prod_ccc_c16i_simd(const c16_t *x, const c16_t *y, const int len);
#endif /* ENABLE_C16 */

SRSLTE_API int srslte_vec_dot_prod_sss_simd(const int16_t *x, const int16_t *y, const int len);

/* SIMD Modulus functions */
SRSLTE_API void srslte_vec_abs_cf_simd(const cf_t *x, float *z, const int len);

SRSLTE_API void srslte_vec_abs_square_cf_simd(const cf_t *x, float *z, const int len);

/* Other Functions */
SRSLTE_API void srslte_vec_lut_sss_simd(const short *x, const unsigned short *lut, short *y, const int len);

SRSLTE_API void srslte_vec_convert_if_simd(const int16_t *x, float *z, const float scale, const int len);

SRSLTE_API void srslte_vec_convert_fi_simd(const float *x, int16_t *z, const float scale, const int len);

SRSLTE_API void srslte_vec_cp_simd(const cf_t *src, cf_t *dst, int len);

SRSLTE_API void srslte_vec_interleave_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len);

SRSLTE_API void srslte_vec_interleave_add_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len);

/* SIMD Find Max functions */
SRSLTE_API uint32_t srslte_vec_max_fi_simd(const float *x, const int len);

SRSLTE_API uint32_t srslte_vec_max_ci_simd(const cf_t *x, const int len);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_VECTOR_SIMD_H
