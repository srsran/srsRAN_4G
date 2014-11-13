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

#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <strings.h> 

#include "liblte/phy/resampling/interp.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

/*************** STATIC FUNCTIONS ***********************/

cf_t interp_linear_onesample(cf_t input0, cf_t input1) {
  float mag0=0, mag1=0, arg0=0, arg1=0, mag=0, arg=0;
  mag0 = cabsf(input0);
  mag1 = cabsf(input1);
  arg0 = cargf(input0);
  arg1 = cargf(input1);
  mag = 2*mag1 -mag0;
  arg = 2*arg1-arg0;
  return mag * cexpf(I * arg);
}

cf_t interp_linear_onesample_cabs(cf_t input0, cf_t input1) {
  float re0=0, im0=0, re1=0, im1=0, re=0, im=0;
  re0 = crealf(input0);
  im0 = cimagf(input1);
  re1 = crealf(input0);
  im1 = cimagf(input1);
  re = 2*re1-re0;
  im = 2*im1-im0;
  return (re+im*_Complex_I);
}


/* Performs 1st order integer linear interpolation */
void interp_linear_f(float *input, float *output, uint32_t M, uint32_t len) {
  uint32_t i, j;
  for (i=0;i<len-1;i++) {
    for (j=0;j<M;j++) {
      output[i*M+j] = input[i] + j * (input[i+1]-input[i]) / M;
    }
  }
}


/* Performs 1st order linear interpolation with out-of-bound interpolation */
void interp_linear_offset_cabs(cf_t *input, cf_t *output, 
                               uint32_t M, uint32_t len, 
                               uint32_t off_st, uint32_t off_end) 
{
  uint32_t i, j;
  float mag0=0, mag1=0, arg0=0, arg1=0, mag=0, arg=0;

  for (i=0;i<len-1;i++) {
    mag0 = cabsf(input[i]);
    mag1 = cabsf(input[i+1]);
    arg0 = cargf(input[i]);
    arg1 = cargf(input[i+1]);
    if (i==0) {
      for (j=0;j<off_st;j++) {
        mag = mag0 - (j+1)*(mag1-mag0)/M;
        arg = arg0 - (j+1)*(arg1-arg0)/M;
        output[j] = mag * cexpf(I * arg);
      }
    }
    for (j=0;j<M;j++) {
      mag = mag0 + j*(mag1-mag0)/M;
      arg = arg0 + j*(arg1-arg0)/M;
      output[i*M+j+off_st] = mag * cexpf(I * arg);
    }
  }
  if (len > 1) {
    for (j=0;j<off_end;j++) {
      mag = mag1 + j*(mag1-mag0)/M;
      arg = arg1 + j*(arg1-arg0)/M;
      output[i*M+j+off_st] = mag * cexpf(I * arg);
    }
  }
}

int interp_linear_vector_init(interp_linvec_t *q, uint32_t vector_len) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  if (q) {
    bzero(q, sizeof(interp_linvec_t));
    ret = LIBLTE_SUCCESS;
    q->diff_vec = vec_malloc(vector_len * sizeof(cf_t));
    if (!q->diff_vec) {
      perror("malloc");
      return LIBLTE_ERROR; 
    }    
    q->vector_len = vector_len; 
  }
  return ret; 
}

void interp_linear_vector_free(interp_linvec_t *q) {
  if (q->diff_vec) {
    free(q->diff_vec);
  }

  bzero(q, sizeof(interp_linvec_t));

}

void interp_linear_vector(interp_linvec_t *q, cf_t *in0, cf_t *in1, cf_t *between, uint32_t M) 
{
  uint32_t i;
  
  vec_sub_ccc(in1, in0, q->diff_vec, q->vector_len);
  vec_sc_prod_cfc(q->diff_vec, (float) 1/M, q->diff_vec, q->vector_len);
  vec_sum_ccc(in0, q->diff_vec, between, q->vector_len);
  for (i=0;i<M-1;i++) {
    vec_sum_ccc(between, q->diff_vec, &between[q->vector_len], q->vector_len);
    between += q->vector_len;
  }
}

int interp_linear_init(interp_lin_t *q, uint32_t vector_len, uint32_t M) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  if (q) {
    bzero(q, sizeof(interp_lin_t));
    ret = LIBLTE_SUCCESS;
    q->diff_vec = vec_malloc(vector_len * sizeof(cf_t));
    if (!q->diff_vec) {
      perror("malloc");
      return LIBLTE_ERROR; 
    }    
    q->diff_vec2 = vec_malloc(M * vector_len * sizeof(cf_t));
    if (!q->diff_vec2) {
      perror("malloc");
      free(q->diff_vec);
      return LIBLTE_ERROR; 
    }    
    q->ramp = vec_malloc(M * sizeof(float));
    if (!q->ramp) {
      perror("malloc");
      free(q->ramp);
      free(q->diff_vec);
      return LIBLTE_ERROR; 
    }    
    
    for (int i=0;i<M;i++) {
      q->ramp[i] = (float) i; 
    }

    q->vector_len = vector_len; 
    q->M = M; 
  }
  return ret; 
}

void interp_linear_free(interp_lin_t *q) {
  if (q->diff_vec) {
    free(q->diff_vec);
  }
  if (q->diff_vec2) {
    free(q->diff_vec2);
  }
  if (q->ramp) {
    free(q->ramp);
  }

  bzero(q, sizeof(interp_lin_t));

}

void interp_linear_offset(interp_lin_t *q, cf_t *input, cf_t *output, 
                          uint32_t off_st, uint32_t off_end) 
{
  uint32_t i, j;
  cf_t diff; 
  
  i=0;
  for (j=0;j<off_st;j++) {
    output[j] = input[i] + (j+1) * (input[i+1]-input[i]) / q->M;
  }
  vec_sub_ccc(&input[1], input, q->diff_vec, (q->vector_len-1));
  vec_sc_prod_cfc(q->diff_vec, (float) 1/q->M, q->diff_vec, q->vector_len-1);
  for (i=0;i<q->vector_len-1;i++) {
    for (j=0;j<q->M;j++) {
      output[i*q->M+j+off_st] = input[i];  
      q->diff_vec2[i*q->M+j] = q->diff_vec[i];
    }
    vec_prod_cfc(&q->diff_vec2[i*q->M],q->ramp,&q->diff_vec2[i*q->M],q->M);
  }
  vec_sum_ccc(&output[off_st], q->diff_vec2, &output[off_st], q->M*(q->vector_len-1));
  
  if (q->vector_len > 1) {
    diff = input[q->vector_len-1]-input[q->vector_len-2];
    for (j=0;j<off_end;j++) {
      output[i*q->M+j+off_st] = input[i] + j * diff / q->M;  
    }
  }
}

