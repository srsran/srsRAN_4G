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

/******************************************************************************
 *  File:         resampler.h
 *
 *  Description:  Linear and vector interpolation
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_RESAMPLER_H
#define SRSLTE_RESAMPLER_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/dft/dft.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Resampler operating modes
 */
typedef enum {
  SRSLTE_RESAMPLER_MODE_INTERPOLATE = 0,
  SRSLTE_RESAMPLER_MODE_DECIMATE,
} srslte_resampler_mode_t;

/**
 * Resampler internal buffers and subcomponents
 */
typedef struct {
  srslte_resampler_mode_t mode;
  uint32_t                ratio;
  uint32_t                window_sz;
  srslte_dft_plan_t       fft;
  srslte_dft_plan_t       ifft;
  uint32_t                state_len;
  cf_t*                   in_buffer;
  cf_t*                   out_buffer;
  cf_t*                   state;
  cf_t*                   filter;
} srslte_resampler_fft_t;

/**
 * Initialise an FFT based resampler which can be configured as decimator or interpolator.
 * @param q Object pointer
 * @param mode Determines whether the operation mode is decimation or interpolation
 * @param ratio Operational ratio
 * @return SRSLTE_SUCCES if no error, otherwise an SRSLTE error code
 */
SRSLTE_API int srslte_resampler_fft_init(srslte_resampler_fft_t* q, srslte_resampler_mode_t mode, uint32_t ratio);

/**
 * Get delay from the FFT based resampler.
 * @param q Object pointer
 * @return the delay in number of samples
 */
SRSLTE_API uint32_t srslte_resampler_fft_get_delay(srslte_resampler_fft_t* q);

/**
 * Run FFT based resampler in the initiated mode.
 * @param q Object pointer, make sure it has been initialised
 * @param input Points at the input complex buffer
 * @param output Points at the output complex buffer
 * @param nsamples Number of samples to apply the processing
 */
SRSLTE_API void srslte_resampler_fft_run(srslte_resampler_fft_t* q, const cf_t* input, cf_t* output, uint32_t nsamples);

/**
 * Free FFT based resampler buffers and subcomponents
 * @param q  Object pointer
 */
SRSLTE_API void srslte_resampler_fft_free(srslte_resampler_fft_t* q);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_RESAMPLER_H
