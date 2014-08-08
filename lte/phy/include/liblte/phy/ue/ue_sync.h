/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef UE_SYNC_
#define UE_SYNC_

#include <stdbool.h>

#include "liblte/config.h"
#include "liblte/phy/sync/sync.h"
#include "liblte/phy/sync/cfo.h"
#include "liblte/phy/ch_estimation/chest.h"
#include "liblte/phy/phch/pbch.h"
#include "liblte/phy/common/fft.h"

/**************************************************************
 *
 * This object automatically manages the cell synchronization procedure. 
 * 
 * The main function is ue_sync_get_buffer(), which returns a pointer
 * to the aligned subframe of samples (before FFT). This function
 * should be called regularly, returning every 1 ms. It reads from the 
 * USRP, aligns the samples to the subframe and performs time/freq synch. 
 *
 * The function returns 1 when the signal is correctly acquired and the 
 * returned buffer is aligned with the subframe. 
 * 
 *************************************************************/

typedef enum LIBLTE_API { SF_FIND, SF_TRACK} ue_sync_state_t;

#define TRACK_MAX_LOST          10
#define MEASURE_EXEC_TIME 

typedef struct LIBLTE_API {
  sync_t sfind;
  sync_t strack;

  void *stream; 
  int (*recv_callback)(void*, void*, uint32_t); 

  ue_sync_state_t state;
  
  cf_t *input_buffer; 
  
  /* These count half frames (5ms) */
  uint64_t frame_ok_cnt;
  uint32_t frame_no_cnt; 
  uint32_t frame_total_cnt; 
  
  /* this is the system frame number (SFN) */
  uint32_t frame_number; 
  
  lte_cell_t cell; 
  uint32_t sf_idx;
  
  cfo_t cfocorr;
  float cur_cfo;
    
  bool decode_sss_on_track; 
  
  uint32_t peak_idx;
  int time_offset;
  float mean_time_offset; 
  #ifdef MEASURE_EXEC_TIME
  float mean_exec_time;
  #endif
} ue_sync_t;


LIBLTE_API int ue_sync_init(ue_sync_t *q, 
                               lte_cell_t cell,
                               int (recv_callback)(void*, void*, uint32_t), 
                               void *stream_handler);

LIBLTE_API void ue_sync_free(ue_sync_t *q);

LIBLTE_API int ue_sync_get_buffer(ue_sync_t *q, 
                                  cf_t **sf_symbols);

LIBLTE_API void ue_sync_reset(ue_sync_t *q);

LIBLTE_API void ue_sync_decode_sss_on_track(ue_sync_t *q, 
                                            bool enabled);

LIBLTE_API ue_sync_state_t ue_sync_get_state(ue_sync_t *q);

LIBLTE_API uint32_t ue_sync_get_sfidx(ue_sync_t *q);

LIBLTE_API float ue_sync_get_cfo(ue_sync_t *q);

LIBLTE_API float ue_sync_get_sfo(ue_sync_t *q);






#endif // SYNC_FRAME_

