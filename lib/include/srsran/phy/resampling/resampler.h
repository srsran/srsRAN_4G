/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

/******************************************************************************
 *  File:         resampler.h
 *
 *  Description:  Linear and vector interpolation
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_RESAMPLER_H
#define SRSRAN_RESAMPLER_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/dft/dft.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Resampler operating modes
 */
typedef enum {
  SRSRAN_RESAMPLER_MODE_INTERPOLATE = 0,
  SRSRAN_RESAMPLER_MODE_DECIMATE,
} srsran_resampler_mode_t;

/**
 * Resampler internal buffers and subcomponents
 */
typedef struct {
  srsran_resampler_mode_t mode;
  uint32_t                ratio;
  uint32_t                window_sz;
  srsran_dft_plan_t       fft;
  srsran_dft_plan_t       ifft;
  uint32_t                state_len;
  cf_t*                   in_buffer;
  cf_t*                   out_buffer;
  cf_t*                   state;
  cf_t*                   filter;
} srsran_resampler_fft_t;

/**
 * Initialise an FFT based resampler which can be configured as decimator or interpolator.
 * @param q Object pointer
 * @param mode Determines whether the operation mode is decimation or interpolation
 * @param ratio Operational ratio
 * @return SRSRAN_SUCCES if no error, otherwise an SRSRAN error code
 */
SRSRAN_API int srsran_resampler_fft_init(srsran_resampler_fft_t* q, srsran_resampler_mode_t mode, uint32_t ratio);

/**
 * @brief resets internal re-sampler state
 * @param q Object pointer
 */
SRSRAN_API void srsran_resampler_fft_reset_state(srsran_resampler_fft_t* q);

/**
 * Get delay from the FFT based resampler.
 * @param q Object pointer
 * @return the delay in number of samples
 */
SRSRAN_API uint32_t srsran_resampler_fft_get_delay(srsran_resampler_fft_t* q);

/**
 * @brief Run FFT based resampler in the initiated mode.
 *
 * @note Setting the input to NULL is equivalent of feeding zeroes
 * @note Setting the output to NULL is equivalent of dropping output samples
 *
 * @param q Object pointer, make sure it has been initialised
 * @param input Points at the input complex buffer
 * @param output Points at the output complex buffer
 * @param nsamples Number of samples to apply the processing
 */
SRSRAN_API void srsran_resampler_fft_run(srsran_resampler_fft_t* q, const cf_t* input, cf_t* output, uint32_t nsamples);

/**
 * Free FFT based resampler buffers and subcomponents
 * @param q  Object pointer
 */
SRSRAN_API void srsran_resampler_fft_free(srsran_resampler_fft_t* q);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_RESAMPLER_H
