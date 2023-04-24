/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_SYNC_H
#define SRSRAN_SYNC_H

#include <math.h>
#include <stdbool.h>

#include "srsran/config.h"
#include "srsran/phy/sync/cfo.h"
#include "srsran/phy/sync/cp.h"
#include "srsran/phy/sync/pss.h"
#include "srsran/phy/sync/sss.h"

#define SRSRAN_SYNC_FFT_SZ_MIN 64
#define SRSRAN_SYNC_FFT_SZ_MAX 2048

typedef enum { SSS_DIFF = 0, SSS_PARTIAL_3 = 2, SSS_FULL = 1 } sss_alg_t;

typedef struct SRSRAN_API {
  srsran_pss_t      pss;
  srsran_pss_t      pss_i[2];
  srsran_sss_t      sss;
  srsran_cp_synch_t cp_synch;
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
  srsran_cp_t       cp;
  uint32_t          m0;
  uint32_t          m1;
  float             m0_value;
  float             m1_value;
  float             M_norm_avg;
  float             M_ext_avg;
  cf_t*             temp;

  uint32_t max_frame_size;

  srsran_frame_type_t frame_type;
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

  srsran_cfo_t cfo_corr_frame;
  srsran_cfo_t cfo_corr_symbol;

  bool sss_channel_equalize;
  bool pss_filtering_enabled;
  cf_t sss_filt[SRSRAN_SYMBOL_SZ_MAX];
  cf_t pss_filt[SRSRAN_SYMBOL_SZ_MAX];

  bool              sss_generated;
  bool              sss_detected;
  bool              sss_available;
  float             sss_corr;
  srsran_dft_plan_t idftp_sss;
  cf_t              sss_recv[SRSRAN_SYMBOL_SZ_MAX];
  cf_t              sss_signal[2][SRSRAN_SYMBOL_SZ_MAX];

} srsran_sync_t;

typedef enum {
  SRSRAN_SYNC_FOUND         = 1,
  SRSRAN_SYNC_FOUND_NOSPACE = 2,
  SRSRAN_SYNC_NOFOUND       = 0,
  SRSRAN_SYNC_ERROR         = -1
} srsran_sync_find_ret_t;

SRSRAN_API int srsran_sync_init(srsran_sync_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size);

SRSRAN_API int
srsran_sync_init_decim(srsran_sync_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size, int decimate);

SRSRAN_API void srsran_sync_free(srsran_sync_t* q);

SRSRAN_API int srsran_sync_resize(srsran_sync_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size);

SRSRAN_API void srsran_sync_reset(srsran_sync_t* q);

/* Finds a correlation peak in the input signal around position find_offset */
SRSRAN_API srsran_sync_find_ret_t srsran_sync_find(srsran_sync_t* q,
                                                   const cf_t*    input,
                                                   uint32_t       find_offset,
                                                   uint32_t*      peak_position);

/* Estimates the CP length */
SRSRAN_API srsran_cp_t srsran_sync_detect_cp(srsran_sync_t* q, const cf_t* input, uint32_t peak_pos);

/* Sets the threshold for peak comparison */
SRSRAN_API void srsran_sync_set_threshold(srsran_sync_t* q, float threshold);

/* Gets the subframe idx (0 or 5) */
SRSRAN_API uint32_t srsran_sync_get_sf_idx(srsran_sync_t* q);

/* Gets the peak value */
SRSRAN_API float srsran_sync_get_peak_value(srsran_sync_t* q);

/* Choose SSS detection algorithm */
SRSRAN_API void srsran_sync_set_sss_algorithm(srsran_sync_t* q, sss_alg_t alg);

/* Sets PSS exponential averaging alpha weight */
SRSRAN_API void srsran_sync_set_em_alpha(srsran_sync_t* q, float alpha);

/* Sets the N_id_2 to search for */
SRSRAN_API int srsran_sync_set_N_id_2(srsran_sync_t* q, uint32_t N_id_2);

SRSRAN_API int srsran_sync_set_N_id_1(srsran_sync_t* q, uint32_t N_id_1);

/* Gets the Physical CellId from the last call to synch_run() */
SRSRAN_API int srsran_sync_get_cell_id(srsran_sync_t* q);

/* Enables/disables filtering of the central PRBs before PSS CFO estimation or SSS correlation*/
SRSRAN_API void srsran_sync_set_pss_filt_enable(srsran_sync_t* q, bool enable);

SRSRAN_API void srsran_sync_set_sss_eq_enable(srsran_sync_t* q, bool enable);

/* Gets the CFO estimation from the last call to synch_run() */
SRSRAN_API float srsran_sync_get_cfo(srsran_sync_t* q);

/* Resets internal CFO state */
SRSRAN_API void srsran_sync_cfo_reset(srsran_sync_t* q, float cfo_Hz);

/* Copies CFO internal state from another object to avoid long transients */
SRSRAN_API void srsran_sync_copy_cfo(srsran_sync_t* q, srsran_sync_t* src_obj);

/* Enable different CFO estimation stages */
SRSRAN_API void srsran_sync_set_cfo_i_enable(srsran_sync_t* q, bool enable);
SRSRAN_API void srsran_sync_set_cfo_cp_enable(srsran_sync_t* q, bool enable, uint32_t nof_symbols);

SRSRAN_API void srsran_sync_set_cfo_pss_enable(srsran_sync_t* q, bool enable);

/* Sets CFO correctors tolerance (in Hz) */
SRSRAN_API void srsran_sync_set_cfo_tol(srsran_sync_t* q, float tol);

SRSRAN_API void srsran_sync_set_frame_type(srsran_sync_t* q, srsran_frame_type_t frame_type);

/* Sets the exponential moving average coefficient for CFO averaging */
SRSRAN_API void srsran_sync_set_cfo_ema_alpha(srsran_sync_t* q, float alpha);

/* Gets the CP length estimation from the last call to synch_run() */
SRSRAN_API srsran_cp_t srsran_sync_get_cp(srsran_sync_t* q);

/* Sets the CP length estimation (must do it if disabled) */
SRSRAN_API void srsran_sync_set_cp(srsran_sync_t* q, srsran_cp_t cp);

/* Enables/Disables SSS detection  */
SRSRAN_API void srsran_sync_sss_en(srsran_sync_t* q, bool enabled);

SRSRAN_API srsran_pss_t* srsran_sync_get_cur_pss_obj(srsran_sync_t* q);

SRSRAN_API bool srsran_sync_sss_detected(srsran_sync_t* q);

SRSRAN_API float srsran_sync_sss_correlation_peak(srsran_sync_t* q);

SRSRAN_API bool srsran_sync_sss_available(srsran_sync_t* q);

/* Enables/Disables CP detection  */
SRSRAN_API void srsran_sync_cp_en(srsran_sync_t* q, bool enabled);

#endif // SRSRAN_SYNC_H
