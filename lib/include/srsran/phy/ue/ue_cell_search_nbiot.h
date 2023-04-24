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

#ifndef SRSRAN_UE_CELL_SEARCH_NBIOT_H
#define SRSRAN_UE_CELL_SEARCH_NBIOT_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_dl_nbiot.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/npbch.h"
#include "srsran/phy/sync/cfo.h"
#include "srsran/phy/ue/ue_mib_nbiot.h"
#include "srsran/phy/ue/ue_sync_nbiot.h"

#define SRSRAN_CS_NOF_PRB 6
#define SRSRAN_CS_SAMP_FREQ 1920000.0

typedef struct SRSRAN_API {
  uint32_t n_id_ncell;
  float    peak;
  float    mode;
  float    psr;
  float    cfo;
} srsran_nbiot_ue_cellsearch_result_t;

/**
 * \brief Wrapper for the nbiot_ue_sync object.
 *
 * This object is a wrapper to the nbiot_ue_sync object. It receives
 * several synchronized frames and obtains the most common n_id_ncell.
 *
 * The I/O stream device sampling frequency must be set to 1.92 MHz
 * (SRSRAN_CS_SAMP_FREQ constant) before calling to
 * srsran_ue_cellsearch_nbiot_scan() functions.
 */
typedef struct SRSRAN_API {
  srsran_nbiot_ue_sync_t ue_sync;
  int32_t                sf_len;

  cf_t* rx_buffer[SRSRAN_MAX_CHANNELS];
  cf_t* nsss_buffer;
  int   nsss_sf_counter;

  uint32_t max_frames;
  uint32_t nof_valid_frames; // number of frames to scan
} srsran_ue_cellsearch_nbiot_t;

SRSRAN_API int
srsran_ue_cellsearch_nbiot_init(srsran_ue_cellsearch_nbiot_t* q,
                                uint32_t                      max_frames_total,
                                int(recv_callback)(void*, cf_t* [SRSRAN_MAX_CHANNELS], uint32_t, srsran_timestamp_t*),
                                void* stream_handler);

SRSRAN_API void srsran_ue_cellsearch_nbiot_free(srsran_ue_cellsearch_nbiot_t* q);

SRSRAN_API int srsran_ue_cellsearch_nbiot_scan(srsran_ue_cellsearch_nbiot_t* q);

SRSRAN_API int srsran_ue_cellsearch_nbiot_detect(srsran_ue_cellsearch_nbiot_t*        q,
                                                 srsran_nbiot_ue_cellsearch_result_t* found_cells);

SRSRAN_API int srsran_ue_cellsearch_nbiot_set_nof_valid_frames(srsran_ue_cellsearch_nbiot_t* q, uint32_t nof_frames);

#endif // SRSRAN_UE_CELL_SEARCH_NBIOT_H
