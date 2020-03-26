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
 *  File:         sync.h
 *
 *  Description:  Time and frequency synchronization using the PSS and SSS signals.
 *
 *                The object is designed to work with signals sampled at 1.92 Mhz
 *                centered at the carrier frequency. Thus, downsampling is required
 *                if the signal is sampled at higher frequencies.
 *
 *                Correlation peak is detected comparing the maximum at the output
 *                of the correlator with a threshold. The comparison accepts two
 *                modes: absolute value or peak-to-mean ratio, which are configured
 *                with the functions sync_pss_det_absolute() and sync_pss_det_peakmean().
 *
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.11.1, 6.11.2
 *****************************************************************************/

#ifndef SRSLTE_SYNC_H
#define SRSLTE_SYNC_H

#include <math.h>
#include <stdbool.h>

#include "srslte/config.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/sync/cp.h"
#include "srslte/phy/sync/pss.h"
#include "srslte/phy/sync/sss.h"

#define SRSLTE_SYNC_FFT_SZ_MIN 64
#define SRSLTE_SYNC_FFT_SZ_MAX 2048

typedef enum { SSS_DIFF = 0, SSS_PARTIAL_3 = 2, SSS_FULL = 1 } sss_alg_t;

typedef struct SRSLTE_API {
  srslte_pss_t      pss;
  srslte_pss_t      pss_i[2];
  srslte_sss_t      sss;
  srslte_cp_synch_t cp_synch;
  cf_t*             cfo_i_corr[2];
  int               decimate;
  float             threshold;
  float             peak_value;
  uint32_t          N_id_2;
  uint32_t          N_id_1;
  uint32_t          sf_idx;
  uint32_t          fft_size;
  uint32_t          frame_size;
  uint32_t          max_offset;
  uint32_t          nof_symbols;
  uint32_t          cp_len;
  float             current_cfo_tol;
  sss_alg_t         sss_alg;
  bool              detect_cp;
  bool              sss_en;
  srslte_cp_t       cp;
  uint32_t          m0;
  uint32_t          m1;
  float             m0_value;
  float             m1_value;
  float             M_norm_avg;
  float             M_ext_avg;
  cf_t*             temp;

  uint32_t max_frame_size;

  srslte_frame_type_t frame_type;
  bool                detect_frame_type;

  // variables for various CFO estimation methods
  bool cfo_cp_enable;
  bool cfo_pss_enable;
  bool cfo_i_enable;

  bool cfo_cp_is_set;
  bool cfo_pss_is_set;
  bool cfo_i_initiated;

  float cfo_cp_mean;
  float cfo_pss;
  float cfo_pss_mean;
  int   cfo_i_value;

  float cfo_ema_alpha;

  uint32_t cfo_cp_nsymbols;

  srslte_cfo_t cfo_corr_frame;
  srslte_cfo_t cfo_corr_symbol;

  bool sss_channel_equalize;
  bool pss_filtering_enabled;
  cf_t sss_filt[SRSLTE_SYMBOL_SZ_MAX];
  cf_t pss_filt[SRSLTE_SYMBOL_SZ_MAX];

  bool              sss_generated;
  bool              sss_detected;
  bool              sss_available;
  float             sss_corr;
  srslte_dft_plan_t idftp_sss;
  cf_t              sss_recv[SRSLTE_SYMBOL_SZ_MAX];
  cf_t              sss_signal[2][SRSLTE_SYMBOL_SZ_MAX];

} srslte_sync_t;

typedef enum {
  SRSLTE_SYNC_FOUND         = 1,
  SRSLTE_SYNC_FOUND_NOSPACE = 2,
  SRSLTE_SYNC_NOFOUND       = 0,
  SRSLTE_SYNC_ERROR         = -1
} srslte_sync_find_ret_t;

SRSLTE_API int srslte_sync_init(srslte_sync_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size);

SRSLTE_API int
srslte_sync_init_decim(srslte_sync_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size, int decimate);

SRSLTE_API void srslte_sync_free(srslte_sync_t* q);

SRSLTE_API int srslte_sync_resize(srslte_sync_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size);

SRSLTE_API void srslte_sync_reset(srslte_sync_t* q);

/* Finds a correlation peak in the input signal around position find_offset */
SRSLTE_API srslte_sync_find_ret_t srslte_sync_find(srslte_sync_t* q,
                                                   const cf_t*    input,
                                                   uint32_t       find_offset,
                                                   uint32_t*      peak_position);

/* Estimates the CP length */
SRSLTE_API srslte_cp_t srslte_sync_detect_cp(srslte_sync_t* q, const cf_t* input, uint32_t peak_pos);

/* Sets the threshold for peak comparison */
SRSLTE_API void srslte_sync_set_threshold(srslte_sync_t* q, float threshold);

/* Gets the subframe idx (0 or 5) */
SRSLTE_API uint32_t srslte_sync_get_sf_idx(srslte_sync_t* q);

/* Gets the peak value */
SRSLTE_API float srslte_sync_get_peak_value(srslte_sync_t* q);

/* Choose SSS detection algorithm */
SRSLTE_API void srslte_sync_set_sss_algorithm(srslte_sync_t* q, sss_alg_t alg);

/* Sets PSS exponential averaging alpha weight */
SRSLTE_API void srslte_sync_set_em_alpha(srslte_sync_t* q, float alpha);

/* Sets the N_id_2 to search for */
SRSLTE_API int srslte_sync_set_N_id_2(srslte_sync_t* q, uint32_t N_id_2);

SRSLTE_API int srslte_sync_set_N_id_1(srslte_sync_t* q, uint32_t N_id_1);

/* Gets the Physical CellId from the last call to synch_run() */
SRSLTE_API int srslte_sync_get_cell_id(srslte_sync_t* q);

/* Enables/disables filtering of the central PRBs before PSS CFO estimation or SSS correlation*/
SRSLTE_API void srslte_sync_set_pss_filt_enable(srslte_sync_t* q, bool enable);

SRSLTE_API void srslte_sync_set_sss_eq_enable(srslte_sync_t* q, bool enable);

/* Gets the CFO estimation from the last call to synch_run() */
SRSLTE_API float srslte_sync_get_cfo(srslte_sync_t* q);

/* Resets internal CFO state */
SRSLTE_API void srslte_sync_cfo_reset(srslte_sync_t* q, float cfo_Hz);

/* Copies CFO internal state from another object to avoid long transients */
SRSLTE_API void srslte_sync_copy_cfo(srslte_sync_t* q, srslte_sync_t* src_obj);

/* Enable different CFO estimation stages */
SRSLTE_API void srslte_sync_set_cfo_i_enable(srslte_sync_t* q, bool enable);
SRSLTE_API void srslte_sync_set_cfo_cp_enable(srslte_sync_t* q, bool enable, uint32_t nof_symbols);

SRSLTE_API void srslte_sync_set_cfo_pss_enable(srslte_sync_t* q, bool enable);

/* Sets CFO correctors tolerance (in Hz) */
SRSLTE_API void srslte_sync_set_cfo_tol(srslte_sync_t* q, float tol);

SRSLTE_API void srslte_sync_set_frame_type(srslte_sync_t* q, srslte_frame_type_t frame_type);

/* Sets the exponential moving average coefficient for CFO averaging */
SRSLTE_API void srslte_sync_set_cfo_ema_alpha(srslte_sync_t* q, float alpha);

/* Gets the CP length estimation from the last call to synch_run() */
SRSLTE_API srslte_cp_t srslte_sync_get_cp(srslte_sync_t* q);

/* Sets the CP length estimation (must do it if disabled) */
SRSLTE_API void srslte_sync_set_cp(srslte_sync_t* q, srslte_cp_t cp);

/* Enables/Disables SSS detection  */
SRSLTE_API void srslte_sync_sss_en(srslte_sync_t* q, bool enabled);

SRSLTE_API srslte_pss_t* srslte_sync_get_cur_pss_obj(srslte_sync_t* q);

SRSLTE_API bool srslte_sync_sss_detected(srslte_sync_t* q);

SRSLTE_API float srslte_sync_sss_correlation_peak(srslte_sync_t* q);

SRSLTE_API bool srslte_sync_sss_available(srslte_sync_t* q);

/* Enables/Disables CP detection  */
SRSLTE_API void srslte_sync_cp_en(srslte_sync_t* q, bool enabled);

#endif // SRSLTE_SYNC_H
