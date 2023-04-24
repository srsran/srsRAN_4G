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
 *  File:         ue_cell_search.h
 *
 *  Description:  Wrapper for the ue_sync object.
 *
 *                This object is a wrapper to the ue_sync object. It receives
 *                several synchronized frames and obtains the most common cell_id
 *                and cp length.
 *
 *                The I/O stream device sampling frequency must be set to 1.92 MHz
 *                (SRSRAN_CS_SAMP_FREQ constant) before calling to
 *                srsran_ue_cellsearch_scan() functions.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_UE_CELL_SEARCH_H
#define SRSRAN_UE_CELL_SEARCH_H

#include <stdbool.h>

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/pbch.h"
#include "srsran/phy/sync/cfo.h"
#include "srsran/phy/ue/ue_mib.h"
#include "srsran/phy/ue/ue_sync.h"

#define SRSRAN_CS_NOF_PRB      6
#define SRSRAN_CS_SAMP_FREQ    1920000.0

typedef struct SRSRAN_API {
  uint32_t cell_id;
  srsran_cp_t         cp;
  srsran_frame_type_t frame_type;
  float peak; 
  float mode; 
  float psr;
  float               cfo;
} srsran_ue_cellsearch_result_t;

typedef struct SRSRAN_API {
  srsran_ue_sync_t ue_sync;

  cf_t*    sf_buffer[SRSRAN_MAX_CHANNELS];
  uint32_t nof_rx_antennas;

  uint32_t max_frames;
  uint32_t nof_valid_frames;  // number of 5 ms frames to scan 
    
  uint32_t *mode_ntimes;
  uint8_t*  mode_counted;

  srsran_ue_cellsearch_result_t* candidates;
} srsran_ue_cellsearch_t;

SRSRAN_API int srsran_ue_cellsearch_init(srsran_ue_cellsearch_t* q,
                                         uint32_t                max_frames_total,
                                         int(recv_callback)(void*, void*, uint32_t, srsran_timestamp_t*),
                                         void* stream_handler);

SRSRAN_API int
srsran_ue_cellsearch_init_multi(srsran_ue_cellsearch_t* q,
                                uint32_t                max_frames_total,
                                int(recv_callback)(void*, cf_t* [SRSRAN_MAX_CHANNELS], uint32_t, srsran_timestamp_t*),
                                uint32_t nof_rx_antennas,
                                void*    stream_handler);

SRSRAN_API void srsran_ue_cellsearch_free(srsran_ue_cellsearch_t* q);

SRSRAN_API int
srsran_ue_cellsearch_scan_N_id_2(srsran_ue_cellsearch_t* q, uint32_t N_id_2, srsran_ue_cellsearch_result_t* found_cell);

SRSRAN_API int srsran_ue_cellsearch_scan(srsran_ue_cellsearch_t*       q,
                                         srsran_ue_cellsearch_result_t found_cells[3],
                                         uint32_t*                     max_N_id_2);

SRSRAN_API int srsran_ue_cellsearch_set_nof_valid_frames(srsran_ue_cellsearch_t* q, uint32_t nof_frames);

SRSRAN_API void srsran_set_detect_cp(srsran_ue_cellsearch_t* q, bool enable);

#endif // SRSRAN_UE_CELL_SEARCH_H

