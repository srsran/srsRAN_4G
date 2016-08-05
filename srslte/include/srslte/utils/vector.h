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

/******************************************************************************
 *  File:         vector.h
 *
 *  Description:  Vector functions using SIMD instructions where possible.
 *
 *  Reference:
 *****************************************************************************/

#ifndef VECTOR_
#define VECTOR_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include "srslte/config.h"


#define SRSLTE_MAX(a,b) ((a)>(b)?(a):(b))
#define SRSLTE_MIN(a,b) ((a)<(b)?(a):(b))

// Cumulative moving average
#define SRSLTE_VEC_CMA(data, average, n) ((average) + ((data) - (average)) / ((n)+1)) 

// Exponential moving average
#define SRSLTE_VEC_EMA(data, average, alpha) ((alpha)*(data)+(1-alpha)*(average))

/** Return the sum of all the elements */
SRSLTE_API int srslte_vec_acc_ii(int *x, uint32_t len);
SRSLTE_API float srslte_vec_acc_ff(float *x, uint32_t len);
SRSLTE_API cf_t srslte_vec_acc_cc(cf_t *x, uint32_t len);

SRSLTE_API void *srslte_vec_malloc(uint32_t size);

SRSLTE_API void *srslte_vec_realloc(void *ptr, uint32_t old_size, uint32_t new_size);

/* print vectors */
SRSLTE_API void srslte_vec_fprint_c(FILE *stream, cf_t *x, uint32_t len);
SRSLTE_API void srslte_vec_fprint_f(FILE *stream, float *x, uint32_t len);
SRSLTE_API void srslte_vec_fprint_b(FILE *stream, uint8_t *x, uint32_t len);
SRSLTE_API void srslte_vec_fprint_byte(FILE *stream, uint8_t *x, uint32_t len);
SRSLTE_API void srslte_vec_fprint_i(FILE *stream, int *x, uint32_t len);
SRSLTE_API void srslte_vec_fprint_s(FILE *stream, short *x, uint32_t len); 
SRSLTE_API void srslte_vec_fprint_hex(FILE *stream, uint8_t *x, uint32_t len);
SRSLTE_API void srslte_vec_sprint_hex(char *str, uint8_t *x, uint32_t len); 

/* Saves/loads a vector to a file */
SRSLTE_API void srslte_vec_save_file(char *filename, void *buffer, uint32_t len);
SRSLTE_API void srslte_vec_load_file(char *filename, void *buffer, uint32_t len);

/* sum two vectors */
SRSLTE_API void srslte_vec_sum_ch(uint8_t *x, uint8_t *y, char *z, uint32_t len);
SRSLTE_API void srslte_vec_sum_fff(float *x, float *y, float *z, uint32_t len);
SRSLTE_API void srslte_vec_sum_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len);
SRSLTE_API void srslte_vec_sub_sss(short *x, short *y, short *z, uint32_t len);
SRSLTE_API void srslte_vec_sum_sss(short *x, short *y, short *z, uint32_t len);

/* substract two vectors z=x-y */
SRSLTE_API void srslte_vec_sub_fff(float *x, float *y, float *z, uint32_t len); 
SRSLTE_API void srslte_vec_sub_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len);

/* EMA filter: output=coeff*new_data + (1-coeff)*average */
SRSLTE_API void srslte_vec_ema_filter(cf_t *new_data, cf_t *average, cf_t *output, float coeff, uint32_t len); 

/* Square distance */
SRSLTE_API void srslte_vec_square_dist(cf_t symbol, cf_t *points, float *distance, uint32_t npoints);

/* scalar addition */
SRSLTE_API void srslte_vec_sc_add_fff(float *x, float h, float *z, uint32_t len); 
SRSLTE_API void srslte_vec_sc_add_cfc(cf_t *x, float h, cf_t *z, uint32_t len); 
SRSLTE_API void srslte_vec_sc_add_ccc(cf_t *x, cf_t h, cf_t *z, uint32_t len); 
SRSLTE_API void srslte_vec_sc_add_sss(int16_t *x, int16_t h, int16_t *z, uint32_t len); 

/* scalar product */
SRSLTE_API void srslte_vec_sc_prod_cfc(cf_t *x, float h, cf_t *z, uint32_t len);
SRSLTE_API void srslte_vec_sc_prod_ccc(cf_t *x, cf_t h, cf_t *z, uint32_t len);
SRSLTE_API void srslte_vec_sc_prod_fff(float *x, float h, float *z, uint32_t len); 
SRSLTE_API void srslte_vec_sc_prod_sfs(short *x, float h, short *z, uint32_t len); 
SRSLTE_API void srslte_vec_sc_div2_sss(short *x, int pow2_div, short *z, uint32_t len); 

/* Normalization */
SRSLTE_API void srslte_vec_norm_cfc(cf_t *x, float amplitude, cf_t *y, uint32_t len);

SRSLTE_API void srslte_vec_convert_fi(float *x, int16_t *z, float scale, uint32_t len);
SRSLTE_API void srslte_vec_convert_if(int16_t *x, float *z, float scale, uint32_t len);
SRSLTE_API void srslte_vec_convert_ci(int8_t *x, int16_t *z, uint32_t len); 

SRSLTE_API void srslte_vec_lut_fuf(float *x, uint32_t *lut, float *y, uint32_t len);
SRSLTE_API void srslte_vec_lut_sss(short *x, unsigned short *lut, short *y, uint32_t len); 

SRSLTE_API void srslte_vec_deinterleave_cf(cf_t *x, float *real, float *imag, uint32_t len); 
SRSLTE_API void srslte_vec_deinterleave_real_cf(cf_t *x, float *real, uint32_t len);

SRSLTE_API void srslte_vec_interleave_cf(float *real, float *imag, cf_t *x, uint32_t len);

/* vector product (element-wise) */
SRSLTE_API void srslte_vec_prod_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len);

/* vector product (element-wise) */
SRSLTE_API void srslte_vec_prod_cfc(cf_t *x, float *y, cf_t *z, uint32_t len);

/* conjugate vector product (element-wise) */
SRSLTE_API void srslte_vec_prod_conj_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len);

/* real vector product (element-wise) */
SRSLTE_API void srslte_vec_prod_fff(float *x, float *y, float *z, uint32_t len);
SRSLTE_API void srslte_vec_prod_sss(short *x, short *y, short *z, uint32_t len);

/* Dot-product */
SRSLTE_API cf_t srslte_vec_dot_prod_cfc(cf_t *x, float *y, uint32_t len);
SRSLTE_API cf_t srslte_vec_dot_prod_ccc(cf_t *x, cf_t *y, uint32_t len);
SRSLTE_API cf_t srslte_vec_dot_prod_conj_ccc(cf_t *x, cf_t *y, uint32_t len);
SRSLTE_API float srslte_vec_dot_prod_fff(float *x, float *y, uint32_t len);
SRSLTE_API int32_t srslte_vec_dot_prod_sss(int16_t *x, int16_t *y, uint32_t len); 

/* z=x/y vector division (element-wise) */
SRSLTE_API void srslte_vec_div_ccc(cf_t *x, cf_t *y, float *y_mod, cf_t *z, float *z_real, float *z_imag, uint32_t len);
void srslte_vec_div_cfc(cf_t *x, float *y, cf_t *z, float *z_real, float *z_imag, uint32_t len);
SRSLTE_API void srslte_vec_div_fff(float *x, float *y, float *z, uint32_t len);

/* conjugate */
SRSLTE_API void srslte_vec_conj_cc(cf_t *x, cf_t *y, uint32_t len);

/* average vector power */
SRSLTE_API float srslte_vec_avg_power_cf(cf_t *x, uint32_t len);

/* return the index of the maximum value in the vector */
SRSLTE_API uint32_t srslte_vec_max_fi(float *x, uint32_t len);
SRSLTE_API uint32_t srslte_vec_max_abs_ci(cf_t *x, uint32_t len);
SRSLTE_API int16_t srslte_vec_max_star_si(int16_t *x, uint32_t len);

/* maximum between two vectors */
SRSLTE_API void srslte_vec_max_fff(float *x, float *y, float *z, uint32_t len);

/* quantify vector of floats or int16 and convert to uint8_t */
SRSLTE_API void srslte_vec_quant_fuc(float *in, uint8_t *out, float gain, float offset, float clip, uint32_t len);
SRSLTE_API void srslte_vec_quant_suc(int16_t *in, uint8_t *out, int16_t norm, int16_t offset, int16_t clip, uint32_t len); 

/* magnitude of each vector element */
SRSLTE_API void srslte_vec_abs_cf(cf_t *x, float *abs, uint32_t len);
SRSLTE_API void srslte_vec_abs_square_cf(cf_t *x, float *abs_square, uint32_t len);

/* argument of each vector element */
SRSLTE_API void srslte_vec_arg_cf(cf_t *x, float *arg, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // VECTOR_
