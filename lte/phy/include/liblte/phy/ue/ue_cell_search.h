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

#ifndef UE_CELLSEARCH_
#define UE_CELLSEARCH_

#include <stdbool.h>

#include "liblte/config.h"
#include "liblte/phy/ue/ue_sync.h"
#include "liblte/phy/ue/ue_mib.h"
#include "liblte/phy/sync/cfo.h"
#include "liblte/phy/ch_estimation/chest_dl.h"
#include "liblte/phy/phch/pbch.h"
#include "liblte/phy/common/fft.h"

/************************************************************
 * 
 * This object is a wrapper to the ue_sync object. It receives
 * several synchronized frames and obtains the most common cell_id 
 * and cp length. 
 * 
 * The I/O stream device sampling frequency must be set to 1.92 MHz (CS_SAMP_FREQ constant)
 * before calling to ue_cell_search_scan() functions. 
 * 
 ************************************************************/

/** 
 * TODO: Check also peak offset 
 */

#define CS_DEFAULT_MAXFRAMES_TOTAL      500
#define CS_DEFAULT_MAXFRAMES_DETECTED   50

#define CS_DEFAULT_NOFFRAMES_TOTAL      50
#define CS_DEFAULT_NOFFRAMES_DETECTED   10

#define CS_NOF_PRB      6
#define CS_SAMP_FREQ    1920000.0

typedef struct LIBLTE_API {
  uint32_t cell_id;
  lte_cp_t cp; 
  float peak; 
  float mode; 
  float psr;
} ue_cell_search_result_t;


typedef struct LIBLTE_API {
  ue_sync_t ue_sync;
  
  uint32_t max_frames;
  uint32_t nof_frames_to_scan;  // number of 5 ms frames to scan 
  float detect_threshold; // early-stops scan if mean PSR above this threshold
    
  uint32_t *mode_ntimes;
  uint8_t *mode_counted; 
  
  ue_cell_search_result_t *candidates; 
} ue_cell_search_t;


LIBLTE_API int ue_cell_search_init(ue_cell_search_t *q, 
                                  int (recv_callback)(void*, void*, uint32_t,timestamp_t*),
                                  void *stream_handler);

LIBLTE_API int ue_cell_search_init_max(ue_cell_search_t *q, 
                                      uint32_t max_frames_total, 
                                      int (recv_callback)(void*, void*, uint32_t,timestamp_t*),
                                      void *stream_handler);

LIBLTE_API void ue_cell_search_free(ue_cell_search_t *q);

LIBLTE_API int ue_cell_search_scan_N_id_2(ue_cell_search_t *q, 
                                          uint32_t N_id_2, 
                                          ue_cell_search_result_t *found_cell);

LIBLTE_API int ue_cell_search_scan(ue_cell_search_t * q, 
                                   ue_cell_search_result_t found_cells[3], 
                                   uint32_t *max_N_id_2); 

LIBLTE_API int ue_cell_search_set_nof_frames_to_scan(ue_cell_search_t *q, 
                                                     uint32_t nof_frames);

LIBLTE_API void ue_cell_search_set_threshold(ue_cell_search_t *q, 
                                            float threshold); 






#endif // SYNC_FRAME_

