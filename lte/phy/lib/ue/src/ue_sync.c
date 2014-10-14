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

#include "liblte/phy/io/filesource.h"
#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"


#define MAX_TIME_OFFSET 128
cf_t dummy[MAX_TIME_OFFSET];

#define TRACK_MAX_LOST          10
#define TRACK_FRAME_SIZE        32
#define FIND_NOF_AVG_FRAMES     2

#define FIND_THRESHOLD          1.2
#define TRACK_THRESHOLD         0.2

    if (filesource_init(&q->file_source, file_name, COMPLEX_FLOAT_BIN)) {
      fprintf(stderr, "Error opening file %s\n", file_name);
      goto clean_exit; 
    }
    
    q->input_buffer = vec_malloc(q->sf_len * sizeof(cf_t));
    if (!q->input_buffer) {
      perror("malloc");
      goto clean_exit;
    }
    
    ue_sync_reset(q);
    
    ret = LIBLTE_SUCCESS;
  }
clean_exit:
  if (ret == LIBLTE_ERROR) {
    ue_sync_free(q);
  }
  return ret; 
}

int ue_sync_init(ue_sync_t *q, 
                 lte_cell_t cell,
                 int (recv_callback)(void*, void*, uint32_t),
                 void *stream_handler) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q                                 != NULL && 
      stream_handler                    != NULL && 
      lte_nofprb_isvalid(cell.nof_prb)      &&
      recv_callback                     != NULL)
  {
    ret = LIBLTE_ERROR;
    
    bzero(q, sizeof(ue_sync_t));

    q->stream = stream_handler;
    q->recv_callback = recv_callback;
    q->cell = cell;
    q->fft_size = lte_symbol_sz(q->cell.nof_prb);
    q->sf_len = SF_LEN(q->fft_size);
    q->file_mode = false; 
    
    if (cell.id == 1000) {
      /* If the cell is unkown, decode SSS on track state */
      q->decode_sss_on_track = true;       
      
      /* If the cell is unkown, we search PSS/SSS in 5 ms */
      q->nof_recv_sf = 5; 
    } else {
      q->decode_sss_on_track = false; 
      
      /* If the cell is known, we work on a 1ms basis */
      q->nof_recv_sf = 1; 
    }

    q->frame_len = q->nof_recv_sf*q->sf_len;

    if(sync_init(&q->sfind, q->frame_len, q->fft_size)) {
      fprintf(stderr, "Error initiating sync find\n");
      goto clean_exit;
    }
    if(sync_init(&q->strack, TRACK_FRAME_SIZE, q->fft_size)) {
      fprintf(stderr, "Error initiating sync track\n");
      goto clean_exit;
    }
    
    if (cell.id == 1000) {
      /* If the cell id is unknown, enable CP detection on find */ 
      sync_cp_en(&q->sfind, true);      
      sync_cp_en(&q->strack, true); 
      
      /* Correct CFO in all cases because both states are called always. 
      */
      sync_correct_cfo(&q->sfind, true);    
      sync_correct_cfo(&q->strack, true); 
      
      sync_set_threshold(&q->sfind, 1.3);
      sync_set_em_alpha(&q->sfind, 0.01);
      q->nof_avg_find_frames = FIND_NOF_AVG_FRAMES; 
      sync_set_threshold(&q->strack, 1.2);
      
    } else {
      sync_set_N_id_2(&q->sfind, cell.id%3);
      sync_set_N_id_2(&q->strack, cell.id%3);
      q->sfind.cp = cell.cp;
      q->strack.cp = cell.cp;
      sync_cp_en(&q->sfind, false);      
      sync_cp_en(&q->strack, false);        

      /* In find phase and if the cell is known, do not average pss correlation
       * because we only capture 1 subframe and do not know where the peak is. 
       */
      sync_set_em_alpha(&q->sfind, 1);
      q->nof_avg_find_frames = 1; 
      sync_set_threshold(&q->sfind, 2.0);
      sync_set_threshold(&q->strack, 1.2);

      /* Correct CFO in the find state but not in the track state, since is called only 
       * 1 every 5 subframes. Will do it in the ue_sync_get_buffer() function. 
      */
      sync_correct_cfo(&q->sfind, true);    
      sync_correct_cfo(&q->strack, false); 
      
    }
      
    q->input_buffer = vec_malloc(2*q->frame_len * sizeof(cf_t));
    if (!q->input_buffer) {
      perror("malloc");
      goto clean_exit;
    }
    
    ue_sync_reset(q);
    
    ret = LIBLTE_SUCCESS;
  }
  
clean_exit:
  if (ret == LIBLTE_ERROR) {
    ue_sync_free(q);
  }
  return ret; 
}

uint32_t ue_sync_sf_len(ue_sync_t *q) {
  return q->frame_len;
}

void ue_sync_free(ue_sync_t *q) {
  if (q->input_buffer) {
    free(q->input_buffer);
  }
  if (!q->file_mode) {
    sync_free(&q->sfind);
    sync_free(&q->strack);    
  } else {
    filesource_free(&q->file_source);
  }
  bzero(q, sizeof(ue_sync_t));
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
  return 15000 * sync_get_cfo(&q->strack);
}

float ue_sync_get_sfo(ue_sync_t *q) {
  return 1000*q->mean_time_offset;
}

void ue_sync_decode_sss_on_track(ue_sync_t *q, bool enabled) {
  q->decode_sss_on_track = enabled; 
}

void ue_sync_set_N_id_2(ue_sync_t *q, uint32_t N_id_2) {
  if (!q->file_mode) {
    ue_sync_reset(q);
    sync_set_N_id_2(&q->strack, N_id_2);
    sync_set_N_id_2(&q->sfind, N_id_2);    
  }
}

static int find_peak_ok(ue_sync_t *q) {

  
  if (sync_sss_detected(&q->sfind)) {    
    /* Get the subframe index (0 or 5) */
    q->sf_idx = sync_get_sf_idx(&q->sfind) + q->nof_recv_sf;
  } else {
    INFO("Found peak at %d, SSS not detected\n", q->peak_idx);
  }
  
  q->frame_find_cnt++;  
  INFO("Found peak %d at %d, value %.3f, Cell_id: %d CP: %s\n", 
       q->frame_find_cnt, q->peak_idx, 
       sync_get_last_peak_value(&q->sfind), q->cell.id, lte_cp_string(q->cell.cp));       

  if (q->frame_find_cnt >= q->nof_avg_find_frames || q->peak_idx < 2*q->fft_size) {
    INFO("Realigning frame, reading %d samples\n", q->peak_idx+q->sf_len/2);
    /* Receive the rest of the subframe so that we are subframe aligned*/
    if (q->recv_callback(q->stream, q->input_buffer, q->peak_idx+q->sf_len/2) < 0) {
      return LIBLTE_ERROR;
    }
    
    /* Reset variables */ 
    q->frame_ok_cnt = 0;
    q->frame_no_cnt = 0;
    q->frame_total_cnt = 0;       
    q->frame_find_cnt = 0; 
    q->mean_time_offset = 0; 
    
    /* Set tracking CFO average to find CFO */
    q->strack.mean_cfo = q->sfind.mean_cfo;

    /* Goto Tracking state */
    q->state = SF_TRACK;            
  }
    
    
  return 0;
}

static int track_peak_ok(ue_sync_t *q, uint32_t track_idx) {
  
   /* Make sure subframe idx is what we expect */
  if ((q->sf_idx != sync_get_sf_idx(&q->strack)) && q->decode_sss_on_track) {
    INFO("Warning: Expected SF idx %d but got %d (%d,%g - %d,%g)!\n", 
          q->sf_idx, sync_get_sf_idx(&q->strack), q->strack.m0, q->strack.m1, q->strack.m0_value, q->strack.m1_value);
    /* FIXME: What should we do in this case? */
    q->sf_idx = sync_get_sf_idx(&q->strack);
    q->state = SF_TRACK; 
  } else {
    // Adjust time offset 
    q->time_offset = ((int) track_idx - (int) q->strack.frame_size/2 - (int) q->strack.fft_size); 
    
    if (q->time_offset) {
      INFO("Time offset adjustment: %d samples\n", q->time_offset);
    }
    
    /* compute cumulative moving average time offset */
    q->mean_time_offset = (float) VEC_CMA((float) q->time_offset, q->mean_time_offset, q->frame_total_cnt);

    /* If the PSS peak is beyond the frame (we sample too slowly), 
      discard the offseted samples to align next frame */
    if (q->time_offset > 0 && q->time_offset < MAX_TIME_OFFSET) {
      INFO("\nPositive time offset %d samples. Mean time offset %f.\n", q->time_offset, q->mean_time_offset);
      if (q->recv_callback(q->stream, dummy, (uint32_t) q->time_offset) < 0) {
        fprintf(stderr, "Error receiving from USRP\n");
        return LIBLTE_ERROR; 
      }
      q->time_offset = 0;
    } 
    
    /* compute cumulative moving average CFO */
    q->cur_cfo = VEC_CMA(sync_get_cfo(&q->strack), q->cur_cfo, q->frame_ok_cnt);
    /* compute cumulative moving average time offset */
    q->mean_time_offset = (float) VEC_CMA((float) q->time_offset, q->mean_time_offset, q->frame_ok_cnt);

    q->peak_idx = CURRENT_SFLEN/2 + q->time_offset;  
    q->frame_ok_cnt++;
    q->frame_no_cnt = 0;    
  }
  
  return 1;
}

static int track_peak_no(ue_sync_t *q) {
  
  /* if we missed too many PSS go back to FIND */
  q->frame_no_cnt++; 
  if (q->frame_no_cnt >= TRACK_MAX_LOST) {
    INFO("\n%d frames lost. Going back to FIND\n", (int) q->frame_no_cnt);
    q->state = SF_FIND;
  } else {
    INFO("Tracking peak not found. Peak %.3f, %d lost\n", 
         sync_get_last_peak_value(&q->strack), (int) q->frame_no_cnt);    
  }

  return 1;
}

static int receive_samples(ue_sync_t *q) {
  
  /* A negative time offset means there are samples in our buffer for the next subframe, 
  because we are sampling too fast. 
  */
  if (q->time_offset < 0) {
    q->time_offset = -q->time_offset;
  }

  /* Get N subframes from the USRP getting more samples and keeping the previous samples, if any */  
  if (q->recv_callback(q->stream, &q->input_buffer[q->time_offset], q->frame_len - q->time_offset) < 0) {
    return LIBLTE_ERROR;
  }
  
  /* reset time offset */
  q->time_offset = 0;

  return LIBLTE_SUCCESS; 
}

bool first_track = true; 

int ue_sync_get_buffer(ue_sync_t *q, cf_t **sf_symbols) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  uint32_t track_idx; 
  
  if (q               != NULL   &&
      sf_symbols      != NULL   &&
      q->input_buffer != NULL)
  {
    
    if (q->file_mode) {
      int n = filesource_read(&q->file_source, q->input_buffer, q->sf_len);
      if (n < 0) {
        fprintf(stderr, "Error reading input file\n");
        return LIBLTE_ERROR; 
      }
      if (n == 0) {
        filesource_seek(&q->file_source, 0);
        q->sf_idx = 9; 
        int n = filesource_read(&q->file_source, q->input_buffer, q->sf_len);
        if (n < 0) {
          fprintf(stderr, "Error reading input file\n");
          return LIBLTE_ERROR; 
        }
      }
      q->sf_idx++;
      if (q->sf_idx == 10) {
        q->sf_idx = 0;
      }
      INFO("Reading %d samples. sf_idx = %d\n", q->sf_len, q->sf_idx);
      ret = 1;
      *sf_symbols = q->input_buffer;
    } else {
      if (receive_samples(q)) {
        fprintf(stderr, "Error receiving samples\n");
        return LIBLTE_ERROR;
      }
      
      switch (q->state) {
        case SF_FIND:        
          ret = sync_find(&q->sfind, q->input_buffer, 0, &q->peak_idx);
          if (ret < 0) {
            fprintf(stderr, "Error finding correlation peak (%d)\n", ret);
            return LIBLTE_ERROR;
          }
          
          if (ret == 1) {
            ret = find_peak_ok(q);
          } 
        break;
        case SF_TRACK:
          ret = 1;
          
          sync_sss_en(&q->strack, q->decode_sss_on_track);
          
          q->sf_idx = (q->sf_idx + q->nof_recv_sf) % 10;

          /* Every SF idx 0 and 5, find peak around known position q->peak_idx */
          if (q->sf_idx == 0 || q->sf_idx == 5) {

            #ifdef MEASURE_EXEC_TIME
            struct timeval t[3];
            gettimeofday(&t[1], NULL);
            #endif
            
            track_idx = 0; 
            
            /* track PSS/SSS around the expected PSS position */
            ret = sync_find(&q->strack, q->input_buffer, 
                            q->frame_len - q->sf_len/2 - q->fft_size - q->strack.frame_size/2, 
                            &track_idx);
            if (ret < 0) {
              fprintf(stderr, "Error tracking correlation peak\n");
              return LIBLTE_ERROR;
            }
            
            #ifdef MEASURE_EXEC_TIME
            gettimeofday(&t[2], NULL);
            get_time_interval(t);
            q->mean_exec_time = (float) VEC_CMA((float) t[0].tv_usec, q->mean_exec_time, q->frame_total_cnt);
            #endif

            if (ret == 1) {
              ret = track_peak_ok(q, track_idx);
            } else {
              ret = track_peak_no(q); 
            }
            if (ret == LIBLTE_ERROR) {
              fprintf(stderr, "Error processing tracking peak\n");
              q->state = SF_FIND; 
              return LIBLTE_SUCCESS;
            } 
                      
            q->frame_total_cnt++;       
          }
          
          #ifdef MEASURE_EXEC_TIME
          gettimeofday(&t[2], NULL);
          get_time_interval(t);
          q->mean_exec_time = (float) VEC_CMA((float) t[0].tv_usec, q->mean_exec_time, q->frame_total_cnt);
          #endif

          if (ret == 1) {
            ret = track_peak_ok(q, track_idx);
          } else {
            ret = track_peak_no(q); 
          }
          *sf_symbols = q->input_buffer;
          
        break;
      }
      
    }
  }  
  return ret; 
}

void ue_sync_reset(ue_sync_t *q) {

  if (!q->file_mode) {
    sync_reset(&q->strack);
  } else {
    q->sf_idx = 9;
  }
  q->state = SF_FIND;
  q->frame_ok_cnt = 0;
  q->frame_no_cnt = 0;
  q->frame_total_cnt = 0; 
  q->mean_time_offset = 0.0;
  q->time_offset = 0;
  q->frame_find_cnt = 0; 
  #ifdef MEASURE_EXEC_TIME
  q->mean_exec_time = 0;
  #endif      
}

