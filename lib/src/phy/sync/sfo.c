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

#include "srslte/phy/sync/sfo.h"
#include <stdio.h>
#include <stdlib.h>

/* Estimate SFO based on the array of time estimates t0
 * of length len. The parameter period is the time between t0 samples
 */
float srslte_sfo_estimate(int* t0, int len, float period)
{
  int   i;
  float sfo = 0.0;
  for (i = 1; i < len; i++) {
    sfo += (t0[i] - t0[i - 1]) / period / len;
  }
  return sfo;
}

/* Same as srslte_sfo_estimate but period is non-uniform.
 * Vector t is the sampling time times period for each t0
 */
float srslte_sfo_estimate_period(int* t0, int* t, int len, float period)
{
  int   i;
  float sfo = 0.0;
  for (i = 1; i < len; i++) {
    if (abs(t0[i] - t0[i - 1]) < 5000) {
      sfo += (t0[i] - t0[i - 1]) / (t[i] - t[i - 1]) / period;
    }
  }
  return sfo / (len - 2);
}
