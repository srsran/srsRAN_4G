/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/phy/ch_estimation/chest_common.h"
#include "srsran/phy/utils/convolution.h"
#include "srsran/phy/utils/vector.h"

uint32_t srsran_chest_set_triangle_filter(float* fil, int filter_len)
{
  for (int i = 0; i < filter_len / 2; i++) {
    fil[i]                      = i + 1;
    fil[i + filter_len / 2 + 1] = filter_len / 2 - i;
  }
  fil[filter_len / 2] = filter_len / 2 + 1;

  float s = 0;
  for (int i = 0; i < filter_len; i++) {
    s += fil[i];
  }
  for (int i = 0; i < filter_len; i++) {
    fil[i] /= s;
  }
  return filter_len;
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
float srsran_chest_estimate_noise_pilots(cf_t* noisy, cf_t* noiseless, cf_t* noise_vec, uint32_t nof_pilots)
{
  /* Substract noisy pilot estimates */
  srsran_vec_sub_ccc(noiseless, noisy, noise_vec, nof_pilots);

  /* Compute average power */
  float power = srsran_vec_avg_power_cf(noise_vec, nof_pilots);
  return power;
}

uint32_t srsran_chest_set_smooth_filter3_coeff(float* smooth_filter, float w)
{
  smooth_filter[0] = w;
  smooth_filter[2] = w;
  smooth_filter[1] = 1 - 2 * w;
  return 3;
}

uint32_t srsran_chest_set_smooth_filter_gauss(float* filter, uint32_t order, float std_dev)
{
  const uint32_t filterlen = order + 1;
  const int      center    = (filterlen - 1) / 2;

  if (!filterlen) {
    return 0;
  }

  for (int i = 0; i < filterlen; i++) {
    filter[i] = expf(-powf(i - center, 2) / (2.0f * powf(std_dev, 2)));
  }

  // Calculate average for normalization
  const float norm = srsran_vec_acc_ff(filter, filterlen);

  // Avoids NAN, INF or ZERO division
  if (!isnormal(norm)) {
    return 0;
  }

  // Normalize filter
  srsran_vec_sc_prod_fff(filter, 1.0f / norm, filter, filterlen);

  return filterlen;
}

void srsran_chest_average_pilots(cf_t*    input,
                                 cf_t*    output,
                                 float*   filter,
                                 uint32_t nof_ref,
                                 uint32_t nof_symbols,
                                 uint32_t filter_len)
{
  for (int l = 0; l < nof_symbols; l++) {
    srsran_conv_same_cf(&input[l * nof_ref], filter, &output[l * nof_ref], nof_ref, filter_len);
  }
}
