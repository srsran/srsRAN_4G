// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include "srsran/phy/resampling/decim.h"
#include "srsran/phy/utils/debug.h"
#include <complex.h>
#include <math.h>

/* Performs integer linear decimation by a factor of M */
void srsran_decim_c(cf_t* input, cf_t* output, int M, int len)
{
  int i;
  for (i = 0; i < len / M; i++) {
    output[i] = input[i * M];
  }
}

/* Performs integer linear decimation by a factor of M */
void srsran_decim_f(float* input, float* output, int M, int len)
{
  int i;
  for (i = 0; i < len / M; i++) {
    output[i] = input[i * M];
  }
}
