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
 *  File:         pss.h
 *
 *  Description:  Primary synchronization signal (PSS) generation and detection.
 *
 *                The srsran_pss_t object provides functions for fast
 *                computation of the crosscorrelation between the PSS and received
 *                signal and CFO estimation. Also, the function srsran_pss_tperiodic()
 *                is designed to be called periodically every subframe, taking
 *                care of the correct data alignment with respect to the PSS sequence.
 *
 *                The object is designed to work with signals sampled at 1.92 Mhz
 *                centered at the carrier frequency. Thus, downsampling is required
 *                if the signal is sampled at higher frequencies.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.11.1
 *****************************************************************************/

#ifndef SRSRAN_PSS_H
#define SRSRAN_PSS_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/utils/convolution.h"
#include "srsran/phy/utils/filter.h"

#define CONVOLUTION_FFT

#define SRSRAN_PSS_LEN 62
#define SRSRAN_PSS_RE (6 * 12)

/* PSS processing options */

#define SRSRAN_PSS_ACCUMULATE_ABS // If enabled, accumulates the correlation absolute value on consecutive calls to
                                  // srsran_pss_find_pss

#define SRSRAN_PSS_RETURN_PSR // If enabled returns peak to side-lobe ratio, otherwise returns absolute peak value

/* Low-level API */
typedef struct SRSRAN_API {

#ifdef CONVOLUTION_FFT
  srsran_conv_fft_cc_t conv_fft;
  srsran_filt_cc_t     filter;

#endif
  int decimate;

  uint32_t max_frame_size;
  uint32_t max_fft_size;

  uint32_t frame_size;
  uint32_t N_id_2;
  uint32_t fft_size;
  cf_t*    pss_signal_freq_full[3];

  cf_t* pss_signal_time[3];
  cf_t* pss_signal_time_scale[3];

  cf_t   pss_signal_freq[3][SRSRAN_PSS_LEN]; // One sequence for each N_id_2
  cf_t*  tmp_input;
  cf_t*  conv_output;
  float* conv_output_abs;
  float  ema_alpha;
  float* conv_output_avg;
  float  peak_value;

  bool              filter_pss_enable;
  srsran_dft_plan_t dftp_input;
  srsran_dft_plan_t idftp_input;
  cf_t              tmp_fft[SRSRAN_SYMBOL_SZ_MAX];
  cf_t              tmp_fft2[SRSRAN_SYMBOL_SZ_MAX];

  cf_t tmp_ce[SRSRAN_PSS_LEN];

  bool chest_on_filter;

} srsran_pss_t;

typedef enum { PSS_TX, PSS_RX } pss_direction_t;

/* Basic functionality */
SRSRAN_API int srsran_pss_init_fft(srsran_pss_t* q, uint32_t frame_size, uint32_t fft_size);

SRSRAN_API int srsran_pss_init_fft_offset(srsran_pss_t* q, uint32_t frame_size, uint32_t fft_size, int cfo_i);

SRSRAN_API int
srsran_pss_init_fft_offset_decim(srsran_pss_t* q, uint32_t frame_size, uint32_t fft_size, int cfo_i, int decimate);

SRSRAN_API int srsran_pss_resize(srsran_pss_t* q, uint32_t frame_size, uint32_t fft_size, int offset);

SRSRAN_API int srsran_pss_init(srsran_pss_t* q, uint32_t frame_size);

SRSRAN_API void srsran_pss_free(srsran_pss_t* q);

SRSRAN_API void srsran_pss_reset(srsran_pss_t* q);

SRSRAN_API void srsran_pss_filter_enable(srsran_pss_t* q, bool enable);

SRSRAN_API void srsran_pss_sic(srsran_pss_t* q, cf_t* input);

SRSRAN_API void srsran_pss_filter(srsran_pss_t* q, const cf_t* input, cf_t* output);

SRSRAN_API int srsran_pss_generate(cf_t* signal, uint32_t N_id_2);

SRSRAN_API void srsran_pss_get_slot(cf_t* slot, cf_t* pss_signal, uint32_t nof_prb, srsran_cp_t cp);

SRSRAN_API void srsran_pss_put_slot(cf_t* pss_signal, cf_t* slot, uint32_t nof_prb, srsran_cp_t cp);

SRSRAN_API void srsran_pss_set_ema_alpha(srsran_pss_t* q, float alpha);

SRSRAN_API int srsran_pss_set_N_id_2(srsran_pss_t* q, uint32_t N_id_2);

SRSRAN_API int srsran_pss_find_pss(srsran_pss_t* q, const cf_t* input, float* corr_peak_value);

SRSRAN_API int srsran_pss_chest(srsran_pss_t* q, const cf_t* input, cf_t ce[SRSRAN_PSS_LEN]);

SRSRAN_API float srsran_pss_cfo_compute(srsran_pss_t* q, const cf_t* pss_recv);

#endif // SRSRAN_PSS_H
