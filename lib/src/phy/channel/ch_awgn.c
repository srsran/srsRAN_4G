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

#include <complex.h>
#include <stdlib.h>
#include <strings.h>

#include "gauss.h"
#include <srslte/phy/channel/ch_awgn.h>
#include <srslte/phy/utils/simd.h>
#include <srslte/phy/utils/vector.h>
#include <srslte/srslte.h>

#define AWGN_TABLE_READ_BURST 32U
#define AWGN_TABLE_SIZE_POW 10U
#define AWGN_TABLE_SIZE (1U << AWGN_TABLE_SIZE_POW)
#define AWGN_TABLE_ALLOC_SIZE (AWGN_TABLE_SIZE + SRSLTE_MAX(SRSLTE_SIMD_F_SIZE, AWGN_TABLE_READ_BURST))

// Linear congruential Generator parameters
#define AWGN_LCG_A 12345U
#define AWGN_LCG_C 1103515245U

static inline int32_t channel_awgn_rand(srslte_channel_awgn_t* q)
{
  q->rand_state = (AWGN_LCG_A + AWGN_LCG_C * q->rand_state);
  return q->rand_state % AWGN_TABLE_SIZE;
}

static inline void channel_awgn_shuffle_tables(srslte_channel_awgn_t* q)
{
  for (uint32_t i = 0; i < AWGN_TABLE_SIZE; i++) {
    int32_t idx;

    do {
      idx = channel_awgn_rand(q);
    } while (idx == i);
    float temp_log    = q->table_log[i];
    q->table_log[i]   = q->table_log[idx];
    q->table_log[idx] = temp_log;

    do {
      idx = channel_awgn_rand(q);
    } while (idx == i);
    float temp_cos    = q->table_cos[i];
    q->table_cos[i]   = q->table_cos[idx];
    q->table_cos[idx] = temp_cos;
  }
}

int srslte_channel_awgn_init(srslte_channel_awgn_t* q, uint32_t seed)
{
  if (!q) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Initialise random generator
  q->rand_state = seed;

  // Allocate complex exponential and logarithmic tables
  q->table_cos = srslte_vec_f_malloc(AWGN_TABLE_ALLOC_SIZE);
  q->table_log = srslte_vec_f_malloc(AWGN_TABLE_ALLOC_SIZE);
  if (!q->table_cos || !q->table_log) {
    ERROR("Malloc\n");
  }

  // Fill tables
  for (uint32_t i = 0; i < AWGN_TABLE_SIZE; i++) {
    float temp1 = (float)i / (float)AWGN_TABLE_SIZE;
    float temp2 = (float)(i + 1) / (float)AWGN_TABLE_SIZE;

    q->table_cos[i] = cosf(2.0f * (float)M_PI * temp1);
    q->table_log[i] = sqrtf(-2.0f * logf(temp2));
  }

  // Shuffle values in tables to break correlation in SIMD registers
  channel_awgn_shuffle_tables(q);

  // Copy head in tail for keeping continuity in SIMD registers
  for (uint32_t i = 0; i < SRSLTE_SIMD_F_SIZE; i++) {
    q->table_log[i + AWGN_TABLE_SIZE] = q->table_log[i];
    q->table_cos[i + AWGN_TABLE_SIZE] = q->table_cos[i];
  }

  return SRSLTE_SUCCESS;
}

int srslte_channel_awgn_set_n0(srslte_channel_awgn_t* q, float n0_dBfs)
{
  if (!q) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  q->std_dev = powf(10.0f, n0_dBfs / 20.0f);

  return isnormal(q->std_dev) ? SRSLTE_SUCCESS : SRSLTE_ERROR;
}

static inline void channel_awgn_run(srslte_channel_awgn_t* q, const float* in, float* out, uint32_t size, float std_dev)
{
  if (!q) {
    return;
  }

  int32_t i    = 0;
  int32_t idx1 = 0;
  int32_t idx2 = 0;

#if SRSLTE_SIMD_F_SIZE
  for (; i < (int)size - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {

    if (i % AWGN_TABLE_READ_BURST == 0) {
      idx1 = channel_awgn_rand(q);
      idx2 = channel_awgn_rand(q);
    } else {
      idx1 = (idx1 + SRSLTE_SIMD_F_SIZE) % AWGN_TABLE_SIZE;
      idx2 = (idx2 + SRSLTE_SIMD_F_SIZE) % AWGN_TABLE_SIZE;
    }

    // Load SIMD registers
    simd_f_t t1   = srslte_simd_f_loadu(&q->table_cos[idx1]);
    simd_f_t t2   = srslte_simd_f_loadu(&q->table_log[idx2]);
    simd_f_t in_  = srslte_simd_f_loadu(&in[i]);
    simd_f_t out_ = srslte_simd_f_set1(std_dev);

    // Compute random number
    out_ = srslte_simd_f_mul(t1, out_);
    out_ = srslte_simd_f_mul(t2, out_);
    out_ = srslte_simd_f_add(in_, out_);

    // Store random number
    srslte_simd_f_storeu(&out[i], out_);
  }
#endif /* SRSLTE_SIMD_F_SIZE */

  for (; i < size; i++) {

    if (i % AWGN_TABLE_READ_BURST == 0) {
      idx1 = channel_awgn_rand(q);
      idx2 = channel_awgn_rand(q);
    } else {
      idx1 = (idx1 + 1) % AWGN_TABLE_SIZE;
      idx2 = (idx2 + 1) % AWGN_TABLE_SIZE;
    }

    float n = std_dev;
    n *= q->table_log[idx1];
    n *= q->table_cos[idx2];

    out[i] = in[i] + n;
  }
}

void srslte_channel_awgn_run_c(srslte_channel_awgn_t* q, const cf_t* in, cf_t* out, uint32_t size)
{
  channel_awgn_run(q, (float*)in, (float*)out, 2 * size, q->std_dev * (float)M_SQRT1_2);
}

void srslte_channel_awgn_run_f(srslte_channel_awgn_t* q, const float* in, float* out, uint32_t size)
{
  channel_awgn_run(q, in, out, size, q->std_dev);
}

void srslte_channel_awgn_free(srslte_channel_awgn_t* q)
{
  if (!q) {
    return;
  }

  if (q->table_cos) {
    free(q->table_cos);
  }

  if (q->table_log) {
    free(q->table_log);
  }
}

float srslte_ch_awgn_get_variance(float ebno_db, float rate)
{
  float esno_db = ebno_db + srslte_convert_power_to_dB(rate);
  return srslte_convert_dB_to_amplitude(-esno_db);
}

void srslte_ch_awgn_c(const cf_t* x, cf_t* y, float variance, uint32_t len)
{
  cf_t     tmp;
  uint32_t i;

  for (i = 0; i < len; i++) {
    __real__ tmp = rand_gauss();
    __imag__ tmp = rand_gauss();
    tmp *= variance;
    y[i] = tmp + x[i];
  }
}
void srslte_ch_awgn_f(const float* x, float* y, float variance, uint32_t len)
{
  uint32_t i;

  for (i = 0; i < len; i++) {
    y[i] = x[i] + variance * rand_gauss();
  }
}
