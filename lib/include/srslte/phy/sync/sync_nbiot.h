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
 *  File:         sync_nbiot.h
 *
 *  Description:  Time and frequency synchronization using the NPSS and NSSS signals.
 *
 *                The object is designed to work with signals sampled at 1.92 Mhz
 *                centered at the carrier frequency. Thus, downsampling is required
 *                if the signal is sampled at higher frequencies.
 *
 *                Correlation peak is detected comparing the maximum at the output
 *                of the correlator with a threshold.
 *
 *  Reference:    3GPP TS 36.211 version 13.2.0 Release 13
 *****************************************************************************/

#ifndef SRSLTE_SYNC_NBIOT_H
#define SRSLTE_SYNC_NBIOT_H

#include <math.h>
#include <stdbool.h>

#include "srslte/config.h"
#include "srslte/phy/sync/npss.h"
#include "srslte/phy/sync/nsss.h"
#include "srslte/phy/sync/sync.h"
#include "srslte/phy/ue/ue_sync.h"

#define MAX_NUM_CFO_CANDITATES 50

typedef struct SRSLTE_API {
  srslte_npss_synch_t npss;
  srslte_nsss_synch_t nsss;
  srslte_cp_synch_t   cp_synch;
  uint32_t            n_id_ncell;

  float        threshold;
  float        peak_value;
  uint32_t     fft_size;
  uint32_t     frame_size;
  uint32_t     max_frame_size;
  uint32_t     max_offset;
  bool         enable_cfo_estimation;
  bool         enable_cfo_cand_test;
  float        cfo_cand[MAX_NUM_CFO_CANDITATES];
  int          cfo_num_cand;
  int          cfo_cand_idx;
  float        mean_cfo;
  float        current_cfo_tol;
  cf_t*        shift_buffer;
  cf_t*        cfo_output;
  int          cfo_i;
  bool         find_cfo_i;
  bool         find_cfo_i_initiated;
  float        cfo_ema_alpha;
  uint32_t     nof_symbols;
  uint32_t     cp_len;
  srslte_cfo_t cfocorr;
  srslte_cp_t  cp;
} srslte_sync_nbiot_t;

SRSLTE_API int
srslte_sync_nbiot_init(srslte_sync_nbiot_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size);

SRSLTE_API void srslte_sync_nbiot_free(srslte_sync_nbiot_t* q);

SRSLTE_API int
srslte_sync_nbiot_resize(srslte_sync_nbiot_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size);

SRSLTE_API srslte_sync_find_ret_t srslte_sync_nbiot_find(srslte_sync_nbiot_t* q,
                                                         cf_t*                input,
                                                         uint32_t             find_offset,
                                                         uint32_t*            peak_position);

SRSLTE_API float cfo_estimate_nbiot(srslte_sync_nbiot_t* q, cf_t* input);

SRSLTE_API void srslte_sync_nbiot_set_threshold(srslte_sync_nbiot_t* q, float threshold);

SRSLTE_API void srslte_sync_nbiot_set_cfo_enable(srslte_sync_nbiot_t* q, bool enable);

SRSLTE_API void srslte_sync_nbiot_set_cfo_cand_test_enable(srslte_sync_nbiot_t* q, bool enable);

SRSLTE_API int srslte_sync_nbiot_set_cfo_cand(srslte_sync_nbiot_t* q, const float* cand, const int num);

SRSLTE_API void srslte_sync_nbiot_set_cfo_tol(srslte_sync_nbiot_t* q, float tol);

SRSLTE_API void srslte_sync_nbiot_set_cfo_ema_alpha(srslte_sync_nbiot_t* q, float alpha);

SRSLTE_API void srslte_sync_nbiot_set_npss_ema_alpha(srslte_sync_nbiot_t* q, float alpha);

SRSLTE_API int srslte_sync_nbiot_find_cell_id(srslte_sync_nbiot_t* q, cf_t* input);

SRSLTE_API int srslte_sync_nbiot_get_cell_id(srslte_sync_nbiot_t* q);

SRSLTE_API float srslte_sync_nbiot_get_cfo(srslte_sync_nbiot_t* q);

SRSLTE_API void srslte_sync_nbiot_set_cfo(srslte_sync_nbiot_t* q, float cfo);

SRSLTE_API bool srslte_sync_nbiot_nsss_detected(srslte_sync_nbiot_t* q);

SRSLTE_API float srslte_sync_nbiot_get_peak_value(srslte_sync_nbiot_t* q);

SRSLTE_API void srslte_sync_nbiot_reset(srslte_sync_nbiot_t* q);

#endif // SRSLTE_SYNC_NBIOT_H
