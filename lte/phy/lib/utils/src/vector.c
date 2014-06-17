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


#include "liblte/phy/utils/vector.h"
#include <float.h>
#include <complex.h>
#include <stdlib.h>

#ifdef HAVE_VOLK
#include "volk/volk.h"
#endif

int vec_acc_ii(int *x, int len) {
  int i;
  int z=0;
  for (i=0;i<len;i++) {
    z+=x[i];
  }
  return z;
}

float vec_acc_ff(float *x, int len) {
#ifdef HAVE_VOLK_ACC_FUNCTION
  float result;
  volk_32f_accumulator_s32f_u(&result,x,(unsigned int) len);
  return result;
#else
  int i;
  float z=0;
  for (i=0;i<len;i++) {
    z+=x[i];
  }
  return z;
#endif
}

cf_t vec_acc_cc(cf_t *x, int len) {
  int i;
  cf_t z=0;
  for (i=0;i<len;i++) {
    z+=x[i];
  }
  return z;
}

void vec_sum_ccc(cf_t *z, cf_t *x, cf_t *y, int len) {
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]+y[i];
  }
}

void vec_sum_bbb(char *z, char *x, char *y, int len) {
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]+y[i];
  }
}

void vec_sc_prod_cfc(cf_t *x, float h, cf_t *z, int len) {
#ifndef HAVE_VOLK_MULT_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*h;
  }
#else
  cf_t hh;
  __real__ hh = h;
  __imag__ hh = 0;
  volk_32fc_s32fc_multiply_32fc_u(z,x,hh,(unsigned int) len);
#endif
}

void vec_sc_prod_ccc(cf_t *x, cf_t h, cf_t *z, int len) {
#ifndef HAVE_VOLK_MULT_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*h;
  }
#else
  volk_32fc_s32fc_multiply_32fc_u(z,x,h,(unsigned int) len);
#endif
}



void *vec_malloc(int size) {
#ifndef HAVE_VOLK
  return malloc(size);
#else
  void *ptr;
  if (posix_memalign(&ptr,64,size)) {
    return NULL;
  } else {
    return ptr;
  }
#endif
}

void vec_fprint_c(FILE *stream, cf_t *x, int len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%+2.2f%+2.2fi, ", __real__ x[i], __imag__ x[i]);
    //if (!((i+1)%10))
    //  fprintf(stream, "\n");
  }
  fprintf(stream, "];\n");
}

void vec_fprint_f(FILE *stream, float *x, int len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%+2.2f, ", x[i]);
  }
  fprintf(stream, "];\n");
}


void vec_fprint_b(FILE *stream, char *x, int len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void vec_fprint_i(FILE *stream, int *x, int len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void vec_conj_cc(cf_t *x, cf_t *y, int len) {
#ifndef HAVE_VOLK_CONJ_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    y[i] = conjf(x[i]);
  }
#else
  volk_32fc_conjugate_32fc_u(y,x,(unsigned int) len);
#endif
}

void vec_prod_ccc(cf_t *x,cf_t *y, cf_t *z, int len) {
#ifndef HAVE_VOLK_MULT2_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*y[i];
  }
#else
  volk_32fc_x2_multiply_32fc_u(z,x,y,(unsigned int) len);
#endif
}

void vec_div_ccc(cf_t *x, cf_t *y, cf_t *z, int len) {
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i] / y[i];
  }
}

float vec_avg_power_cf(cf_t *x, int len) {
  int j;
  float power = 0;
  for (j=0;j<len;j++) {
    power += (__real__ x[j]) * (__real__ x[j]) +
        (__imag__ x[j]) * (__imag__ x[j]);
  }
  return power / len;
}

void vec_prod_ccc_unalign(cf_t *x,cf_t *y, cf_t *z, int len) {
#ifndef HAVE_VOLK_MULT_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*y[i];
  }
#else
  volk_32fc_x2_multiply_32fc_u(z,x,y,(unsigned int) len);
#endif
}

void vec_abs_cf(cf_t *x, float *abs, int len) {
#ifndef HAVE_VOLK_MAG_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    abs[i] = cabsf(x[i]);
  }
#else
  volk_32fc_magnitude_32f_u(abs,x,(unsigned int) len);

#endif

}

int vec_max_fi(float *x, int len) {
#ifdef HAVE_VOLK_MAX_FUNCTION
  unsigned int target=0;
  volk_32f_index_max_16u_u(&target,x,(unsigned int) len);
  return (int) target;

#else
  int i;
  float m=-FLT_MAX;
  int p=0;
  for (i=0;i<len;i++) {
    if (x[i]>m) {
      m=x[i];
      p=i;
    }
  }
  return p;
#endif
}

void vec_quant_fuc(float *in, unsigned char *out, float gain, float offset, float clip, int len) {
  int i;
  int tmp;
  for (i=0;i<len;i++) {
    tmp = (int) (offset + gain * in[i]);
    if (tmp < 0)
      tmp = 0;
    if (tmp > clip)
      tmp = clip;
    out[i] = (unsigned char) tmp;
  }

}

