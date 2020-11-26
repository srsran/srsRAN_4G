/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_UE_CELL_SEARCH_NBIOT_H
#define SRSLTE_UE_CELL_SEARCH_NBIOT_H

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/npbch.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/ue/ue_mib_nbiot.h"
#include "srslte/phy/ue/ue_sync_nbiot.h"

#define SRSLTE_CS_NOF_PRB 6
#define SRSLTE_CS_SAMP_FREQ 1920000.0

typedef struct SRSLTE_API {
  uint32_t n_id_ncell;
  float    peak;
  float    mode;
  float    psr;
  float    cfo;
} srslte_nbiot_ue_cellsearch_result_t;

/**
 * \brief Wrapper for the nbiot_ue_sync object.
 *
 * This object is a wrapper to the nbiot_ue_sync object. It receives
 * several synchronized frames and obtains the most common n_id_ncell.
 *
 * The I/O stream device sampling frequency must be set to 1.92 MHz
 * (SRSLTE_CS_SAMP_FREQ constant) before calling to
 * srslte_ue_cellsearch_nbiot_scan() functions.
 */
typedef struct SRSLTE_API {
  srslte_nbiot_ue_sync_t ue_sync;
  int32_t                sf_len;

  cf_t* rx_buffer[SRSLTE_MAX_CHANNELS];
  cf_t* nsss_buffer;
  int   nsss_sf_counter;

  uint32_t max_frames;
  uint32_t nof_valid_frames; // number of frames to scan
} srslte_ue_cellsearch_nbiot_t;

SRSLTE_API int
srslte_ue_cellsearch_nbiot_init(srslte_ue_cellsearch_nbiot_t* q,
                                uint32_t                      max_frames_total,
                                int(recv_callback)(void*, cf_t* [SRSLTE_MAX_CHANNELS], uint32_t, srslte_timestamp_t*),
                                void* stream_handler);

SRSLTE_API void srslte_ue_cellsearch_nbiot_free(srslte_ue_cellsearch_nbiot_t* q);

SRSLTE_API int srslte_ue_cellsearch_nbiot_scan(srslte_ue_cellsearch_nbiot_t* q);

SRSLTE_API int srslte_ue_cellsearch_nbiot_detect(srslte_ue_cellsearch_nbiot_t*        q,
                                                 srslte_nbiot_ue_cellsearch_result_t* found_cells);

SRSLTE_API int srslte_ue_cellsearch_nbiot_set_nof_valid_frames(srslte_ue_cellsearch_nbiot_t* q, uint32_t nof_frames);

#endif // SRSLTE_UE_CELL_SEARCH_NBIOT_H
