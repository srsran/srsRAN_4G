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
 * @brief resets internal re-sampler state
 * @param q Object pointer
 */
SRSLTE_API void srslte_resampler_fft_reset_state(srslte_resampler_fft_t* q);

/**
 * Get delay from the FFT based resampler.
 * @param q Object pointer
 * @return the delay in number of samples
 */
SRSLTE_API uint32_t srslte_resampler_fft_get_delay(srslte_resampler_fft_t* q);

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
