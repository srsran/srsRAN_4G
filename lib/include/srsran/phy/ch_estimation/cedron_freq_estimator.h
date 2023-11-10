/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
