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


#include <math.h>
#include <complex.h>
#include <fftw3.h>
#include <string.h>

#include "liblte/phy/utils/dft.h"
#include "liblte/phy/utils/vector.h"

#define dft_ceil(a,b) ((a-1)/b+1)
#define dft_floor(a,b) (a/b)

int dft_plan(dft_plan_t *plan, const int dft_points, dft_dir_t dir,
             dft_mode_t mode) {
  if(mode == COMPLEX){
    return dft_plan_c(plan,dft_points,dir);
  } else {
    return dft_plan_r(plan,dft_points,dir);
  }
  return 0;
}

static void allocate(dft_plan_t *plan, int size_in, int size_out, int len) {
  plan->in = fftwf_malloc(size_in*len);
  plan->out = fftwf_malloc(size_out*len);
}

int dft_plan_c(dft_plan_t *plan, const int dft_points, dft_dir_t dir) {
  allocate(plan,sizeof(fftwf_complex),sizeof(fftwf_complex), dft_points);
  int sign = (dir == FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD;
  plan->p = fftwf_plan_dft_1d(dft_points, plan->in, plan->out, sign, 0U);
  if (!plan->p) {
    return -1;
  }
  plan->size = dft_points;
  plan->mode = COMPLEX;
  plan->dir = dir;
  plan->forward = (dir==FORWARD)?true:false;
  plan->mirror = false;
  plan->db = false;
  plan->norm = false;
  plan->dc = false;

  return 0;
}

int dft_plan_r(dft_plan_t *plan, const int dft_points, dft_dir_t dir) {
  allocate(plan,sizeof(float),sizeof(float), dft_points);
  int sign = (dir == FORWARD) ? FFTW_R2HC : FFTW_HC2R;
  plan->p = fftwf_plan_r2r_1d(dft_points, plan->in, plan->out, sign, 0U);
  if (!plan->p) {
    return -1;
  }
  plan->size = dft_points;
  plan->mode = REAL;
  plan->dir = dir;
  plan->forward = (dir==FORWARD)?true:false;
  plan->mirror = false;
  plan->db = false;
  plan->norm = false;
  plan->dc = false;

  return 0;
}

void dft_plan_set_mirror(dft_plan_t *plan, bool val){
  plan->mirror = val;
}
void dft_plan_set_db(dft_plan_t *plan, bool val){
  plan->db = val;
}
void dft_plan_set_norm(dft_plan_t *plan, bool val){
  plan->norm = val;
}
void dft_plan_set_dc(dft_plan_t *plan, bool val){
  plan->dc = val;
}

static void copy_pre(uint8_t *dst, uint8_t *src, int size_d, int len,
                     bool forward, bool mirror, bool dc) {
  int offset = dc?1:0;
  if(mirror && !forward){
    int hlen = dft_floor(len,2);
    memset(dst,0,size_d*offset);
    memcpy(&dst[size_d*offset], &src[size_d*hlen], size_d*(len-hlen-offset));
    memcpy(&dst[(len-hlen)*size_d], src, size_d*hlen);
  } else {
    memcpy(dst,src,size_d*len);
  }
}

static void copy_post(uint8_t *dst, uint8_t *src, int size_d, int len,
                      bool forward, bool mirror, bool dc) {
  int offset = dc?1:0;
  if(mirror && forward){
    int hlen = dft_ceil(len,2);
    memcpy(dst, &src[size_d*hlen], size_d*(len-hlen));
    memcpy(&dst[(len-hlen)*size_d], &src[size_d*offset], size_d*(hlen-offset));
  } else {
    memcpy(dst,src,size_d*len);
  }
}

void dft_run(dft_plan_t *plan, void *in, void *out) {
  if(plan->mode == COMPLEX) {
    dft_run_c(plan,in,out);
  } else {
    dft_run_r(plan,in,out);
  }
}

void dft_run_c(dft_plan_t *plan, dft_c_t *in, dft_c_t *out) {
  float norm;
  int i;
  fftwf_complex *f_out = plan->out;

  copy_pre((uint8_t*)plan->in, (uint8_t*)in, sizeof(dft_c_t), plan->size,
           plan->forward, plan->mirror, plan->dc);
  fftwf_execute(plan->p);
  if (plan->norm) {
    norm = 1.0/sqrtf(plan->size);
    vec_sc_prod_cfc(f_out, norm, f_out, plan->size);    
  }
  if (plan->db) {
    for (i=0;i<plan->size;i++) {
      f_out[i] = 10*log10(f_out[i]);
    }
  }
  copy_post((uint8_t*)out, (uint8_t*)plan->out, sizeof(dft_c_t), plan->size,
            plan->forward, plan->mirror, plan->dc);
}

void dft_run_r(dft_plan_t *plan, dft_r_t *in, dft_r_t *out) {
  float norm;
  int i;
  int len = plan->size;
  float *f_out = plan->out;

  memcpy(plan->in,in,sizeof(dft_r_t)*plan->size);
  fftwf_execute(plan->p);
  if (plan->norm) {
    norm = 1.0/plan->size;
    vec_sc_prod_fff(f_out, norm, f_out, plan->size);    
  }
  if (plan->db) {
    for (i=0;i<len;i++) {
      f_out[i] = 10*log10(f_out[i]);
    }
  }
  memcpy(out,plan->out,sizeof(dft_r_t)*plan->size);
}

void dft_plan_free(dft_plan_t *plan) {
  if (!plan) return;
  if (!plan->size) return;
  if (plan->in) fftwf_free(plan->in);
  if (plan->out) fftwf_free(plan->out);
  if (plan->p) fftwf_destroy_plan(plan->p);
  bzero(plan, sizeof(dft_plan_t));
}



