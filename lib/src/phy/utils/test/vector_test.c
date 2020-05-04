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
#include <srslte/phy/utils/random.h>

bool zf_solver   = false;
bool mmse_solver = false;
bool verbose     = false;

static uint32_t nof_repetitions = 1;

#define MAX_MSE (1e-3)
#define MAX_FUNCTIONS (64)
#define MAX_BLOCKS (16)

static srslte_random_t random_h = NULL;
#define RANDOM_F() srslte_random_uniform_real_dist(random_h, -1.0f, +1.0f)
#define RANDOM_S() ((int16_t)srslte_random_uniform_int_dist(random_h, -255, +255))
#define RANDOM_B() ((int8_t)srslte_random_uniform_int_dist(random_h, -127, +127))
#define RANDOM_CF() srslte_random_uniform_complex_dist(random_h, -1.0f, +1.0f)

#define TEST_CALL(TEST_CODE)                                                                                           \
  gettimeofday(&start, NULL);                                                                                          \
  for (int i = 0; i < nof_repetitions; i++) {                                                                          \
    TEST_CODE;                                                                                                         \
  }                                                                                                                    \
  gettimeofday(&end, NULL);                                                                                            \
  *timing = elapsed_us(&start, &end);

#define TEST(X, CODE)                                                                                                  \
  static bool test_##X(char* func_name, double* timing, uint32_t block_size)                                           \
  {                                                                                                                    \
    struct timeval start, end;                                                                                         \
    bzero(&start, sizeof(start));                                                                                      \
    bzero(&end, sizeof(end));                                                                                          \
    float mse    = 0.0f;                                                                                               \
    bool  passed = false;                                                                                              \
    strncpy(func_name, #X, 32);                                                                                        \
    CODE;                                                                                                              \
    passed = (mse < MAX_MSE);                                                                                          \
    printf("%32s (%5d) ... %7.1f MSamp/s ... %3s Passed (%.6f)\n",                                                     \
           func_name,                                                                                                  \
           block_size,                                                                                                 \
           (double)block_size* nof_repetitions / *timing,                                                              \
           passed ? "" : "Not",                                                                                        \
           mse);                                                                                                       \
    return passed;                                                                                                     \
  }

#define MALLOC(TYPE, NAME) TYPE* NAME = srslte_vec_malloc(sizeof(TYPE) * block_size)

static double elapsed_us(struct timeval* ts_start, struct timeval* ts_end)
{
  if (ts_end->tv_usec > ts_start->tv_usec) {
    return ((double)ts_end->tv_sec - (double)ts_start->tv_sec) * 1000000 + (double)ts_end->tv_usec -
           (double)ts_start->tv_usec;
  } else {
    return ((double)ts_end->tv_sec - (double)ts_start->tv_sec - 1) * 1000000 + ((double)ts_end->tv_usec + 1000000) -
           (double)ts_start->tv_usec;
  }
}

float squared_error(cf_t a, cf_t b)
{
  float diff_re = __real__ a - __real__ b;
  float diff_im = __imag__ a - __imag__ b;
  return diff_re * diff_re + diff_im * diff_im;
}

TEST(
    srslte_vec_xor_bbb, MALLOC(int8_t, x); MALLOC(int8_t, y); MALLOC(int8_t, z);

    cf_t gold = 0.0f;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_B();
      y[i] = RANDOM_B();
    }

    TEST_CALL(srslte_vec_xor_bbb(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x[i] ^ y[i];
          mse += cabsf(gold - z[i]);
        }

    free(x);
    free(y);
    free(z);)

TEST(srslte_vec_acc_ff, MALLOC(float, x); float z = 0;

     cf_t gold = 0.0f;
     for (int i = 0; i < block_size; i++) { x[i] = RANDOM_F(); }

     TEST_CALL(z = srslte_vec_acc_ff(x, block_size))

         for (int i = 0; i < block_size; i++) { gold += x[i]; }

     mse += cabsf(gold - z) / gold;

     free(x);)

TEST(
    srslte_vec_dot_prod_sss, MALLOC(int16_t, x); MALLOC(int16_t, y); int16_t z = 0;

    int16_t gold = 0.0f;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_S();
      y[i] = RANDOM_S();
    }

    TEST_CALL(z = srslte_vec_dot_prod_sss(x, y, block_size))

        for (int i = 0; i < block_size; i++) { gold += x[i] * y[i]; }

    mse = (gold - z) / abs(gold);

    free(x);
    free(y);)

TEST(
    srslte_vec_sum_sss, MALLOC(int16_t, x); MALLOC(int16_t, y); MALLOC(int16_t, z);

    int16_t gold = 0;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_S();
      y[i] = RANDOM_S();
    }

    TEST_CALL(srslte_vec_sum_sss(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x[i] + y[i];
          mse += abs(gold - z[i]);
        }

    free(x);
    free(y);
    free(z);)

TEST(
    srslte_vec_sub_sss, MALLOC(int16_t, x); MALLOC(int16_t, y); MALLOC(int16_t, z);

    int16_t gold = 0.0f;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_S();
      y[i] = RANDOM_S();
    }

    TEST_CALL(srslte_vec_sub_sss(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x[i] - y[i];
          mse += abs(gold - z[i]);
        }

    free(x);
    free(y);
    free(z);)

TEST(
    srslte_vec_prod_sss, MALLOC(int16_t, x); MALLOC(int16_t, y); MALLOC(int16_t, z);

    int16_t gold = 0.0f;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_S();
      y[i] = RANDOM_S();
    }

    TEST_CALL(srslte_vec_prod_sss(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x[i] * y[i];
          mse += abs(gold - z[i]);
        }

    free(x);
    free(y);
    free(z);)

TEST(
    srslte_vec_neg_sss, MALLOC(int16_t, x); MALLOC(int16_t, y); MALLOC(int16_t, z);

    int16_t gold = 0.0f;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_S();
      do {
        y[i] = RANDOM_S();
      } while (!y[i]);
    }

    TEST_CALL(srslte_vec_neg_sss(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = y[i] < 0 ? -x[i] : x[i];
          mse += abs(gold - z[i]);
        }

    free(x);
    free(y);
    free(z);)

TEST(srslte_vec_acc_cc, MALLOC(cf_t, x); cf_t z = 0.0f;

     cf_t gold = 0.0f;
     for (int i = 0; i < block_size; i++) { x[i] = RANDOM_F(); }

     TEST_CALL(z = srslte_vec_acc_cc(x, block_size))

         for (int i = 0; i < block_size; i++) { gold += x[i]; }

     mse += cabsf(gold - z) / cabsf(gold);

     free(x);)

TEST(
    srslte_vec_sum_fff, MALLOC(float, x); MALLOC(float, y); MALLOC(float, z);

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
    free(z);)

TEST(
    srslte_vec_sub_fff, MALLOC(float, x); MALLOC(float, y); MALLOC(float, z);

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
    free(z);)

TEST(
    srslte_vec_dot_prod_ccc, MALLOC(cf_t, x); MALLOC(cf_t, y); cf_t z = 0.0f;

    cf_t gold = 0.0f;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_CF();
      y[i] = RANDOM_CF();
    }

    TEST_CALL(z = srslte_vec_dot_prod_ccc(x, y, block_size))

        for (int i = 0; i < block_size; i++) { gold += x[i] * y[i]; }

    mse = cabsf(gold - z) / cabsf(gold);

    free(x);
    free(y);)

TEST(
    srslte_vec_dot_prod_conj_ccc, MALLOC(cf_t, x); MALLOC(cf_t, y); cf_t z = 0.0f;

    cf_t gold = 0.0f;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_CF();
      y[i] = RANDOM_CF();
    }

    TEST_CALL(z = srslte_vec_dot_prod_conj_ccc(x, y, block_size))

        for (int i = 0; i < block_size; i++) { gold += x[i] * conjf(y[i]); }

    mse = cabsf(gold - z) / cabsf(gold);

    free(x);
    free(y);)

TEST(
    srslte_vec_prod_ccc, MALLOC(cf_t, x); MALLOC(cf_t, y); MALLOC(cf_t, z);

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
    free(y);
    free(z);)

TEST(
    srslte_vec_prod_ccc_split, MALLOC(float, x_re); MALLOC(float, x_im); MALLOC(float, y_re); MALLOC(float, y_im);
    MALLOC(float, z_re);
    MALLOC(float, z_im);

    cf_t gold;
    for (int i = 0; i < block_size; i++) {
      x_re[i] = RANDOM_F();
      x_im[i] = RANDOM_F();
      y_re[i] = RANDOM_F();
      y_im[i] = RANDOM_F();
    }

    TEST_CALL(srslte_vec_prod_ccc_split(x_re, x_im, y_re, y_im, z_re, z_im, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = (x_re[i] + I * x_im[i]) * (y_re[i] + I * y_im[i]);
          mse += cabsf(gold - (z_re[i] + I * z_im[i]));
        }

    free(x_re);
    free(x_im);
    free(y_re);
    free(y_im);
    free(z_re);
    free(z_im);)

TEST(
    srslte_vec_prod_conj_ccc, MALLOC(cf_t, x); MALLOC(cf_t, y); MALLOC(cf_t, z);

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
    free(y);
    free(z);)

TEST(srslte_vec_sc_prod_ccc, MALLOC(cf_t, x); MALLOC(cf_t, z); cf_t y = RANDOM_CF();

     cf_t gold;
     for (int i = 0; i < block_size; i++) { x[i] = RANDOM_CF(); }

     TEST_CALL(srslte_vec_sc_prod_ccc(x, y, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold = x[i] * y;
           mse += cabsf(gold - z[i]);
         }

     free(x);
     free(z);)

TEST(srslte_vec_convert_fi, MALLOC(float, x); MALLOC(short, z); float scale = 1000.0f;

     short gold;
     for (int i = 0; i < block_size; i++) { x[i] = (float)RANDOM_F(); }

     TEST_CALL(srslte_vec_convert_fi(x, scale, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold       = (short)((x[i] * scale));
           double err = fabsf((float)gold - (float)z[i]);
           if (err > mse) {
             mse = err;
           }
         }

     free(x);
     free(z);)

TEST(srslte_vec_convert_if, MALLOC(int16_t, x); MALLOC(float, z); float scale = 1000.0f;

     float gold;
     float k = 1.0f / scale;
     for (int i = 0; i < block_size; i++) { x[i] = (int16_t)RANDOM_S(); }

     TEST_CALL(srslte_vec_convert_if(x, scale, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold       = ((float)x[i]) * k;
           double err = fabsf((float)gold - (float)z[i]);
           if (err > mse) {
             mse = err;
           }
         }

     free(x);
     free(z);)

TEST(
    srslte_vec_prod_fff, MALLOC(float, x); MALLOC(float, y); MALLOC(float, z);

    float gold;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_F();
      y[i] = RANDOM_F();
      if (isnan(x[i])) {
        printf("RANDOM_F!! x=%f; y=%f\n", x[i], y[i]);
      }
    }

    TEST_CALL(srslte_vec_prod_fff(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x[i] * y[i];
          if (isnan(gold)) {
            printf("x=%f; y=%f\n", x[i], y[i]);
          }
          mse += fabsf(gold - z[i]);
        }

    free(x);
    free(y);
    free(z);)

TEST(
    srslte_vec_prod_cfc, MALLOC(cf_t, x); MALLOC(float, y); MALLOC(cf_t, z);

    cf_t gold;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_CF();
      y[i] = RANDOM_F();
    }

    TEST_CALL(srslte_vec_prod_cfc(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x[i] * y[i];
          mse += cabsf(gold - z[i]);
        }

    free(x);
    free(y);
    free(z);)

TEST(srslte_vec_sc_prod_fff, MALLOC(float, x); MALLOC(float, z); float y = RANDOM_F();

     float gold;
     for (int i = 0; i < block_size; i++) { x[i] = RANDOM_CF(); }

     TEST_CALL(srslte_vec_sc_prod_fff(x, y, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold = x[i] * y;
           mse += fabsf(gold - z[i]);
         }

     free(x);
     free(z);)

TEST(
    srslte_vec_abs_cf, MALLOC(cf_t, x); MALLOC(float, z); float gold;

    for (int i = 0; i < block_size; i++) { x[i] = RANDOM_CF(); } x[0] = 0.0f;

    TEST_CALL(srslte_vec_abs_cf(x, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = sqrtf(crealf(x[i]) * crealf(x[i]) + cimagf(x[i]) * cimagf(x[i]));
          mse += fabsf(gold - z[i]) / block_size;
        }

    free(x);
    free(z);)

TEST(srslte_vec_abs_square_cf, MALLOC(cf_t, x); MALLOC(float, z); float gold;

     for (int i = 0; i < block_size; i++) { x[i] = RANDOM_CF(); }

     TEST_CALL(srslte_vec_abs_square_cf(x, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold = crealf(x[i]) * crealf(x[i]) + cimagf(x[i]) * cimagf(x[i]);
           mse += fabsf(gold - z[i]);
         }

     free(x);
     free(z);)

TEST(srslte_vec_sc_prod_cfc, MALLOC(cf_t, x); MALLOC(cf_t, z); cf_t gold; float h = RANDOM_F();

     for (int i = 0; i < block_size; i++) { x[i] = RANDOM_CF(); }

     TEST_CALL(srslte_vec_sc_prod_cfc(x, h, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold = x[i] * h;
           mse += cabsf(gold - z[i]);
         }

     free(x);
     free(z);)

TEST(
    srslte_vec_div_ccc, MALLOC(cf_t, x); MALLOC(cf_t, y); MALLOC(cf_t, z);

    cf_t gold;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_CF();
      y[i] = RANDOM_CF();
    }

    TEST_CALL(srslte_vec_div_ccc(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x[i] / y[i];
          mse += cabsf(gold - z[i]) / cabsf(gold);
        } mse /= block_size;

    free(x);
    free(y);
    free(z);)

TEST(
    srslte_vec_div_cfc, MALLOC(cf_t, x); MALLOC(float, y); MALLOC(cf_t, z);

    cf_t gold;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_CF();
      y[i] = RANDOM_F() + 0.0001f;
    }

    TEST_CALL(srslte_vec_div_cfc(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x[i] / y[i];
          mse += cabsf(gold - z[i]) / cabsf(gold);
        } mse /= block_size;

    free(x);
    free(y);
    free(z);)

TEST(
    srslte_vec_div_fff, MALLOC(float, x); MALLOC(float, y); MALLOC(float, z);

    cf_t gold;
    for (int i = 0; i < block_size; i++) {
      x[i] = RANDOM_F();
      y[i] = RANDOM_F() + 0.0001f;
    }

    TEST_CALL(srslte_vec_div_fff(x, y, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x[i] / y[i];
          mse += cabsf(gold - z[i]) / cabsf(gold);
        } mse /= block_size;

    free(x);
    free(y);
    free(z);)

TEST(
    srslte_vec_max_fi, MALLOC(float, x);

    for (int i = 0; i < block_size; i++) { x[i] = RANDOM_F(); }

    uint32_t max_index = 0;
    TEST_CALL(max_index = srslte_vec_max_fi(x, block_size);)

        float gold_value = -INFINITY;
    uint32_t  gold_index = 0;
    for (int i = 0; i < block_size; i++) {
      if (gold_value < x[i]) {
        gold_value = x[i];
        gold_index = i;
      }
    } mse = (gold_index != max_index) ? 1 : 0;

    free(x);)

TEST(
    srslte_vec_max_abs_fi, MALLOC(float, x);

    for (int i = 0; i < block_size; i++) { x[i] = RANDOM_F(); }

    uint32_t max_index = 0;
    TEST_CALL(max_index = srslte_vec_max_abs_fi(x, block_size);)

        float gold_value = -INFINITY;
    uint32_t  gold_index = 0;
    for (int i = 0; i < block_size; i++) {
      if (gold_value < fabsf(x[i])) {
        gold_value = fabsf(x[i]);
        gold_index = i;
      }
    } mse = (gold_index != max_index) ? 1 : 0;

    free(x);)

TEST(
    srslte_vec_max_abs_ci, MALLOC(cf_t, x);

    for (int i = 0; i < block_size; i++) { x[i] = RANDOM_CF(); }

    uint32_t max_index = 0;
    TEST_CALL(max_index = srslte_vec_max_abs_ci(x, block_size);)

        float gold_value = -INFINITY;
    uint32_t  gold_index = 0;
    for (int i = 0; i < block_size; i++) {
      cf_t  a    = x[i];
      float abs2 = __real__ a * __real__ a + __imag__ a * __imag__ a;
      if (abs2 > gold_value) {
        gold_value = abs2;
        gold_index = (uint32_t)i;
      }
    } mse = (gold_index != max_index) ? 1 : 0;

    free(x);)

TEST(srslte_vec_apply_cfo, MALLOC(cf_t, x); MALLOC(cf_t, z);

     const float cfo = 0.1f;
     cf_t        gold;
     for (int i = 0; i < block_size; i++) { x[i] = RANDOM_CF(); }

     TEST_CALL(srslte_vec_apply_cfo(x, cfo, z, block_size))

         for (int i = 0; i < block_size; i++) {
           gold = x[i] * cexpf(_Complex_I * 2.0f * (float)M_PI * i * cfo);
           mse += cabsf(gold - z[i]) / cabsf(gold);
         } mse /= block_size;

     free(x);
     free(z);)

TEST(
    srslte_vec_gen_sine, MALLOC(cf_t, z);

    const float freq = 0.1f;
    cf_t        gold;
    cf_t        x = RANDOM_CF();

    TEST_CALL(srslte_vec_gen_sine(x, freq, z, block_size))

        for (int i = 0; i < block_size; i++) {
          gold = x * cexpf(_Complex_I * 2.0f * (float)M_PI * i * freq);
          mse += cabsf(gold - z[i]) / cabsf(gold);
        } mse /= block_size;

    free(z);)

TEST(srslte_vec_estimate_frequency, MALLOC(cf_t, x); float freq_gold = 0.1f; float freq = 0.1f;

     for (int i = 0; i < block_size; i++) { x[i] = cexpf(-I * 2.0f * M_PI * (float)i * freq_gold); }

     TEST_CALL(freq = srslte_vec_estimate_frequency(x, block_size);) if (block_size < 6) { mse = 0.0f; } else {
       mse = fabsf(freq - freq_gold);
     }

     free(x);)

TEST(srslte_cfo_correct, srslte_cfo_t srslte_cfo; bzero(&srslte_cfo, sizeof(srslte_cfo)); MALLOC(cf_t, x);
     MALLOC(cf_t, z);

     const float cfo = 0.1f;
     cf_t        gold;
     for (int i = 0; i < block_size; i++) { x[i] = RANDOM_CF(); }

     srslte_cfo_init(&srslte_cfo, block_size);

     TEST_CALL(srslte_cfo_correct(&srslte_cfo, x, z, cfo))

         for (int i = 0; i < block_size; i++) {
           gold = x[i] * cexpf(_Complex_I * 2.0f * (float)M_PI * i * cfo);
           mse += cabsf(gold - z[i]) / cabsf(gold);
         } mse /= block_size;

     free(x);
     free(z);
     srslte_cfo_free(&srslte_cfo);)

TEST(srslte_cfo_correct_change, srslte_cfo_t srslte_cfo; bzero(&srslte_cfo, sizeof(srslte_cfo)); MALLOC(cf_t, x);
     MALLOC(cf_t, z);

     float cfo = 0.1f;
     cf_t  gold;
     for (int i = 0; i < block_size; i++) { x[i] = RANDOM_CF(); }

     srslte_cfo_init(&srslte_cfo, block_size);

     TEST_CALL(cfo = (i % 2) ? 0.1 : -0.1; srslte_cfo_correct(&srslte_cfo, x, z, cfo))

         for (int i = 0; i < block_size; i++) {
           gold = x[i] * cexpf(_Complex_I * 2.0f * (float)M_PI * i * cfo);
           mse += cabsf(gold - z[i]) / cabsf(gold);
         } mse /= block_size;

     free(x);
     free(z);
     srslte_cfo_free(&srslte_cfo);)

int main(int argc, char** argv)
{
  char     func_names[MAX_FUNCTIONS][32];
  double   timmings[MAX_FUNCTIONS][MAX_BLOCKS];
  uint32_t sizes[32];
  uint32_t size_count = 0;
  uint32_t func_count = 0;
  bool     passed[MAX_FUNCTIONS][MAX_BLOCKS];
  bool     all_passed = true;
  random_h            = srslte_random_init(0x1234);

  if (argc > 1) {
    nof_repetitions = (uint32_t)strtol(argv[1], NULL, 10);
  }

  for (uint32_t block_size = 1; block_size <= 1024 * 32; block_size *= 2) {
    func_count = 0;

    passed[func_count][size_count] =
        test_srslte_vec_xor_bbb(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_acc_ff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_dot_prod_sss(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_sum_sss(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_sub_sss(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_prod_sss(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_neg_sss(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_acc_cc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_sum_fff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_sub_fff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_dot_prod_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_dot_prod_conj_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_convert_fi(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_convert_if(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_prod_fff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_prod_cfc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_prod_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_prod_ccc_split(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_prod_conj_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_sc_prod_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_sc_prod_fff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_abs_cf(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_abs_square_cf(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_sc_prod_cfc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_div_ccc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_div_cfc(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_div_fff(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_max_fi(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_max_abs_fi(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_max_abs_ci(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_apply_cfo(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_gen_sine(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_vec_estimate_frequency(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_cfo_correct(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    passed[func_count][size_count] =
        test_srslte_cfo_correct_change(func_names[func_count], &timmings[func_count][size_count], block_size);
    func_count++;

    sizes[size_count] = block_size;
    size_count++;
  }

  char  fname[69];
  FILE* f = NULL;
  void* p = popen("(date +%g%m%d && hostname) | tr '\\r\\n' '__'", "r");
  if (p) {
    fgets(fname, 64, p);
    strncpy(fname + strnlen(fname, 64) - 1, ".tsv", 5);
    f = fopen(fname, "w");
    if (f) {
      printf("Saving benchmark results in '%s'\n", fname);
    }
    pclose(p);
  }

  printf("\n");
  printf("%32s |", "Subroutine/MSps");
  if (f)
    fprintf(f, "Subroutine/MSps Vs Vector size\t");
  for (int i = 0; i < size_count; i++) {
    printf(" %7d", sizes[i]);
    if (f)
      fprintf(f, "%d\t", sizes[i]);
  }
  printf("  |\n");
  if (f)
    fprintf(f, "\n");

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
    if (f)
      fprintf(f, "%s\t", func_names[i]);

    for (int j = 0; j < size_count; j++) {
      printf(" %s%7.1f\x1b[0m",
             (passed[i][j]) ? "" : "\x1B[31m",
             (double)nof_repetitions * (double)sizes[j] / timmings[i][j]);
      if (f)
        fprintf(f, "%.1f\t", (double)nof_repetitions * (double)sizes[j] / timmings[i][j]);

      all_passed &= passed[i][j];
    }
    printf(" |\n");
    if (f)
      fprintf(f, "\n");
  }

  if (f)
    fclose(f);
  srslte_random_free(random_h);

  return (all_passed) ? SRSLTE_SUCCESS : SRSLTE_ERROR;
}
