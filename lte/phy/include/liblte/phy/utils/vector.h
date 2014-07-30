/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef VECTOR_
#define VECTOR_

#include <stdio.h>
#include <stdint.h>
#include "liblte/config.h"

typedef _Complex float cf_t;

#define EXPAVERAGE(data, average, nframes) ((data + average * nframes) / (nframes + 1))  


/** Return the sum of all the elements */
LIBLTE_API int vec_acc_ii(int *x, uint32_t len);
LIBLTE_API float vec_acc_ff(float *x, uint32_t len);
LIBLTE_API cf_t vec_acc_cc(cf_t *x, uint32_t len);

LIBLTE_API void *vec_malloc(uint32_t size);

LIBLTE_API void *vec_realloc(void *ptr, uint32_t old_size, uint32_t new_size);

/* print vectors */
LIBLTE_API void vec_fprint_c(FILE *stream, cf_t *x, uint32_t len);
LIBLTE_API void vec_fprint_f(FILE *stream, float *x, uint32_t len);
LIBLTE_API void vec_fprint_b(FILE *stream, char *x, uint32_t len);
LIBLTE_API void vec_fprint_i(FILE *stream, int *x, uint32_t len);
LIBLTE_API void vec_fprint_hex(FILE *stream, char *x, uint32_t len);

/* Saves a vector to a file */
LIBLTE_API void vec_save_file(char *filename, void *buffer, uint32_t len);

/* sum two vectors */
LIBLTE_API void vec_sum_ch(char *x, char *y, char *z, uint32_t len);
LIBLTE_API void vec_sum_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len);

/* substract two vectors z=x-y */
LIBLTE_API void vec_sub_fff(float *x, float *y, float *z, uint32_t len); 

/* scalar product */
LIBLTE_API void vec_sc_prod_cfc(cf_t *x, float h, cf_t *z, uint32_t len);
LIBLTE_API void vec_sc_prod_ccc(cf_t *x, cf_t h, cf_t *z, uint32_t len);
LIBLTE_API void vec_sc_prod_fff(float *x, float h, float *z, uint32_t len); 

LIBLTE_API void vec_convert_fi(float *x, int16_t *z, float scale, uint32_t len);

LIBLTE_API void vec_deinterleave_cf(cf_t *x, float *real, float *imag, uint32_t len); 
LIBLTE_API void vec_deinterleave_real_cf(cf_t *x, float *real, uint32_t len);

/* vector product (element-wise) */
LIBLTE_API void vec_prod_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len);

/* vector product (element-wise) */
LIBLTE_API void vec_prod_cfc(cf_t *x, float *y, cf_t *z, uint32_t len);

/* conjugate vector product (element-wise) */
LIBLTE_API void vec_prod_conj_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len);

/* Dot-product */
LIBLTE_API cf_t vec_dot_prod_ccc(cf_t *x, cf_t *y, uint32_t len);
LIBLTE_API cf_t vec_dot_prod_conj_ccc(cf_t *x, cf_t *y, uint32_t len);
LIBLTE_API float vec_dot_prod_fff(float *x, float *y, uint32_t len);

/* z=x/y vector division (element-wise) */
LIBLTE_API void vec_div_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len);

/* conjugate */
LIBLTE_API void vec_conj_cc(cf_t *x, cf_t *y, uint32_t len);

/* average vector power */
LIBLTE_API float vec_avg_power_cf(cf_t *x, uint32_t len);

/* return the index of the maximum value in the vector */
LIBLTE_API uint32_t vec_max_fi(float *x, uint32_t len);

/* quantify vector of floats and convert to unsigned char */
LIBLTE_API void vec_quant_fuc(float *in, unsigned char *out, float gain, float offset, float clip, uint32_t len);

/* magnitude of each vector element */
LIBLTE_API void vec_abs_cf(cf_t *x, float *abs, uint32_t len);

/* argument of each vector element */
LIBLTE_API void vec_arg_cf(cf_t *x, float *arg, uint32_t len);

#endif // VECTOR_
