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

SRSLTE_API int srslte_vec_dot_prod_sss_sse(short *x, short *y, uint32_t len); 

SRSLTE_API int srslte_vec_dot_prod_sss_avx2(short *x, short *y, uint32_t len); 

SRSLTE_API void srslte_vec_sum_sss_sse(short *x, short *y, short *z, uint32_t len);

SRSLTE_API void srslte_vec_sum_sss_avx2(short *x, short *y, short *z, uint32_t len);

SRSLTE_API void srslte_vec_sub_sss_sse(short *x, short *y, short *z, uint32_t len); 

SRSLTE_API void srslte_vec_sub_sss_avx2(short *x, short *y, short *z, uint32_t len);

SRSLTE_API void srslte_vec_sum_fff_sse(float *x, float *y, float *z, uint32_t len);

SRSLTE_API void srslte_vec_sub_fff_sse(float *x, float *y, float *z, uint32_t len);

SRSLTE_API void srslte_vec_sc_prod_fff_sse(float *x, float h, float *z, uint32_t len);

SRSLTE_API void srslte_vec_sc_prod_ccc_sse(cf_t *x, cf_t h, cf_t *z, uint32_t len);

SRSLTE_API void srslte_vec_prod_ccc_sse(cf_t *x,cf_t *y, cf_t *z, uint32_t len);

SRSLTE_API void srslte_vec_prod_sss_avx2(short *x, short *y, short *z, uint32_t len);

SRSLTE_API cf_t srslte_vec_dot_prod_conj_ccc_sse(cf_t *x, cf_t *y, uint32_t len);

SRSLTE_API void srslte_vec_prod_conj_ccc_sse(cf_t *x,cf_t *y, cf_t *z, uint32_t len);

SRSLTE_API cf_t srslte_vec_dot_prod_ccc_sse(cf_t *x, cf_t *y, uint32_t len);

SRSLTE_API  void srslte_vec_sc_div2_sss_avx2(short *x, int k, short *z, uint32_t len);

SRSLTE_API void srslte_vec_abs_square_cf_sse(cf_t *x, float *z, uint32_t len);

SRSLTE_API void srslte_vec_prod_sss_sse(short *x, short *y, short *z, uint32_t len); 

SRSLTE_API void srslte_vec_prod_sss_avx(short *x, short *y, short *z, uint32_t len);

SRSLTE_API void srslte_vec_sc_div2_sss_sse(short *x, int n_rightshift, short *z, uint32_t len); 

SRSLTE_API  void srslte_vec_sc_div2_sss_avx(short *x, int k, short *z, uint32_t len);

SRSLTE_API void srslte_vec_lut_sss_sse(short *x, unsigned short *lut, short *y, uint32_t len); 

SRSLTE_API void srslte_vec_convert_fi_sse(float *x, int16_t *z, float scale, uint32_t len); 

SRSLTE_API void srslte_vec_mult_scalar_cf_f_avx( cf_t *z,const cf_t *x,const float h,const uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
