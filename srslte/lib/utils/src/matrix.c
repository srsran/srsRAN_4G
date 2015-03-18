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



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "srslte/utils/matrix.h"

int matrix_init(void ***q, int sz_x, int sz_y, int elem_sz) {
  int i;
  int ret = -1;
  *q = malloc(sizeof(void*) * sz_x);
  if (!*q) {
    goto free_and_exit;
  }
  for (i=0;i<sz_x;i++) {
    (*q)[i] = malloc(elem_sz * sz_y);
    if (!(*q)[i]) {
      goto free_and_exit;
    }
  }
  ret = 0;
free_and_exit:
  if (ret == -1) {
    matrix_free(*q, sz_x);
  }
  return ret;
}

void matrix_free(void **q, int sz_x) {
  int i;
  if (q) {
    for (i=0;i<sz_x;i++) {
      if (q[i]) {
        free(q[i]);
      }
    }
    free(q);
  }
}

void matrix_bzero(void **q, int sz_x, int sz_y, int elem_sz) {
  int i;
  for (i=0;i<sz_x;i++) {
    bzero(q[i], sz_y * elem_sz);
  }
}

void matrix_copy(void **dst, void **src, int sz_x, int sz_y, int elem_sz) {
  int i;
  for (i=0;i<sz_x;i++) {
    memcpy(dst[i], src[i], sz_y * elem_sz);
  }
}

void matrix_fprintf_cf(FILE *f, cf_t **q, int sz_x, int sz_y) {
  int i, j;
  for (i=0;i<sz_x;i++) {
    printf("(");
    for (j=0;j<sz_y;j++) {
      fprintf(f, "%+6.2f%+6.2fi ", __real__ q[i][j], __imag__ q[i][j]);
    }
    fprintf(f, ")\n");
  }
}


void matrix_fprintf_f(FILE *f, float **q, int sz_x, int sz_y) {
  int i, j;
  for (i=0;i<sz_x;i++) {
    printf("(");
    for (j=0;j<sz_y;j++) {
      fprintf(f, "%-6.2f", q[i][j]);
    }
    fprintf(f, ")\n");
  }
}

#define DOTPROD_GEN(x, y, out, sz_x, sz_y) \
      int i,j; \
      for (i=0;i<sz_x;i++) { \
        for (j=0;j<sz_y;j++) { \
          out[i][j] = x[i][j] * y[i][j]; \
        } \
      } \

/**TODO: Use volk for vector multiplication
 *
 */
void matrix_dotprod_cf(cf_t **x, cf_t **y, cf_t **out, int sz_x, int sz_y) {
  DOTPROD_GEN(x, y, out, sz_x, sz_y);
}
void matrix_dotprod_float(float **x, float **y, float **out, int sz_x, int sz_y) {
  DOTPROD_GEN(x, y, out, sz_x, sz_y);
}
void matrix_dotprod_int(int **x, int **y, int **out, int sz_x, int sz_y) {
  DOTPROD_GEN(x, y, out, sz_x, sz_y);
}

