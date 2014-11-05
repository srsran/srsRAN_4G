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

#include "liblte/phy/resampling/interp.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

#define TABLE_SIZE      1024


#ifdef TABLE_SIZE
  #define ARG2IDX(arg) ((uint32_t) ((1+(arg)/M_PI)*TABLE_SIZE/2))
  #define MYCEXP(arg) q->cexptable[ARG2IDX(arg)]
#else
  #define MYCEXP(arg) (cosf(arg) + I*sinf(arg))
#endif

#define MAX_OFFSET      64

int interp_init(interp_t *q, interp_type_t type, uint32_t len, uint32_t M) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  
  if (q != NULL && len > 0 && M > 0) {
    ret = LIBLTE_ERROR; 
    
    q->in_arg = vec_malloc(len * sizeof(float));
    if (!q->in_arg) {
      goto clean_and_exit;
    }
    q->in_mag = vec_malloc(len * sizeof(float));
    if (!q->in_mag) {
      goto clean_and_exit;
    }
    q->out_arg = vec_malloc((MAX_OFFSET + M * len) * sizeof(float));
    if (!q->out_arg) {
      goto clean_and_exit;
    }
    q->out_arg2 = vec_malloc((MAX_OFFSET + M * len) * sizeof(float));
    if (!q->out_arg2) {
      goto clean_and_exit;
    }
    q->table_idx = vec_malloc((MAX_OFFSET + M * len) * sizeof(int16_t));
    if (!q->table_idx) {
      goto clean_and_exit;
    }
    q->out_mag = vec_malloc((MAX_OFFSET + M * len) * sizeof(float));
    if (!q->out_mag) {
      goto clean_and_exit;
    }
    q->out_cexp = vec_malloc((MAX_OFFSET + M * len) * sizeof(cf_t));
    if (!q->out_cexp) {
      goto clean_and_exit;
    }
    q->out_prod = vec_malloc((MAX_OFFSET + M * len) * sizeof(cf_t));
    if (!q->out_prod) {
      goto clean_and_exit;
    }
#ifdef TABLE_SIZE
    q->cexptable = vec_malloc(TABLE_SIZE * sizeof(cf_t));
    uint32_t i;
    for (i=0;i<TABLE_SIZE;i++) {
      q->cexptable[i] = cexpf(I*M_PI*(2*((float) i/TABLE_SIZE) - 1));
    }
#endif
    q->M = M; 
    q->len = len; 
    ret = LIBLTE_SUCCESS;
  }
  
clean_and_exit:
  if (ret == LIBLTE_ERROR) {
    interp_free(q);
  }
  return ret; 
}

void interp_free(interp_t *q) {
  if (q) {
    if (q->in_arg) {
      free(q->in_arg);
    }
    if (q->in_mag) {
      free(q->in_mag);
    }
    if (q->out_arg) {
      free(q->out_arg);
    }
    if (q->out_cexp) {
      free(q->out_cexp);
    }
    if (q->out_mag) {
      free(q->out_mag);
    }
    
    if (q->out_prod) {
      free(q->out_prod);
    }
#ifdef TABLE_SIZE
    if (q->cexptable) {
      free(q->cexptable);
    }
#endif
  }
}

void interp_run_offset(interp_t *q, cf_t *input, cf_t *output, uint32_t off_st, uint32_t off_end) {
  uint32_t i, j, n;
  float mag0=0, mag1=0, arg0=0, arg1=0;
  float dmag, darg; 
  uint32_t M = q->M; 
  uint32_t len1 = q->len-1;
  
  if (off_st + off_end < MAX_OFFSET) {
    vec_abs_cf(input, q->in_mag, q->len);
    vec_arg_cf(input, q->in_arg, q->len);   
    
    mag0 = q->in_mag[0];
    mag1 = q->in_mag[1];
    arg0 = q->in_arg[0];
    arg1 = q->in_arg[1];
    dmag=(mag1-mag0)/M;
    darg=(arg1-arg0)/M; 
    for (j=0;j<off_st;j++) {
      q->out_mag[j] = mag0 - (j+1)*dmag;
      q->out_arg[j] = arg0 - (j+1)*darg;
    }
    
    for (i=0;i<len1;i++) {
      mag0 = q->in_mag[i];
      mag1 = q->in_mag[i+1];
      arg0 = q->in_arg[i];
      arg1 = q->in_arg[i+1];
      dmag=(mag1-mag0)/M;
      darg=(arg1-arg0)/M;
      for (j=0;j<M;j++) {
        q->out_mag[i*M+j+off_st] = mag0 + j*dmag;
        q->out_arg[i*M+j+off_st] = arg0 + j*darg;
      }
    }
    if (q->len > 1) {
      for (j=0;j<off_end;j++) {
        q->out_mag[i*M+j+off_st] = mag1 + j*dmag;
        q->out_arg[i*M+j+off_st] = arg1 + j*darg;
      }
    }
    uint32_t len=i*M+j+off_st;
#ifdef TABLE_SIZE
    vec_convert_fi(q->out_arg, q->table_idx, (float) TABLE_SIZE/2/M_PI, len);
    for (n=0;n<len;n++) {
      q->out_cexp[n] = q->cexptable[q->table_idx[n]+TABLE_SIZE/2];
    }
#else
    for (n=0;n<len;n++) {
      q->out_cexp[n] = MYCEXP(q->out_arg[n]);
    }
#endif
    vec_prod_cfc(q->out_cexp, q->out_mag, output, len);
  }

}

void interp_run(interp_t *q, cf_t *input, cf_t *output) {
    interp_run_offset(q, input, output, 0, 1);
}

cf_t interp_linear_onesample(cf_t *input) {
  float mag0=0, mag1=0, arg0=0, arg1=0, mag=0, arg=0;
  mag0 = cabsf(input[0]);
  mag1 = cabsf(input[1]);
  arg0 = cargf(input[0]);
  arg1 = cargf(input[1]);
  mag = 2*mag1 -mag0;
  arg = 2*arg1-arg0;
  return mag * cexpf(I * arg);
}

cf_t interp_linear_onesample2(cf_t *input) {
  float re0=0, im0=0, re1=0, im1=0, re=0, im=0;
  re0 = crealf(input[0]);
  im0 = cimagf(input[1]);
  re1 = crealf(input[0]);
  im1 = cimagf(input[1]);
  re = 2*re1-re0;
  im = 2*im1-im0;
  return (re+im*_Complex_I);
}

/* Performs 1st order linear interpolation with out-of-bound interpolation */
void interp_linear_offset(cf_t *input, cf_t *output, uint32_t M, uint32_t len, uint32_t off_st, uint32_t off_end) {
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

/* Performs 1st order linear interpolation */
void interp_linear_c(cf_t *input, cf_t *output, uint32_t M, uint32_t len) {
  interp_linear_offset(input, output, M, len, 0, 1);
}


/* Performs 1st order uint32_teger linear interpolation */
void interp_linear_f(float *input, float *output, uint32_t M, uint32_t len) {
  uint32_t i, j;
  for (i=0;i<len-1;i++) {
    for (j=0;j<M;j++) {
      output[i*M+j] = input[i] + j * (input[i+1]-input[i]) / M;
    }
  }
}
