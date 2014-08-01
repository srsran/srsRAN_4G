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


#include "liblte/phy/ue/ue_sync.h"

#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"

#define MAX_TIME_OFFSET 128
cf_t dummy[MAX_TIME_OFFSET];

#define CURRENT_FFTSIZE   lte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SF_LEN_RE(q->cell.nof_prb, q->cell.cp)

#define FIND_THRESHOLD          1.0
#define TRACK_THRESHOLD         0.2


int ue_sync_init(ue_sync_t *q, 
                 lte_cell_t cell,
                 int (recv_callback)(void*, void*, uint32_t),
                 void *stream_handler) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q                    != NULL && 
      stream_handler       != NULL && 
      lte_cell_isvalid(&cell)      &&
      recv_callback        != NULL)
  {
    ret = LIBLTE_ERROR;
    
    bzero(q, sizeof(ue_sync_t));

    ue_sync_reset(q);
    
    q->decode_sss_on_track = false; 
    q->stream = stream_handler;
    q->recv_callback = recv_callback;
    
    if(sync_init(&q->sfind, 5 * CURRENT_SFLEN, CURRENT_FFTSIZE)) {
      goto clean_exit;
    }
    if(sync_init(&q->strack, CURRENT_FFTSIZE, CURRENT_FFTSIZE)) {
      goto clean_exit;
    }
    
    sync_set_N_id_2(&q->sfind, cell.id%3);
    sync_set_threshold(&q->sfind, FIND_THRESHOLD);

    sync_set_N_id_2(&q->strack, cell.id%3);
    sync_set_threshold(&q->strack, TRACK_THRESHOLD);
    
    if (cfo_init(&q->cfocorr, CURRENT_SFLEN)) {
      fprintf(stderr, "Error initiating CFO\n");
      goto clean_exit;
    }
   
    q->input_buffer = vec_malloc(5 * CURRENT_SFLEN * sizeof(cf_t));
    if (!q->input_buffer) {
      perror("malloc");
      goto clean_exit;
    }
    
    ret = LIBLTE_SUCCESS;
  }
  
clean_exit:
  if (ret == LIBLTE_ERROR) {
    ue_sync_free(q);
  }
  return ret; 
}

void ue_sync_free(ue_sync_t *q) {
  if (q->input_buffer) {
    free(q->input_buffer);
  }
  cfo_free(&q->cfocorr);
  sync_free(&q->sfind);
  sync_free(&q->strack);
}

uint32_t ue_sync_peak_idx(ue_sync_t *q) {
  return q->peak_idx;
}

ue_sync_state_t ue_sync_get_state(ue_sync_t *q) {
  return q->state;
}
uint32_t ue_sync_get_sfidx(ue_sync_t *q) {
  return q->sf_idx;    
}

float ue_sync_get_cfo(ue_sync_t *q) {
  return 15000 * q->cur_cfo;
}

float ue_sync_get_sfo(ue_sync_t *q) {
  return 1000*q->mean_time_offset/5;
}

void ue_sync_decode_sss_on_track(ue_sync_t *q, bool enabled) {
  q->decode_sss_on_track = enabled; 
}


static int find_peak_ok(ue_sync_t *q) {
  int ret; 
  
  if (q->peak_idx < CURRENT_SFLEN) {
    /* Receive the rest of the next subframe */
    if (q->recv_callback(q->stream, &q->input_buffer[CURRENT_SFLEN], q->peak_idx+CURRENT_SFLEN/2) < 0) {
      return LIBLTE_ERROR;
    }
  }
  
  if (sync_sss_detected(&q->sfind)) {
    /* Get the subframe index (0 or 5) */
    q->sf_idx = sync_get_sf_idx(&q->sfind);
   
    /* Reset variables */ 
    q->frame_ok_cnt = 0;
    q->frame_no_cnt = 0;
    q->frame_total_cnt = 0;       

    /* Goto Tracking state */
    q->state = SF_TRACK;      
    ret = LIBLTE_SUCCESS;
  
    INFO("Found peak at %d, value %.3f, SF_idx: %d, Cell_id: %d CP: %s\n", 
        q->peak_idx, sync_get_peak_value(&q->sfind), q->sf_idx, q->cell.id, lte_cp_string(q->cell.cp));       
    
    if (q->peak_idx < CURRENT_SFLEN) {
      q->sf_idx++;
    }    
  } else {
    INFO("Found peak at %d, SSS not detected\n", q->peak_idx);
    ret = 0;
  }
  return ret;
}

int track_peak_ok(ue_sync_t *q, uint32_t track_idx) {
  int ret = LIBLTE_SUCCESS; 
  
   /* Make sure subframe idx is what we expect */
  if ((q->sf_idx != sync_get_sf_idx(&q->strack)) && q->decode_sss_on_track) {
    INFO("\nWarning: Expected SF idx %d but got %d!\n", 
          q->sf_idx, sync_get_sf_idx(&q->strack));
    q->sf_idx = sync_get_sf_idx(&q->strack);
  } else {
    q->time_offset = ((int) track_idx - (int) CURRENT_FFTSIZE); 

    /* If the PSS peak is beyond the frame (we sample too slowly), 
      discard the offseted samples to align next frame */
    if (q->time_offset > 0 && q->time_offset < MAX_TIME_OFFSET) {
      ret = q->recv_callback(q->stream, dummy, (uint32_t) q->time_offset);        
    } else {
      ret = LIBLTE_SUCCESS;
    }
    
    /* compute cumulative moving average CFO */
    q->cur_cfo = EXPAVERAGE(sync_get_cfo(&q->strack), q->cur_cfo, q->frame_ok_cnt);
    
    /* compute cumulative moving average time offset */
    q->mean_time_offset = (float) EXPAVERAGE((float) q->time_offset, q->mean_time_offset, q->frame_ok_cnt);

    q->peak_idx = CURRENT_SFLEN/2 + q->time_offset;  
    q->frame_ok_cnt++;
    q->frame_no_cnt = 0;
    
    
    if (ret >= LIBLTE_SUCCESS) {
      ret = LIBLTE_SUCCESS; 
    }
  }
  
  return ret;
}

int track_peak_no(ue_sync_t *q) {
  
  /* if we missed too many PSS go back to FIND */
  q->frame_no_cnt++; 
  if (q->frame_no_cnt >= TRACK_MAX_LOST) {
    printf("\n%d frames lost. Going back to FIND\n", (int) q->frame_no_cnt);
    q->state = SF_FIND;
  } else {
    INFO("Tracking peak not found. Peak %.3f, %d lost\n", 
         sync_get_peak_value(&q->strack), (int) q->frame_no_cnt);    
  }

  return LIBLTE_SUCCESS;
}

static int receive_samples(ue_sync_t *q) {
  uint32_t read_len; 
  
  /* A negative time offset means there are samples in our buffer for the next subframe, 
  because we are sampling too fast. 
  */
  if (q->time_offset < 0) {
    q->time_offset = -q->time_offset;
  }
  
  if (q->state == SF_FIND) {
    read_len = 5 * CURRENT_SFLEN; 
  } else {
    read_len = CURRENT_SFLEN; 
  }

  /* copy last part of the last subframe (use move since there could be overlapping) */
  memcpy(q->input_buffer, &q->input_buffer[read_len-q->time_offset], q->time_offset*sizeof(cf_t));
  
  /* Get 1 subframe from the USRP getting more samples and keeping the previous samples, if any */  
  if (q->recv_callback(q->stream, &q->input_buffer[q->time_offset], read_len - q->time_offset) < 0) {
    return LIBLTE_ERROR;
  }
  
  /* reset time offset */
  q->time_offset = 0;

  return LIBLTE_SUCCESS; 
}

int ue_sync_get_buffer(ue_sync_t *q, cf_t **sf_symbols) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  uint32_t track_idx; 
  struct timeval t[3];

  if (q               != NULL   &&
      sf_symbols      != NULL   &&
      q->input_buffer != NULL)
  {
    
    if (receive_samples(q)) {
      fprintf(stderr, "Error receiving samples\n");
      return -1;
    }
    
    switch (q->state) {
      case SF_FIND:        
        ret = sync_find(&q->sfind, q->input_buffer, 0, &q->peak_idx);
        if (ret < 0) {
          fprintf(stderr, "Error finding correlation peak (%d)\n", ret);
          return -1;
        }
        
        DEBUG("Find PAR=%.2f\n", sync_get_last_peak_value(&q->sfind));
        
        if (ret == 1) {
          ret = find_peak_ok(q);
        } else if (q->peak_idx != 0) {
          uint32_t rlen; 
          if (q->peak_idx < CURRENT_SFLEN/2) {
            rlen = CURRENT_SFLEN/2-q->peak_idx;
          } else {
            rlen = q->peak_idx;
          }
          if (q->recv_callback(q->stream, q->input_buffer, rlen) < 0) {
            return LIBLTE_ERROR;
          }
        }
      break;
      case SF_TRACK:
        ret = LIBLTE_SUCCESS;
        
        q->strack.sss_en = q->decode_sss_on_track; 
        
        q->sf_idx = (q->sf_idx + 1) % 10;

        DEBUG("TRACK: SF=%d FrameCNT: %d\n", q->sf_idx, q->frame_total_cnt);
        
        /* Every SF idx 0 and 5, find peak around known position q->peak_idx */
        if (q->sf_idx == 0 || q->sf_idx == 5) {

          #ifdef MEASURE_EXEC_TIME
          gettimeofday(&t[1], NULL);
          #endif
          
          track_idx = 0; 
          
          /* track pss around the middle of the subframe, where the PSS is */
          ret = sync_find(&q->strack, q->input_buffer, CURRENT_SFLEN/2-CURRENT_FFTSIZE, &track_idx);
          if (ret < 0) {
            fprintf(stderr, "Error tracking correlation peak\n");
            return -1;
          }
          
          #ifdef MEASURE_EXEC_TIME
          gettimeofday(&t[2], NULL);
          get_time_interval(t);
          q->mean_exec_time = (float) EXPAVERAGE((float) t[0].tv_usec, q->mean_exec_time, q->frame_total_cnt);
          #endif

          if (ret == 1) {
            ret = track_peak_ok(q, track_idx);
          } else {
            ret = track_peak_no(q); 
          }
          
          INFO("TRACK %3d: Value=%.3f SF=%d Track_idx=%d Offset=%d CFO: %f\n", 
                (int) q->frame_total_cnt, sync_get_peak_value(&q->strack), 
               q->sf_idx, track_idx, q->time_offset, sync_get_cfo(&q->strack));
          
          q->frame_total_cnt++; 
          
          if (ret == LIBLTE_ERROR) {
            fprintf(stderr, "Error processing tracking peak\n");
            q->state = SF_FIND; 
            return LIBLTE_SUCCESS;
          } 
        }
        
        /* Do CFO Correction and deliver the frame */
        cfo_correct(&q->cfocorr, q->input_buffer, q->input_buffer, -q->cur_cfo / CURRENT_FFTSIZE);         
        *sf_symbols = q->input_buffer;
        
        if (ret == LIBLTE_SUCCESS) {
          ret = 1;
        }
      break;
    }
  }  
  DEBUG("UE SYNC returns %d\n", ret);
  return ret; 
}

void ue_sync_reset(ue_sync_t *q) {
  q->state = SF_FIND;
    
  q->frame_ok_cnt = 0;
  q->frame_no_cnt = 0;
  q->frame_total_cnt = 0; 
  q->cur_cfo = 0;
  q->mean_time_offset = 0;
  q->time_offset = 0;
  #ifdef MEASURE_EXEC_TIME
  q->mean_exec_time = 0;
  #endif  
}

