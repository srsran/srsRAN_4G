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
 *  File:         npss.h
 *
 *  Description:  Narrowband Primary synchronization signal (NPSS) generation and detection.
 *
 *                The srslte_npss_synch_t object provides functions for fast
 *                computation of the crosscorrelation between the NPSS and received
 *                signal and CFO estimation. Also, the function srslte_npss_synch_tperiodic()
 *                is designed to be called periodically every subframe, taking
 *                care of the correct data alignment with respect to the NPSS sequence.
 *
 *                The object is designed to work with signals sampled at ?.? Mhz
 *                centered at the carrier frequency. Thus, downsampling is required
 *                if the signal is sampled at higher frequencies.
 *
 *  Reference:    3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.x.x
 *****************************************************************************/

#ifndef SRSLTE_NPSS_H
#define SRSLTE_NPSS_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/utils/convolution.h"

#define CONVOLUTION_FFT

#define SRSLTE_NPSS_RETURN_PSR

#define SRSLTE_NPSS_LEN 11
#define SRSLTE_NPSS_NUM_OFDM_SYMS 11
#define SRSLTE_NPSS_TOT_LEN (SRSLTE_NPSS_LEN * SRSLTE_NPSS_NUM_OFDM_SYMS)

#define SRSLTE_NPSS_CORR_FILTER_LEN                                                                                    \
  ((SRSLTE_NPSS_NUM_OFDM_SYMS * SRSLTE_NBIOT_FFT_SIZE) +                                                               \
   (SRSLTE_NPSS_NUM_OFDM_SYMS - 1) * SRSLTE_CP_LEN_NORM(1, SRSLTE_NBIOT_FFT_SIZE) +                                    \
   SRSLTE_CP_LEN_NORM(0, SRSLTE_NBIOT_FFT_SIZE))

// The below value corresponds to the time-domain representation of the first
// three OFDM-symbols plus cyclic prefix that are not transmitted in the sub-frame
// carrying the NPSS
#define SRSLTE_NPSS_CORR_OFFSET (SRSLTE_SF_LEN(SRSLTE_NBIOT_FFT_SIZE) - SRSLTE_NPSS_CORR_FILTER_LEN)

// CFO estimation based on the NPSS is done using the second slot of the sub-frame
#define SRSLTE_NPSS_CFO_OFFSET (SRSLTE_SF_LEN(SRSLTE_NBIOT_FFT_SIZE) / 2 - SRSLTE_NPSS_CORR_OFFSET)
#define SRSLTE_NPSS_CFO_NUM_SYMS 6 // number of symbols for CFO estimation
#define SRSLTE_NPSS_CFO_NUM_SAMPS                                                                                      \
  ((SRSLTE_NPSS_CFO_NUM_SYMS * SRSLTE_NBIOT_FFT_SIZE) +                                                                \
   (SRSLTE_NPSS_CFO_NUM_SYMS - 1) * SRSLTE_CP_LEN_NORM(1, SRSLTE_NBIOT_FFT_SIZE) +                                     \
   SRSLTE_CP_LEN_NORM(0, SRSLTE_NBIOT_FFT_SIZE)) // resulting number of samples

// NPSS processing options
#define SRSLTE_NPSS_ACCUMULATE_ABS // If enabled, accumulates the correlation absolute value on consecutive calls to
                                   // srslte_pss_synch_find_pss

#define SRSLTE_NPSS_ABS_SQUARE // If enabled, compute abs square, otherwise computes absolute value only

#define SRSLTE_NPSS_RETURN_PSR // If enabled returns peak to side-lobe ratio, otherwise returns absolute peak value

/* Low-level API */
typedef struct SRSLTE_API {
#ifdef CONVOLUTION_FFT
  srslte_conv_fft_cc_t conv_fft;
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
} srslte_npss_synch_t;

// Basic functionality
SRSLTE_API int srslte_npss_synch_init(srslte_npss_synch_t* q, uint32_t frame_size, uint32_t fft_size);

SRSLTE_API void srslte_npss_synch_reset(srslte_npss_synch_t* q);

SRSLTE_API int srslte_npss_synch_resize(srslte_npss_synch_t* q, uint32_t frame_size, uint32_t fft_size);

SRSLTE_API void srslte_npss_synch_set_ema_alpha(srslte_npss_synch_t* q, float alpha);

SRSLTE_API void srslte_npss_synch_free(srslte_npss_synch_t* q);

SRSLTE_API int srslte_npss_sync_find(srslte_npss_synch_t* q, cf_t* input, float* corr_peak_value);

// Internal functions
SRSLTE_API int srslte_npss_corr_init(cf_t* npss_signal_time, uint32_t fft_size, uint32_t frame_size);

SRSLTE_API int srslte_npss_generate(cf_t* signal);

SRSLTE_API void srslte_npss_put_subframe(srslte_npss_synch_t* q,
                                         cf_t*                npss_signal,
                                         cf_t*                sf,
                                         const uint32_t       nof_prb,
                                         const uint32_t       nbiot_prb_offset);

#endif // SRSLTE_NPSS_H
