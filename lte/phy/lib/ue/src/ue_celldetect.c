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

#define CS_CELL_DETECT_THRESHOLD  1.2

#define CS_SFLEN     5*SF_LEN(CS_FFTSIZE)

int ue_celldetect_init(ue_celldetect_t * q) {
  return ue_celldetect_init_max(q, CS_DEFAULT_MAXFRAMES_TOTAL); 
}

int ue_celldetect_init_max(ue_celldetect_t * q, uint32_t max_frames_total) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = LIBLTE_ERROR;

    bzero(q, sizeof(ue_celldetect_t));

    q->candidates = calloc(sizeof(ue_celldetect_result_t), max_frames_total);
    if (!q->candidates) {
      perror("malloc");
      goto clean_exit; 
    }
    if (sync_init(&q->sfind, CS_SFLEN, CS_FFTSIZE)) {
      goto clean_exit;
    }
    q->mode_ntimes = calloc(sizeof(uint32_t), max_frames_total);
    if (!q->mode_ntimes) {
      perror("malloc");
      goto clean_exit;  
    }
    q->mode_counted = calloc(sizeof(uint8_t), max_frames_total);
    if (!q->mode_counted) {
      perror("malloc");
      goto clean_exit;  
    }

    /* Accept all peaks because search space is 5 ms and there is always a peak */
    sync_set_threshold(&q->sfind, 1.0);
    sync_sss_en(&q->sfind, true);
    sync_set_sss_algorithm(&q->sfind, SSS_PARTIAL_3);
    sync_set_em_alpha(&q->sfind, 0.01);
    
    q->max_frames_total = max_frames_total;
    q->nof_frames_total = CS_DEFAULT_NOFFRAMES_TOTAL; 

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
  
  bzero(q, sizeof(ue_celldetect_t));

}

void ue_celldetect_reset(ue_celldetect_t * q)
{
  q->current_nof_detected = 0; 
  q->current_nof_total = 0; 
}

void ue_celldetect_set_threshold(ue_celldetect_t * q, float threshold)
{
  q->detect_threshold = threshold;
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

/* Decide the most likely cell based on the mode */
void ue_celldetect_get_cell(ue_celldetect_t * q, ue_celldetect_result_t *found_cell)
{
  uint32_t i, j;
  
  if (!q->current_nof_detected) {
    return; 
  }
  
  bzero(q->mode_counted, q->current_nof_detected);
  bzero(q->mode_ntimes, sizeof(uint32_t) * q->current_nof_detected);
  
  /* First find mode of CELL IDs */
  for (i = 0; i < q->current_nof_detected; i++) {
    uint32_t cnt = 1;
    for (j=i+1;j<q->current_nof_detected;j++) {
      if (q->candidates[j].cell_id == q->candidates[i].cell_id && !q->mode_counted[j]) {
        q->mode_counted[j]=1;
        cnt++;
      }
    }
    q->mode_ntimes[i] = cnt; 
  }
  uint32_t max_times=0, mode_pos=0; 
  for (i=0;i<q->current_nof_detected;i++) {
    if (q->mode_ntimes[i] > max_times) {
      max_times = q->mode_ntimes[i];
      mode_pos = i;
    }
  }
  found_cell->cell_id = q->candidates[mode_pos].cell_id;
  /* Now in all these cell IDs, find most frequent CP */
  uint32_t nof_normal = 0;
  for (i=0;i<q->current_nof_detected;i++) {
    if (q->candidates[i].cell_id == found_cell->cell_id) {
      if (CP_ISNORM(q->candidates[i].cp)) {
        nof_normal++;
      } 
    }
  }
  if (nof_normal > q->mode_ntimes[mode_pos]/2) {
    found_cell->cp = CPNORM;
  } else {
    found_cell->cp = CPEXT; 
  }
  found_cell->mode = (float) q->mode_ntimes[mode_pos]/q->current_nof_detected;  
  found_cell->peak = q->candidates[q->current_nof_detected-1].peak; 
  q->current_nof_detected = q->current_nof_total = 0; 
}

int ue_celldetect_set_N_id_2(ue_celldetect_t *q, uint32_t N_id_2) {
  return sync_set_N_id_2(&q->sfind, N_id_2); 
}

int ue_celldetect_scan(ue_celldetect_t * q, 
                       cf_t *signal, 
                       uint32_t nsamples)
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  uint32_t peak_idx;
  uint32_t nof_input_frames; 


  if (q                 != NULL &&
      signal            != NULL && 
      nsamples          >= CS_FLEN) 
  {
    ret = LIBLTE_SUCCESS; 
    
    if (nsamples % CS_FLEN) {
      printf("Warning: nsamples must be a multiple of %d. Some samples will be ignored\n", CS_FLEN);
      nsamples = (nsamples/CS_FLEN) * CS_FLEN;
    }
    nof_input_frames = nsamples/CS_FLEN; 
    
    for (uint32_t nf=0;nf<nof_input_frames;nf++) {

      INFO("[%3d/%3d]: Searching cells with N_id_2=%d. %d frames\n", 
           q->current_nof_detected, q->current_nof_total, q->sfind.N_id_2, nof_input_frames);

      /* Find peak and cell id */
      ret = sync_find(&q->sfind, &signal[nf*CS_FLEN], 0, &peak_idx);
      if (ret < 0) {
        fprintf(stderr, "Error finding correlation peak (%d)\n", ret);
        return LIBLTE_ERROR;
      }

      /* Process the peak result */
      if (ret == 1) {
        if (sync_sss_detected(&q->sfind)) {
          ret = sync_get_cell_id(&q->sfind);
          if (ret >= 0) {
            /* Save cell id, cp and peak */
            q->candidates[q->current_nof_detected].cell_id = (uint32_t) ret;
            q->candidates[q->current_nof_detected].cp = sync_get_cp(&q->sfind);
            q->candidates[q->current_nof_detected].peak = sync_get_peak_value(&q->sfind);
          }
          INFO
            ("[%3d/%3d]: Found peak at %4d, value %.3f, Cell_id: %d CP: %s\n",
              q->current_nof_detected, q->current_nof_total, peak_idx,
              q->candidates[q->current_nof_detected].peak, q->candidates[q->current_nof_detected].cell_id,
              lte_cp_string(q->candidates[q->current_nof_detected].cp));
          q->current_nof_detected++;
        } else {
          /* If peak position does not allow to read SSS, return error -3 */
          return CS_FRAME_UNALIGNED; 
        }
      }
      q->current_nof_total++; 
      
      /* Decide cell ID and CP if we detected up to nof_frames_detected */
      if (sync_get_peak_value(&q->sfind) > q->detect_threshold) {
        ret = CS_CELL_DETECTED;
      } else if (q->current_nof_total == q->nof_frames_total) {
        if (sync_get_peak_value(&q->sfind) > CS_CELL_DETECT_THRESHOLD) {
          ret = CS_CELL_DETECTED;           
        } else {
          ret = CS_CELL_NOT_DETECTED;           
          q->current_nof_detected = q->current_nof_total = 0; 
        }
      } else {
        ret = 0;
      }
    } 
  }

  return ret;
}
