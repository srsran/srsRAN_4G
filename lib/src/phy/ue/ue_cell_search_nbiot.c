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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srsran/phy/ue/ue_cell_search_nbiot.h"

#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

int srsran_ue_cellsearch_nbiot_init(srsran_ue_cellsearch_nbiot_t* q,
                                    uint32_t                      max_frames_total,
                                    int(recv_callback)(void*, cf_t* [SRSRAN_MAX_PORTS], uint32_t, srsran_timestamp_t*),
                                    void* stream_handler)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSRAN_ERROR;
    bzero(q, sizeof(srsran_ue_cellsearch_nbiot_t));

    q->sf_len = SRSRAN_SF_LEN_PRB_NBIOT;
    if (q->sf_len < 0) {
      return ret;
    }

    if (srsran_ue_sync_nbiot_init_multi(
            &q->ue_sync, SRSRAN_NBIOT_MAX_PRB, recv_callback, SRSRAN_NBIOT_NUM_RX_ANTENNAS, stream_handler)) {
      fprintf(stderr, "Error initiating ue_sync\n");
      goto clean_exit;
    }

    for (uint32_t i = 0; i < SRSRAN_NBIOT_NUM_RX_ANTENNAS; i++) {
      q->rx_buffer[i] = srsran_vec_cf_malloc(SRSRAN_NOF_SF_X_FRAME * q->sf_len);
      if (!q->rx_buffer[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }

    // buffer to hold subframes for NSSS detection
    q->nsss_buffer = srsran_vec_cf_malloc(SRSRAN_NSSS_NUM_SF_DETECT * q->sf_len);
    if (!q->nsss_buffer) {
      perror("malloc");
      goto clean_exit;
    }

    q->max_frames       = max_frames_total;
    q->nof_valid_frames = max_frames_total;

    ret = SRSRAN_SUCCESS;
  }

clean_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_ue_cellsearch_nbiot_free(q);
  }
  return ret;
}

void srsran_ue_cellsearch_nbiot_free(srsran_ue_cellsearch_nbiot_t* q)
{
  for (uint32_t i = 0; i < SRSRAN_NBIOT_NUM_RX_ANTENNAS; i++) {
    if (q->rx_buffer[i] != NULL) {
      free(q->rx_buffer[i]);
    }
  }

  srsran_ue_sync_nbiot_free(&q->ue_sync);
  bzero(q, sizeof(srsran_ue_cellsearch_nbiot_t));
}

int srsran_ue_cellsearch_nbiot_set_nof_valid_frames(srsran_ue_cellsearch_nbiot_t* q, uint32_t nof_frames)
{
  if (nof_frames <= q->max_frames) {
    q->nof_valid_frames = nof_frames;
    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR;
  }
}

/** This function tries to receive one valid NSSS subframe for cell id detection.
 *  It stores subframe 9 of two consecutive frames in a buffer and returns.
 */
int srsran_ue_cellsearch_nbiot_scan(srsran_ue_cellsearch_nbiot_t* q)
{
  int      ret                 = SRSRAN_ERROR_INVALID_INPUTS;
  uint32_t nof_detected_frames = 0;
  uint32_t nof_scanned_frames  = 0;

  if (q != NULL) {
    ret = SRSRAN_ERROR;
    srsran_ue_sync_nbiot_reset(&q->ue_sync);
    q->nsss_sf_counter = 0;

    do {
      ret = srsran_ue_sync_nbiot_zerocopy_multi(&q->ue_sync, q->rx_buffer);
      if (ret < 0) {
        fprintf(stderr, "Error calling srsran_nbiot_ue_sync_get_buffer()\n");
        break;
      } else if (ret == 1) {
        // we are in sync, wait until we have received two full frames, store subframe 9 for both
        DEBUG("In tracking state sf_idx=%d", srsran_ue_sync_nbiot_get_sfidx(&q->ue_sync));
        if (srsran_ue_sync_nbiot_get_sfidx(&q->ue_sync) == 9) {
          // accumulate NSSS subframes for cell id detection
          srsran_vec_cf_copy(&q->nsss_buffer[q->nsss_sf_counter * q->sf_len], q->rx_buffer[0], q->sf_len);
          q->nsss_sf_counter++;
          if (q->nsss_sf_counter == SRSRAN_NSSS_NUM_SF_DETECT) {
            DEBUG("Captured %d subframes for NSSS detection.", q->nsss_sf_counter);
            return SRSRAN_SUCCESS;
          }
        }
      } else if (ret == 0) {
        //< This means a peak is not yet found and we are in find state, do nothing, just wait and increase
        // nof_scanned_frames counter
        nof_scanned_frames++;
      }
    } while (nof_scanned_frames < q->max_frames && nof_detected_frames < q->nof_valid_frames);
  }
  return SRSRAN_ERROR;
}

int srsran_ue_cellsearch_nbiot_detect(srsran_ue_cellsearch_nbiot_t* q, srsran_nbiot_ue_cellsearch_result_t* found_cells)
{
  int ret = srsran_sync_nbiot_find_cell_id(&q->ue_sync.strack, q->nsss_buffer);
  if (ret == SRSRAN_SUCCESS) {
    int cell_id = srsran_sync_nbiot_get_cell_id(&q->ue_sync.strack);
    if (srsran_cellid_isvalid(cell_id)) {
      // save cell, CP and peak
      found_cells[0].n_id_ncell = (uint32_t)cell_id;
      found_cells[0].peak       = q->ue_sync.strack.npss.peak_value;
      found_cells[0].psr        = srsran_sync_nbiot_get_peak_value(&q->ue_sync.strack);
      found_cells[0].cfo        = srsran_ue_sync_nbiot_get_cfo(&q->ue_sync);
      INFO("CELL SEARCH: Found peak PSR=%.3f, Cell_id: %d", found_cells[0].psr, found_cells[0].n_id_ncell);
    }
  }
  return ret;
}
