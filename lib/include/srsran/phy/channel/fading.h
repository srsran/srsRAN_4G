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

#ifndef SRSRAN_FADING_H
#define SRSRAN_FADING_H

#include "srsran/config.h"
#include "srsran/phy/common/timestamp.h"
#include "srsran/phy/dft/dft.h"
#include <inttypes.h>
#include <stdint.h>

#define SRSRAN_CHANNEL_FADING_MAXTAPS 9
#define SRSRAN_CHANNEL_FADING_NTERMS 16

typedef enum {
  srsran_channel_fading_model_none = 0,
  srsran_channel_fading_model_epa,
  srsran_channel_fading_model_eva,
  srsran_channel_fading_model_etu,
} srsran_channel_fading_model_t;

typedef struct {
  // Configuration parameters
  float                         srate;   // Sampling rate: 1.92e6, 3.84e6, ..., 23.04e6, 30.72e6
  srsran_channel_fading_model_t model;   // None, EPA, EVA, ETU
  float                         doppler; // Maximum doppler: 5, 70, 300

  // Internal tap parametrisation
  uint32_t N;          // FFT size
  uint32_t path_delay; // Path delay
  uint32_t state_len;  // Length of the impulse response saved in the state

  float coeff_alpha[SRSRAN_CHANNEL_FADING_MAXTAPS][SRSRAN_CHANNEL_FADING_NTERMS]; // Angle of arrival
  float coeff_a[SRSRAN_CHANNEL_FADING_MAXTAPS][SRSRAN_CHANNEL_FADING_NTERMS];     // Random phase
  float coeff_b[SRSRAN_CHANNEL_FADING_MAXTAPS][SRSRAN_CHANNEL_FADING_NTERMS];     // Random phase
  cf_t* h_tap[SRSRAN_CHANNEL_FADING_MAXTAPS]; // Static tap signal in frequency domain

  // Utils
  srsran_dft_plan_t fft;             // DFT to frequency domain
  srsran_dft_plan_t ifft;            // DFT to time domain
  cf_t*             temp;            // Temporal buffer, length fft_size
  cf_t*             h_freq;          // Channel frequency response, length fft_size
  cf_t*             y_freq;          // Intermediate frequency domain buffer
  float             sin_table[1024]; // Table of sinus values

  // State variables
  cf_t* state; // To save impulse response of the filter
} srsran_channel_fading_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSRAN_API int srsran_channel_fading_init(srsran_channel_fading_t* q, double srate, const char* model, uint32_t seed);

SRSRAN_API void srsran_channel_fading_free(srsran_channel_fading_t* q);

SRSRAN_API double srsran_channel_fading_execute(srsran_channel_fading_t* q,
                                                const cf_t*              in,
                                                cf_t*                    out,
                                                uint32_t                 nof_samples,
                                                double                   init_time);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_FADING_H
