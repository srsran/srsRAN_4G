/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include <srslte/phy/utils/vector.h>

float srslte_ch_awgn_get_variance(float ebno_db, float rate) {
  float esno_db = ebno_db + srslte_convert_power_to_dB(rate);
  return srslte_convert_dB_to_amplitude(-esno_db);
}

void srslte_ch_awgn_c(const cf_t* x, cf_t* y, float variance, uint32_t len) {
  cf_t tmp;
  uint32_t i;

  for (i=0;i<len;i++) {
    __real__ tmp = rand_gauss();
    __imag__ tmp = rand_gauss();
    tmp *= variance;
    y[i] = tmp + x[i];
  }
}
void srslte_ch_awgn_f(const float* x, float* y, float variance, uint32_t len) {
  uint32_t i;

  for (i=0;i<len;i++) {
    y[i] = x[i] + variance * rand_gauss();
  }
}
