/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 *  File:         interp.h
 *
 *  Description:  Linear and vector interpolation
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_INTERP_H
#define SRSRAN_INTERP_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"

/************* STATIC LINEAR INTERPOLATION FUNCTIONS */

SRSRAN_API cf_t srsran_interp_linear_onesample(cf_t input0, cf_t input1);

SRSRAN_API cf_t srsran_interp_linear_onesample_cabs(cf_t input0, cf_t input1);

SRSRAN_API void srsran_interp_linear_offset_cabs(cf_t*    input,
                                                 cf_t*    output,
                                                 uint32_t M,
                                                 uint32_t len,
                                                 uint32_t off_st,
                                                 uint32_t off_end);

SRSRAN_API void srsran_interp_linear_f(float* input, float* output, uint32_t M, uint32_t len);

/* Interpolation between vectors */

typedef struct {
  cf_t*    diff_vec;
  uint32_t vector_len;
  uint32_t max_vector_len;
} srsran_interp_linsrsran_vec_t;

SRSRAN_API int srsran_interp_linear_vector_init(srsran_interp_linsrsran_vec_t* q, uint32_t vector_len);

SRSRAN_API void srsran_interp_linear_vector_free(srsran_interp_linsrsran_vec_t* q);

SRSRAN_API int srsran_interp_linear_vector_resize(srsran_interp_linsrsran_vec_t* q, uint32_t vector_len);

SRSRAN_API void srsran_interp_linear_vector(srsran_interp_linsrsran_vec_t* q,
                                            cf_t*                          in0,
                                            cf_t*                          in1,
                                            cf_t*                          between,
                                            uint32_t                       in1_in0_d,
                                            uint32_t                       M);

SRSRAN_API void srsran_interp_linear_vector2(srsran_interp_linsrsran_vec_t* q,
                                             cf_t*                          in0,
                                             cf_t*                          in1,
                                             cf_t*                          start,
                                             cf_t*                          between,
                                             uint32_t                       in1_in0_d,
                                             uint32_t                       M);

SRSRAN_API void srsran_interp_linear_vector3(srsran_interp_linsrsran_vec_t* q,
                                             cf_t*                          in0,
                                             cf_t*                          in1,
                                             cf_t*                          start,
                                             cf_t*                          between,
                                             uint32_t                       in1_in0_d,
                                             uint32_t                       M,
                                             bool                           to_right,
                                             uint32_t                       len);

/* Interpolation within a vector */

typedef struct {
  cf_t*    diff_vec;
  cf_t*    diff_vec2;
  float*   ramp;
  uint32_t vector_len;
  uint32_t M;
  uint32_t max_vector_len;
  uint32_t max_M;
} srsran_interp_lin_t;

SRSRAN_API int srsran_interp_linear_init(srsran_interp_lin_t* q, uint32_t vector_len, uint32_t M);

SRSRAN_API void srsran_interp_linear_free(srsran_interp_lin_t* q);

SRSRAN_API int srsran_interp_linear_resize(srsran_interp_lin_t* q, uint32_t vector_len, uint32_t M);

SRSRAN_API void
srsran_interp_linear_offset(srsran_interp_lin_t* q, cf_t* input, cf_t* output, uint32_t off_st, uint32_t off_end);

#endif // SRSRAN_INTERP_H
