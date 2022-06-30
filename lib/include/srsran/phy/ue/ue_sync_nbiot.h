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

#ifndef SRSRAN_UE_SYNC_NBIOT_H
#define SRSRAN_UE_SYNC_NBIOT_H

#include <stdbool.h>

#include "srsran/config.h"
#include "srsran/phy/agc/agc.h"
#include "srsran/phy/ch_estimation/chest_dl_nbiot.h"
#include "srsran/phy/common/timestamp.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/io/filesource.h"
#include "srsran/phy/phch/npbch.h"
#include "srsran/phy/sync/cfo.h"
#include "srsran/phy/sync/sync_nbiot.h"

typedef srsran_ue_sync_state_t srsran_nbiot_ue_sync_state_t;

//#define MEASURE_EXEC_TIME

typedef struct SRSRAN_API {
  srsran_sync_nbiot_t sfind;
  srsran_sync_nbiot_t strack;

  srsran_agc_t agc;
  bool         do_agc;
  uint32_t     agc_period;

  void* stream;
  void* stream_single;
  int (*recv_callback)(void*, cf_t* [SRSRAN_MAX_PORTS], uint32_t, srsran_timestamp_t*);
  int (*recv_callback_single)(void*, void*, uint32_t, srsran_timestamp_t*);
  srsran_timestamp_t last_timestamp;

  srsran_filesource_t file_source;
  bool                file_mode;
  float               file_cfo;
  srsran_cfo_t        file_cfo_correct;

  srsran_nbiot_ue_sync_state_t state;

  uint32_t nof_rx_antennas;
  uint32_t frame_len;
  uint32_t fft_size;
  uint32_t nof_recv_sf; // Number of subframes received each call to srsran_ue_sync_get_buffer
  uint32_t nof_avg_find_frames;
  uint32_t frame_find_cnt;
  uint32_t sf_len;

  /* These count half frames (5ms) */
  uint64_t frame_ok_cnt;
  uint32_t frame_no_cnt;
  uint32_t frame_total_cnt;

  /* this is the system frame number (SFN) */
  uint32_t frame_number;

  srsran_nbiot_cell_t cell;
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
} srsran_nbiot_ue_sync_t;

SRSRAN_API int srsran_ue_sync_nbiot_init(srsran_nbiot_ue_sync_t* q,
                                         srsran_nbiot_cell_t     cell,
                                         int(recv_callback)(void*, void*, uint32_t, srsran_timestamp_t*),
                                         void* stream_handler);

SRSRAN_API int
srsran_ue_sync_nbiot_init_multi(srsran_nbiot_ue_sync_t* q,
                                uint32_t                max_prb,
                                int(recv_callback)(void*, cf_t* [SRSRAN_MAX_PORTS], uint32_t, srsran_timestamp_t*),
                                uint32_t nof_rx_antennas,
                                void*    stream_handler);

SRSRAN_API int srsran_ue_sync_nbiot_init_file(srsran_nbiot_ue_sync_t* q,
                                              srsran_nbiot_cell_t     cell,
                                              char*                   file_name,
                                              int                     offset_time,
                                              float                   offset_freq);

SRSRAN_API int srsran_ue_sync_nbiot_init_file_multi(srsran_nbiot_ue_sync_t* q,
                                                    srsran_nbiot_cell_t     cell,
                                                    char*                   file_name,
                                                    int                     offset_time,
                                                    float                   offset_freq,
                                                    uint32_t                nof_rx_ant);

SRSRAN_API void srsran_ue_sync_nbiot_free(srsran_nbiot_ue_sync_t* q);

SRSRAN_API int srsran_ue_sync_nbiot_set_cell(srsran_nbiot_ue_sync_t* q, srsran_nbiot_cell_t cell);

SRSRAN_API int srsran_ue_sync_nbiot_start_agc(srsran_nbiot_ue_sync_t* q,
                                              SRSRAN_AGC_CALLBACK(set_gain_callback),
                                              float init_gain_value);

SRSRAN_API uint32_t srsran_ue_sync_nbiot_sf_len(srsran_nbiot_ue_sync_t* q);

SRSRAN_API int srsran_nbiot_ue_sync_get_buffer(srsran_nbiot_ue_sync_t* q, cf_t** sf_symbols);

SRSRAN_API void srsran_ue_sync_nbiot_set_agc_period(srsran_nbiot_ue_sync_t* q, uint32_t period);

/* CAUTION: input_buffer MUST have space for 2 subframes */
SRSRAN_API int srsran_ue_sync_nbiot_zerocopy(srsran_nbiot_ue_sync_t* q, cf_t* input_buffer);

SRSRAN_API int srsran_ue_sync_nbiot_zerocopy_multi(srsran_nbiot_ue_sync_t* q, cf_t** input_buffer);

SRSRAN_API void srsran_ue_sync_nbiot_set_cfo(srsran_nbiot_ue_sync_t* q, float cfo);

SRSRAN_API void srsran_ue_sync_nbiot_reset(srsran_nbiot_ue_sync_t* q);

SRSRAN_API srsran_nbiot_ue_sync_state_t srsran_ue_sync_nbiot_get_state(srsran_nbiot_ue_sync_t* q);

SRSRAN_API uint32_t srsran_ue_sync_nbiot_get_sfidx(srsran_nbiot_ue_sync_t* q);

SRSRAN_API void srsran_ue_sync_nbiot_set_cfo_enable(srsran_nbiot_ue_sync_t* q, bool enable);

SRSRAN_API float srsran_ue_sync_nbiot_get_cfo(srsran_nbiot_ue_sync_t* q);

SRSRAN_API float srsran_ue_sync_nbiot_get_sfo(srsran_nbiot_ue_sync_t* q);

SRSRAN_API void srsran_ue_sync_nbiot_set_cfo_ema(srsran_nbiot_ue_sync_t* q, float ema);

SRSRAN_API void srsran_ue_sync_nbiot_set_cfo_tol(srsran_nbiot_ue_sync_t* q, float cfo_tol);

SRSRAN_API int srsran_ue_sync_nbiot_get_last_sample_offset(srsran_nbiot_ue_sync_t* q);

SRSRAN_API void srsran_ue_sync_nbiot_set_sample_offset_correct_period(srsran_nbiot_ue_sync_t* q,
                                                                      uint32_t                nof_subframes);

SRSRAN_API void srsran_ue_sync_nbiot_get_last_timestamp(srsran_nbiot_ue_sync_t* q, srsran_timestamp_t* timestamp);

#endif // SRSRAN_UE_SYNC_NBIOT_H