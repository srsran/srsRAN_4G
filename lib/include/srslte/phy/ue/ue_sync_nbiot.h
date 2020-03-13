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

#ifndef SRSLTE_UE_SYNC_NBIOT_H
#define SRSLTE_UE_SYNC_NBIOT_H

#include <stdbool.h>

#include "srslte/config.h"
#include "srslte/phy/agc/agc.h"
#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/common/timestamp.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/phch/npbch.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/sync/sync_nbiot.h"

typedef srslte_ue_sync_state_t srslte_nbiot_ue_sync_state_t;

//#define MEASURE_EXEC_TIME

typedef struct SRSLTE_API {
  srslte_sync_nbiot_t sfind;
  srslte_sync_nbiot_t strack;

  srslte_agc_t agc;
  bool         do_agc;
  uint32_t     agc_period;

  void* stream;
  void* stream_single;
  int (*recv_callback)(void*, cf_t* [SRSLTE_MAX_PORTS], uint32_t, srslte_timestamp_t*);
  int (*recv_callback_single)(void*, void*, uint32_t, srslte_timestamp_t*);
  srslte_timestamp_t last_timestamp;

  srslte_filesource_t file_source;
  bool                file_mode;
  float               file_cfo;
  srslte_cfo_t        file_cfo_correct;

  srslte_nbiot_ue_sync_state_t state;

  uint32_t nof_rx_antennas;
  uint32_t frame_len;
  uint32_t fft_size;
  uint32_t nof_recv_sf; // Number of subframes received each call to srslte_ue_sync_get_buffer
  uint32_t nof_avg_find_frames;
  uint32_t frame_find_cnt;
  uint32_t sf_len;

  /* These count half frames (5ms) */
  uint64_t frame_ok_cnt;
  uint32_t frame_no_cnt;
  uint32_t frame_total_cnt;

  /* this is the system frame number (SFN) */
  uint32_t frame_number;

  srslte_nbiot_cell_t cell;
  uint32_t            sf_idx;
  bool                correct_cfo;

  uint32_t peak_idx;
  int      next_rf_sample_offset;
  int      last_sample_offset;
  float    mean_sample_offset;
  float    mean_sfo;
  uint32_t sample_offset_correct_period;
  float    sfo_ema;
  uint32_t max_prb;

#ifdef MEASURE_EXEC_TIME
  float mean_exec_time;
#endif
} srslte_nbiot_ue_sync_t;

SRSLTE_API int srslte_ue_sync_nbiot_init(srslte_nbiot_ue_sync_t* q,
                                         srslte_nbiot_cell_t     cell,
                                         int(recv_callback)(void*, void*, uint32_t, srslte_timestamp_t*),
                                         void* stream_handler);

SRSLTE_API int
srslte_ue_sync_nbiot_init_multi(srslte_nbiot_ue_sync_t* q,
                                uint32_t                max_prb,
                                int(recv_callback)(void*, cf_t* [SRSLTE_MAX_PORTS], uint32_t, srslte_timestamp_t*),
                                uint32_t nof_rx_antennas,
                                void*    stream_handler);

SRSLTE_API int srslte_ue_sync_nbiot_init_file(srslte_nbiot_ue_sync_t* q,
                                              srslte_nbiot_cell_t     cell,
                                              char*                   file_name,
                                              int                     offset_time,
                                              float                   offset_freq);

SRSLTE_API int srslte_ue_sync_nbiot_init_file_multi(srslte_nbiot_ue_sync_t* q,
                                                    srslte_nbiot_cell_t     cell,
                                                    char*                   file_name,
                                                    int                     offset_time,
                                                    float                   offset_freq,
                                                    uint32_t                nof_rx_ant);

SRSLTE_API void srslte_ue_sync_nbiot_free(srslte_nbiot_ue_sync_t* q);

SRSLTE_API int srslte_ue_sync_nbiot_set_cell(srslte_nbiot_ue_sync_t* q, srslte_nbiot_cell_t cell);

SRSLTE_API int srslte_ue_sync_nbiot_start_agc(srslte_nbiot_ue_sync_t* q,
                                              SRSLTE_AGC_CALLBACK(set_gain_callback),
                                              float init_gain_value);

SRSLTE_API uint32_t srslte_ue_sync_nbiot_sf_len(srslte_nbiot_ue_sync_t* q);

SRSLTE_API int srslte_nbiot_ue_sync_get_buffer(srslte_nbiot_ue_sync_t* q, cf_t** sf_symbols);

SRSLTE_API void srslte_ue_sync_nbiot_set_agc_period(srslte_nbiot_ue_sync_t* q, uint32_t period);

/* CAUTION: input_buffer MUST have space for 2 subframes */
SRSLTE_API int srslte_ue_sync_nbiot_zerocopy(srslte_nbiot_ue_sync_t* q, cf_t* input_buffer);

SRSLTE_API int srslte_ue_sync_nbiot_zerocopy_multi(srslte_nbiot_ue_sync_t* q, cf_t** input_buffer);

SRSLTE_API void srslte_ue_sync_nbiot_set_cfo(srslte_nbiot_ue_sync_t* q, float cfo);

SRSLTE_API void srslte_ue_sync_nbiot_reset(srslte_nbiot_ue_sync_t* q);

SRSLTE_API srslte_nbiot_ue_sync_state_t srslte_ue_sync_nbiot_get_state(srslte_nbiot_ue_sync_t* q);

SRSLTE_API uint32_t srslte_ue_sync_nbiot_get_sfidx(srslte_nbiot_ue_sync_t* q);

SRSLTE_API void srslte_ue_sync_nbiot_set_cfo_enable(srslte_nbiot_ue_sync_t* q, bool enable);

SRSLTE_API float srslte_ue_sync_nbiot_get_cfo(srslte_nbiot_ue_sync_t* q);

SRSLTE_API float srslte_ue_sync_nbiot_get_sfo(srslte_nbiot_ue_sync_t* q);

SRSLTE_API void srslte_ue_sync_nbiot_set_cfo_ema(srslte_nbiot_ue_sync_t* q, float ema);

SRSLTE_API void srslte_ue_sync_nbiot_set_cfo_tol(srslte_nbiot_ue_sync_t* q, float cfo_tol);

SRSLTE_API int srslte_ue_sync_nbiot_get_last_sample_offset(srslte_nbiot_ue_sync_t* q);

SRSLTE_API void srslte_ue_sync_nbiot_set_sample_offset_correct_period(srslte_nbiot_ue_sync_t* q,
                                                                      uint32_t                nof_subframes);

SRSLTE_API void srslte_ue_sync_nbiot_get_last_timestamp(srslte_nbiot_ue_sync_t* q, srslte_timestamp_t* timestamp);

#endif // SRSLTE_UE_SYNC_NBIOT_H