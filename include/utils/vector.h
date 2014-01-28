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

int sum_i(int *x, int len);
float sum_r(float *x, int len);
_Complex float sum_c(_Complex float *x, int len);

void *vec_malloc(int size);
void vec_fprint_c(FILE *stream, _Complex float *x, int len);
void vec_fprint_f(FILE *stream, float *x, int len);
void vec_fprint_i(FILE *stream, int *x, int len);

void vec_sum_ch(char *z, char *x, char *y, int len);
void vec_sum_c(_Complex float *z, _Complex float *x, _Complex float *y, int len);
void vec_mult_c_r(_Complex float *x,_Complex float *y, float h, int len);
void vec_mult_c(_Complex float *x,_Complex float *y, _Complex float h, int len);
void vec_conj(_Complex float *x, _Complex float *y, int len);
float vec_power(_Complex float *x, int len);
void vec_dot_prod(_Complex float *x,_Complex float *y, _Complex float *z, int len);
void vec_dot_prod_u(_Complex float *x,_Complex float *y, _Complex float *z, int len);
void vec_max(float *x, float *max, int *pos, int len);
void vec_abs(_Complex float *x, float *abs, int len);

#endif
