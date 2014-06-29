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

#include "liblte/phy/resampling/decim.h"
#include "liblte/phy/resampling/resample_arb.h"
#include "liblte/phy/utils/debug.h"
#include "liblte/phy/sync/sync_frame.h"


int sync_frame_init(sync_frame_t *q, int downsampling) {
  int ret = -1;
  bzero(q, sizeof(sync_frame_t));

  if(sync_init(&q->s, SYNC_SF_LEN)) {
    goto clean_exit;
  }
  sync_pss_det_peak_to_avg(&q->s);

  if (cfo_init(&q->cfocorr, SYNC_SF_LEN * downsampling)) {
    fprintf(stderr, "Error initiating CFO\n");
    goto clean_exit;
  }
  
  q->input_buffer = malloc(2 * SYNC_SF_LEN * downsampling * sizeof(cf_t));
  if (!q->input_buffer) {
    perror("malloc");
    goto clean_exit;
  }
  
  q->input_downsampled = malloc(SYNC_SF_LEN * sizeof(cf_t));
  if (!q->input_downsampled) {
    perror("malloc");
    goto clean_exit;
  }

  resample_arb_init(&q->resample, (float) 1/downsampling);  
  
  q->downsampling = downsampling;
  sync_frame_reset(q);
  ret = 0;
  
clean_exit:
  if (ret == -1) {
    sync_frame_free(q);
  }
  return ret; 
}

void sync_frame_free(sync_frame_t *q) {
  if (q->input_buffer) {
    free(q->input_buffer);
  }
  if (q->input_downsampled) {
    free(q->input_downsampled);
  }

  cfo_free(&q->cfocorr);
  sync_free(&q->s);
}

void sync_frame_run(sync_frame_t *q, cf_t *input) {
  int track_idx; 
  
  switch (q->state) {
    
  case SF_FIND:
    q->peak_idx = sync_find(&q->s, input);
    q->cell_id = sync_get_cell_id(&q->s);
      
    INFO("FIND %3d:\tPAR=%.2f\n", (int) q->frame_cnt, sync_get_peak_to_avg(&q->s));
    
    if (q->peak_idx != -1 && q->cell_id != -1) {

      /* Get the subframe index (0 or 5) */
      q->sf_idx = sync_get_slot_id(&q->s)/2;

      /* Reset variables */ 
      q->last_found = 0;
      q->timeoffset = 0;
      q->frame_cnt = 0;

      /* Goto Tracking state */
      q->state = SF_TRACK; 
    }  
    break;      
  case SF_TRACK:

    q->sf_idx = (q->sf_idx + 1) % 10;
    
    /* Every SF idx 0 and 5, find peak around known position q->peak_idx */
    if (q->sf_idx != 0 && q->sf_idx != 5) {
      break;
    }
        
    track_idx = sync_track(&q->s, &input[q->peak_idx - TRACK_LEN]);

    INFO("TRACK %3d: SF=%d. Previous idx is %d New Offset is %d\n", 
         (int) q->frame_cnt, q->sf_idx, q->peak_idx, track_idx - TRACK_LEN);

    if (track_idx != -1) {
        INFO("Expected SF idx %d but got %d. Going back to FIND\n", q->sf_idx,
             sync_get_slot_id(&q->s)/2);
      
      /* Make sure subframe idx is what we expect */
      if (q->sf_idx != sync_get_slot_id(&q->s)/2) {
        INFO("Expected SF idx %d but got %d. Going back to FIND\n", q->sf_idx,
             sync_get_slot_id(&q->s)/2);
        q->state = SF_FIND;
      }
      
      /* compute cumulative moving average CFO */
      q->cur_cfo = (sync_get_cfo(&q->s) + q->frame_cnt * q->cur_cfo) / (q->frame_cnt + 1);
      
      /* compute cumulative moving average time offset */
      q->timeoffset = (float) (track_idx - TRACK_LEN + q->timeoffset * q->frame_cnt)
          / (q->frame_cnt + 1);
      
      q->last_found = q->frame_cnt;
      q->peak_idx = (q->peak_idx + track_idx - TRACK_LEN) % SYNC_SF_LEN;  
      
      if (q->peak_idx < 0) {
        INFO("PSS lost (peak_idx=%d). Going back to FIND\n", q->peak_idx);
        q->state = SF_FIND;        
      }
    } else {
      /* if sync not found, adjust time offset with the averaged value */
      q->peak_idx = (q->peak_idx + (int) q->timeoffset) % SYNC_SF_LEN;

      /* if we missed too many PSS go back to FIND */
      if (q->frame_cnt - q->last_found > TRACK_MAX_LOST) {
        INFO("%d frames lost. Going back to FIND", (int) q->frame_cnt - q->last_found);
        q->state = SF_FIND;
      }
    }
    q->frame_cnt++;
    break;
  }
}

void sync_frame_set_threshold(sync_frame_t *q, float threshold) {
  sync_set_threshold(&q->s, threshold);
}

int sync_frame_cell_id(sync_frame_t *q) {
  return q->cell_id;
}

int sync_frame_sfidx(sync_frame_t *q) {
  return q->sf_idx;
}


int sync_frame_push(sync_frame_t *q, cf_t *input, cf_t *output) {
  int retval = 0; 
  int frame_start;
  cf_t *input_ds;
  int sf_len; 
  
  if (q->downsampling == 1) {
    input_ds = input;
  } else {
    //resample_arb_compute(&q->resample, input, q->input_downsampled, SYNC_SF_LEN * q->downsampling);
    decim_c(input, q->input_downsampled, q->downsampling, SYNC_SF_LEN * q->downsampling);
    input_ds = q->input_downsampled;
  }
  
  sync_frame_run(q, input_ds);
  
  sf_len = q->downsampling * SYNC_SF_LEN;
  
  if (q->state == SF_FIND) {
    memcpy(q->input_buffer, input, sf_len * sizeof(cf_t));
  } else {
    frame_start = q->downsampling * q->peak_idx - sf_len/2;
    
    DEBUG("Peak_idx=%d, frame_start=%d cfo=%.3f\n",q->peak_idx, 
        frame_start, q->cur_cfo);
    
    if (frame_start > 0) {
      if (q->fb_wp) {
        memcpy(&q->input_buffer[(sf_len - frame_start)], input, frame_start * sizeof(cf_t));
        memcpy(output, q->input_buffer, sf_len * sizeof(cf_t));
        retval = 1;
      } 
      memcpy(q->input_buffer, &input[frame_start], (sf_len - frame_start) * sizeof(cf_t));
      q->fb_wp = true;
    } else {
      memcpy(output, &q->input_buffer[sf_len + frame_start], (-frame_start) * sizeof(cf_t));
      memcpy(&output[-frame_start], input, (sf_len + frame_start) * sizeof(cf_t));
      memcpy(&q->input_buffer[sf_len + frame_start], &input[sf_len + frame_start], (-frame_start) * sizeof(cf_t));
      retval = 1;
    }
  }

  /* Frequency Synchronization */
  if (retval) {
    cfo_correct(&q->cfocorr, output, -q->cur_cfo / 128);   
  }
  
  if (!retval) {
    DEBUG("Frame Buffered\n",0);
  }
  
  return retval; 
}

void sync_frame_reset(sync_frame_t *q) {
  q->state = SF_FIND;
  q->frame_cnt = 0;
  q->fb_wp = false; 
  q->cur_cfo = 0;
}

