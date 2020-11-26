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

#include <math.h>
#include <stdlib.h>

float rand_gauss(void)
{
  float v1, v2, s;

  do {
    v1 = 2.0 * (rand() / (float)RAND_MAX) - 1;
    v2 = 2.0 * (rand() / (float)RAND_MAX) - 1;

    s = v1 * v1 + v2 * v2;
  } while (s >= 1.0 || s == 0.0);

  return (v1 * sqrtf(-2.0 * log(s) / s));
}
