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

#include "srslte/srslte.h"
#include <complex.h>
#include <fftw3.h>
#include <math.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

#include "srslte/phy/dft/dft.h"
#include "srslte/phy/utils/vector.h"

#define dft_ceil(a, b) ((a - 1) / b + 1)
#define dft_floor(a, b) (a / b)

#define FFTW_WISDOM_FILE "%s/.srslte_fftwisdom"

static int get_fftw_wisdom_file(char* full_path, uint32_t n)
{
  const char* homedir = NULL;
  if ((homedir = getenv("HOME")) == NULL) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  return snprintf(full_path, n, FFTW_WISDOM_FILE, homedir);
}

#ifdef FFTW_WISDOM_FILE
#define FFTW_TYPE FFTW_MEASURE
#else
#define FFTW_TYPE 0
#endif

static pthread_mutex_t fft_mutex = PTHREAD_MUTEX_INITIALIZER;

// This function is called in the beggining of any executable where it is linked
__attribute__((constructor)) static void srslte_dft_load()
{
#ifdef FFTW_WISDOM_FILE
  char full_path[256];
  get_fftw_wisdom_file(full_path, sizeof(full_path));
  fftwf_import_wisdom_from_filename(full_path);
#else
  printf("Warning: FFTW Wisdom file not defined\n");
#endif
}

// This function is called in the ending of any executable where it is linked
__attribute__((destructor)) static void srslte_dft_exit()
{
#ifdef FFTW_WISDOM_FILE
  char full_path[256];
  get_fftw_wisdom_file(full_path, sizeof(full_path));
  fftwf_export_wisdom_to_filename(full_path);
#endif
  fftwf_cleanup();
}

int srslte_dft_plan(srslte_dft_plan_t* plan, const int dft_points, srslte_dft_dir_t dir, srslte_dft_mode_t mode)
{
  bzero(plan, sizeof(srslte_dft_plan_t));
  if (mode == SRSLTE_DFT_COMPLEX) {
    return srslte_dft_plan_c(plan, dft_points, dir);
  } else {
    return srslte_dft_plan_r(plan, dft_points, dir);
  }
  return 0;
}

int srslte_dft_replan(srslte_dft_plan_t* plan, const int new_dft_points)
{
  if (new_dft_points <= plan->init_size) {
    if (plan->mode == SRSLTE_DFT_COMPLEX) {
      return srslte_dft_replan_c(plan, new_dft_points);
    } else {
      return srslte_dft_replan_r(plan, new_dft_points);
    }
  } else {
    ERROR("DFT: Error calling replan: new_dft_points (%d) must be lower or equal "
          "dft_size passed initially (%d)\n",
          new_dft_points,
          plan->init_size);
    return -1;
  }
}

static void allocate(srslte_dft_plan_t* plan, int size_in, int size_out, int len)
{
  plan->in  = fftwf_malloc((size_t)size_in * len);
  plan->out = fftwf_malloc((size_t)size_out * len);
}

int srslte_dft_replan_guru_c(srslte_dft_plan_t* plan,
                             const int          new_dft_points,
                             cf_t*              in_buffer,
                             cf_t*              out_buffer,
                             int                istride,
                             int                ostride,
                             int                how_many,
                             int                idist,
                             int                odist)
{
  int sign = (plan->forward) ? FFTW_FORWARD : FFTW_BACKWARD;

  const fftwf_iodim iodim        = {new_dft_points, istride, ostride};
  const fftwf_iodim howmany_dims = {how_many, idist, odist};

  pthread_mutex_lock(&fft_mutex);

  /* Destroy current plan */
  fftwf_destroy_plan(plan->p);

  plan->p = fftwf_plan_guru_dft(1, &iodim, 1, &howmany_dims, in_buffer, out_buffer, sign, FFTW_TYPE);

  pthread_mutex_unlock(&fft_mutex);

  if (!plan->p) {
    return -1;
  }
  plan->size      = new_dft_points;
  plan->init_size = plan->size;

  return 0;
}

int srslte_dft_replan_c(srslte_dft_plan_t* plan, const int new_dft_points)
{
  int sign = (plan->dir == SRSLTE_DFT_FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD;

  pthread_mutex_lock(&fft_mutex);
  if (plan->p) {
    fftwf_destroy_plan(plan->p);
    plan->p = NULL;
  }
  plan->p = fftwf_plan_dft_1d(new_dft_points, plan->in, plan->out, sign, FFTW_TYPE);
  pthread_mutex_unlock(&fft_mutex);

  if (!plan->p) {
    return -1;
  }
  plan->size = new_dft_points;
  return 0;
}

int srslte_dft_plan_guru_c(srslte_dft_plan_t* plan,
                           const int          dft_points,
                           srslte_dft_dir_t   dir,
                           cf_t*              in_buffer,
                           cf_t*              out_buffer,
                           int                istride,
                           int                ostride,
                           int                how_many,
                           int                idist,
                           int                odist)
{
  int sign = (dir == SRSLTE_DFT_FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD;

  const fftwf_iodim iodim        = {dft_points, istride, ostride};
  const fftwf_iodim howmany_dims = {how_many, idist, odist};

  pthread_mutex_lock(&fft_mutex);

  plan->p = fftwf_plan_guru_dft(1, &iodim, 1, &howmany_dims, in_buffer, out_buffer, sign, FFTW_TYPE);
  if (!plan->p) {
    return -1;
  }
  pthread_mutex_unlock(&fft_mutex);

  plan->size      = dft_points;
  plan->init_size = plan->size;
  plan->mode      = SRSLTE_DFT_COMPLEX;
  plan->dir       = dir;
  plan->forward   = (dir == SRSLTE_DFT_FORWARD) ? true : false;
  plan->mirror    = false;
  plan->db        = false;
  plan->norm      = false;
  plan->dc        = false;
  plan->is_guru   = true;

  return 0;
}

int srslte_dft_plan_c(srslte_dft_plan_t* plan, const int dft_points, srslte_dft_dir_t dir)
{
  allocate(plan, sizeof(fftwf_complex), sizeof(fftwf_complex), dft_points);

  pthread_mutex_lock(&fft_mutex);

  int sign = (dir == SRSLTE_DFT_FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD;
  plan->p  = fftwf_plan_dft_1d(dft_points, plan->in, plan->out, sign, FFTW_TYPE);

  pthread_mutex_unlock(&fft_mutex);

  if (!plan->p) {
    return -1;
  }
  plan->size      = dft_points;
  plan->init_size = plan->size;
  plan->mode      = SRSLTE_DFT_COMPLEX;
  plan->dir       = dir;
  plan->forward   = (dir == SRSLTE_DFT_FORWARD) ? true : false;
  plan->mirror    = false;
  plan->db        = false;
  plan->norm      = false;
  plan->dc        = false;
  plan->is_guru   = false;

  return 0;
}

int srslte_dft_replan_r(srslte_dft_plan_t* plan, const int new_dft_points)
{
  int sign = (plan->dir == SRSLTE_DFT_FORWARD) ? FFTW_R2HC : FFTW_HC2R;

  pthread_mutex_lock(&fft_mutex);
  if (plan->p) {
    fftwf_destroy_plan(plan->p);
    plan->p = NULL;
  }
  plan->p = fftwf_plan_r2r_1d(new_dft_points, plan->in, plan->out, sign, FFTW_TYPE);
  pthread_mutex_unlock(&fft_mutex);

  if (!plan->p) {
    return -1;
  }
  plan->size = new_dft_points;
  return 0;
}

int srslte_dft_plan_r(srslte_dft_plan_t* plan, const int dft_points, srslte_dft_dir_t dir)
{
  allocate(plan, sizeof(float), sizeof(float), dft_points);
  int sign = (dir == SRSLTE_DFT_FORWARD) ? FFTW_R2HC : FFTW_HC2R;

  pthread_mutex_lock(&fft_mutex);
  plan->p = fftwf_plan_r2r_1d(dft_points, plan->in, plan->out, sign, FFTW_TYPE);
  pthread_mutex_unlock(&fft_mutex);

  if (!plan->p) {
    return -1;
  }
  plan->size      = dft_points;
  plan->init_size = plan->size;
  plan->mode      = SRSLTE_REAL;
  plan->dir       = dir;
  plan->forward   = (dir == SRSLTE_DFT_FORWARD) ? true : false;
  plan->mirror    = false;
  plan->db        = false;
  plan->norm      = false;
  plan->dc        = false;

  return 0;
}

void srslte_dft_plan_set_mirror(srslte_dft_plan_t* plan, bool val)
{
  plan->mirror = val;
}
void srslte_dft_plan_set_db(srslte_dft_plan_t* plan, bool val)
{
  plan->db = val;
}
void srslte_dft_plan_set_norm(srslte_dft_plan_t* plan, bool val)
{
  plan->norm = val;
}
void srslte_dft_plan_set_dc(srslte_dft_plan_t* plan, bool val)
{
  plan->dc = val;
}

static void copy_pre(uint8_t* dst, uint8_t* src, int size_d, int len, bool forward, bool mirror, bool dc)
{
  int offset = dc ? 1 : 0;
  if (mirror && !forward) {
    int hlen = dft_floor(len, 2);
    memset(dst, 0, (size_t)size_d * offset);
    memcpy(&dst[size_d * offset], &src[size_d * hlen], (size_t)size_d * (len - hlen - offset));
    memcpy(&dst[(len - hlen) * size_d], src, (size_t)size_d * hlen);
  } else {
    memcpy(dst, src, (size_t)size_d * len);
  }
}

static void copy_post(uint8_t* dst, uint8_t* src, int size_d, int len, bool forward, bool mirror, bool dc)
{
  int offset = dc ? 1 : 0;
  if (mirror && forward) {
    int hlen = dft_ceil(len, 2);
    memcpy(dst, &src[size_d * hlen], (size_t)size_d * (len - hlen));
    memcpy(&dst[(len - hlen) * size_d], &src[size_d * offset], (size_t)size_d * (hlen - offset));
  } else {
    memcpy(dst, src, (size_t)size_d * len);
  }
}

void srslte_dft_run(srslte_dft_plan_t* plan, const void* in, void* out)
{
  if (plan->mode == SRSLTE_DFT_COMPLEX) {
    srslte_dft_run_c(plan, in, out);
  } else {
    srslte_dft_run_r(plan, in, out);
  }
}

void srslte_dft_run_c_zerocopy(srslte_dft_plan_t* plan, const cf_t* in, cf_t* out)
{
  fftwf_execute_dft(plan->p, (cf_t*)in, out);
}

void srslte_dft_run_c(srslte_dft_plan_t* plan, const cf_t* in, cf_t* out)
{
  float          norm;
  int            i;
  fftwf_complex* f_out = plan->out;

  copy_pre((uint8_t*)plan->in, (uint8_t*)in, sizeof(cf_t), plan->size, plan->forward, plan->mirror, plan->dc);
  fftwf_execute(plan->p);
  if (plan->norm) {
    norm = 1.0 / sqrtf(plan->size);
    srslte_vec_sc_prod_cfc(f_out, norm, f_out, plan->size);
  }
  if (plan->db) {
    for (i = 0; i < plan->size; i++) {
      f_out[i] = srslte_convert_power_to_dB(f_out[i]);
    }
  }
  copy_post((uint8_t*)out, (uint8_t*)plan->out, sizeof(cf_t), plan->size, plan->forward, plan->mirror, plan->dc);
}

void srslte_dft_run_guru_c(srslte_dft_plan_t* plan)
{
  if (plan->is_guru == true) {
    fftwf_execute(plan->p);
  } else {
    ERROR("srslte_dft_run_guru_c: the selected plan is not guru!\n");
  }
}

void srslte_dft_run_r(srslte_dft_plan_t* plan, const float* in, float* out)
{
  float  norm;
  int    i;
  int    len   = plan->size;
  float* f_out = plan->out;

  memcpy(plan->in, in, sizeof(float) * plan->size);
  fftwf_execute(plan->p);
  if (plan->norm) {
    norm = 1.0 / plan->size;
    srslte_vec_sc_prod_fff(f_out, norm, f_out, plan->size);
  }
  if (plan->db) {
    for (i = 0; i < len; i++) {
      f_out[i] = srslte_convert_power_to_dB(f_out[i]);
    }
  }
  memcpy(out, plan->out, sizeof(float) * plan->size);
}

void srslte_dft_plan_free(srslte_dft_plan_t* plan)
{
  if (!plan)
    return;
  if (!plan->size)
    return;

  pthread_mutex_lock(&fft_mutex);
  if (!plan->is_guru) {
    if (plan->in)
      fftwf_free(plan->in);
    if (plan->out)
      fftwf_free(plan->out);
  }
  if (plan->p)
    fftwf_destroy_plan(plan->p);
  pthread_mutex_unlock(&fft_mutex);
  bzero(plan, sizeof(srslte_dft_plan_t));
}
