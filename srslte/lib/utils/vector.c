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


#include <float.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "srslte/utils/vector.h"
#include "srslte/utils/vector_simd.h"
#include "srslte/utils/bit.h"

#ifdef HAVE_VOLK
#include "volk/volk.h"
#endif

int srslte_vec_acc_ii(int *x, uint32_t len) {
  int i;
  int z=0;
  for (i=0;i<len;i++) {
    z+=x[i];
  }
  return z;
}

float srslte_vec_acc_ff(float *x, uint32_t len) {
#ifdef HAVE_VOLK_ACC_FUNCTION
  float result;
  volk_32f_accumulator_s32f(&result,x,len);
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

void srslte_vec_ema_filter(cf_t *new_data, cf_t *average, cf_t *output, float coeff, uint32_t len) {
  srslte_vec_sc_prod_cfc(new_data, coeff, new_data, len);
  srslte_vec_sc_prod_cfc(average, 1-coeff, output, len);
  srslte_vec_sum_ccc(output, new_data, output, len);
}

cf_t srslte_vec_acc_cc(cf_t *x, uint32_t len) {
  int i;
  cf_t z=0;
  for (i=0;i<len;i++) {
    z+=x[i];
  }
  return z;
}

void srslte_vec_square_dist(cf_t symbol, cf_t *points, float *distance, uint32_t npoints) {
#ifndef HAVE_VOLK_SQUARE_DIST_FUNCTION
  uint32_t i;
  cf_t diff; 
  for (i=0;i<npoints;i++) {
    diff = symbol - points[i];
    distance[i] = crealf(diff) * crealf(diff) + cimagf(diff) * cimagf(diff);
  }
#else
  volk_32fc_x2_square_dist_32f(distance,&symbol,points,npoints);
#endif 
}

void srslte_vec_sub_fff(float *x, float *y, float *z, uint32_t len) {
#ifndef HAVE_VOLK_SUB_FLOAT_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]-y[i];
  }
#else
  volk_32f_x2_subtract_32f(z,x,y,len);
#endif 
}

void srslte_vec_sub_sss(short *x, short *y, short *z, uint32_t len) {
#ifndef LV_HAVE_SSE
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]-y[i];
  }
#else
  srslte_vec_sub_sss_simd(x, y, z, len);
#endif
}

void srslte_vec_sub_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len) {
  return srslte_vec_sub_fff((float*) x,(float*) y,(float*) z, 2*len);
}

void srslte_vec_sum_fff(float *x, float *y, float *z, uint32_t len) {
#ifndef HAVE_VOLK_ADD_FLOAT_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]+y[i];
  }
#else
  volk_32f_x2_add_32f(z,x,y,len);
#endif
}

void srslte_vec_sum_sss(short *x, short *y, short *z, uint32_t len) {
#ifndef LV_HAVE_SSE
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]+y[i];
  }
#else
  srslte_vec_sum_sss_simd(x, y, z, len);
#endif
}

void srslte_vec_sum_ccc(cf_t *x, cf_t *y, cf_t *z, uint32_t len) {
  srslte_vec_sum_fff((float*) x,(float*) y,(float*) z,2*len);
}

void srslte_vec_sum_bbb(uint8_t *x, uint8_t *y, uint8_t *z, uint32_t len) {
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]+y[i];
  }
}

void srslte_vec_sc_add_fff(float *x, float h, float *z, uint32_t len) {
  int i; 
  for (i=0;i<len;i++) {
    z[i] += h;
  }
}

void srslte_vec_sc_add_cfc(cf_t *x, float h, cf_t *z, uint32_t len) {
  int i; 
  for (i=0;i<len;i++) {
    z[i] += h;
  }
}

void srslte_vec_sc_add_ccc(cf_t *x, cf_t h, cf_t *z, uint32_t len) {
  int i; 
  for (i=0;i<len;i++) {
    z[i] += h;
  }
}

void srslte_vec_sc_add_sss(int16_t *x, int16_t h, int16_t *z, uint32_t len) {
  int i; 
  for (i=0;i<len;i++) {
    z[i] += h;
  }
}

void srslte_vec_sc_prod_fff(float *x, float h, float *z, uint32_t len) {
#ifndef HAVE_VOLK_MULT_FLOAT_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*h;
  }
#else
  volk_32f_s32f_multiply_32f(z,x,h,len);
#endif
}

void srslte_vec_sc_prod_sfs(short *x, float h, short *z, uint32_t len) {
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*h;
  }
}

void srslte_vec_sc_div2_sss(short *x, int n_rightshift, short *z, uint32_t len) {
#ifndef LV_HAVE_SSE
  int i;
  int pow2_div = 1<<n_rightshift;
  for (i=0;i<len;i++) {
    z[i] = x[i]/pow2_div;
  }
#else
  srslte_vec_sc_div2_sss_simd(x, n_rightshift, z, len);
#endif
}

// TODO: Improve this implementation
void srslte_vec_norm_cfc(cf_t *x, float amplitude, cf_t *y, uint32_t len) {
  // We should use fabs() here but is statistically should be similar
  float *xp = (float*) x; 
  uint32_t idx = srslte_vec_max_fi(xp, 2*len);
  float max = xp[idx]; 

  // Normalize before TX 
  srslte_vec_sc_prod_cfc(x, amplitude/max, y, len);
}

void srslte_vec_sc_prod_cfc(cf_t *x, float h, cf_t *z, uint32_t len) {
#ifndef HAVE_VOLK_MULT_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*h;
  }
#else
  cf_t hh;
  __real__ hh = h;
  __imag__ hh = 0;
  volk_32fc_s32fc_multiply_32fc(z,x,hh,len);
#endif
}

void srslte_vec_sc_prod_ccc(cf_t *x, cf_t h, cf_t *z, uint32_t len) {
#ifndef HAVE_VOLK_MULT_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*h;
  }
#else
  volk_32fc_s32fc_multiply_32fc(z,x,h,len);
#endif
}

void srslte_vec_convert_if(int16_t *x, float *z, float scale, uint32_t len) {
#ifndef HAVE_VOLK_CONVERT_IF_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = ((float) x[i])/scale;
  }
#else
  volk_16i_s32f_convert_32f(z,x,scale,len);
#endif  
}


void srslte_vec_convert_ci(int8_t *x, int16_t *z, uint32_t len) {
#ifndef HAVE_VOLK_CONVERT_CI_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = ((int16_t) x[i]);
  }
#else
  volk_8i_convert_16i(z,x,len);
#endif  
}

void srslte_vec_convert_fi(float *x, int16_t *z, float scale, uint32_t len) {
#ifndef LV_HAVE_SSE
  int i;
  for (i=0;i<len;i++) {
    z[i] = (int16_t) (x[i]*scale);
  }
#else 
  srslte_vec_convert_fi_simd(x, z, scale, len);
#endif
}

void srslte_vec_lut_fuf(float *x, uint32_t *lut, float *y, uint32_t len) {
  for (int i=0;i<len;i++) {
    y[lut[i]] = x[i];
  }
}

void srslte_vec_lut_sss(short *x, unsigned short *lut, short *y, uint32_t len) {
#ifndef LV_HAVE_SSE
  for (int i=0;i<len;i++) {
    y[lut[i]] = x[i];
  }
#else
  srslte_vec_lut_sss_simd(x, lut, y, len);
#endif
}

void srslte_vec_interleave_cf(float *real, float *imag, cf_t *x, uint32_t len) {
 #ifdef HAVE_VOLK_INTERLEAVE_FUNCTION
  volk_32f_x2_interleave_32fc(x, real, imag, len);
#else 
  int i;
  for (i=0;i<len;i++) {
    x[i] = real[i] + _Complex_I*imag[i];
  }
#endif 
}

void srslte_vec_deinterleave_cf(cf_t *x, float *real, float *imag, uint32_t len) {
 #ifdef HAVE_VOLK_DEINTERLEAVE_FUNCTION
  volk_32fc_deinterleave_32f_x2(real, imag, x, len);
#else 
  int i;
  for (i=0;i<len;i++) {
    real[i] = __real__ x[i];
    imag[i] = __imag__ x[i];
  }
#endif 
}

void srslte_vec_deinterleave_real_cf(cf_t *x, float *real, uint32_t len) {
#ifdef HAVE_VOLK_DEINTERLEAVE_REAL_FUNCTION
  volk_32fc_deinterleave_real_32f(real, x, len);
#else 
  int i;
  for (i=0;i<len;i++) {
    real[i] = __real__ x[i];
  }
#endif  
}

/* Note: We align memory to 32 bytes (for AVX compatibility) 
 * because in some cases volk can incorrectly detect the architecture. 
 * This could be inefficient for SSE or non-SIMD platforms but shouldn't 
 * be a huge problem. 
 */
void *srslte_vec_malloc(uint32_t size) {
  void *ptr;
  if (posix_memalign(&ptr,32,size)) {
    return NULL;
  } else {
    return ptr;
  }
}

void *srslte_vec_realloc(void *ptr, uint32_t old_size, uint32_t new_size) {
#ifndef HAVE_VOLK
  return realloc(ptr, new_size);
#else
  void *new_ptr;
  if (posix_memalign(&new_ptr,volk_get_alignment(),new_size)) {
    return NULL;
  } else {
    memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return new_ptr;
  }
#endif
}


void srslte_vec_fprint_c(FILE *stream, cf_t *x, uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%+2.2f%+2.2fi, ", __real__ x[i], __imag__ x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_f(FILE *stream, float *x, uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%+2.2f, ", x[i]);
  }
  fprintf(stream, "];\n");
}


void srslte_vec_fprint_b(FILE *stream, uint8_t *x, uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_byte(FILE *stream, uint8_t *x, uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%02x ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_i(FILE *stream, int *x, uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_s(FILE *stream, short *x, uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_hex(FILE *stream, uint8_t *x, uint32_t len) {
  uint32_t i, nbytes; 
  uint8_t byte;
  nbytes = len/8;
  fprintf(stream, "[");
  for (i=0;i<nbytes;i++) {
    byte = (uint8_t) srslte_bit_pack(&x, 8);
    fprintf(stream, "%02x ", byte);
  }
  if (len%8) {
    byte = (uint8_t) srslte_bit_pack(&x, len%8)<<(8-(len%8));
    fprintf(stream, "%02x ", byte);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_sprint_hex(char *str, uint8_t *x, uint32_t len) {
  uint32_t i, nbytes; 
  uint8_t byte;
  nbytes = len/8;
  int n=0;
  n+=sprintf(&str[n], "[");
  for (i=0;i<nbytes;i++) {
    byte = (uint8_t) srslte_bit_pack(&x, 8);
    n+=sprintf(&str[n], "%02x ", byte);
  }
  if (len%8) {
    byte = (uint8_t) srslte_bit_pack(&x, len%8)<<(8-(len%8));
    n+=sprintf(&str[n], "%02x ", byte);
  }
  n+=sprintf(&str[n], "]");
}

void srslte_vec_save_file(char *filename, void *buffer, uint32_t len) {
  FILE *f; 
  f = fopen(filename, "w");
  if (f) {
    fwrite(buffer, len, 1, f);
    fclose(f);
  } else {
    perror("fopen");
  }  
}

void srslte_vec_load_file(char *filename, void *buffer, uint32_t len) {
  FILE *f; 
  f = fopen(filename, "r");
  if (f) {
    fread(buffer, len, 1, f);
    fclose(f);
  } else {
    perror("fopen");
  }  
}


void srslte_vec_conj_cc(cf_t *x, cf_t *y, uint32_t len) {
#ifndef HAVE_VOLK_CONJ_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    y[i] = conjf(x[i]);
  }
#else
  volk_32fc_conjugate_32fc(y,x,len);
#endif
}

void srslte_vec_prod_cfc(cf_t *x, float *y, cf_t *z, uint32_t len) {
#ifndef HAVE_VOLK_MULT_REAL_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*y[i];
  }
#else
  volk_32fc_32f_multiply_32fc(z,x,y,len);
#endif
}

void srslte_vec_prod_fff(float *x, float *y, float *z, uint32_t len) {
#ifndef HAVE_VOLK_MULT_REAL2_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*y[i];
  }
#else
  volk_32f_x2_multiply_32f(z,x,y,len);
#endif
}

void srslte_vec_prod_sss(short *x, short *y, short *z, uint32_t len) {
#ifndef LV_HAVE_SSE
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*y[i];
  }
#else
  srslte_vec_prod_sss_simd(x,y,z,len);
#endif
}

void srslte_vec_prod_ccc(cf_t *x,cf_t *y, cf_t *z, uint32_t len) {
#ifndef HAVE_VOLK_MULT2_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*y[i];
  }
#else
  volk_32fc_x2_multiply_32fc(z,x,y,len);
#endif
}


void srslte_vec_prod_conj_ccc(cf_t *x,cf_t *y, cf_t *z, uint32_t len) {
#ifndef HAVE_VOLK_MULT2_CONJ_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i]*conjf(y[i]);
  }
#else
  volk_32fc_x2_multiply_conjugate_32fc(z,x,y,len);
#endif
}

#define DIV_USE_VEC

/* Complex division is conjugate multiplication + real division */
void srslte_vec_div_ccc(cf_t *x, cf_t *y, float *y_mod, cf_t *z, float *z_real, float *z_imag, uint32_t len) {
#ifdef DIV_USE_VEC
  srslte_vec_prod_conj_ccc(x,y,z,len);
  srslte_vec_abs_square_cf(y,y_mod,len);
  srslte_vec_div_cfc(z,y_mod,z,z_real,z_imag,len);  
#else 
  int i; 
  for (i=0;i<len;i++) {
    z[i] = x[i] / y[i]; 
  }
#endif
}

/* Complex division by float z=x/y */
void srslte_vec_div_cfc(cf_t *x, float *y, cf_t *z, float *z_real, float *z_imag, uint32_t len) {
#ifdef DIV_USE_VEC
  srslte_vec_deinterleave_cf(x, z_real, z_imag, len);
  srslte_vec_div_fff(z_real, y, z_real, len);
  srslte_vec_div_fff(z_imag, y, z_imag, len);
  srslte_vec_interleave_cf(z_real, z_imag, z, len);
#else
  int i; 
  for (i=0;i<len;i++) {
    z[i] = x[i] / y[i]; 
  }
#endif
}

void srslte_vec_div_fff(float *x, float *y, float *z, uint32_t len) {
#ifdef HAVE_VOLK_DIVIDE_FUNCTION
  volk_32f_x2_divide_32f(z, x, y, len);
#else
  int i;
  for (i=0;i<len;i++) {
    z[i] = x[i] / y[i];
  }
#endif
}

cf_t srslte_vec_dot_prod_ccc(cf_t *x, cf_t *y, uint32_t len) {
#ifdef HAVE_VOLK_DOTPROD_FC_FUNCTION
  cf_t res;
  volk_32fc_x2_dot_prod_32fc(&res, x, y, len);
  return res; 
#else 
  uint32_t i;
  cf_t res = 0;
  for (i=0;i<len;i++) {
    res += x[i]*y[i];
  }
  return res;
#endif
}

cf_t srslte_vec_dot_prod_cfc(cf_t *x, float *y, uint32_t len) {
#ifdef HAVE_VOLK_DOTPROD_CFC_FUNCTION
  cf_t res;
  volk_32fc_32f_dot_prod_32fc(&res, x, y, len);
  return res; 
#else  
  uint32_t i;
  cf_t res = 0;
  for (i=0;i<len;i++) {
    res += x[i]*y[i];
  }
  return res;
#endif
}

cf_t srslte_vec_dot_prod_conj_ccc(cf_t *x, cf_t *y, uint32_t len) {
#ifdef HAVE_VOLK_DOTPROD_CONJ_FC_FUNCTION
  cf_t res;
  volk_32fc_x2_conjugate_dot_prod_32fc(&res, x, y, len);
  return res; 
#else 
  uint32_t i;
  cf_t res = 0;
  for (i=0;i<len;i++) {
    res += x[i]*conjf(y[i]);
  }
  return res;
#endif
}


float srslte_vec_dot_prod_fff(float *x, float *y, uint32_t len) {
#ifdef HAVE_VOLK_DOTPROD_F_FUNCTION
  float res;
  volk_32f_x2_dot_prod_32f(&res, x, y, len);
  return res; 
#else 
  uint32_t i;
  float res = 0;
  for (i=0;i<len;i++) {
    res += x[i]*y[i];
  }
  return res;
#endif  
}

int32_t srslte_vec_dot_prod_sss(int16_t *x, int16_t *y, uint32_t len) {
#ifndef LV_HAVE_SSE
  uint32_t i;
  int32_t res = 0;
  for (i=0;i<len;i++) {
    res += x[i]*y[i];
  }
  return res;
#else
  return srslte_vec_dot_prod_sss_simd(x, y, len); 
#endif
}

float srslte_vec_avg_power_cf(cf_t *x, uint32_t len) {
  return crealf(srslte_vec_dot_prod_conj_ccc(x,x,len)) / len;
}

void srslte_vec_abs_cf(cf_t *x, float *abs, uint32_t len) {
#ifndef HAVE_VOLK_MAG_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    abs[i] = cabsf(x[i]);
  }
#else
  volk_32fc_magnitude_32f(abs,x,len);
#endif
}
void srslte_vec_abs_square_cf(cf_t *x, float *abs_square, uint32_t len) {
#ifndef HAVE_VOLK_MAG_SQUARE_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    abs_square[i] = crealf(x[i])*crealf(x[i])+cimagf(x[i])*cimagf(x[i]);
  }
#else
  volk_32fc_magnitude_squared_32f(abs_square,x,len);
#endif
}


void srslte_vec_arg_cf(cf_t *x, float *arg, uint32_t len) {
#ifndef HAVE_VOLK_ATAN_FUNCTION
  int i;
  for (i=0;i<len;i++) {
    arg[i] = cargf(x[i]);
  }
#else
  volk_32fc_s32f_atan2_32f(arg,x,1,len);

#endif

}

uint32_t srslte_vec_max_fi(float *x, uint32_t len) {
#ifdef HAVE_VOLK_MAX_FUNCTION
  uint32_t target=0;
  volk_32f_index_max_16u(&target,x,len);
  return target;

#else
  uint32_t i;
  float m=-FLT_MAX;
  uint32_t p=0;
  for (i=0;i<len;i++) {
    if (x[i]>m) {
      m=x[i];
      p=i;
    }
  }
  return p;
#endif
}

int16_t srslte_vec_max_star_si(int16_t *x, uint32_t len) {
#ifdef HAVE_VOLK_MAX_STAR_S_FUNCTION
  int16_t target=0;
  volk_16i_max_star_16i(&target,x,len);
  return target;

#else
  uint32_t i;
  int16_t m=-INT16_MIN;
  for (i=0;i<len;i++) {
    if (x[i]>m) {
      m=x[i];
    }
  }
  return m;
#endif
}

void srslte_vec_max_fff(float *x, float *y, float *z, uint32_t len) {
#ifdef HAVE_VOLK_MAX_VEC_FUNCTION
  volk_32f_x2_max_32f(z,x,y,len);
#else
  uint32_t i; 
  for (i=0;i<len;i++) {
    if (x[i] > y[i]) {
      z[i] = x[i]; 
    } else {
      z[i] = y[i]; 
    }
  }
#endif  
}


uint32_t srslte_vec_max_abs_ci(cf_t *x, uint32_t len) {
#ifdef HAVE_VOLK_MAX_ABS_FUNCTION
  uint32_t target=0;
  volk_32fc_index_max_16u(&target,x,len);
  return target;

#else
  uint32_t i;
  float m=-FLT_MAX;
  uint32_t p=0;
  float tmp;
  for (i=0;i<len;i++) {
    tmp = crealf(x[i])*crealf(x[i]) + cimagf(x[i])*cimagf(x[i]);
    if (tmp>m) {
      m=tmp;
      p=i;
    }
  }
  return p;
#endif
}

void srslte_vec_quant_fuc(float *in, uint8_t *out, float gain, float offset, float clip, uint32_t len) {
  int i;
  int tmp;
  
  for (i=0;i<len;i++) {
    tmp = (int) (offset + gain * in[i]);
    if (tmp < 0)
      tmp = 0;
    if (tmp > clip)
      tmp = clip;
    out[i] = (uint8_t) tmp;    
  }
}

void srslte_vec_quant_suc(int16_t *in, uint8_t *out, int16_t norm, int16_t offset, int16_t clip, uint32_t len) {
  int i;
  int16_t tmp;
  
  for (i=0;i<len;i++) {
    tmp = (int16_t) (offset + in[i]/norm);
    if (tmp < 0)
      tmp = 0;
    if (tmp > clip)
      tmp = clip;
    out[i] = (uint8_t) tmp;    
  }
}

