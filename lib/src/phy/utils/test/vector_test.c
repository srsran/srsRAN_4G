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
#include <immintrin.h>
#include <sys/time.h>
#include <srslte/phy/utils/vector_simd.h>
#include <memory.h>
#include <math.h>

#include "srslte/phy/utils/mat.h"
#include "srslte/phy/utils/simd.h"
#include "srslte/phy/utils/vector.h"


bool zf_solver = false;
bool mmse_solver = false;
bool verbose = false;

#define MAX_MSE (1e-3)
#define NOF_REPETITIONS (1024*128)
#define MAX_FUNCTIONS (64)
#define MAX_BLOCKS (16)

#define RANDOM_F() ((float)rand())/((float)RAND_MAX)
#define RANDOM_S() ((int16_t)(rand() && 0x800F))
#define RANDOM_CF() (RANDOM_F() + _Complex_I*RANDOM_F())

#define TEST_CALL(TEST_CODE)   gettimeofday(&start, NULL);\
  for (int i = 0; i < NOF_REPETITIONS; i++){TEST_CODE;}\
  gettimeofday(&end, NULL); \
  *timing = elapsed_us(&start, &end);

#define TEST(X, CODE) static bool test_##X (char *func_name, double *timing, uint32_t block_size) {\
    struct timeval start, end;\
    float mse = 0.0f;\
    bool passed;\
    strncpy(func_name, #X, 32);\
    CODE;\
    passed = (mse < MAX_MSE);\
    printf("%32s (%5d) ... %7.1f MSamp/s ... %3s Passed\n", func_name, block_size, \
    (double) block_size*NOF_REPETITIONS/ *timing, passed?"":"Not");\
    return passed;\
}

#define MALLOC(TYPE, NAME) TYPE *NAME = srslte_vec_malloc(sizeof(TYPE)*block_size)


static double elapsed_us(struct timeval *ts_start, struct timeval *ts_end) {
  if (ts_end->tv_usec > ts_start->tv_usec) {
    return ((double) ts_end->tv_sec - (double) ts_start->tv_sec) * 1000000 +
           (double) ts_end->tv_usec - (double) ts_start->tv_usec;
  } else {
    return ((double) ts_end->tv_sec - (double) ts_start->tv_sec - 1) * 1000000 +
           ((double) ts_end->tv_usec + 1000000) - (double) ts_start->tv_usec;
  }
}

float squared_error (cf_t a, cf_t b) {
  float diff_re = __real__ a - __real__ b;
  float diff_im = __imag__ a - __imag__ b;
  return diff_re*diff_re + diff_im*diff_im;
}

TEST(srslte_vec_dot_prod_sss,
     MALLOC(int16_t, x);
         MALLOC(int16_t, y);
         int16_t z;

         cf_t gold = 0.0f;
         for (int i = 0; i < block_size; i++) {
           x[i] = RANDOM_S();
           y[i] = RANDOM_S();
         }

         TEST_CALL(z = srslte_vec_dot_prod_sss(x, y, block_size))

         for (int i = 0; i < block_size; i++) {
           gold += x[i] * y[i];
         }

         mse += cabsf(gold - z) / cabsf(gold);

         free(x);
         free(y);
)

TEST(srslte_vec_sum_sss,
     MALLOC(int16_t, x);
         MALLOC(int16_t, y);
         MALLOC(int16_t, z);

         cf_t gold = 0.0f;
         for (int i = 0; i < block_size; i++) {
           x[i] = RANDOM_S();
           y[i] = RANDOM_S();
         }

         TEST_CALL(srslte_vec_sum_sss(x, y, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold = x[i] + y[i];
           mse += cabsf(gold - z[i]);
         }

         free(x);
         free(y);
         free(z);
)

TEST(srslte_vec_sub_sss,
     MALLOC(int16_t, x);
         MALLOC(int16_t, y);
         MALLOC(int16_t, z);

         cf_t gold = 0.0f;
         for (int i = 0; i < block_size; i++) {
           x[i] = RANDOM_S();
           y[i] = RANDOM_S();
         }

         TEST_CALL(srslte_vec_sub_sss(x, y, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold = x[i] - y[i];
           mse += cabsf(gold - z[i]);
         }

         free(x);
         free(y);
         free(z);
)

TEST(srslte_vec_prod_sss,
     MALLOC(int16_t, x);
         MALLOC(int16_t, y);
         MALLOC(int16_t, z);

         cf_t gold = 0.0f;
         for (int i = 0; i < block_size; i++) {
           x[i] = RANDOM_S();
           y[i] = RANDOM_S();
         }

         TEST_CALL(srslte_vec_prod_sss(x, y, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold = x[i] * y[i];
           mse += cabsf(gold - z[i]);
         }

         free(x);
         free(y);
         free(z);
)

TEST(srslte_vec_acc_cc,
     MALLOC(cf_t, x);
         cf_t z;

         cf_t gold = 0.0f;
         for (int i = 0; i < block_size; i++) {
           x[i] = RANDOM_F();
         }

         TEST_CALL(z = srslte_vec_acc_cc(x, block_size))

         for (int i = 0; i < block_size; i++) {
           gold += x[i];
         }

         mse += cabsf(gold - z)/cabsf(gold);

         free(x);
)


TEST(srslte_vec_sum_fff,
     MALLOC(float, x);
         MALLOC(float, y);
         MALLOC(float, z);

         cf_t gold = 0.0f;
         for (int i = 0; i < block_size; i++) {
         x[i] = RANDOM_F();
         y[i] = RANDOM_F();
     }

         TEST_CALL(srslte_vec_sum_fff(x, y, z, block_size))

         for (int i = 0; i < block_size; i++) {
         gold = x[i] + y[i];
         mse += cabsf(gold - z[i]);
     }

         free(x);
         free(y);
)

TEST(srslte_vec_sub_fff,
     MALLOC(float, x);
         MALLOC(float, y);
         MALLOC(float, z);

         cf_t gold = 0.0f;
         for (int i = 0; i < block_size; i++) {
         x[i] = RANDOM_F();
         y[i] = RANDOM_F();
     }

         TEST_CALL(srslte_vec_sub_fff(x, y, z, block_size))

         for (int i = 0; i < block_size; i++) {
         gold = x[i] - y[i];
         mse += cabsf(gold - z[i]);
     }

         free(x);
         free(y);
)

TEST(srslte_vec_dot_prod_ccc,
     MALLOC(cf_t, x);
         MALLOC(cf_t, y);
         cf_t z;

         cf_t gold = 0.0f;
         for (int i = 0; i < block_size; i++) {
           x[i] = RANDOM_CF();
           y[i] = RANDOM_CF();
         }

         TEST_CALL(z = srslte_vec_dot_prod_ccc(x, y, block_size))

         for (int i = 0; i < block_size; i++) {
           gold += x[i] * y[i];
         }

         mse = cabsf(gold - z) / cabsf(gold);

         free(x);
         free(y);
)

TEST(srslte_vec_dot_prod_conj_ccc,
     MALLOC(cf_t, x);
         MALLOC(cf_t, y);
         cf_t z;

         cf_t gold = 0.0f;
         for (int i = 0; i < block_size; i++) {
           x[i] = RANDOM_CF();
           y[i] = RANDOM_CF();
         }

         TEST_CALL(z = srslte_vec_dot_prod_conj_ccc(x, y, block_size))

         for (int i = 0; i < block_size; i++) {
           gold += x[i] * conjf(y[i]);
         }

         mse = cabsf(gold - z) / cabsf(gold);

         free(x);
         free(y);
)

TEST(srslte_vec_prod_ccc,
  MALLOC(cf_t, x);
  MALLOC(cf_t, y);
  MALLOC(cf_t, z);

  cf_t gold;
  for (int i = 0; i < block_size; i++) {
    x[i] = RANDOM_CF();
    y[i] = RANDOM_CF();
  }

  TEST_CALL(srslte_vec_prod_ccc(x, y, z, block_size))

  for (int i = 0; i < block_size; i++) {
    gold = x[i] * y[i];
    mse += cabsf(gold - z[i]);
  }

  free(x);
  free(z);
)

TEST(srslte_vec_prod_conj_ccc,
  MALLOC(cf_t, x);
  MALLOC(cf_t, y);
  MALLOC(cf_t, z);

  cf_t gold;
  for (int i = 0; i < block_size; i++) {
    x[i] = RANDOM_CF();
    y[i] = RANDOM_CF();
  }

  TEST_CALL(srslte_vec_prod_conj_ccc(x, y, z, block_size))

  for (int i = 0; i < block_size; i++) {
    gold = x[i] * conjf(y[i]);
    mse += cabsf(gold - z[i]);
  }

  free(x);
  free(z);
)

TEST(srslte_vec_sc_prod_ccc,
  MALLOC(cf_t, x);
  MALLOC(cf_t, z);
  cf_t y = RANDOM_F();

  cf_t gold;
  for (int i = 0; i < block_size; i++) {
    x[i] = RANDOM_CF();
  }

  TEST_CALL(srslte_vec_sc_prod_ccc(x, y, z, block_size))

  for (int i = 0; i < block_size; i++) {
    gold = x[i] * y;
    mse += cabsf(gold - z[i]);
  }

  free(x);
  free(z);
)

TEST(srslte_vec_prod_fff,
  MALLOC(float, x);
  MALLOC(float, y);
  MALLOC(float, z);

  cf_t gold;
  for (int i = 0; i < block_size; i++) {
    x[i] = RANDOM_CF();
    y[i] = RANDOM_CF();
  }

  TEST_CALL(srslte_vec_prod_fff(x, y, z, block_size))

  for (int i = 0; i < block_size; i++) {
    gold = x[i] * y[i];
    mse += cabsf(gold - z[i]);
  }

  free(x);
  free(z);
)

TEST(srslte_vec_sc_prod_fff,
  MALLOC(float, x);
  MALLOC(float, z);
  float y = RANDOM_F();

  float gold;
  for (int i = 0; i < block_size; i++) {
    x[i] = RANDOM_CF();
  }

  TEST_CALL(srslte_vec_sc_prod_fff(x, y, z, block_size))

  for (int i = 0; i < block_size; i++) {
    gold = x[i] * y;
    mse += cabsf(gold - z[i]);
  }

  free(x);
  free(z);
)

TEST(srslte_vec_abs_cf,
  MALLOC(cf_t, x);
  MALLOC(float, z);
  float gold;

  for (int i = 0; i < block_size; i++) {
    x[i] = RANDOM_CF();
  }

  TEST_CALL(srslte_vec_abs_cf(x, z, block_size))

  for (int i = 0; i < block_size; i++) {
    gold = sqrtf(crealf(x[i]) * crealf(x[i]) + cimagf(x[i])*cimagf(x[i]));
    mse += cabsf(gold - z[i]);
  }

  free(x);
  free(z);
)

TEST(srslte_vec_abs_square_cf,
  MALLOC(cf_t, x);
  MALLOC(float, z);
  float gold;

  for (int i = 0; i < block_size; i++) {
    x[i] = RANDOM_CF();
  }

  TEST_CALL(srslte_vec_abs_square_cf(x, z, block_size))

  for (int i = 0; i < block_size; i++) {
    gold = crealf(x[i]) * crealf(x[i]) + cimagf(x[i])*cimagf(x[i]);
    mse += cabsf(gold - z[i]);
  }

  free(x);
  free(z);
)

TEST(srslte_vec_sc_prod_cfc,
  MALLOC(cf_t, x);
  MALLOC(cf_t, z);
  cf_t gold;
  float h = RANDOM_F();

  for (int i = 0; i < block_size; i++) {
    x[i] = RANDOM_CF();
  }

  TEST_CALL(srslte_vec_sc_prod_cfc(x, h, z, block_size))

  for (int i = 0; i < block_size; i++) {
    gold = x[i] * h;
    mse += cabsf(gold - z[i]);
  }

  free(x);
  free(z);
)

int main(int argc, char **argv) {
  char func_names[MAX_FUNCTIONS][32];
  double timmings[MAX_FUNCTIONS][MAX_BLOCKS];
  uint32_t sizes[32];
  uint32_t size_count = 0;
  uint32_t func_count = 0;
  bool passed = true;

  for (uint32_t block_size = 1; block_size <= 1024*16; block_size *= 2) {
    func_count = 0;

    passed &= test_srslte_vec_dot_prod_sss(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_sum_sss(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_sub_sss(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_prod_sss(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_acc_cc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_sum_fff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_sub_fff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_dot_prod_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_dot_prod_conj_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_prod_fff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_prod_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_prod_conj_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_sc_prod_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_sc_prod_fff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_abs_cf(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_abs_square_cf(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed &= test_srslte_vec_sc_prod_cfc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    sizes[size_count] = block_size;
    size_count++;
  }

  printf("\n");
  printf("%32s |", "Subroutine/MSps");
  for (int i = 0; i < size_count; i++) {
    printf(" %7d", sizes[i]);
  }
  printf("  |\n");

  for (int j = 0; j < 32; j++) {
    printf("-");
  }
  printf("-+-");
  for (int j = 0; j < size_count; j++) {
    printf("--------");
  }
  printf("-|\n");

  for (int i = 0; i < func_count; i++) {
    printf("%32s | ", func_names[i]);
    for (int j = 0; j < size_count; j++) {
      printf(" %7.1f", (double) NOF_REPETITIONS*(double)sizes[j]/timmings[i][j]);
    }
    printf(" |\n");
  }

  return (passed)?SRSLTE_SUCCESS:SRSLTE_ERROR;
}
