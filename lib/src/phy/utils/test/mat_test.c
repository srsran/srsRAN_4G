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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <complex.h>
#include <stdbool.h>
#include <sys/time.h>

#include "srslte/phy/utils/mat.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/vector_simd.h"


bool zf_solver = false;
bool mmse_solver = false;
bool verbose = false;

#define RANDOM_F() ((float)rand())/((float)RAND_MAX)
#define RANDOM_S() ((int16_t)(rand() && 0x800F))
#define RANDOM_CF() (RANDOM_F() + _Complex_I*RANDOM_F())

double elapsed_us(struct timeval *ts_start, struct timeval *ts_end) {
  if (ts_end->tv_usec > ts_start->tv_usec) {
    return ((double) ts_end->tv_sec - (double) ts_start->tv_sec) * 1000000 +
           (double) ts_end->tv_usec - (double) ts_start->tv_usec;
  } else {
    return ((double) ts_end->tv_sec - (double) ts_start->tv_sec - 1) * 1000000 +
           ((double) ts_end->tv_usec + 1000000) - (double) ts_start->tv_usec;
  }
}

#define BLOCK_SIZE 1000
#define RUN_TEST(FUNCTION) /*TYPE NAME (void)*/ { \
  int i;\
  struct timeval start, end;\
  gettimeofday(&start, NULL); \
  bool ret = true; \
  for (i = 0; i < BLOCK_SIZE; i++) {ret &= FUNCTION ();}\
  gettimeofday(&end, NULL);\
  if (verbose) printf("%32s: %s ... %6.2f us/call\n", #FUNCTION, (ret)?"Pass":"Fail", \
                      elapsed_us(&start, &end)/BLOCK_SIZE);\
  passed &= ret;\
}

void usage(char *prog) {
  printf("Usage: %s [mzvh]\n", prog);
  printf("\t-m Test Minimum Mean Squared Error (MMSE) solver\n");
  printf("\t-z Test Zero Forcing (ZF) solver\n");
  printf("\t-v Verbose\n");
  printf("\t-h Show this message\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "mzvh")) != -1) {
    switch (opt) {
      case 'm':
        mmse_solver = true;
        break;
      case 'z':
        zf_solver = true;
        break;
      case 'v':
        verbose = true;
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

bool test_zf_solver_gen(void) {
  cf_t x0, x1, cf_error0, cf_error1;
  float error;

  cf_t x0_gold = RANDOM_CF();
  cf_t x1_gold = RANDOM_CF();
  cf_t h00 = RANDOM_CF();
  cf_t h01 = RANDOM_CF();
  cf_t h10 = RANDOM_CF();
  cf_t h11 = (1 - h01 * h10) / h00;
  cf_t y0 = x0_gold * h00 + x1_gold * h01;
  cf_t y1 = x0_gold * h10 + x1_gold * h11;

  srslte_mat_2x2_zf_gen(y0, y1, h00, h01, h10, h11, &x0, &x1, 1.0f);

  cf_error0 = x0 - x0_gold;
  cf_error1 = x1 - x1_gold;
  error = crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
          crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  return (error < 1e-6);
}

bool test_mmse_solver_gen(void) {
  cf_t x0, x1, cf_error0, cf_error1;
  float error;

  cf_t x0_gold = RANDOM_CF();
  cf_t x1_gold = RANDOM_CF();
  cf_t h00 = RANDOM_CF();
  cf_t h01 = RANDOM_CF();
  cf_t h10 = RANDOM_CF();
  cf_t h11 = (1 - h01 * h10) / h00;
  cf_t y0 = x0_gold * h00 + x1_gold * h01;
  cf_t y1 = x0_gold * h10 + x1_gold * h11;

  srslte_mat_2x2_mmse_gen(y0, y1, h00, h01, h10, h11, &x0, &x1, 0.0f, 1.0f);

  cf_error0 = x0 - x0_gold;
  cf_error1 = x1 - x1_gold;
  error = crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
          crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  return (error < 1e-6);
}

#ifdef LV_HAVE_SSE

bool test_zf_solver_sse(void) {
  cf_t cf_error0, cf_error1;
  float error = 0.0f;

  cf_t x0_gold_1 = RANDOM_CF();
  cf_t x1_gold_1 = RANDOM_CF();
  cf_t h00_1 = RANDOM_CF();
  cf_t h01_1 = RANDOM_CF();
  cf_t h10_1 = RANDOM_CF();
  cf_t h11_1 = (1 - h01_1 * h10_1) / h00_1;
  cf_t y0_1 = x0_gold_1 * h00_1 + x1_gold_1 * h01_1;
  cf_t y1_1 = x0_gold_1 * h10_1 + x1_gold_1 * h11_1;

  cf_t x0_gold_2 = RANDOM_CF();
  cf_t x1_gold_2 = RANDOM_CF();
  cf_t h00_2 = RANDOM_CF();
  cf_t h01_2 = RANDOM_CF();
  cf_t h10_2 = RANDOM_CF();
  cf_t h11_2 = (1 - h01_2 * h10_2) / h00_2;
  cf_t y0_2 = x0_gold_2 * h00_2 + x1_gold_2 * h01_2;
  cf_t y1_2 = x0_gold_2 * h10_2 + x1_gold_2 * h11_2;

  __m128 _y0 = _mm_set_ps(cimagf(y0_1), crealf(y0_1), cimagf(y0_2), crealf(y0_2));
  __m128 _y1 = _mm_set_ps(cimagf(y1_1), crealf(y1_1), cimagf(y1_2), crealf(y1_2));

  __m128 _h00 = _mm_set_ps(cimagf(h00_1), crealf(h00_1), cimagf(h00_2), crealf(h00_2));
  __m128 _h01 = _mm_set_ps(cimagf(h01_1), crealf(h01_1), cimagf(h01_2), crealf(h01_2));
  __m128 _h10 = _mm_set_ps(cimagf(h10_1), crealf(h10_1), cimagf(h10_2), crealf(h10_2));
  __m128 _h11 = _mm_set_ps(cimagf(h11_1), crealf(h11_1), cimagf(h11_2), crealf(h11_2));

  __m128 _x0, _x1;

  srslte_mat_2x2_zf_sse(_y0, _y1, _h00, _h01, _h10, _h11, &_x0, &_x1, 1.0f);


  __attribute__((aligned(128))) cf_t x0[2];
  __attribute__((aligned(128))) cf_t x1[2];

  _mm_store_ps((float *) x0, _x0);
  _mm_store_ps((float *) x1, _x1);

  cf_error0 = x0[1] - x0_gold_1;
  cf_error1 = x1[1] - x1_gold_1;
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
           crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  cf_error0 = x0[0] - x0_gold_2;
  cf_error1 = x1[0] - x1_gold_2;
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
           crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  return (error < 1e-3);
}

bool test_mmse_solver_sse(void) {
  cf_t cf_error0, cf_error1;
  float error = 0.0f;

  cf_t x0_gold_1 = RANDOM_CF();
  cf_t x1_gold_1 = RANDOM_CF();
  cf_t h00_1 = RANDOM_CF();
  cf_t h01_1 = RANDOM_CF();
  cf_t h10_1 = RANDOM_CF();
  cf_t h11_1 = (1 - h01_1 * h10_1) / h00_1;
  cf_t y0_1 = x0_gold_1 * h00_1 + x1_gold_1 * h01_1;
  cf_t y1_1 = x0_gold_1 * h10_1 + x1_gold_1 * h11_1;

  cf_t x0_gold_2 = RANDOM_CF();
  cf_t x1_gold_2 = RANDOM_CF();
  cf_t h00_2 = RANDOM_CF();
  cf_t h01_2 = RANDOM_CF();
  cf_t h10_2 = RANDOM_CF();
  cf_t h11_2 = (1 - h01_2 * h10_2) / h00_2;
  cf_t y0_2 = x0_gold_2 * h00_2 + x1_gold_2 * h01_2;
  cf_t y1_2 = x0_gold_2 * h10_2 + x1_gold_2 * h11_2;

  __m128 _y0 = _mm_set_ps(cimagf(y0_1), crealf(y0_1), cimagf(y0_2), crealf(y0_2));
  __m128 _y1 = _mm_set_ps(cimagf(y1_1), crealf(y1_1), cimagf(y1_2), crealf(y1_2));

  __m128 _h00 = _mm_set_ps(cimagf(h00_1), crealf(h00_1), cimagf(h00_2), crealf(h00_2));
  __m128 _h01 = _mm_set_ps(cimagf(h01_1), crealf(h01_1), cimagf(h01_2), crealf(h01_2));
  __m128 _h10 = _mm_set_ps(cimagf(h10_1), crealf(h10_1), cimagf(h10_2), crealf(h10_2));
  __m128 _h11 = _mm_set_ps(cimagf(h11_1), crealf(h11_1), cimagf(h11_2), crealf(h11_2));

  __m128 _x0, _x1;

  srslte_mat_2x2_mmse_sse(_y0, _y1, _h00, _h01, _h10, _h11, &_x0, &_x1, 0.0f, 1.0f);


  __attribute__((aligned(128))) cf_t x0[2];
  __attribute__((aligned(128))) cf_t x1[2];

  _mm_store_ps((float *) x0, _x0);
  _mm_store_ps((float *) x1, _x1);

  cf_error0 = x0[1] - x0_gold_1;
  cf_error1 = x1[1] - x1_gold_1;
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
           crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  cf_error0 = x0[0] - x0_gold_2;
  cf_error1 = x1[0] - x1_gold_2;
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
           crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  return (error < 1e-3);
}

#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX

bool test_zf_solver_avx(void) {
  cf_t cf_error0, cf_error1;
  float error = 0.0f;

  cf_t x0_gold_1 = RANDOM_CF();
  cf_t x1_gold_1 = RANDOM_CF();
  cf_t h00_1 = RANDOM_CF();
  cf_t h01_1 = RANDOM_CF();
  cf_t h10_1 = RANDOM_CF();
  cf_t h11_1 = (1 - h01_1 * h10_1) / h00_1;
  cf_t y0_1 = x0_gold_1 * h00_1 + x1_gold_1 * h01_1;
  cf_t y1_1 = x0_gold_1 * h10_1 + x1_gold_1 * h11_1;

  cf_t x0_gold_2 = RANDOM_CF();
  cf_t x1_gold_2 = RANDOM_CF();
  cf_t h00_2 = RANDOM_CF();
  cf_t h01_2 = RANDOM_CF();
  cf_t h10_2 = RANDOM_CF();
  cf_t h11_2 = (1 - h01_2 * h10_2) / h00_2;
  cf_t y0_2 = x0_gold_2 * h00_2 + x1_gold_2 * h01_2;
  cf_t y1_2 = x0_gold_2 * h10_2 + x1_gold_2 * h11_2;

  __m256 _y0 = _mm256_set_ps(cimagf(y0_1), crealf(y0_1), cimagf(y0_2), crealf(y0_2),
                             cimagf(y0_1), crealf(y0_1), cimagf(y0_2), crealf(y0_2));
  __m256 _y1 = _mm256_set_ps(cimagf(y1_1), crealf(y1_1), cimagf(y1_2), crealf(y1_2),
                             cimagf(y1_1), crealf(y1_1), cimagf(y1_2), crealf(y1_2));

  __m256 _h00 = _mm256_set_ps(cimagf(h00_1), crealf(h00_1), cimagf(h00_2), crealf(h00_2),
                              cimagf(h00_1), crealf(h00_1), cimagf(h00_2), crealf(h00_2));
  __m256 _h01 = _mm256_set_ps(cimagf(h01_1), crealf(h01_1), cimagf(h01_2), crealf(h01_2),
                              cimagf(h01_1), crealf(h01_1), cimagf(h01_2), crealf(h01_2));
  __m256 _h10 = _mm256_set_ps(cimagf(h10_1), crealf(h10_1), cimagf(h10_2), crealf(h10_2),
                              cimagf(h10_1), crealf(h10_1), cimagf(h10_2), crealf(h10_2));
  __m256 _h11 = _mm256_set_ps(cimagf(h11_1), crealf(h11_1), cimagf(h11_2), crealf(h11_2),
                              cimagf(h11_1), crealf(h11_1), cimagf(h11_2), crealf(h11_2));

  __m256 _x0, _x1;

  srslte_mat_2x2_zf_avx(_y0, _y1, _h00, _h01, _h10, _h11, &_x0, &_x1, 1.0f);


  __attribute__((aligned(256))) cf_t x0[4];
  __attribute__((aligned(256))) cf_t x1[4];

  _mm256_store_ps((float *) x0, _x0);
  _mm256_store_ps((float *) x1, _x1);

  cf_error0 = x0[1] - x0_gold_1;
  cf_error1 = x1[1] - x1_gold_1;
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
           crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  cf_error0 = x0[0] - x0_gold_2;
  cf_error1 = x1[0] - x1_gold_2;
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
           crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  return (error < 1e-3);
}

bool test_mmse_solver_avx(void) {
  cf_t cf_error0, cf_error1;
  float error = 0.0f;

  cf_t x0_gold_1 = RANDOM_CF();
  cf_t x1_gold_1 = RANDOM_CF();
  cf_t h00_1 = RANDOM_CF();
  cf_t h01_1 = RANDOM_CF();
  cf_t h10_1 = RANDOM_CF();
  cf_t h11_1 = (1 - h01_1 * h10_1) / h00_1;
  cf_t y0_1 = x0_gold_1 * h00_1 + x1_gold_1 * h01_1;
  cf_t y1_1 = x0_gold_1 * h10_1 + x1_gold_1 * h11_1;

  cf_t x0_gold_2 = RANDOM_CF();
  cf_t x1_gold_2 = RANDOM_CF();
  cf_t h00_2 = RANDOM_CF();
  cf_t h01_2 = RANDOM_CF();
  cf_t h10_2 = RANDOM_CF();
  cf_t h11_2 = (1 - h01_2 * h10_2) / h00_2;
  cf_t y0_2 = x0_gold_2 * h00_2 + x1_gold_2 * h01_2;
  cf_t y1_2 = x0_gold_2 * h10_2 + x1_gold_2 * h11_2;

  __m256 _y0 = _mm256_set_ps(cimagf(y0_1), crealf(y0_1), cimagf(y0_2), crealf(y0_2),
                             cimagf(y0_1), crealf(y0_1), cimagf(y0_2), crealf(y0_2));
  __m256 _y1 = _mm256_set_ps(cimagf(y1_1), crealf(y1_1), cimagf(y1_2), crealf(y1_2),
                             cimagf(y1_1), crealf(y1_1), cimagf(y1_2), crealf(y1_2));

  __m256 _h00 = _mm256_set_ps(cimagf(h00_1), crealf(h00_1), cimagf(h00_2), crealf(h00_2),
                              cimagf(h00_1), crealf(h00_1), cimagf(h00_2), crealf(h00_2));
  __m256 _h01 = _mm256_set_ps(cimagf(h01_1), crealf(h01_1), cimagf(h01_2), crealf(h01_2),
                              cimagf(h01_1), crealf(h01_1), cimagf(h01_2), crealf(h01_2));
  __m256 _h10 = _mm256_set_ps(cimagf(h10_1), crealf(h10_1), cimagf(h10_2), crealf(h10_2),
                              cimagf(h10_1), crealf(h10_1), cimagf(h10_2), crealf(h10_2));
  __m256 _h11 = _mm256_set_ps(cimagf(h11_1), crealf(h11_1), cimagf(h11_2), crealf(h11_2),
                              cimagf(h11_1), crealf(h11_1), cimagf(h11_2), crealf(h11_2));

  __m256 _x0, _x1;

  srslte_mat_2x2_mmse_avx(_y0, _y1, _h00, _h01, _h10, _h11, &_x0, &_x1, 0.0f, 1.0f);


  __attribute__((aligned(256))) cf_t x0[4];
  __attribute__((aligned(256))) cf_t x1[4];

  _mm256_store_ps((float *) x0, _x0);
  _mm256_store_ps((float *) x1, _x1);

  cf_error0 = x0[1] - x0_gold_1;
  cf_error1 = x1[1] - x1_gold_1;
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
           crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  cf_error0 = x0[0] - x0_gold_2;
  cf_error1 = x1[0] - x1_gold_2;
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
           crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  return (error < 1e-3);
}

#endif /* LV_HAVE_AVX */

#if SRSLTE_SIMD_CF_SIZE != 0

bool test_zf_solver_simd(void) {
  cf_t cf_error0, cf_error1;
  float error = 0.0f;

  cf_t x0_gold_1 = RANDOM_CF();
  cf_t x1_gold_1 = RANDOM_CF();
  cf_t h00_1 = RANDOM_CF();
  cf_t h01_1 = RANDOM_CF();
  cf_t h10_1 = RANDOM_CF();
  cf_t h11_1 = (1 - h01_1 * h10_1) / h00_1;
  cf_t y0_1 = x0_gold_1 * h00_1 + x1_gold_1 * h01_1;
  cf_t y1_1 = x0_gold_1 * h10_1 + x1_gold_1 * h11_1;

  simd_cf_t _y0 = srslte_simd_cf_set1(y0_1);
  simd_cf_t _y1 = srslte_simd_cf_set1(y1_1);

  simd_cf_t _h00 = srslte_simd_cf_set1(h00_1);
  simd_cf_t _h01 = srslte_simd_cf_set1(h01_1);
  simd_cf_t _h10 = srslte_simd_cf_set1(h10_1);
  simd_cf_t _h11 = srslte_simd_cf_set1(h11_1);

  simd_cf_t _x0, _x1;

  srslte_mat_2x2_zf_simd(_y0, _y1, _h00, _h01, _h10, _h11, &_x0, &_x1, 1.0f);

  __attribute__((aligned(SRSLTE_SIMD_BIT_ALIGN))) cf_t x0[SRSLTE_SIMD_CF_SIZE];
  __attribute__((aligned(SRSLTE_SIMD_BIT_ALIGN))) cf_t x1[SRSLTE_SIMD_CF_SIZE];

  srslte_simd_cfi_store(x0, _x0);
  srslte_simd_cfi_store(x1, _x1);

  cf_error0 = x0[1] - x0_gold_1;
  cf_error1 = x1[1] - x1_gold_1;
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
      crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  return (error < 1e-3);
}

bool test_mmse_solver_simd(void) {
  cf_t cf_error0, cf_error1;
  float error = 0.0f;

  cf_t x0_gold[SRSLTE_SIMD_CF_SIZE];
  cf_t x1_gold[SRSLTE_SIMD_CF_SIZE];
  cf_t h00[SRSLTE_SIMD_CF_SIZE];
  cf_t h01[SRSLTE_SIMD_CF_SIZE];
  cf_t h10[SRSLTE_SIMD_CF_SIZE];
  cf_t h11[SRSLTE_SIMD_CF_SIZE];
  cf_t y0[SRSLTE_SIMD_CF_SIZE];
  cf_t y1[SRSLTE_SIMD_CF_SIZE];
  for (int i = 0; i < SRSLTE_SIMD_CF_SIZE; i++) {
    x0_gold[i] = RANDOM_CF();
    x1_gold[i] = RANDOM_CF();
    h00[i] = RANDOM_CF();
    h01[i] = RANDOM_CF();
    h10[i] = RANDOM_CF();
    h11[i] = (1 - h01[i] * h10[i]) / h00[i];
    y0[i] = x0_gold[i] * h00[i]+ x1_gold[i] * h01[i];
    y1[i] = x0_gold[i] * h10[i] + x1_gold[i] * h11[i];
  }

  simd_cf_t _y0 = srslte_simd_cfi_loadu(y0);
  simd_cf_t _y1 = srslte_simd_cfi_loadu(y1);

  simd_cf_t _h00 = srslte_simd_cfi_loadu(h00);
  simd_cf_t _h01 = srslte_simd_cfi_loadu(h01);
  simd_cf_t _h10 = srslte_simd_cfi_loadu(h10);
  simd_cf_t _h11 = srslte_simd_cfi_loadu(h11);

  simd_cf_t _x0, _x1;

  srslte_mat_2x2_mmse_simd(_y0, _y1, _h00, _h01, _h10, _h11, &_x0, &_x1, 0.0f, 1.0f);

  __attribute__((aligned(SRSLTE_SIMD_BIT_ALIGN))) cf_t x0[SRSLTE_SIMD_CF_SIZE];
  __attribute__((aligned(SRSLTE_SIMD_BIT_ALIGN))) cf_t x1[SRSLTE_SIMD_CF_SIZE];

  srslte_simd_cfi_store(x0, _x0);
  srslte_simd_cfi_store(x1, _x1);

  cf_error0 = x0[1] - x0_gold[1];
  cf_error1 = x1[1] - x1_gold[1];
  error += crealf(cf_error0) * crealf(cf_error0) + cimagf(cf_error0) * cimagf(cf_error0) +
      crealf(cf_error1) * crealf(cf_error1) + cimagf(cf_error1) * cimagf(cf_error1);

  return (error < 1e-3);
}

#endif /* SRSLTE_SIMD_CF_SIZE != 0 */

bool test_vec_dot_prod_ccc(void) {
  __attribute__((aligned(256))) cf_t a[14];
  __attribute__((aligned(256))) cf_t b[14];
  cf_t res = 0, gold = 0;

  for (int i = 0; i < 14; i++) {
    a[i] = RANDOM_CF();
    b[i] = RANDOM_CF();
  }

  res = srslte_vec_dot_prod_ccc(a, b, 14);

  for (int i=0;i<14;i++) {
    gold += a[i]*b[i];
  }

  return (cabsf(res - gold) < 1e-3);
}

int main(int argc, char **argv) {
  bool passed = true;
  int ret = 0;

  parse_args(argc, argv);

  if (zf_solver) {
    RUN_TEST(test_zf_solver_gen);

#ifdef LV_HAVE_SSE
    RUN_TEST(test_zf_solver_sse);
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX
    RUN_TEST(test_zf_solver_avx);
#endif /* LV_HAVE_AVX */

#if SRSLTE_SIMD_CF_SIZE != 0
    RUN_TEST(test_zf_solver_simd);
#endif /* SRSLTE_SIMD_CF_SIZE != 0*/
  }

  if (mmse_solver) {
    RUN_TEST(test_mmse_solver_gen);

#ifdef LV_HAVE_SSE
    RUN_TEST(test_mmse_solver_sse);
#endif /* LV_HAVE_SSE */


#ifdef LV_HAVE_AVX
    RUN_TEST(test_mmse_solver_avx);
#endif /* LV_HAVE_AVX */

#if SRSLTE_SIMD_CF_SIZE != 0
    RUN_TEST(test_mmse_solver_simd);
#endif /* SRSLTE_SIMD_CF_SIZE != 0*/
  }

  RUN_TEST(test_vec_dot_prod_ccc);

  printf("%s!\n", (passed) ? "Ok" : "Failed");

  if (!passed) {
    exit(ret);
  }

  exit(ret);
}
