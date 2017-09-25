/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
 *  File:         ue_sync.h
 *
 *  Description:  This object automatically manages the cell synchronization
 *                procedure.
 *
 *                The main function is srslte_ue_sync_get_buffer(), which returns
 *                a pointer to the aligned subframe of samples (before FFT). This
 *                function should be called regularly, returning every 1 ms.
 *                It reads from the USRP, aligns the samples to the subframe and
 *                performs time/freq synch.
 *
 *                It is also possible to read the signal from a file using the
 *                init function srslte_ue_sync_init_file(). The sampling frequency
 *                is derived from the number of PRB.
 *
 *                The function returns 1 when the signal is correctly acquired and
 *                the returned buffer is aligned with the subframe.
 *
 *  Reference:
 *****************************************************************************/

#ifndef UE_SYNC_
#define UE_SYNC_

#include <stdbool.h>

#include "srslte/config.h"
#include "srslte/phy/sync/sync.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/agc/agc.h"
#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/phch/pbch.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/common/timestamp.h"
#include "srslte/phy/io/filesource.h"


typedef enum SRSLTE_API { SF_FIND, SF_TRACK} srslte_ue_sync_state_t;

//#define MEASURE_EXEC_TIME 

typedef struct SRSLTE_API {
  srslte_sync_t sfind;
  srslte_sync_t strack;

  uint32_t max_prb;
  
  srslte_agc_t agc; 
  bool do_agc; 
  uint32_t agc_period; 
  int decimate;
  void *stream; 
  void *stream_single; 
  int (*recv_callback)(void*, cf_t*[SRSLTE_MAX_PORTS], uint32_t, srslte_timestamp_t*);
  int (*recv_callback_single)(void*, void*, uint32_t, srslte_timestamp_t*); 
  srslte_timestamp_t last_timestamp;
  
  uint32_t nof_rx_antennas; 
  
  srslte_filesource_t file_source; 
  bool file_mode; 
  float file_cfo; 
  srslte_cfo_t file_cfo_correct; 
  
  srslte_ue_sync_state_t state;
  
  uint32_t frame_len; 
  uint32_t fft_size;
  uint32_t nof_recv_sf;  // Number of subframes received each call to srslte_ue_sync_get_buffer
  uint32_t nof_avg_find_frames;
  uint32_t frame_find_cnt;
  uint32_t sf_len;

  /* These count half frames (5ms) */
  uint64_t frame_ok_cnt;
  uint32_t frame_no_cnt; 
  uint32_t frame_total_cnt; 
  
  /* this is the system frame number (SFN) */
  uint32_t frame_number; 
  
  srslte_cell_t cell; 
  uint32_t sf_idx;
      
  bool decode_sss_on_track; 
  bool correct_cfo; 
  
  uint32_t peak_idx;
  int next_rf_sample_offset;
  int last_sample_offset; 
  float mean_sample_offset; 
  float mean_sfo; 
  uint32_t sample_offset_correct_period; 
  float sfo_ema; 
  

  #ifdef MEASURE_EXEC_TIME
  float mean_exec_time;
  #endif
} srslte_ue_sync_t;

SRSLTE_API int srslte_ue_sync_init(srslte_ue_sync_t *q, 
                                   uint32_t max_prb,
                                   bool search_cell,
                                   int (recv_callback)(void*, void*, uint32_t, srslte_timestamp_t*), 
                                   void *stream_handler);

SRSLTE_API int srslte_ue_sync_init_multi(srslte_ue_sync_t *q,
                                         uint32_t max_prb,
                                         bool search_cell,
                                         int (recv_callback)(void*, cf_t*[SRSLTE_MAX_PORTS], uint32_t, srslte_timestamp_t*), 
                                         uint32_t nof_rx_antennas,
                                         void *stream_handler);

SRSLTE_API int srslte_ue_sync_init_multi_decim(srslte_ue_sync_t *q,
                                               uint32_t max_prb,
                                               bool search_cell,
                                               int (recv_callback)(void*, cf_t*[SRSLTE_MAX_PORTS], uint32_t, srslte_timestamp_t*),
                                               uint32_t nof_rx_antennas,
                                               void *stream_handler,
                                               int decimate);

SRSLTE_API int srslte_ue_sync_init_file(srslte_ue_sync_t *q, 
                                        uint32_t nof_prb,
                                        char *file_name, 
                                        int offset_time, 
                                        float offset_freq);

SRSLTE_API int srslte_ue_sync_init_file_multi(srslte_ue_sync_t *q,
                                              uint32_t nof_prb,
                                              char *file_name,
                                              int offset_time,
                                              float offset_freq,
                                              uint32_t nof_rx_ant);

SRSLTE_API void srslte_ue_sync_free(srslte_ue_sync_t *q);

SRSLTE_API int srslte_ue_sync_set_cell(srslte_ue_sync_t *q,
                                       srslte_cell_t cell);

SRSLTE_API int srslte_ue_sync_start_agc(srslte_ue_sync_t *q, 
                                        double (set_gain_callback)(void*, double), 
                                        float init_gain_value); 

SRSLTE_API uint32_t srslte_ue_sync_sf_len(srslte_ue_sync_t *q); 

SRSLTE_API void srslte_ue_sync_set_agc_period(srslte_ue_sync_t *q, 
                                              uint32_t period); 

/* CAUTION: input_buffer MUST have space for 2 subframes */
SRSLTE_API int srslte_ue_sync_zerocopy(srslte_ue_sync_t *q, 
                                       cf_t *input_buffer);

SRSLTE_API int srslte_ue_sync_zerocopy_multi(srslte_ue_sync_t *q, 
                                             cf_t *input_buffer[SRSLTE_MAX_PORTS]);

SRSLTE_API void srslte_ue_sync_set_cfo_tol(srslte_ue_sync_t *q,
                                           float tol);

SRSLTE_API void srslte_ue_sync_set_cfo(srslte_ue_sync_t *q, 
                                       float cfo); 

SRSLTE_API void srslte_ue_sync_cfo_i_detec_en(srslte_ue_sync_t *q, 
                                              bool enable); 

SRSLTE_API void srslte_ue_sync_reset(srslte_ue_sync_t *q);

SRSLTE_API void srslte_ue_sync_set_N_id_2(srslte_ue_sync_t *q, 
                                          uint32_t N_id_2);

SRSLTE_API void srslte_ue_sync_decode_sss_on_track(srslte_ue_sync_t *q, 
                                                   bool enabled);

SRSLTE_API srslte_ue_sync_state_t srslte_ue_sync_get_state(srslte_ue_sync_t *q);

SRSLTE_API uint32_t srslte_ue_sync_get_sfidx(srslte_ue_sync_t *q);

SRSLTE_API float srslte_ue_sync_get_cfo(srslte_ue_sync_t *q);

SRSLTE_API float srslte_ue_sync_get_sfo(srslte_ue_sync_t *q);

SRSLTE_API int srslte_ue_sync_get_last_sample_offset(srslte_ue_sync_t *q); 

SRSLTE_API void srslte_ue_sync_set_sample_offset_correct_period(srslte_ue_sync_t *q, 
                                                                uint32_t nof_subframes); 

SRSLTE_API void srslte_ue_sync_get_last_timestamp(srslte_ue_sync_t *q, 
                                                  srslte_timestamp_t *timestamp);





#endif // SYNC_FRAME_

