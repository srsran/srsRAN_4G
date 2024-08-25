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

#ifndef SRSRAN_CEDRON_FREQ_ESTIMATOR_H
#define SRSRAN_CEDRON_FREQ_ESTIMATOR_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include <stdio.h>

typedef struct {
  // DFT
  void* in;  // Input buffer
  void* out; // Output buffer
  void* p;   // DFT plan
  cf_t* X;   // Output buffer as cf_t*
  int   init_size;
  int   fft_size; // Currently used FFT size

} srsran_cedron_freq_est_t;

SRSRAN_API int srsran_cedron_freq_est_init(srsran_cedron_freq_est_t* q, uint32_t nof_prbs);

SRSRAN_API void srsran_cedron_freq_est_free(srsran_cedron_freq_est_t* q);

SRSRAN_API int srsran_cedron_freq_est_replan_c(srsran_cedron_freq_est_t* q, int new_dft_points);

SRSRAN_API float srsran_cedron_freq_estimate(srsran_cedron_freq_est_t* q, const cf_t* x, int len);

#endif // SRSRAN_CEDRON_FREQ_ESTIMATOR_H
