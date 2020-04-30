/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#ifndef SRSLTE_VECTOR_H
#define SRSLTE_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "srslte/config.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define SRSLTE_MAX(a, b) ((a) > (b) ? (a) : (b))
#define SRSLTE_MIN(a, b) ((a) < (b) ? (a) : (b))

// Cumulative moving average
#define SRSLTE_VEC_CMA(data, average, n) ((average) + ((data) - (average)) / ((n) + 1))

// Proportional moving average
#define SRSLTE_VEC_PMA(average1, n1, average2, n2) (((average1) * (n1) + (average2) * (n2)) / ((n1) + (n2)))

// Exponential moving average
#define SRSLTE_VEC_EMA(data, average, alpha) ((alpha) * (data) + (1 - alpha) * (average))

static inline float srslte_convert_amplitude_to_dB(float v)
{
  return 20.0f * log10f(v);
}
static inline float srslte_convert_power_to_dB(float v)
{
  return 10.0f * log10f(v);
}
static inline float srslte_convert_power_to_dBm(float v)
{
  return srslte_convert_power_to_dB(v) + 30.0f;
}
static inline float srslte_convert_dB_to_amplitude(float v)
{
  return powf(10.0f, v / 20.0f);
}
static inline float srslte_convert_dB_to_power(float v)
{
  return powf(10.0f, v / 10.0f);
}

/*logical operations */
SRSLTE_API void srslte_vec_xor_bbb(int8_t* x, int8_t* y, int8_t* z, const uint32_t len);

/** Return the sum of all the elements */
SRSLTE_API float srslte_vec_acc_ff(const float* x, const uint32_t len);
SRSLTE_API cf_t srslte_vec_acc_cc(const cf_t* x, const uint32_t len);

SRSLTE_API void* srslte_vec_malloc(uint32_t size);
SRSLTE_API cf_t*  srslte_vec_cf_malloc(uint32_t size);
SRSLTE_API float* srslte_vec_f_malloc(uint32_t size);
SRSLTE_API int32_t* srslte_vec_i32_malloc(uint32_t nsamples);
SRSLTE_API uint32_t* srslte_vec_u32_malloc(uint32_t nsamples);
SRSLTE_API int16_t* srslte_vec_i16_malloc(uint32_t nsamples);
SRSLTE_API uint16_t* srslte_vec_u16_malloc(uint32_t nsamples);
SRSLTE_API int8_t* srslte_vec_i8_malloc(uint32_t nsamples);
SRSLTE_API uint8_t* srslte_vec_u8_malloc(uint32_t nsamples);

SRSLTE_API void* srslte_vec_realloc(void* ptr, uint32_t old_size, uint32_t new_size);

/* Zero memory */
SRSLTE_API void srslte_vec_cf_zero(cf_t* ptr, uint32_t nsamples);
SRSLTE_API void srslte_vec_f_zero(float* ptr, uint32_t nsamples);
SRSLTE_API void srslte_vec_u8_zero(uint8_t* ptr, uint32_t nsamples);
SRSLTE_API void srslte_vec_i16_zero(int16_t* ptr, uint32_t nsamples);
SRSLTE_API void srslte_vec_u32_zero(uint32_t* ptr, uint32_t nsamples);

/* Copy memory */
SRSLTE_API void srslte_vec_cf_copy(cf_t* dst, const cf_t* src, uint32_t len);
SRSLTE_API void srslte_vec_f_copy(float* dst, const float* src, uint32_t len);
SRSLTE_API void srslte_vec_u8_copy(uint8_t* dst, const uint8_t* src, uint32_t len);

/* print vectors */
SRSLTE_API void srslte_vec_fprint_c(FILE* stream, const cf_t* x, const uint32_t len);
SRSLTE_API void srslte_vec_fprint_f(FILE* stream, const float* x, const uint32_t len);
SRSLTE_API void srslte_vec_fprint_b(FILE* stream, const uint8_t* x, const uint32_t len);
SRSLTE_API void srslte_vec_fprint_bs(FILE* stream, const int8_t* x, const uint32_t len);
SRSLTE_API void srslte_vec_fprint_byte(FILE* stream, const uint8_t* x, const uint32_t len);
SRSLTE_API void srslte_vec_fprint_i(FILE* stream, const int* x, const uint32_t len);
SRSLTE_API void srslte_vec_fprint_s(FILE* stream, const int16_t* x, const uint32_t len);
SRSLTE_API void srslte_vec_fprint_hex(FILE* stream, uint8_t* x, const uint32_t len);
SRSLTE_API void srslte_vec_sprint_hex(char* str, const uint32_t max_str_len, uint8_t* x, const uint32_t len);

/* Saves/loads a vector to a file */
SRSLTE_API void srslte_vec_save_file(char* filename, const void* buffer, const uint32_t len);
SRSLTE_API void srslte_vec_load_file(char* filename, void* buffer, const uint32_t len);

/* sum two vectors */
SRSLTE_API void srslte_vec_sum_fff(const float* x, const float* y, float* z, const uint32_t len);
SRSLTE_API void srslte_vec_sum_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_sum_sss(const int16_t* x, const int16_t* y, int16_t* z, const uint32_t len);

/* substract two vectors z=x-y */
SRSLTE_API void srslte_vec_sub_fff(const float* x, const float* y, float* z, const uint32_t len);
SRSLTE_API void srslte_vec_sub_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_sub_sss(const int16_t* x, const int16_t* y, int16_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_sub_bbb(const int8_t* x, const int8_t* y, int8_t* z, const uint32_t len);

/* scalar product */
SRSLTE_API void srslte_vec_sc_prod_cfc(const cf_t* x, const float h, cf_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_sc_prod_ccc(const cf_t* x, const cf_t h, cf_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_sc_prod_fff(const float* x, const float h, float* z, const uint32_t len);

SRSLTE_API void srslte_vec_convert_fi(const float* x, const float scale, int16_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_convert_if(const int16_t* x, const float scale, float* z, const uint32_t len);
SRSLTE_API void srslte_vec_convert_fb(const float* x, const float scale, int8_t* z, const uint32_t len);

SRSLTE_API void srslte_vec_lut_sss(const short* x, const unsigned short* lut, short* y, const uint32_t len);
SRSLTE_API void srslte_vec_lut_bbb(const int8_t* x, const unsigned short* lut, int8_t* y, const uint32_t len);
SRSLTE_API void srslte_vec_lut_sis(const short* x, const unsigned int* lut, short* y, const uint32_t len);

/* vector product (element-wise) */
SRSLTE_API void srslte_vec_prod_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_prod_ccc_split(const float*   x_re,
                                          const float*   x_im,
                                          const float*   y_re,
                                          const float*   y_im,
                                          float*         z_re,
                                          float*         z_im,
                                          const uint32_t len);

/* vector product (element-wise) */
SRSLTE_API void srslte_vec_prod_cfc(const cf_t* x, const float* y, cf_t* z, const uint32_t len);

/* conjugate vector product (element-wise) */
SRSLTE_API void srslte_vec_prod_conj_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len);

/* real vector product (element-wise) */
SRSLTE_API void srslte_vec_prod_fff(const float* x, const float* y, float* z, const uint32_t len);
SRSLTE_API void srslte_vec_prod_sss(const int16_t* x, const int16_t* y, int16_t* z, const uint32_t len);

// Negate sign (scrambling)
SRSLTE_API void srslte_vec_neg_sss(const int16_t* x, const int16_t* y, int16_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_neg_bbb(const int8_t* x, const int8_t* y, int8_t* z, const uint32_t len);

/* Dot-product */
SRSLTE_API cf_t srslte_vec_dot_prod_cfc(const cf_t* x, const float* y, const uint32_t len);
SRSLTE_API cf_t srslte_vec_dot_prod_ccc(const cf_t* x, const cf_t* y, const uint32_t len);
SRSLTE_API cf_t  srslte_vec_dot_prod_conj_ccc(const cf_t* x, const cf_t* y, const uint32_t len);
SRSLTE_API float srslte_vec_dot_prod_fff(const float* x, const float* y, const uint32_t len);
SRSLTE_API int32_t srslte_vec_dot_prod_sss(const int16_t* x, const int16_t* y, const uint32_t len);

/* z=x/y vector division (element-wise) */
SRSLTE_API void srslte_vec_div_ccc(const cf_t* x, const cf_t* y, cf_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_div_cfc(const cf_t* x, const float* y, cf_t* z, const uint32_t len);
SRSLTE_API void srslte_vec_div_fff(const float* x, const float* y, float* z, const uint32_t len);

/* conjugate */
SRSLTE_API void srslte_vec_conj_cc(const cf_t* x, cf_t* y, const uint32_t len);

/* average vector power */
SRSLTE_API float srslte_vec_avg_power_cf(const cf_t* x, const uint32_t len);
SRSLTE_API float srslte_vec_avg_power_sf(const int16_t* x, const uint32_t len);

/* Correlation between complex vectors x and y */
SRSLTE_API float srslte_vec_corr_ccc(const cf_t* x, cf_t* y, const uint32_t len);

/* return the index of the maximum value in the vector */
SRSLTE_API uint32_t srslte_vec_max_fi(const float* x, const uint32_t len);
SRSLTE_API uint32_t srslte_vec_max_abs_fi(const float* x, const uint32_t len);
SRSLTE_API uint32_t srslte_vec_max_abs_ci(const cf_t* x, const uint32_t len);

/* quantify vector of floats or int16 and convert to uint8_t */
SRSLTE_API void srslte_vec_quant_fuc(const float*   in,
                                     uint8_t*       out,
                                     const float    gain,
                                     const float    offset,
                                     const uint8_t  clip,
                                     const uint32_t len);
SRSLTE_API void srslte_vec_quant_fus(const float*   in,
                                     uint16_t*      out,
                                     const float    gain,
                                     const float    offset,
                                     const uint16_t clip,
                                     const uint32_t len);
SRSLTE_API void srslte_vec_quant_suc(const int16_t* in,
                                     uint8_t*       out,
                                     const float    gain,
                                     const float    offset,
                                     const uint8_t  clip,
                                     const uint32_t len);

SRSLTE_API void srslte_vec_quant_sus(const int16_t* in,
                                     uint16_t*      out,
                                     const float    gain,
                                     const float    offset,
                                     const uint16_t clip,
                                     const uint32_t len);
/* magnitude of each vector element */
SRSLTE_API void srslte_vec_abs_cf(const cf_t* x, float* abs, const uint32_t len);
SRSLTE_API void srslte_vec_abs_square_cf(const cf_t* x, float* abs_square, const uint32_t len);

/**
 * @brief Extracts module in decibels of a complex vector
 *
 * This function extracts the module in decibels of a complex array input. Abnormal absolute value inputs (zero,
 * infinity and not-a-number) are set to default_value outputs.
 *
 * Equivalent code:
 *   for (int i = 0; i < len; i++) {
 *     float mag = x[i];
 *
 *     // Check boundaries
 *     if (isnormal(mag)) {
 *       // Avoid infinites and zeros
 *       abs[i] = 20.0f * log10f(mag);
 *     } else {
 *       // Set to default value instead
 *       abs[i] = default_value;
 *     }
 *   }
 *
 * @param x is the input complex vector
 * @param default_value is the value to use in case of having an abnormal absolute value.
 * @param abs is the destination vector
 * @param len is the input and output number of samples
 *
 */
SRSLTE_API void srslte_vec_abs_dB_cf(const cf_t* x, float default_value, float* abs, const uint32_t len);

/**
 * @brief Extracts argument in degrees from a complex vector
 *
 * This function extracts the argument from a complex vector. Infinity and not-a-number results are set to
 * default_value.
 *
 * Equivalent code:
 *   for(int i = 0; i < len; i++) {
 *     arg[i] = cargf(x[i]) * (180.0f / M_PI);
 *
 *     if (arg[i]!=0.0f && !isnormal(arg[i])) {
 *      arg[i] = default_value;
 *     }
 *   }
 *
 * @param x is the input complex vector
 * @param default_value is the value to use in case of having an abnormal result.
 * @param arg is the destination vector
 * @param len is the input and output number of samples
 *
 */
SRSLTE_API void srslte_vec_arg_deg_cf(const cf_t* x, float default_value, float* arg, const uint32_t len);

SRSLTE_API void srslte_vec_interleave(const cf_t* x, const cf_t* y, cf_t* z, const int len);

SRSLTE_API void srslte_vec_interleave_add(const cf_t* x, const cf_t* y, cf_t* z, const int len);

SRSLTE_API void srslte_vec_gen_sine(cf_t amplitude, float freq, cf_t* z, int len);

SRSLTE_API void srslte_vec_apply_cfo(const cf_t* x, float cfo, cf_t* z, int len);

SRSLTE_API float srslte_vec_estimate_frequency(const cf_t* x, int len);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_VECTOR_H
