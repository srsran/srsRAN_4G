/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <complex.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/simd.h"
#include "srsran/phy/utils/vector.h"
#include "srsran/phy/utils/vector_simd.h"

void srsran_vec_xor_bbb(const uint8_t* x, const uint8_t* y, uint8_t* z, const uint32_t len)
{
  srsran_vec_xor_bbb_simd(x, y, z, len);
}

// Used in PRACH detector, AGC and chest_dl for noise averaging
float srsran_vec_acc_ff(const float* x, const uint32_t len)
{
  return srsran_vec_acc_ff_simd(x, len);
}

cf_t srsran_vec_acc_cc(const cf_t* x, const uint32_t len)
{
  return srsran_vec_acc_cc_simd(x, len);
}

void srsran_vec_sub_fff(const float* x, const float* y, float* z, const uint32_t len)
{
  srsran_vec_sub_fff_simd(x, y, z, len);
}

void srsran_vec_sub_sss(const int16_t* x, const int16_t* y, int16_t* z, const uint32_t len)
{
  srsran_vec_sub_sss_simd(x, y, z, len);
}

void srsran_vec_sub_bbb(const int8_t* x, const int8_t* y, int8_t* z, const uint32_t len)
{
  srsran_vec_sub_bbb_simd(x, y, z, len);
}

// Noise estimation in chest_dl, interpolation
void srsran_vec_sub_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len)
{
  return srsran_vec_sub_fff((const float*)x, (const float*)y, (float*)z, 2 * len);
}

// Used in PSS/SSS and sum_ccc
void srsran_vec_sum_fff(const float* x, const float* y, float* z, const uint32_t len)
{
  srsran_vec_add_fff_simd(x, y, z, len);
}

void srsran_vec_sum_sss(const int16_t* x, const int16_t* y, int16_t* z, const uint32_t len)
{
  srsran_vec_sum_sss_simd(x, y, z, len);
}

void srsran_vec_sum_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len)
{
  srsran_vec_sum_fff((float*)x, (float*)y, (float*)z, 2 * len);
}

// PSS, PBCH, DEMOD, FFTW, etc.
void srsran_vec_sc_prod_fff(const float* x, const float h, float* z, const uint32_t len)
{
  srsran_vec_sc_prod_fff_simd(x, h, z, len);
}

// Used throughout
void srsran_vec_sc_prod_cfc(const cf_t* x, const float h, cf_t* z, const uint32_t len)
{
  srsran_vec_sc_prod_cfc_simd(x, h, z, len);
}

void srsran_vec_sc_prod_fcc(const float* x, const cf_t h, cf_t* z, const uint32_t len)
{
  srsran_vec_sc_prod_fcc_simd(x, h, z, len);
}

// Chest UL
void srsran_vec_sc_prod_ccc(const cf_t* x, const cf_t h, cf_t* z, const uint32_t len)
{
  srsran_vec_sc_prod_ccc_simd(x, h, z, len);
}

// Used in turbo decoder
void srsran_vec_convert_if(const int16_t* x, const float scale, float* z, const uint32_t len)
{
  srsran_vec_convert_if_simd(x, z, scale, len);
}

void srsran_vec_convert_fi(const float* x, const float scale, int16_t* z, const uint32_t len)
{
  srsran_vec_convert_fi_simd(x, z, scale, len);
}

void srsran_vec_convert_conj_cs(const cf_t* x, const float scale, int16_t* z, const uint32_t len)
{
  srsran_vec_convert_conj_cs_simd(x, z, scale, len);
}

void srsran_vec_convert_fb(const float* x, const float scale, int8_t* z, const uint32_t len)
{
  srsran_vec_convert_fb_simd(x, z, scale, len);
}

void srsran_vec_lut_sss(const short* x, const unsigned short* lut, short* y, const uint32_t len)
{
  srsran_vec_lut_sss_simd(x, lut, y, len);
}

void srsran_vec_lut_bbb(const int8_t* x, const unsigned short* lut, int8_t* y, const uint32_t len)
{
  srsran_vec_lut_bbb_simd(x, lut, y, len);
}

void srsran_vec_lut_sis(const short* x, const unsigned int* lut, short* y, const uint32_t len)
{
  for (int i = 0; i < len; i++) {
    y[lut[i]] = x[i];
  }
}

void* srsran_vec_malloc(uint32_t size)
{
  void* ptr;
  if (posix_memalign(&ptr, SRSRAN_SIMD_BIT_ALIGN, size)) {
    return NULL;
  } else {
    return ptr;
  }
}

cf_t* srsran_vec_cf_malloc(uint32_t nsamples)
{
  return SRSRAN_MEM_ALLOC(cf_t, nsamples);
}

float* srsran_vec_f_malloc(uint32_t nsamples)
{
  return SRSRAN_MEM_ALLOC(float, nsamples);
}

int32_t* srsran_vec_i32_malloc(uint32_t nsamples)
{
  return SRSRAN_MEM_ALLOC(int32_t, nsamples);
}

uint32_t* srsran_vec_u32_malloc(uint32_t nsamples)
{
  return SRSRAN_MEM_ALLOC(uint32_t, nsamples);
}

int16_t* srsran_vec_i16_malloc(uint32_t nsamples)
{
  return SRSRAN_MEM_ALLOC(int16_t, nsamples);
}

uint16_t* srsran_vec_u16_malloc(uint32_t nsamples)
{
  return SRSRAN_MEM_ALLOC(uint16_t, nsamples);
}

int8_t* srsran_vec_i8_malloc(uint32_t nsamples)
{
  return SRSRAN_MEM_ALLOC(int8_t, nsamples);
}

uint8_t* srsran_vec_u8_malloc(uint32_t nsamples)
{
  return SRSRAN_MEM_ALLOC(uint8_t, nsamples);
}

void srsran_vec_zero(void* ptr, uint32_t nbytes)
{
  memset(ptr, 0, nbytes);
}

void srsran_vec_u8_zero(uint8_t* ptr, uint32_t nsamples)
{
  SRSRAN_MEM_ZERO(ptr, uint8_t, nsamples);
}

void srsran_vec_i8_zero(int8_t* ptr, uint32_t nsamples)
{
  SRSRAN_MEM_ZERO(ptr, int8_t, nsamples);
}

void srsran_vec_i16_zero(int16_t* ptr, uint32_t nsamples)
{
  SRSRAN_MEM_ZERO(ptr, int16_t, nsamples);
}

void srsran_vec_u32_zero(uint32_t* ptr, uint32_t nsamples)
{
  SRSRAN_MEM_ZERO(ptr, uint32_t, nsamples);
}

void srsran_vec_cf_zero(cf_t* ptr, uint32_t nsamples)
{
  SRSRAN_MEM_ZERO(ptr, cf_t, nsamples);
}

void srsran_vec_f_zero(float* ptr, uint32_t nsamples)
{
  SRSRAN_MEM_ZERO(ptr, float, nsamples);
}

void srsran_vec_cf_copy(cf_t* dst, const cf_t* src, uint32_t len)
{
  memcpy(dst, src, sizeof(cf_t) * len);
}

void srsran_vec_f_copy(float* dst, const float* src, uint32_t len)
{
  memcpy(dst, src, sizeof(float) * len);
}

void srsran_vec_u8_copy(uint8_t* dst, const uint8_t* src, uint32_t len)
{
  memcpy(dst, src, sizeof(uint8_t) * len);
}

void srsran_vec_i8_copy(int8_t* dst, const int8_t* src, uint32_t len)
{
  memcpy(dst, src, sizeof(int8_t) * len);
}

void srsran_vec_i16_copy(int16_t* dst, const int16_t* src, uint32_t len)
{
  memcpy(dst, src, sizeof(int16_t) * len);
}

void srsran_vec_u16_copy(uint16_t* dst, const uint16_t* src, uint32_t len)
{
  memcpy(dst, src, sizeof(uint16_t) * len);
}

void* srsran_vec_realloc(void* ptr, uint32_t old_size, uint32_t new_size)
{
#ifndef LV_HAVE_SSE
  return realloc(ptr, new_size);
#else
  void* new_ptr;
  if (posix_memalign(&new_ptr, SRSRAN_SIMD_BIT_ALIGN, new_size)) {
    return NULL;
  } else {
    memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return new_ptr;
  }
#endif
}

void srsran_vec_fprint_c(FILE* stream, const cf_t* x, const uint32_t len)
{
  int i;
  fprintf(stream, "[");
  for (i = 0; i < len; i++) {
    fprintf(stream, "%+2.5f%+2.5fi, ", __real__ x[i], __imag__ x[i]);
  }
  fprintf(stream, "];\n");
}

void srsran_vec_fprint_f(FILE* stream, const float* x, const uint32_t len)
{
  int i;
  fprintf(stream, "[");
  for (i = 0; i < len; i++) {
    fprintf(stream, "%+2.2f, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srsran_vec_fprint_b(FILE* stream, const uint8_t* x, const uint32_t len)
{
  int i;
  fprintf(stream, "[");
  for (i = 0; i < len; i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srsran_vec_fprint_bs(FILE* stream, const int8_t* x, const uint32_t len)
{
  int i;
  fprintf(stream, "[");
  for (i = 0; i < len; i++) {
    fprintf(stream, "%4d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srsran_vec_fprint_byte(FILE* stream, const uint8_t* x, const uint32_t len)
{
  int i;
  fprintf(stream, "[");
  for (i = 0; i < len; i++) {
    fprintf(stream, "%02x ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srsran_vec_fprint_i(FILE* stream, const int* x, const uint32_t len)
{
  int i;
  fprintf(stream, "[");
  for (i = 0; i < len; i++) {
    fprintf(stream, "%d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srsran_vec_fprint_s(FILE* stream, const int16_t* x, const uint32_t len)
{
  int i;
  fprintf(stream, "[");
  for (i = 0; i < len; i++) {
    fprintf(stream, "%4d, ", x[i]);
  }
  fprintf(stream, "];\n");
}

void srsran_vec_fprint_hex(FILE* stream, uint8_t* x, const uint32_t len)
{
  uint32_t i, nbytes;
  uint8_t  byte;
  nbytes = len / 8;
  fprintf(stream, "[");
  for (i = 0; i < nbytes; i++) {
    byte = (uint8_t)srsran_bit_pack(&x, 8);
    fprintf(stream, "%02x ", byte);
  }
  if (len % 8) {
    byte = (uint8_t)srsran_bit_pack(&x, len % 8) << (8 - (len % 8));
    fprintf(stream, "%02x ", byte);
  }
  fprintf(stream, "];\n");
}

void srsran_vec_sprint_hex(char* str, const uint32_t max_str_len, uint8_t* x, const uint32_t len)
{
  uint32_t i, nbytes;
  uint8_t  byte;
  nbytes = len / 8;
  // check that hex string fits in buffer (every byte takes 3 characters, plus brackets)
  if ((3 * (len / 8 + ((len % 8) ? 1 : 0))) + 2 >= max_str_len) {
    ERROR("Buffer too small for printing hex string (max_str_len=%d, payload_len=%d).", max_str_len, len);
    return;
  }

  int n = 0;
  n += sprintf(&str[n], "[");
  for (i = 0; i < nbytes; i++) {
    byte = (uint8_t)srsran_bit_pack(&x, 8);
    n += sprintf(&str[n], "%02x ", byte);
  }
  if (len % 8) {
    byte = (uint8_t)srsran_bit_pack(&x, len % 8) << (8 - (len % 8));
    n += sprintf(&str[n], "%02x ", byte);
  }
  n += sprintf(&str[n], "]");
  str[max_str_len - 1] = 0;
}

void srsran_vec_sprint_bin(char* str, const uint32_t max_str_len, const uint8_t* x, const uint32_t len)
{
  // Trim maximum size
  uint32_t N = SRSRAN_MIN(max_str_len - 1, len);

  // If the number of bits does not fit in the string, leave space for "..." if possible
  if (N < len) {
    if (N >= 3) {
      N -= 3;
    } else {
      N = 0;
    }
  }

  // Write 1s and 0s
  for (uint32_t i = 0; i < N; i++) {
    str[i] = x[i] == 0 ? '0' : '1';
  }

  // Write "..." if all 1s and 0s did not fit
  if (N < len) {
    for (uint32_t i = N; i < max_str_len - 1; i++) {
      str[i] = '.';
    }
    str[max_str_len - 1] = 0;
  } else {
    str[N] = 0;
  }
}

void srsran_vec_save_file(char* filename, const void* buffer, const uint32_t len)
{
  FILE* f;
  f = fopen(filename, "w");
  if (f) {
    fwrite(buffer, len, 1, f);
    fclose(f);
  } else {
    perror("fopen");
  }
}

#define SAFE_READ(PTR, SIZE, N, FILE)                                                                                  \
  do {                                                                                                                 \
    size_t nbytes = SIZE * N;                                                                                          \
    if (nbytes != fread(PTR, SIZE, N, FILE)) {                                                                         \
      perror("read");                                                                                                  \
      fclose(FILE);                                                                                                    \
      exit(1);                                                                                                         \
    }                                                                                                                  \
  } while (false)

void srsran_vec_load_file(char* filename, void* buffer, const uint32_t len)
{
  FILE* f;
  f = fopen(filename, "r");
  if (f) {
    SAFE_READ(buffer, len, 1, f);
    fclose(f);
  } else {
    perror("fopen");
  }
}

// Used in PSS
void srsran_vec_conj_cc(const cf_t* x, cf_t* y, const uint32_t len)
{
  /* This function is used in initialisation only, then no optimisation is required */
  int i;
  for (i = 0; i < len; i++) {
    y[i] = conjf(x[i]);
  }
}

// Used in scrambling complex
void srsran_vec_prod_cfc(const cf_t* x, const float* y, cf_t* z, const uint32_t len)
{
  srsran_vec_prod_cfc_simd(x, y, z, len);
}

// Used in scrambling float
void srsran_vec_prod_fff(const float* x, const float* y, float* z, const uint32_t len)
{
  srsran_vec_prod_fff_simd(x, y, z, len);
}

void srsran_vec_prod_sss(const int16_t* x, const int16_t* y, int16_t* z, const uint32_t len)
{
  srsran_vec_prod_sss_simd(x, y, z, len);
}

// Scrambling
void srsran_vec_neg_sss(const int16_t* x, const int16_t* y, int16_t* z, const uint32_t len)
{
  srsran_vec_neg_sss_simd(x, y, z, len);
}

void srsran_vec_neg_bbb(const int8_t* x, const int8_t* y, int8_t* z, const uint32_t len)
{
  srsran_vec_neg_bbb_simd(x, y, z, len);
}

void srsran_vec_neg_bb(const int8_t* x, int8_t* z, const uint32_t len)
{
  for (uint32_t i = 0; i < len; i++) {
    z[i] = -x[i];
  }
}

// CFO and OFDM processing
void srsran_vec_prod_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len)
{
  srsran_vec_prod_ccc_simd(x, y, z, len);
}

void srsran_vec_prod_ccc_split(const float*   x_re,
                               const float*   x_im,
                               const float*   y_re,
                               const float*   y_im,
                               float*         z_re,
                               float*         z_im,
                               const uint32_t len)
{
  srsran_vec_prod_ccc_split_simd(x_re, x_im, y_re, y_im, z_re, z_im, len);
}

// PRACH, CHEST UL, etc.
void srsran_vec_prod_conj_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len)
{
  srsran_vec_prod_conj_ccc_simd(x, y, z, len);
}

//#define DIV_USE_VEC

// Used in SSS
void srsran_vec_div_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len)
{
  srsran_vec_div_ccc_simd(x, y, z, len);
}

/* Complex division by float z=x/y */
void srsran_vec_div_cfc(const cf_t* x, const float* y, cf_t* z, const uint32_t len)
{
  srsran_vec_div_cfc_simd(x, y, z, len);
}

void srsran_vec_div_fff(const float* x, const float* y, float* z, const uint32_t len)
{
  srsran_vec_div_fff_simd(x, y, z, len);
}

// PSS. convolution
cf_t srsran_vec_dot_prod_ccc(const cf_t* x, const cf_t* y, const uint32_t len)
{
  return srsran_vec_dot_prod_ccc_simd(x, y, len);
}

// Convolution filter and in SSS search
cf_t srsran_vec_dot_prod_cfc(const cf_t* x, const float* y, const uint32_t len)
{
  uint32_t i;
  cf_t     res = 0;
  for (i = 0; i < len; i++) {
    res += x[i] * y[i];
  }
  return res;
}

// SYNC
cf_t srsran_vec_dot_prod_conj_ccc(const cf_t* x, const cf_t* y, const uint32_t len)
{
  return srsran_vec_dot_prod_conj_ccc_simd(x, y, len);
}

// PHICH
float srsran_vec_dot_prod_fff(const float* x, const float* y, const uint32_t len)
{
  uint32_t i;
  float    res = 0;
  for (i = 0; i < len; i++) {
    res += x[i] * y[i];
  }
  return res;
}

int32_t srsran_vec_dot_prod_sss(const int16_t* x, const int16_t* y, const uint32_t len)
{
  return srsran_vec_dot_prod_sss_simd(x, y, len);
}

float srsran_vec_avg_power_cf(const cf_t* x, const uint32_t len)
{
  return crealf(srsran_vec_dot_prod_conj_ccc(x, x, len)) / len;
}

float srsran_vec_avg_power_sf(const int16_t* x, const uint32_t len)
{
  // Accumulator
  float acc = 0.0f;

  for (uint32_t i = 0; i < len; i++) {
    // Read value and typecast to float
    float t = (float)x[i];

    // Square value
    acc += t * t;
  }

  // Do average
  if (len) {
    acc /= len;
  }

  // Return accumulated value
  return acc;
}

float srsran_vec_avg_power_bf(const int8_t* x, const uint32_t len)
{
  // Accumulator
  float acc = 0.0f;

  for (uint32_t i = 0; i < len; i++) {
    // Read value and typecast to float
    float t = (float)x[i];

    // Square value
    acc += t * t;
  }

  // Do average
  if (len) {
    acc /= len;
  }

  // Return accumulated value
  return acc;
}

// Correlation assumes zero-mean x and y
float srsran_vec_corr_ccc(const cf_t* x, cf_t* y, const uint32_t len)
{
  //  return crealf(srsran_vec_dot_prod_conj_ccc(x,y,len)) / len;
  float s_x = crealf(srsran_vec_dot_prod_conj_ccc(x, x, len)) / len;
  float s_y = crealf(srsran_vec_dot_prod_conj_ccc(y, y, len)) / len;
  float cov = crealf(srsran_vec_dot_prod_conj_ccc(x, y, len)) / len;
  return cov / (sqrtf(s_x * s_y));
}

// PSS (disabled and using abs_square )
void srsran_vec_abs_cf(const cf_t* x, float* abs, const uint32_t len)
{
  srsran_vec_abs_cf_simd(x, abs, len);
}

void srsran_vec_abs_dB_cf(const cf_t* x, float default_value, float* abs, const uint32_t len)
{
  // Convert complex input to absplute value
  srsran_vec_abs_cf(x, abs, len);

  // Convert absolute value to dB
  for (int i = 0; i < len; i++) {
    // Check boundaries
    if (isnormal(abs[i])) {
      // Avoid infinites and zeros
      abs[i] = srsran_convert_amplitude_to_dB(abs[i]);
    } else {
      // Set to default value instead
      abs[i] = default_value;
    }
  }
}

void srsran_vec_arg_deg_cf(const cf_t* x, float default_value, float* arg, const uint32_t len)
{
  for (int i = 0; i < len; i++) {
    // Convert complex value to argument in degrees
    arg[i] = cargf(x[i]) * (180.0f / M_PI);

    // Check boundaries
    if (arg[i] != 0.0f && !isnormal(arg[i])) {
      // different than zero and not normal
      arg[i] = default_value;
    }
  }
}

// PRACH
void srsran_vec_abs_square_cf(const cf_t* x, float* abs_square, const uint32_t len)
{
  srsran_vec_abs_square_cf_simd(x, abs_square, len);
}

uint32_t srsran_vec_max_fi(const float* x, const uint32_t len)
{
  return srsran_vec_max_fi_simd(x, len);
}

uint32_t srsran_vec_max_abs_fi(const float* x, const uint32_t len)
{
  return srsran_vec_max_abs_fi_simd(x, len);
}

// CP autocorr
uint32_t srsran_vec_max_abs_ci(const cf_t* x, const uint32_t len)
{
  return srsran_vec_max_ci_simd(x, len);
}

void srsran_vec_quant_fs(const float*   in,
                         int16_t*       out,
                         const float    gain,
                         const float    offset,
                         const float    clip,
                         const uint32_t len)
{
  int  i   = 0;
  long tmp = 0;

  const int16_t inf = (1U << 15U) - 1;

  for (i = 0; i < len; i++) {
    if (isinf(in[i])) {
      tmp = inf * (-2 * (in[i] < 0) + 1);
    } else {
      tmp = (long)(offset + gain * in[i] + INT16_MAX + 0.5) - INT16_MAX;
      if (tmp < -clip) {
        tmp = -clip;
      }
      if (tmp > clip) {
        tmp = clip;
      }
    }

    out[i] = (int16_t)tmp;
  }
}

void srsran_vec_quant_fc(const float*   in,
                         int8_t*        out,
                         const float    gain,
                         const float    offset,
                         const float    clip,
                         const uint32_t len)
{
  int  i   = 0;
  long tmp = 0;

  for (i = 0; i < len; i++) {
    if (isinf(in[i])) {
      tmp = 127 * (-2 * (in[i] < 0) + 1);
    } else {
      tmp = (long)(offset + gain * in[i] + INT8_MAX + 0.5) - INT8_MAX;
      if (tmp < -clip) {
        tmp = -clip;
      }
      if (tmp > clip) {
        tmp = clip;
      }
    }

    out[i] = (int8_t)tmp;
  }
}

void srsran_vec_quant_fus(const float*   in,
                          uint16_t*      out,
                          const float    gain,
                          const float    offset,
                          const uint16_t clip,
                          const uint32_t len)
{
  for (uint32_t i = 0; i < len; i++) {
    int32_t tmp = (int32_t)(offset + gain * in[i]);
    tmp         = SRSRAN_MAX(tmp, 0);
    tmp         = SRSRAN_MIN(tmp, (int32_t)clip);
    out[i]      = (uint16_t)tmp;
  }
}

void srsran_vec_quant_fuc(const float*   in,
                          uint8_t*       out,
                          const float    gain,
                          const float    offset,
                          const uint8_t  clip,
                          const uint32_t len)
{
  for (uint32_t i = 0; i < len; i++) {
    int32_t tmp = (int32_t)(offset + gain * in[i]);
    tmp         = SRSRAN_MAX(tmp, 0);
    tmp         = SRSRAN_MIN(tmp, (int32_t)clip);
    out[i]      = (uint8_t)tmp;
  }
}

void srsran_vec_quant_suc(const int16_t* in,
                          uint8_t*       out,
                          const float    gain,
                          const float    offset,
                          const uint8_t  clip,
                          const uint32_t len)
{
  for (uint32_t i = 0; i < len; i++) {
    int32_t tmp = (int32_t)(offset + (float)in[i] * gain);
    tmp         = SRSRAN_MAX(tmp, 0);
    tmp         = SRSRAN_MIN(tmp, (int32_t)clip);
    out[i]      = (uint8_t)tmp;
  }
}

void srsran_vec_quant_sus(const int16_t* in,
                          uint16_t*      out,
                          const float    gain,
                          const float    offset,
                          const uint16_t clip,
                          const uint32_t len)
{
  for (uint32_t i = 0; i < len; i++) {
    int32_t tmp = (int32_t)(offset + gain * (float)in[i]);
    tmp         = SRSRAN_MAX(tmp, 0);
    tmp         = SRSRAN_MIN(tmp, (int32_t)clip);
    out[i]      = (uint16_t)tmp;
  }
}

void srsran_vec_interleave(const cf_t* x, const cf_t* y, cf_t* z, const int len)
{
  srsran_vec_interleave_simd(x, y, z, len);
}

void srsran_vec_interleave_add(const cf_t* x, const cf_t* y, cf_t* z, const int len)
{
  srsran_vec_interleave_add_simd(x, y, z, len);
}

void srsran_vec_gen_sine(cf_t amplitude, float freq, cf_t* z, int len)
{
  srsran_vec_gen_sine_simd(amplitude, freq, z, len);
}

void srsran_vec_apply_cfo(const cf_t* x, float cfo, cf_t* z, int len)
{
  srsran_vec_apply_cfo_simd(x, cfo, z, len);
}

float srsran_vec_estimate_frequency(const cf_t* x, int len)
{
  return srsran_vec_estimate_frequency_simd(x, len);
}
