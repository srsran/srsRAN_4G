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

#ifndef SRSLTE_VECTOR_SIMD_H
#define SRSLTE_VECTOR_SIMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "srslte/config.h"
#include <stdint.h>
#include <stdio.h>

/*SIMD Logical operations*/
SRSLTE_API void srslte_vec_xor_bbb_simd(const int8_t* x, const int8_t* y, int8_t* z, int len);

/* SIMD Basic vector math */
SRSLTE_API void srslte_vec_sum_sss_simd(const int16_t* x, const int16_t* y, int16_t* z, int len);

SRSLTE_API void srslte_vec_sub_sss_simd(const int16_t* x, const int16_t* y, int16_t* z, int len);

SRSLTE_API void srslte_vec_sub_bbb_simd(const int8_t* x, const int8_t* y, int8_t* z, int len);

SRSLTE_API float srslte_vec_acc_ff_simd(const float* x, int len);

SRSLTE_API cf_t srslte_vec_acc_cc_simd(const cf_t* x, int len);

SRSLTE_API void srslte_vec_add_fff_simd(const float* x, const float* y, float* z, int len);

SRSLTE_API void srslte_vec_sub_fff_simd(const float* x, const float* y, float* z, int len);

/* SIMD Vector Scalar Product */
SRSLTE_API void srslte_vec_sc_prod_cfc_simd(const cf_t* x, const float h, cf_t* y, const int len);

SRSLTE_API void srslte_vec_sc_prod_fff_simd(const float* x, const float h, float* z, const int len);

SRSLTE_API void srslte_vec_sc_prod_ccc_simd(const cf_t* x, const cf_t h, cf_t* z, const int len);

SRSLTE_API int srslte_vec_sc_prod_ccc_simd2(const cf_t* x, const cf_t h, cf_t* z, const int len);

/* SIMD Vector Product */
SRSLTE_API void srslte_vec_prod_ccc_split_simd(const float* a_re,
                                               const float* a_im,
                                               const float* b_re,
                                               const float* b_im,
                                               float*       r_re,
                                               float*       r_im,
                                               const int    len);

SRSLTE_API void srslte_vec_prod_ccc_c16_simd(const int16_t* a_re,
                                             const int16_t* a_im,
                                             const int16_t* b_re,
                                             const int16_t* b_im,
                                             int16_t*       r_re,
                                             int16_t*       r_im,
                                             const int      len);

SRSLTE_API void srslte_vec_prod_sss_simd(const int16_t* x, const int16_t* y, int16_t* z, const int len);

SRSLTE_API void srslte_vec_neg_sss_simd(const int16_t* x, const int16_t* y, int16_t* z, const int len);

SRSLTE_API void srslte_vec_neg_bbb_simd(const int8_t* x, const int8_t* y, int8_t* z, const int len);

SRSLTE_API void srslte_vec_prod_cfc_simd(const cf_t* x, const float* y, cf_t* z, const int len);

SRSLTE_API void srslte_vec_prod_fff_simd(const float* x, const float* y, float* z, const int len);

SRSLTE_API void srslte_vec_prod_ccc_simd(const cf_t* x, const cf_t* y, cf_t* z, const int len);

SRSLTE_API void srslte_vec_prod_conj_ccc_simd(const cf_t* x, const cf_t* y, cf_t* z, const int len);

/* SIMD Division */
SRSLTE_API void srslte_vec_div_ccc_simd(const cf_t* x, const cf_t* y, cf_t* z, const int len);

SRSLTE_API void srslte_vec_div_cfc_simd(const cf_t* x, const float* y, cf_t* z, const int len);

SRSLTE_API void srslte_vec_div_fff_simd(const float* x, const float* y, float* z, const int len);

/* SIMD Dot product */
SRSLTE_API cf_t srslte_vec_dot_prod_conj_ccc_simd(const cf_t* x, const cf_t* y, const int len);

SRSLTE_API cf_t srslte_vec_dot_prod_ccc_simd(const cf_t* x, const cf_t* y, const int len);

#ifdef ENABLE_C16
SRSLTE_API c16_t srslte_vec_dot_prod_ccc_c16i_simd(const c16_t* x, const c16_t* y, const int len);
#endif /* ENABLE_C16 */

SRSLTE_API int srslte_vec_dot_prod_sss_simd(const int16_t* x, const int16_t* y, const int len);

/* SIMD Modulus functions */
SRSLTE_API void srslte_vec_abs_cf_simd(const cf_t* x, float* z, const int len);

SRSLTE_API void srslte_vec_abs_square_cf_simd(const cf_t* x, float* z, const int len);

/* Other Functions */
SRSLTE_API void srslte_vec_lut_sss_simd(const short* x, const unsigned short* lut, short* y, const int len);

SRSLTE_API void srslte_vec_lut_bbb_simd(const int8_t* x, const unsigned short* lut, int8_t* y, const int len);

SRSLTE_API void srslte_vec_convert_if_simd(const int16_t* x, float* z, const float scale, const int len);

SRSLTE_API void srslte_vec_convert_fi_simd(const float* x, int16_t* z, const float scale, const int len);

SRSLTE_API void srslte_vec_convert_conj_cs_simd(const cf_t* x, int16_t* z, const float scale, const int len);

SRSLTE_API void srslte_vec_convert_fb_simd(const float* x, int8_t* z, const float scale, const int len);

SRSLTE_API void srslte_vec_interleave_simd(const cf_t* x, const cf_t* y, cf_t* z, const int len);

SRSLTE_API void srslte_vec_interleave_add_simd(const cf_t* x, const cf_t* y, cf_t* z, const int len);

SRSLTE_API void srslte_vec_gen_sine_simd(cf_t amplitude, float freq, cf_t* z, int len);

SRSLTE_API void srslte_vec_apply_cfo_simd(const cf_t* x, float cfo, cf_t* z, int len);

SRSLTE_API float srslte_vec_estimate_frequency_simd(const cf_t* x, int len);

/* SIMD Find Max functions */
SRSLTE_API uint32_t srslte_vec_max_fi_simd(const float* x, const int len);

SRSLTE_API uint32_t srslte_vec_max_abs_fi_simd(const float* x, const int len);

SRSLTE_API uint32_t srslte_vec_max_ci_simd(const cf_t* x, const int len);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_VECTOR_SIMD_H
