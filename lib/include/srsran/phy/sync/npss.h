/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
 *  File:         npss.h
 *
 *  Description:  Narrowband Primary synchronization signal (NPSS) generation and detection.
 *
 *                The srsran_npss_synch_t object provides functions for fast
 *                computation of the crosscorrelation between the NPSS and received
 *                signal and CFO estimation. Also, the function srsran_npss_synch_tperiodic()
 *                is designed to be called periodically every subframe, taking
 *                care of the correct data alignment with respect to the NPSS sequence.
 *
 *                The object is designed to work with signals sampled at ?.? Mhz
 *                centered at the carrier frequency. Thus, downsampling is required
 *                if the signal is sampled at higher frequencies.
 *
 *  Reference:    3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.x.x
 *****************************************************************************/

#ifndef SRSRAN_NPSS_H
#define SRSRAN_NPSS_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/utils/convolution.h"

#define CONVOLUTION_FFT

#define SRSRAN_NPSS_RETURN_PSR

#define SRSRAN_NPSS_LEN 11
#define SRSRAN_NPSS_NUM_OFDM_SYMS 11
#define SRSRAN_NPSS_TOT_LEN (SRSRAN_NPSS_LEN * SRSRAN_NPSS_NUM_OFDM_SYMS)

#define SRSRAN_NPSS_CORR_FILTER_LEN                                                                                    \
  ((SRSRAN_NPSS_NUM_OFDM_SYMS * SRSRAN_NBIOT_FFT_SIZE) +                                                               \
   (SRSRAN_NPSS_NUM_OFDM_SYMS - 1) * SRSRAN_CP_LEN_NORM(1, SRSRAN_NBIOT_FFT_SIZE) +                                    \
   SRSRAN_CP_LEN_NORM(0, SRSRAN_NBIOT_FFT_SIZE))

// The below value corresponds to the time-domain representation of the first
// three OFDM-symbols plus cyclic prefix that are not transmitted in the sub-frame
// carrying the NPSS
#define SRSRAN_NPSS_CORR_OFFSET (SRSRAN_SF_LEN(SRSRAN_NBIOT_FFT_SIZE) - SRSRAN_NPSS_CORR_FILTER_LEN)

// CFO estimation based on the NPSS is done using the second slot of the sub-frame
#define SRSRAN_NPSS_CFO_OFFSET (SRSRAN_SF_LEN(SRSRAN_NBIOT_FFT_SIZE) / 2 - SRSRAN_NPSS_CORR_OFFSET)
#define SRSRAN_NPSS_CFO_NUM_SYMS 6 // number of symbols for CFO estimation
#define SRSRAN_NPSS_CFO_NUM_SAMPS                                                                                      \
  ((SRSRAN_NPSS_CFO_NUM_SYMS * SRSRAN_NBIOT_FFT_SIZE) +                                                                \
   (SRSRAN_NPSS_CFO_NUM_SYMS - 1) * SRSRAN_CP_LEN_NORM(1, SRSRAN_NBIOT_FFT_SIZE) +                                     \
   SRSRAN_CP_LEN_NORM(0, SRSRAN_NBIOT_FFT_SIZE)) // resulting number of samples

// NPSS processing options
#define SRSRAN_NPSS_ACCUMULATE_ABS // If enabled, accumulates the correlation absolute value on consecutive calls to
                                   // srsran_pss_synch_find_pss

#define SRSRAN_NPSS_ABS_SQUARE // If enabled, compute abs square, otherwise computes absolute value only

#define SRSRAN_NPSS_RETURN_PSR // If enabled returns peak to side-lobe ratio, otherwise returns absolute peak value

/* Low-level API */
typedef struct SRSRAN_API {
#ifdef CONVOLUTION_FFT
  srsran_conv_fft_cc_t conv_fft;
#endif

  uint32_t frame_size, max_frame_size;
  uint32_t fft_size, max_fft_size;

  cf_t*  npss_signal_time;
  cf_t*  tmp_input;
  cf_t*  conv_output;
  float* conv_output_abs;
  float  ema_alpha;
  float* conv_output_avg;
  float  peak_value;
} srsran_npss_synch_t;

// Basic functionality
SRSRAN_API int srsran_npss_synch_init(srsran_npss_synch_t* q, uint32_t frame_size, uint32_t fft_size);

SRSRAN_API void srsran_npss_synch_reset(srsran_npss_synch_t* q);

SRSRAN_API int srsran_npss_synch_resize(srsran_npss_synch_t* q, uint32_t frame_size, uint32_t fft_size);

SRSRAN_API void srsran_npss_synch_set_ema_alpha(srsran_npss_synch_t* q, float alpha);

SRSRAN_API void srsran_npss_synch_free(srsran_npss_synch_t* q);

SRSRAN_API int srsran_npss_sync_find(srsran_npss_synch_t* q, cf_t* input, float* corr_peak_value);

// Internal functions
SRSRAN_API int srsran_npss_corr_init(cf_t* npss_signal_time, uint32_t fft_size, uint32_t frame_size);

SRSRAN_API int srsran_npss_generate(cf_t* signal);

SRSRAN_API void srsran_npss_put_subframe(srsran_npss_synch_t* q,
                                         cf_t*                npss_signal,
                                         cf_t*                sf,
                                         const uint32_t       nof_prb,
                                         const uint32_t       nbiot_prb_offset);

#endif // SRSRAN_NPSS_H
