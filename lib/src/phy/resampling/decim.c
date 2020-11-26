/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/phy/resampling/decim.h"
#include "srslte/phy/utils/debug.h"
#include <complex.h>
#include <math.h>

/* Performs integer linear decimation by a factor of M */
void srslte_decim_c(cf_t* input, cf_t* output, int M, int len)
{
  int i;
  for (i = 0; i < len / M; i++) {
    output[i] = input[i * M];
  }
}

/* Performs integer linear decimation by a factor of M */
void srslte_decim_f(float* input, float* output, int M, int len)
{
  int i;
  for (i = 0; i < len / M; i++) {
    output[i] = input[i * M];
  }
}
