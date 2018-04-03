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

/******************************************************************************
 *  File:         interp.h
 *
 *  Description:  Linear and vector interpolation
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_INTERP_H
#define SRSLTE_INTERP_H

#include <stdint.h>
#include <stdbool.h>

#include "srslte/config.h"


/************* STATIC LINEAR INTERPOLATION FUNCTIONS */

SRSLTE_API cf_t srslte_interp_linear_onesample(cf_t input0, 
                                               cf_t input1); 

SRSLTE_API cf_t srslte_interp_linear_onesample_cabs(cf_t input0, 
                                             cf_t input1); 

SRSLTE_API void srslte_interp_linear_offset_cabs(cf_t *input, 
                                                 cf_t *output, 
                                                 uint32_t M, 
                                                 uint32_t len, 
                                                 uint32_t off_st, 
                                                 uint32_t off_end);

SRSLTE_API void srslte_interp_linear_f(float *input, 
                                       float *output, 
                                       uint32_t M, 
                                       uint32_t len);



/* Interpolation between vectors */

typedef struct {
  cf_t *diff_vec; 
  uint32_t vector_len;
  uint32_t max_vector_len;
} srslte_interp_linsrslte_vec_t;

SRSLTE_API int srslte_interp_linear_vector_init(srslte_interp_linsrslte_vec_t *q, 
                                                uint32_t vector_len);

SRSLTE_API void srslte_interp_linear_vector_free(srslte_interp_linsrslte_vec_t *q); 

SRSLTE_API int srslte_interp_linear_vector_resize(srslte_interp_linsrslte_vec_t *q,
                                                  uint32_t vector_len);

SRSLTE_API void srslte_interp_linear_vector(srslte_interp_linsrslte_vec_t *q, 
                                            cf_t *in0, 
                                            cf_t *in1, 
                                            cf_t *between, 
                                            uint32_t in1_in0_d, 
                                            uint32_t M); 

SRSLTE_API void srslte_interp_linear_vector2(srslte_interp_linsrslte_vec_t *q, 
                                             cf_t *in0, 
                                             cf_t *in1, 
                                             cf_t *start, 
                                             cf_t *between, 
                                             uint32_t in1_in0_d, 
                                             uint32_t M); 

SRSLTE_API void srslte_interp_linear_vector3(srslte_interp_linsrslte_vec_t *q, 
                                             cf_t *in0, 
                                             cf_t *in1, 
                                             cf_t *start, 
                                             cf_t *between, 
                                             uint32_t in1_in0_d, 
                                             uint32_t M, 
                                             bool to_right, 
                                             uint32_t len); 

/* Interpolation within a vector */

typedef struct {
  cf_t *diff_vec; 
  cf_t *diff_vec2;
  float *ramp;
  uint32_t vector_len; 
  uint32_t M;
  uint32_t max_vector_len;
  uint32_t max_M;
} srslte_interp_lin_t;

SRSLTE_API int srslte_interp_linear_init(srslte_interp_lin_t *q, 
                                         uint32_t vector_len, 
                                         uint32_t M); 

SRSLTE_API void srslte_interp_linear_free(srslte_interp_lin_t *q);

SRSLTE_API int srslte_interp_linear_resize(srslte_interp_lin_t *q,
                                           uint32_t vector_len,
                                           uint32_t M);

SRSLTE_API void srslte_interp_linear_offset(srslte_interp_lin_t *q, 
                                            cf_t *input, 
                                            cf_t *output, 
                                            uint32_t off_st, 
                                            uint32_t off_end);


#endif // SRSLTE_INTERP_H
