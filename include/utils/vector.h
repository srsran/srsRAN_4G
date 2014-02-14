/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VECTOR_
#define VECTOR_

#include <stdio.h>

typedef _Complex float cf_t;

/** Return the sum of all the elements */
int vec_acc_ii(int *x, int len);
float vec_acc_ff(float *x, int len);
cf_t vec_acc_cc(cf_t *x, int len);

void *vec_malloc(int size);

/* print vectors */
void vec_fprint_c(FILE *stream, cf_t *x, int len);
void vec_fprint_f(FILE *stream, float *x, int len);
void vec_fprint_i(FILE *stream, int *x, int len);

/* sum two vectors */
void vec_sum_ch(char *z, char *x, char *y, int len);
void vec_sum_ccc(cf_t *z, cf_t *x, cf_t *y, int len);

/* scalar product */
void vec_sc_prod_cfc(cf_t *x, float h, cf_t *z, int len);
void vec_sc_prod_ccc(cf_t *x, cf_t h, cf_t *z, int len);

/* vector product */
void vec_prod_ccc(cf_t *x, cf_t *y, cf_t *z, int len);
void vec_prod_ccc_unalign(cf_t *x, cf_t *y, cf_t *z, int len);

/* conjugate */
void vec_conj_cc(cf_t *x, cf_t *y, int len);

/* average vector power */
float vec_avg_power_cf(cf_t *x, int len);

/* return the index of the maximum value in the vector */
int vec_max_fi(float *x, int len);

/* magnitude of each vector element */
void vec_abs_cf(cf_t *x, float *abs, int len);

#endif
