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

#ifndef INTERP_H
#define INTERP_H

#include <stdint.h>

#include "srslte/config.h"

typedef _Complex float cf_t;


/************* STATIC LINEAR INTERPOLATION FUNCTIONS */

LIBLTE_API cf_t interp_linear_onesample(cf_t input0, 
                                        cf_t input1); 

LIBLTE_API cf_t interp_linear_onesample_cabs(cf_t input0, 
                                             cf_t input1); 

LIBLTE_API void interp_linear_offset_cabs(cf_t *input, 
                                          cf_t *output, 
                                          uint32_t M, 
                                          uint32_t len, 
                                          uint32_t off_st, 
                                          uint32_t off_end);

LIBLTE_API void interp_linear_f(float *input, 
                                float *output, 
                                uint32_t M, 
                                uint32_t len);



/* Interpolation between vectors */

typedef struct {
  cf_t *diff_vec; 
  uint32_t vector_len; 
} interp_linvec_t;

LIBLTE_API int interp_linear_vector_init(interp_linvec_t *q, 
                                         uint32_t vector_len);

LIBLTE_API void interp_linear_vector_free(interp_linvec_t *q); 

LIBLTE_API void interp_linear_vector(interp_linvec_t *q, 
                                     cf_t *in0, 
                                     cf_t *in1, 
                                     cf_t *between, 
                                     uint32_t M); 



/* Interpolation within a vector */

typedef struct {
  cf_t *diff_vec; 
  cf_t *diff_vec2;
  float *ramp;
  uint32_t vector_len; 
  uint32_t M; 
} interp_lin_t;

LIBLTE_API int interp_linear_init(interp_lin_t *q, 
                                  uint32_t vector_len, 
                                  uint32_t M); 

LIBLTE_API void interp_linear_free(interp_lin_t *q);

LIBLTE_API void interp_linear_offset(interp_lin_t *q, 
                                     cf_t *input, 
                                     cf_t *output, 
                                     uint32_t off_st, 
                                     uint32_t off_end);


#endif // INTERP_H
