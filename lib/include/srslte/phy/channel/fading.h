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

#ifndef SRSLTE_FADING_H
#define SRSLTE_FADING_H

#include "srslte/config.h"
#include "srslte/phy/common/timestamp.h"
#include "srslte/phy/dft/dft.h"
#include <inttypes.h>
#include <stdint.h>

#define SRSLTE_CHANNEL_FADING_MAXTAPS 9
#define SRSLTE_CHANNEL_FADING_NTERMS 16

typedef enum {
  srslte_channel_fading_model_none = 0,
  srslte_channel_fading_model_epa,
  srslte_channel_fading_model_eva,
  srslte_channel_fading_model_etu,
} srslte_channel_fading_model_t;

typedef struct {
  // Configuration parameters
  float                         srate;   // Sampling rate: 1.92e6, 3.84e6, ..., 23.04e6, 30.72e6
  srslte_channel_fading_model_t model;   // None, EPA, EVA, ETU
  float                         doppler; // Maximum doppler: 5, 70, 300

  // Internal tap parametrisation
  uint32_t N;          // FFT size
  uint32_t path_delay; // Path delay
  uint32_t state_len;  // Length of the impulse response saved in the state

  float coeff_alpha[SRSLTE_CHANNEL_FADING_MAXTAPS][SRSLTE_CHANNEL_FADING_NTERMS]; // Angle of arrival
  float coeff_a[SRSLTE_CHANNEL_FADING_MAXTAPS][SRSLTE_CHANNEL_FADING_NTERMS];     // Random phase
  float coeff_b[SRSLTE_CHANNEL_FADING_MAXTAPS][SRSLTE_CHANNEL_FADING_NTERMS];     // Random phase
  cf_t* h_tap[SRSLTE_CHANNEL_FADING_MAXTAPS]; // Static tap signal in frequency domain

  // Utils
  srslte_dft_plan_t fft;             // DFT to frequency domain
  srslte_dft_plan_t ifft;            // DFT to time domain
  cf_t*             temp;            // Temporal buffer, length fft_size
  cf_t*             h_freq;          // Channel frequency response, length fft_size
  cf_t*             y_freq;          // Intermediate frequency domain buffer
  float             sin_table[1024]; // Table of sinus values

  // State variables
  cf_t* state; // To save impulse response of the filter
} srslte_channel_fading_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSLTE_API int srslte_channel_fading_init(srslte_channel_fading_t* q, double srate, const char* model, uint32_t seed);

SRSLTE_API void srslte_channel_fading_free(srslte_channel_fading_t* q);

SRSLTE_API double srslte_channel_fading_execute(srslte_channel_fading_t* q,
                                                const cf_t*              in,
                                                cf_t*                    out,
                                                uint32_t                 nof_samples,
                                                double                   init_time);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_FADING_H
