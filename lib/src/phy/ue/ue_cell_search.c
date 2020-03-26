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

#include "srslte/srslte.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srslte/phy/ue/ue_cell_search.h"

#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define CELL_SEARCH_BUFFER_MAX_SAMPLES (3 * SRSLTE_SF_LEN_MAX)

int srslte_ue_cellsearch_init(srslte_ue_cellsearch_t* q,
                              uint32_t                max_frames,
                              int(recv_callback)(void*, void*, uint32_t, srslte_timestamp_t*),
                              void* stream_handler)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSLTE_ERROR;
    srslte_cell_t cell;

    bzero(q, sizeof(srslte_ue_cellsearch_t));

    bzero(&cell, sizeof(srslte_cell_t));
    cell.id      = SRSLTE_CELL_ID_UNKNOWN;
    cell.nof_prb = SRSLTE_CS_NOF_PRB;

    if (srslte_ue_sync_init(&q->ue_sync, cell.nof_prb, true, recv_callback, stream_handler)) {
      ERROR("Error initiating ue_sync\n");
      goto clean_exit;
    }

    if (srslte_ue_sync_set_cell(&q->ue_sync, cell)) {
      ERROR("Error initiating ue_sync\n");
      goto clean_exit;
    }

    for (int p = 0; p < SRSLTE_MAX_CHANNELS; p++) {
      q->sf_buffer[p] = NULL;
    }
    q->sf_buffer[0]    = srslte_vec_cf_malloc(CELL_SEARCH_BUFFER_MAX_SAMPLES);
    q->nof_rx_antennas = 1;

    q->candidates = calloc(sizeof(srslte_ue_cellsearch_result_t), max_frames);
    if (!q->candidates) {
      perror("malloc");
      goto clean_exit;
    }
    q->mode_ntimes = calloc(sizeof(uint32_t), max_frames);
    if (!q->mode_ntimes) {
      perror("malloc");
      goto clean_exit;
    }
    q->mode_counted = calloc(sizeof(uint8_t), max_frames);
    if (!q->mode_counted) {
      perror("malloc");
      goto clean_exit;
    }

    q->max_frames       = max_frames;
    q->nof_valid_frames = max_frames;

    ret = SRSLTE_SUCCESS;
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_cellsearch_free(q);
  }
  return ret;
}

int srslte_ue_cellsearch_init_multi(
    srslte_ue_cellsearch_t* q,
    uint32_t                max_frames,
    int(recv_callback)(void*, cf_t* [SRSLTE_MAX_CHANNELS], uint32_t, srslte_timestamp_t*),
    uint32_t nof_rx_antennas,
    void*    stream_handler)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && nof_rx_antennas < SRSLTE_MAX_CHANNELS) {
    ret = SRSLTE_ERROR;
    srslte_cell_t cell;

    bzero(q, sizeof(srslte_ue_cellsearch_t));

    bzero(&cell, sizeof(srslte_cell_t));
    cell.id      = SRSLTE_CELL_ID_UNKNOWN;
    cell.nof_prb = SRSLTE_CS_NOF_PRB;

    if (srslte_ue_sync_init_multi(&q->ue_sync, cell.nof_prb, true, recv_callback, nof_rx_antennas, stream_handler)) {
      fprintf(stderr, "Error initiating ue_sync\n");
      goto clean_exit;
    }
    if (srslte_ue_sync_set_cell(&q->ue_sync, cell)) {
      ERROR("Error setting cell in ue_sync\n");
      goto clean_exit;
    }

    for (int i = 0; i < nof_rx_antennas; i++) {
      q->sf_buffer[i] = srslte_vec_cf_malloc(CELL_SEARCH_BUFFER_MAX_SAMPLES);
    }
    q->nof_rx_antennas = nof_rx_antennas;

    q->candidates = calloc(sizeof(srslte_ue_cellsearch_result_t), max_frames);
    if (!q->candidates) {
      perror("malloc");
      goto clean_exit;
    }
    q->mode_ntimes = calloc(sizeof(uint32_t), max_frames);
    if (!q->mode_ntimes) {
      perror("malloc");
      goto clean_exit;
    }
    q->mode_counted = calloc(sizeof(uint8_t), max_frames);
    if (!q->mode_counted) {
      perror("malloc");
      goto clean_exit;
    }

    q->max_frames       = max_frames;
    q->nof_valid_frames = max_frames;

    ret = SRSLTE_SUCCESS;
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_cellsearch_free(q);
  }
  return ret;
}

void srslte_ue_cellsearch_free(srslte_ue_cellsearch_t* q)
{
  for (int i = 0; i < q->nof_rx_antennas; i++) {
    if (q->sf_buffer[i]) {
      free(q->sf_buffer[i]);
    }
  }
  if (q->candidates) {
    free(q->candidates);
  }
  if (q->mode_counted) {
    free(q->mode_counted);
  }
  if (q->mode_ntimes) {
    free(q->mode_ntimes);
  }
  srslte_ue_sync_free(&q->ue_sync);

  bzero(q, sizeof(srslte_ue_cellsearch_t));
}

int srslte_ue_cellsearch_set_nof_valid_frames(srslte_ue_cellsearch_t* q, uint32_t nof_frames)
{
  if (nof_frames <= q->max_frames) {
    q->nof_valid_frames = nof_frames;
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR;
  }
}

/* Decide the most likely cell based on the mode */
static void get_cell(srslte_ue_cellsearch_t* q, uint32_t nof_detected_frames, srslte_ue_cellsearch_result_t* found_cell)
{
  uint32_t i, j;

  bzero(q->mode_counted, nof_detected_frames);
  bzero(q->mode_ntimes, sizeof(uint32_t) * nof_detected_frames);

  /* First find mode of CELL IDs */
  for (i = 0; i < nof_detected_frames; i++) {
    uint32_t cnt = 1;
    for (j = i + 1; j < nof_detected_frames; j++) {
      if (q->candidates[j].cell_id == q->candidates[i].cell_id && !q->mode_counted[j]) {
        q->mode_counted[j] = 1;
        cnt++;
      }
    }
    q->mode_ntimes[i] = cnt;
  }
  uint32_t max_times = 0, mode_pos = 0;
  for (i = 0; i < nof_detected_frames; i++) {
    if (q->mode_ntimes[i] > max_times) {
      max_times = q->mode_ntimes[i];
      mode_pos  = i;
    }
  }
  found_cell->cell_id = q->candidates[mode_pos].cell_id;
  /* Now in all these cell IDs, find most frequent CP and duplex mode */
  uint32_t nof_normal = 0;
  uint32_t nof_fdd    = 0;
  found_cell->peak    = 0;
  for (i = 0; i < nof_detected_frames; i++) {
    if (q->candidates[i].cell_id == found_cell->cell_id) {
      if (SRSLTE_CP_ISNORM(q->candidates[i].cp)) {
        nof_normal++;
      }
      if (q->candidates[i].frame_type == SRSLTE_FDD) {
        nof_fdd++;
      }
    }
    // average absolute peak value
    found_cell->peak += q->candidates[i].peak;
  }
  found_cell->peak /= nof_detected_frames;

  if (nof_normal > q->mode_ntimes[mode_pos] / 2) {
    found_cell->cp = SRSLTE_CP_NORM;
  } else {
    found_cell->cp = SRSLTE_CP_EXT;
  }
  if (nof_fdd > q->mode_ntimes[mode_pos] / 2) {
    found_cell->frame_type = SRSLTE_FDD;
  } else {
    found_cell->frame_type = SRSLTE_TDD;
  }
  found_cell->mode = (float)q->mode_ntimes[mode_pos] / nof_detected_frames;

  // PSR is already averaged so take the last value
  found_cell->psr = q->candidates[nof_detected_frames - 1].psr;

  // CFO is also already averaged
  found_cell->cfo = q->candidates[nof_detected_frames - 1].cfo;
}

/** Finds up to 3 cells, one per each N_id_2=0,1,2 and stores ID and CP in the structure pointed by found_cell.
 * Each position in found_cell corresponds to a different N_id_2.
 * Saves in the pointer max_N_id_2 the N_id_2 index of the cell with the highest PSR
 * Returns the number of found cells or a negative number if error
 */
int srslte_ue_cellsearch_scan(srslte_ue_cellsearch_t*       q,
                              srslte_ue_cellsearch_result_t found_cells[3],
                              uint32_t*                     max_N_id_2)
{
  int      ret                = 0;
  float    max_peak_value     = -1.0;
  uint32_t nof_detected_cells = 0;

  for (uint32_t N_id_2 = 0; N_id_2 < 3 && ret >= 0; N_id_2++) {
    INFO("CELL SEARCH: Starting scan for N_id_2=%d\n", N_id_2);
    ret = srslte_ue_cellsearch_scan_N_id_2(q, N_id_2, &found_cells[N_id_2]);
    if (ret < 0) {
      ERROR("Error searching cell\n");
      return ret;
    }
    nof_detected_cells += ret;
    if (max_N_id_2) {
      if (found_cells[N_id_2].peak > max_peak_value) {
        max_peak_value = found_cells[N_id_2].peak;
        *max_N_id_2    = N_id_2;
      }
    }
  }
  return nof_detected_cells;
}

/** Finds a cell for a given N_id_2 and stores ID and CP in the structure pointed by found_cell.
 * Returns 1 if the cell is found, 0 if not or -1 on error
 */
int srslte_ue_cellsearch_scan_N_id_2(srslte_ue_cellsearch_t*        q,
                                     uint32_t                       N_id_2,
                                     srslte_ue_cellsearch_result_t* found_cell)
{
  int      ret                 = SRSLTE_ERROR_INVALID_INPUTS;
  uint32_t nof_detected_frames = 0;
  uint32_t nof_scanned_frames  = 0;

  if (q != NULL) {
    ret = SRSLTE_SUCCESS;

    bzero(q->candidates, sizeof(srslte_ue_cellsearch_result_t) * q->max_frames);
    bzero(q->mode_ntimes, sizeof(uint32_t) * q->max_frames);
    bzero(q->mode_counted, sizeof(uint8_t) * q->max_frames);

    srslte_ue_sync_set_N_id_2(&q->ue_sync, N_id_2);
    srslte_ue_sync_reset(&q->ue_sync);
    srslte_ue_sync_cfo_reset(&q->ue_sync, 0.0f);
    srslte_ue_sync_set_nof_find_frames(&q->ue_sync, q->max_frames);

    do {
      ret = srslte_ue_sync_zerocopy(&q->ue_sync, q->sf_buffer, CELL_SEARCH_BUFFER_MAX_SAMPLES);
      if (ret < 0) {
        ERROR("Error calling srslte_ue_sync_work()\n");
        return -1;
      } else if (ret == 1) {
        /* This means a peak was found in find state */
        ret = srslte_sync_get_cell_id(&q->ue_sync.sfind);
        if (ret >= 0) {
          /* Save cell id, cp and peak */
          q->candidates[nof_detected_frames].cell_id    = (uint32_t)ret;
          q->candidates[nof_detected_frames].cp         = srslte_sync_get_cp(&q->ue_sync.sfind);
          q->candidates[nof_detected_frames].peak       = q->ue_sync.sfind.pss.peak_value;
          q->candidates[nof_detected_frames].psr        = srslte_sync_get_peak_value(&q->ue_sync.sfind);
          q->candidates[nof_detected_frames].cfo        = 15000 * srslte_sync_get_cfo(&q->ue_sync.sfind);
          q->candidates[nof_detected_frames].frame_type = srslte_ue_sync_get_frame_type(&q->ue_sync);
          INFO("CELL SEARCH: [%d/%d/%d]: Found peak PSR=%.3f, Cell_id: %d CP: %s, CFO=%.1f KHz\n",
               nof_detected_frames,
               nof_scanned_frames,
               q->nof_valid_frames,
               q->candidates[nof_detected_frames].psr,
               q->candidates[nof_detected_frames].cell_id,
               srslte_cp_string(q->candidates[nof_detected_frames].cp),
               q->candidates[nof_detected_frames].cfo / 1000);

          nof_detected_frames++;
        }
      } else if (ret == 0) {
        /* This means a peak is not yet found and ue_sync is in find state
         * Do nothing, just wait and increase nof_scanned_frames counter.
         */
      }

      nof_scanned_frames++;

    } while (nof_scanned_frames < q->max_frames && nof_detected_frames < q->nof_valid_frames);

    /* In either case, check if the mean PSR is above the minimum threshold */
    if (nof_detected_frames > 0) {
      ret = 1; // A cell has been found.
      if (found_cell) {
        get_cell(q, nof_detected_frames, found_cell);
      }
    } else {
      ret = 0; // A cell was not found.
    }
  }

  return ret;
}
