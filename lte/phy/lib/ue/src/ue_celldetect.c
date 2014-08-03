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

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <unistd.h>

#include "liblte/phy/ue/ue_celldetect.h"

#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"

#define FIND_FFTSIZE   64
#define FIND_SFLEN     5*SF_LEN(FIND_FFTSIZE)

int ue_celldetect_init(ue_celldetect_t * q) {
  return ue_celldetect_init_max(q, CS_DEFAULT_MAXFRAMES_TOTAL, CS_DEFAULT_MAXFRAMES_DETECTED); 
}

int ue_celldetect_init_max(ue_celldetect_t * q, uint32_t max_frames_total, uint32_t max_frames_detected) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = LIBLTE_ERROR;

    bzero(q, sizeof(ue_celldetect_t));

    q->candidates = malloc(sizeof(ue_celldetect_result_t) * max_frames_detected);
    if (!q->candidates) {
      perror("malloc");
      goto clean_exit; 
    }
    if (sync_init(&q->sfind, FIND_SFLEN, FIND_FFTSIZE)) {
      goto clean_exit;
    }
    q->mode_ntimes = malloc(sizeof(uint32_t) * max_frames_detected);
    if (!q->mode_ntimes) {
      perror("malloc");
      goto clean_exit;  
    }
    q->mode_counted = malloc(sizeof(char) * max_frames_detected);
    if (!q->mode_counted) {
      perror("malloc");
      goto clean_exit;  
    }

    sync_set_threshold(&q->sfind, CS_FIND_THRESHOLD);
    sync_sss_en(&q->sfind, true);

    q->max_frames_total = max_frames_total;
    q->max_frames_detected = max_frames_detected;
    q->nof_frames_total = CS_DEFAULT_NOFFRAMES_TOTAL; 
    q->nof_frames_detected = CS_DEFAULT_NOFFRAMES_DETECTED; 

    ue_celldetect_reset(q);
    
    ret = LIBLTE_SUCCESS;
  }

clean_exit:
  if (ret == LIBLTE_ERROR) {
    ue_celldetect_free(q);
  }
  return ret;
}

void ue_celldetect_free(ue_celldetect_t * q)
{
  if (q->candidates) {
    free(q->candidates);
  }
  if (q->mode_counted) {
    free(q->mode_counted);
  }
  if (q->mode_ntimes) {
    free(q->mode_ntimes);
  }
  sync_free(&q->sfind);
}


void ue_celldetect_reset(ue_celldetect_t * q)
{
  q->current_nof_detected = 0; 
  q->current_nof_total = 0; 
  q->current_N_id_2 = 0; 
}

void ue_celldetect_set_threshold(ue_celldetect_t * q, float threshold)
{
  sync_set_threshold(&q->sfind, threshold);
}

int ue_celldetect_set_nof_frames_total(ue_celldetect_t * q, uint32_t nof_frames)
{
  if (nof_frames <= q->max_frames_total) {
    q->nof_frames_total = nof_frames;    
    return LIBLTE_SUCCESS; 
  } else {
    return LIBLTE_ERROR;
  }
}

int ue_celldetect_set_nof_frames_detected(ue_celldetect_t * q, uint32_t nof_frames)
{
  if (nof_frames <= q->max_frames_detected) {
    q->nof_frames_detected = nof_frames;    
    return LIBLTE_SUCCESS; 
  } else {
    return LIBLTE_ERROR;
  }
}

/* Decide the most likely cell based on the mode */
void decide_cell(ue_celldetect_t * q, ue_celldetect_result_t *found_cell)
{
  uint32_t i, j;
  
  bzero(q->mode_counted, q->nof_frames_detected);
  bzero(q->mode_ntimes, sizeof(uint32_t) * q->nof_frames_detected);
  
  /* First find mode of CELL IDs */
  for (i = 0; i < q->nof_frames_detected; i++) {
    uint32_t cnt = 1;
    for (j=i+1;j<q->nof_frames_detected;j++) {
      if (q->candidates[j].cell_id == q->candidates[i].cell_id && !q->mode_counted[j]) {
        q->mode_counted[j]=1;
        cnt++;
      }
    }
    q->mode_ntimes[i] = cnt; 
  }
  uint32_t max_times=0, mode_pos=0; 
  for (i=0;i<q->nof_frames_detected;i++) {
    if (q->mode_ntimes[i] > 0) {
      DEBUG("ntimes[%d]=%d (CID: %d)\n",i,q->mode_ntimes[i],q->candidates[i].cell_id);      
    }
    if (q->mode_ntimes[i] > max_times) {
      max_times = q->mode_ntimes[i];
      mode_pos = i;
    }
  }
  found_cell->cell_id = q->candidates[mode_pos].cell_id;
  /* Now in all these cell IDs, find most frequent CP */
  uint32_t nof_normal = 0;
  found_cell->peak = 0; 
  for (i=0;i<q->nof_frames_detected;i++) {
    if (q->candidates[i].cell_id == found_cell->cell_id) {
      if (CP_ISNORM(q->candidates[i].cp)) {
        nof_normal++;
      } 
      found_cell->peak += q->candidates[i].peak/q->mode_ntimes[mode_pos];
    }
  }
  if (nof_normal > q->mode_ntimes[mode_pos]/2) {
    found_cell->cp = CPNORM;
  } else {
    found_cell->cp = CPEXT; 
  }
  found_cell->mode = q->mode_ntimes[mode_pos];  
}

int ue_celldetect_scan(ue_celldetect_t * q, 
                       cf_t *signal, 
                       uint32_t nsamples,
                       ue_celldetect_result_t *found_cell)
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  uint32_t peak_idx;
  uint32_t nof_input_frames; 


  if (q                 != NULL &&
      signal            != NULL && 
      nsamples          >= 4800) 
  {
    ret = LIBLTE_SUCCESS; 
    
    if (nsamples % 4800) {
      printf("Warning: nsamples must be a multiple of 4800. Some samples will be ignored\n");
      nsamples = (nsamples/4800) * 4800;
    }
    nof_input_frames = nsamples/4800; 
    
    for (uint32_t nf=0;nf<nof_input_frames;nf++) {
      sync_set_N_id_2(&q->sfind, q->current_N_id_2);

      INFO("[%3d/%3d]: Searching cells with N_id_2=%d. %d frames\n", 
           q->current_nof_detected, q->current_nof_total, q->current_N_id_2, nof_input_frames);

      /* Find peak and cell id */
      ret = sync_find(&q->sfind, &signal[nf*4800], 0, &peak_idx);
      if (ret < 0) {
        fprintf(stderr, "Error finding correlation peak (%d)\n", ret);
        return -1;
      }

      /* If peak position does not allow to read SSS, return error -3 */
      if (ret == LIBLTE_SUCCESS && peak_idx != 0) {
        return CS_FRAME_UNALIGNED; 
      }

      /* Process the peak result */
      if (ret == 1) {
        if (sync_sss_detected(&q->sfind)) {
          ret = sync_get_cell_id(&q->sfind);
          if (ret >= 0) {
            /* Save cell id, cp and peak */
            q->candidates[q->current_nof_detected].cell_id = (uint32_t) ret;
            q->candidates[q->current_nof_detected].cp = sync_get_cp(&q->sfind);
            q->candidates[q->current_nof_detected].peak = sync_get_last_peak_value(&q->sfind);
          }
          INFO
            ("[%3d/%3d]: Found peak at %4d, value %.3f, Cell_id: %d CP: %s\n",
              q->current_nof_detected, q->current_nof_total, peak_idx,
              q->candidates[q->current_nof_detected].peak, q->candidates[q->current_nof_detected].cell_id,
              lte_cp_string(q->candidates[q->current_nof_detected].cp));
          q->current_nof_detected++;
        }
      }
      q->current_nof_total++; 
      
      /* Decide cell ID and CP if we detected up to nof_frames_detected */
      if (q->current_nof_detected == q->nof_frames_detected) {
        decide_cell(q, found_cell);
        q->current_N_id_2++;
        q->current_nof_detected = q->current_nof_total = 0; 
        ret = CS_CELL_DETECTED;
      /* Or go to the next N_id_2 if we didn't detect the cell */
      } else if (q->current_nof_total == q->nof_frames_total) {
        q->current_N_id_2++; 
        q->current_nof_detected = q->current_nof_total = 0; 
        ret = CS_CELL_NOT_DETECTED; 
      }    
      if (q->current_N_id_2 == 3) {
        q->current_N_id_2 = 0; 
      }
    } 
  }

  return ret;
}
