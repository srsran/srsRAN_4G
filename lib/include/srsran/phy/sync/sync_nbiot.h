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

#ifndef SRSRAN_SYNC_NBIOT_H
#define SRSRAN_SYNC_NBIOT_H

#include <math.h>
#include <stdbool.h>

#include "srsran/config.h"
#include "srsran/phy/sync/npss.h"
#include "srsran/phy/sync/nsss.h"
#include "srsran/phy/sync/sync.h"
#include "srsran/phy/ue/ue_sync.h"

#define MAX_NUM_CFO_CANDITATES 50

typedef struct SRSRAN_API {
  srsran_npss_synch_t npss;
  srsran_nsss_synch_t nsss;
  srsran_cp_synch_t   cp_synch;
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
  srsran_cfo_t cfocorr;
  srsran_cp_t  cp;
} srsran_sync_nbiot_t;

SRSRAN_API int
srsran_sync_nbiot_init(srsran_sync_nbiot_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size);

SRSRAN_API void srsran_sync_nbiot_free(srsran_sync_nbiot_t* q);

SRSRAN_API int
srsran_sync_nbiot_resize(srsran_sync_nbiot_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size);

SRSRAN_API srsran_sync_find_ret_t srsran_sync_nbiot_find(srsran_sync_nbiot_t* q,
                                                         cf_t*                input,
                                                         uint32_t             find_offset,
                                                         uint32_t*            peak_position);

SRSRAN_API float cfo_estimate_nbiot(srsran_sync_nbiot_t* q, cf_t* input);

SRSRAN_API void srsran_sync_nbiot_set_threshold(srsran_sync_nbiot_t* q, float threshold);

SRSRAN_API void srsran_sync_nbiot_set_cfo_enable(srsran_sync_nbiot_t* q, bool enable);

SRSRAN_API void srsran_sync_nbiot_set_cfo_cand_test_enable(srsran_sync_nbiot_t* q, bool enable);

SRSRAN_API int srsran_sync_nbiot_set_cfo_cand(srsran_sync_nbiot_t* q, const float* cand, const int num);

SRSRAN_API void srsran_sync_nbiot_set_cfo_tol(srsran_sync_nbiot_t* q, float tol);

SRSRAN_API void srsran_sync_nbiot_set_cfo_ema_alpha(srsran_sync_nbiot_t* q, float alpha);

SRSRAN_API void srsran_sync_nbiot_set_npss_ema_alpha(srsran_sync_nbiot_t* q, float alpha);

SRSRAN_API int srsran_sync_nbiot_find_cell_id(srsran_sync_nbiot_t* q, cf_t* input);

SRSRAN_API int srsran_sync_nbiot_get_cell_id(srsran_sync_nbiot_t* q);

SRSRAN_API float srsran_sync_nbiot_get_cfo(srsran_sync_nbiot_t* q);

SRSRAN_API void srsran_sync_nbiot_set_cfo(srsran_sync_nbiot_t* q, float cfo);

SRSRAN_API bool srsran_sync_nbiot_nsss_detected(srsran_sync_nbiot_t* q);

SRSRAN_API float srsran_sync_nbiot_get_peak_value(srsran_sync_nbiot_t* q);

SRSRAN_API void srsran_sync_nbiot_reset(srsran_sync_nbiot_t* q);

#endif // SRSRAN_SYNC_NBIOT_H
