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

#include "liblte/config.h"

typedef _Complex float cf_t;

typedef enum LIBLTE_API {LINEAR} interp_type_t;

typedef struct LIBLTE_API {
  interp_type_t type; 
  
  float *in_mag; 
  float *in_arg;
  float *in_mag0; 
  float *in_arg0;
  float *in_mag1; 
  float *in_arg1;
  
  float *out_mag; 
  float *out_arg;
  float *out_arg2;
  int16_t *table_idx; 
  
  cf_t *out_cexp;
  cf_t *out_prod;
  
  cf_t *cexptable; 
  
  uint32_t len; 
  uint32_t M; 
  
}interp_t;

LIBLTE_API int interp_init(interp_t *q, 
                           interp_type_t type, 
                           uint32_t len,
                           uint32_t M);

LIBLTE_API void interp_free(interp_t *q); 

LIBLTE_API void interp_run(interp_t *q, 
                           cf_t *input, 
                           cf_t *output);

LIBLTE_API void interp_run_offset(interp_t *q, 
                                  cf_t *input, 
                                  cf_t *output, 
                                  uint32_t off_st, 
                                  uint32_t off_end);

LIBLTE_API void interp_linear_offset(cf_t *input, 
                                     cf_t *output, 
                                     uint32_t M, 
                                     uint32_t len, 
                                     uint32_t off_st, 
                                     uint32_t off_end);

LIBLTE_API void interp_linear_c(cf_t *input, 
                                cf_t *output, 
                                uint32_t M, 
                                uint32_t len);

LIBLTE_API void interp_linear_f(float *input, 
                                float *output, 
                                uint32_t M, 
                                uint32_t len);

#endif // INTERP_H
