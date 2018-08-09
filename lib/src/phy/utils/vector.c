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

#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/vector_simd.h"
#include "srslte/phy/utils/bit.h"



void srslte_vec_xor_bbb(int8_t *x,int8_t *y,int8_t *z, const uint32_t len) {
  srslte_vec_xor_bbb_simd(x, y, z, len);
}

// Used in PRACH detector, AGC and chest_dl for noise averaging
float srslte_vec_acc_ff(const float *x, const uint32_t len) {
  return srslte_vec_acc_ff_simd(x, len);
}

cf_t srslte_vec_acc_cc(const cf_t *x, const uint32_t len) {
  return srslte_vec_acc_cc_simd(x, len);
}

void srslte_vec_sub_fff(const float *x, const float *y, float *z, const uint32_t len) {
  srslte_vec_sub_fff_simd(x, y, z, len);
}

void srslte_vec_sub_sss(const int16_t *x, const int16_t *y, int16_t *z, const uint32_t len) {
  srslte_vec_sub_sss_simd(x, y, z, len);
}

// Noise estimation in chest_dl, interpolation 
void srslte_vec_sub_ccc(const cf_t *x, const cf_t *y, cf_t *z, const uint32_t len) {
  return srslte_vec_sub_fff((const float*) x,(const float*) y,(float*) z, 2*len);
}

// Used in PSS/SSS and sum_ccc
void srslte_vec_sum_fff(const float *x, const float *y, float *z, const uint32_t len) {
  srslte_vec_add_fff_simd(x, y, z, len);
}

void srslte_vec_sum_sss(const int16_t *x, const int16_t *y, int16_t *z, const uint32_t len) {
  srslte_vec_sum_sss_simd(x, y, z, len);
}

void srslte_vec_sum_ccc(const cf_t *x, const cf_t *y, cf_t *z, const uint32_t len) {
  srslte_vec_sum_fff((float*) x,(float*) y,(float*) z,2*len);
}

// PSS, PBCH, DEMOD, FFTW, etc.
void srslte_vec_sc_prod_fff(const float *x, const float h, float *z, const uint32_t len) {
  srslte_vec_sc_prod_fff_simd(x, h, z, len);
}

// Used throughout 
void srslte_vec_sc_prod_cfc(const cf_t *x, const float h, cf_t *z, const uint32_t len) {
  srslte_vec_sc_prod_cfc_simd(x,h,z,len);
}

// Chest UL 
void srslte_vec_sc_prod_ccc(const cf_t *x, const cf_t h, cf_t *z, const uint32_t len) {
  srslte_vec_sc_prod_ccc_simd(x,h,z,len);
}

// Used in turbo decoder 
void srslte_vec_convert_if(const int16_t *x, const float scale, float *z, const uint32_t len) {
  srslte_vec_convert_if_simd(x, z, scale, len);
}

void srslte_vec_convert_fi(const float *x, const float scale, int16_t *z, const uint32_t len) {
  srslte_vec_convert_fi_simd(x, z, scale, len);
}

void srslte_vec_lut_sss(const short *x, const unsigned short *lut, short *y, const uint32_t len) {
  srslte_vec_lut_sss_simd(x, lut, y, len);
}

void srslte_vec_lut_sis(const short *x, const unsigned int *lut, short *y, const uint32_t len) {
  for (int i=0; i < len; i++) {
    y[lut[i]] = x[i];
  }
}

void *srslte_vec_malloc(uint32_t size) {
  void *ptr;
  if (posix_memalign(&ptr, SRSLTE_SIMD_BIT_ALIGN, size)) {
    return NULL;
  } else {
    return ptr;
  }
}

void *srslte_vec_realloc(void *ptr, uint32_t old_size, uint32_t new_size) {
#ifndef LV_HAVE_SSE
  return realloc(ptr, new_size);
#else
  void *new_ptr;
  if (posix_memalign(&new_ptr, SRSLTE_SIMD_BIT_ALIGN, new_size)) {
    return NULL;
  } else {
    memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return new_ptr;
  }
#endif
}


void srslte_vec_fprint_c(FILE *stream, cf_t *x, const uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%+2.5f%+2.5fi, ", __real__ x[i], __imag__ x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_f(FILE *stream, float *x, const uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%+2.2f, ", x[i]);
  }
  fprintf(stream, "];\n");
}


void srslte_vec_fprint_b(FILE *stream, uint8_t *x, const uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_byte(FILE *stream, uint8_t *x, const uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%02x ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_i(FILE *stream, int *x, const uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_s(FILE *stream, short *x, const uint32_t len) {
  int i;
  fprintf(stream, "[");
  for (i=0;i<len;i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srslte_vec_fprint_hex(FILE *stream, uint8_t *x, const uint32_t len) {
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

void srslte_vec_sprint_hex(char *str, const uint32_t max_str_len, uint8_t *x, const uint32_t len) {
  uint32_t i, nbytes; 
  uint8_t byte;
  nbytes = len/8;
  // check that hex string fits in buffer (every byte takes 3 characters, plus brackets)
  if ((3*(len/8 + ((len%8)?1:0))) + 2 >= max_str_len) {
    fprintf(stderr, "Buffer too small for printing hex string (max_str_len=%d, payload_len=%d).\n", max_str_len, len);
    return;
  }

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
  str[max_str_len-1] = 0;
}

void srslte_vec_save_file(char *filename, const void *buffer, const uint32_t len) {
  FILE *f; 
  f = fopen(filename, "w");
  if (f) {
    fwrite(buffer, len, 1, f);
    fclose(f);
  } else {
    perror("fopen");
  }  
}

void srslte_vec_load_file(char *filename, void *buffer, const uint32_t len) {
  FILE *f; 
  f = fopen(filename, "r");
  if (f) {
    fread(buffer, len, 1, f);
    fclose(f);
  } else {
    perror("fopen");
  }  
}

// Used in PSS
void srslte_vec_conj_cc(const cf_t *x, cf_t *y, const uint32_t len) {
  /* This function is used in initialisation only, then no optimisation is required */
  int i;
  for (i=0;i<len;i++) {
    y[i] = conjf(x[i]);
  }
}

// Used in scrambling complex 
void srslte_vec_prod_cfc(const cf_t *x, const float *y, cf_t *z, const uint32_t len) {
  srslte_vec_prod_cfc_simd(x, y, z, len);
}

// Used in scrambling float
void srslte_vec_prod_fff(const float *x, const float *y, float *z, const uint32_t len) {
  srslte_vec_prod_fff_simd(x, y, z, len);
}

// Scrambling Short
void srslte_vec_prod_sss(const int16_t *x, const int16_t *y, int16_t *z, const uint32_t len) {
  srslte_vec_prod_sss_simd(x,y,z,len);
}

// CFO and OFDM processing
void srslte_vec_prod_ccc(const cf_t *x, const cf_t *y, cf_t *z, const uint32_t len) {
  srslte_vec_prod_ccc_simd(x,y,z,len);
}

void srslte_vec_prod_ccc_split(const float *x_re, const float *x_im, const float *y_re, const float *y_im,
                               float *z_re, float *z_im, const uint32_t len) {
  srslte_vec_prod_ccc_split_simd(x_re, x_im, y_re , y_im, z_re,z_im, len);
}

// PRACH, CHEST UL, etc. 
void srslte_vec_prod_conj_ccc(const cf_t *x, const cf_t *y, cf_t *z, const uint32_t len) {
  srslte_vec_prod_conj_ccc_simd(x,y,z,len);
}

//#define DIV_USE_VEC

// Used in SSS 
void srslte_vec_div_ccc(const cf_t *x, const cf_t *y, cf_t *z, const uint32_t len) {
  srslte_vec_div_ccc_simd(x, y, z, len);
}

/* Complex division by float z=x/y */
void srslte_vec_div_cfc(const cf_t *x, const float *y, cf_t *z, const uint32_t len) {
  srslte_vec_div_cfc_simd(x, y, z, len);
}

void srslte_vec_div_fff(const float *x, const float *y, float *z, const uint32_t len) {
  srslte_vec_div_fff_simd(x, y, z, len);
}

// PSS. convolution 
cf_t srslte_vec_dot_prod_ccc(const cf_t *x, const cf_t *y, const uint32_t len) {
  return srslte_vec_dot_prod_ccc_simd(x, y, len);
}

// Convolution filter and in SSS search 
cf_t srslte_vec_dot_prod_cfc(const cf_t *x, const float *y, const uint32_t len) {
  uint32_t i;
  cf_t res = 0;
  for (i=0;i<len;i++) {
    res += x[i]*y[i];
  }
  return res;
}

// SYNC 
cf_t srslte_vec_dot_prod_conj_ccc(const cf_t *x, const cf_t *y, const uint32_t len) {
  return srslte_vec_dot_prod_conj_ccc_simd(x, y, len);
}

// PHICH 
float srslte_vec_dot_prod_fff(const float *x, const float *y, const uint32_t len) {
  uint32_t i;
  float res = 0;
  for (i=0;i<len;i++) {
    res += x[i]*y[i];
  }
  return res;
}

int32_t srslte_vec_dot_prod_sss(const int16_t *x, const int16_t *y, const uint32_t len) {
  return srslte_vec_dot_prod_sss_simd(x, y, len);
}

float srslte_vec_avg_power_cf(const cf_t *x, const uint32_t len) {
  return crealf(srslte_vec_dot_prod_conj_ccc(x,x,len)) / len;
}

// Correlation assumes zero-mean x and y
float srslte_vec_corr_ccc(const cf_t *x, cf_t *y, const uint32_t len) {
//  return crealf(srslte_vec_dot_prod_conj_ccc(x,y,len)) / len;
  float s_x = crealf(srslte_vec_dot_prod_conj_ccc(x, x, len))/len;
  float s_y = crealf(srslte_vec_dot_prod_conj_ccc(y, y, len))/len;
  float cov = crealf(srslte_vec_dot_prod_conj_ccc(x, y, len))/len;
  return cov/(sqrtf(s_x*s_y));
}

// PSS (disabled and using abs_square )
void srslte_vec_abs_cf(const cf_t *x, float *abs, const uint32_t len) {
  srslte_vec_abs_cf_simd(x, abs, len);
}

// PRACH 
void srslte_vec_abs_square_cf(const cf_t *x, float *abs_square, const uint32_t len) {
  srslte_vec_abs_square_cf_simd(x,abs_square,len);
}

uint32_t srslte_vec_max_fi(const float *x, const uint32_t len) {
  return srslte_vec_max_fi_simd(x, len);
}

uint32_t srslte_vec_max_abs_fi(const float *x, const uint32_t len) {
  return srslte_vec_max_abs_fi_simd(x, len);
}

// CP autocorr
uint32_t srslte_vec_max_abs_ci(const cf_t *x, const uint32_t len) {
  return srslte_vec_max_ci_simd(x, len);
}

void srslte_vec_quant_fus(float *in, uint16_t *out, float gain, float offset, float clip, uint32_t len) {
  int i;
  long tmp;
  
  for (i=0;i<len;i++) {
    tmp = (long) (offset + gain * in[i]);
    if (tmp < 0)
      tmp = 0;
    if (tmp > clip)
      tmp = clip;
    out[i] = (uint16_t) tmp;    
  }
}

void srslte_vec_quant_fuc(const float *in, uint8_t *out, const float gain, const float offset, const float clip, const uint32_t len) {
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

void srslte_vec_quant_suc(const int16_t *in, uint8_t *out, const float gain, const int16_t offset, const int16_t clip, const uint32_t len) {
  int i;
  int16_t tmp;
  
  for (i=0;i<len;i++) {
    tmp = (int16_t) (offset + in[i]*gain);
    if (tmp < 0)
      tmp = 0;
    if (tmp > clip)
      tmp = clip;
    out[i] = (uint8_t) tmp;    
  }
}

void srslte_vec_quant_sus(const int16_t *in, uint16_t *out, const float gain, const int16_t offset, const uint32_t len) {
  int i;
  int16_t tmp;
  
  for (i=0;i<len;i++) {
    tmp =  (offset + in[i]*gain);
    if (tmp < 0)
      tmp = 0;
    out[i] = (uint16_t) tmp;    
  }
}

void srs_vec_cf_cpy(const cf_t *src, cf_t *dst, int len) {
  srslte_vec_cp_simd(src, dst, len);
}

void srslte_vec_interleave(const cf_t *x, const cf_t *y, cf_t *z, const int len) {
  srslte_vec_interleave_simd(x, y, z, len);
}

void srslte_vec_interleave_add(const cf_t *x, const cf_t *y, cf_t *z, const int len) {
  srslte_vec_interleave_add_simd(x, y, z, len);
}

void srslte_vec_apply_cfo(const cf_t *x, float cfo, cf_t *z, int len) {
  srslte_vec_apply_cfo_simd(x, cfo, z, len);
}
