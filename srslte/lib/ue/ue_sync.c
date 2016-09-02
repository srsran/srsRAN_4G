/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <unistd.h>


#include "srslte/ue/ue_sync.h"

#include "srslte/io/filesource.h"
#include "srslte/utils/debug.h"
#include "srslte/utils/vector.h"


#define MAX_TIME_OFFSET 128
cf_t dummy[MAX_TIME_OFFSET];

#define TRACK_MAX_LOST          4
#define TRACK_FRAME_SIZE        32
#define FIND_NOF_AVG_FRAMES     4
#define DEFAULT_SAMPLE_OFFSET_CORRECT_PERIOD  0
#define DEFAULT_SFO_EMA_COEFF                 0.1

cf_t dummy_offset_buffer[1024*1024];

int srslte_ue_sync_init_file(srslte_ue_sync_t *q, uint32_t nof_prb, char *file_name, int offset_time, float offset_freq) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q                   != NULL && 
      file_name           != NULL && 
      srslte_nofprb_isvalid(nof_prb))
  {
    ret = SRSLTE_ERROR;
    bzero(q, sizeof(srslte_ue_sync_t));
    q->file_mode = true; 
    q->sf_len = SRSLTE_SF_LEN(srslte_symbol_sz(nof_prb));
    q->file_cfo = -offset_freq; 
    q->correct_cfo = true; 
    q->fft_size = srslte_symbol_sz(nof_prb);
    
    if (srslte_cfo_init(&q->file_cfo_correct, 2*q->sf_len)) {
      fprintf(stderr, "Error initiating CFO\n");
      goto clean_exit; 
    }
    
    if (srslte_filesource_init(&q->file_source, file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
      fprintf(stderr, "Error opening file %s\n", file_name);
      goto clean_exit; 
    }
    
    q->input_buffer = srslte_vec_malloc(2 * q->sf_len * sizeof(cf_t));
    if (!q->input_buffer) {
      perror("malloc");
      goto clean_exit;
    }
    
    INFO("Offseting input file by %d samples and %.1f kHz\n", offset_time, offset_freq/1000);
    
    srslte_filesource_read(&q->file_source, dummy_offset_buffer, offset_time);
    srslte_ue_sync_reset(q);
    
    ret = SRSLTE_SUCCESS;
  }
clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_sync_free(q);
  }
  return ret; 
}

int srslte_ue_sync_start_agc(srslte_ue_sync_t *q, double (set_gain_callback)(void*, double), float init_gain_value) {
  uint32_t nframes; 
  if (q->nof_recv_sf == 1) {
    nframes = 10; 
  } else {
    nframes = 0; 
  }
  int n = srslte_agc_init_uhd(&q->agc, SRSLTE_AGC_MODE_PEAK_AMPLITUDE, nframes, set_gain_callback, q->stream); 
  q->do_agc = n==0?true:false;
  if (q->do_agc) {
    srslte_agc_set_gain(&q->agc, init_gain_value);
  }
  return n; 
}

int srslte_ue_sync_init(srslte_ue_sync_t *q, 
                 srslte_cell_t cell,
                 int (recv_callback)(void*, void*, uint32_t,srslte_timestamp_t*),
                 void *stream_handler) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (q                                 != NULL && 
      stream_handler                    != NULL && 
      srslte_nofprb_isvalid(cell.nof_prb)      &&
      recv_callback                     != NULL)
  {
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(srslte_ue_sync_t));

    q->stream = stream_handler;
    q->recv_callback = recv_callback;
    q->cell = cell;
    q->fft_size = srslte_symbol_sz(q->cell.nof_prb);
    q->sf_len = SRSLTE_SF_LEN(q->fft_size);
    q->file_mode = false; 
    q->correct_cfo = true; 
    q->agc_period = 0; 
    q->sample_offset_correct_period = DEFAULT_SAMPLE_OFFSET_CORRECT_PERIOD; 
    q->sfo_ema                      = DEFAULT_SFO_EMA_COEFF; 
    
    if (cell.id == 1000) {
      
      /* If the cell is unkown, we search PSS/SSS in 5 ms */
      q->nof_recv_sf = 5; 

      q->decode_sss_on_track = true; 

    } else {
      
      /* If the cell is known, we work on a 1ms basis */
      q->nof_recv_sf = 1; 

      q->decode_sss_on_track = true; 
    }

    q->frame_len = q->nof_recv_sf*q->sf_len;

    if(srslte_sync_init(&q->sfind, q->frame_len, q->frame_len, q->fft_size)) {
      fprintf(stderr, "Error initiating sync find\n");
      goto clean_exit;
    }
    if (cell.id == 1000) {
      if(srslte_sync_init(&q->strack, q->frame_len, TRACK_FRAME_SIZE, q->fft_size)) {
        fprintf(stderr, "Error initiating sync track\n");
        goto clean_exit;
      }      
    } else {
      if(srslte_sync_init(&q->strack, q->frame_len, SRSLTE_CP_LEN_NORM(1,q->fft_size), q->fft_size)) {
        fprintf(stderr, "Error initiating sync track\n");
        goto clean_exit;
      }
    }
    
    if (cell.id == 1000) {
      /* If the cell id is unknown, enable CP detection on find */ 
      // FIXME: CP detection not working very well. Not supporting Extended CP right now
      srslte_sync_cp_en(&q->sfind, false);      
      srslte_sync_cp_en(&q->strack, false); 

      srslte_sync_set_cfo_ema_alpha(&q->sfind, 0.8);
      srslte_sync_set_cfo_ema_alpha(&q->strack, 0.1);

      srslte_sync_cfo_i_detec_en(&q->sfind, false); 

      q->nof_avg_find_frames = FIND_NOF_AVG_FRAMES; 
      srslte_sync_set_threshold(&q->sfind, 2.0);
      srslte_sync_set_threshold(&q->strack, 1.2);
      
    } else {
      srslte_sync_set_N_id_2(&q->sfind, cell.id%3);
      srslte_sync_set_N_id_2(&q->strack, cell.id%3);
      q->sfind.cp = cell.cp;
      q->strack.cp = cell.cp;
      srslte_sync_cp_en(&q->sfind, false);      
      srslte_sync_cp_en(&q->strack, false);        

      srslte_sync_cfo_i_detec_en(&q->sfind, false); 
      
      srslte_sync_set_cfo_ema_alpha(&q->sfind, 0.1);
      srslte_sync_set_cfo_ema_alpha(&q->strack, 0.1);

      /* In find phase and if the cell is known, do not average pss correlation
       * because we only capture 1 subframe and do not know where the peak is. 
       */
      q->nof_avg_find_frames = 1; 
      srslte_sync_set_em_alpha(&q->sfind, 1);
      srslte_sync_set_threshold(&q->sfind, 3.0);
      
      srslte_sync_set_em_alpha(&q->strack, 0.2);
      srslte_sync_set_threshold(&q->strack, 1.2);

    }
      
    /* FIXME: Go for zerocopy only and eliminate this allocation */
    q->input_buffer = srslte_vec_malloc(2*q->frame_len * sizeof(cf_t));
    if (!q->input_buffer) {
      perror("malloc");
      goto clean_exit;
    }
    
    srslte_ue_sync_reset(q);
    
    ret = SRSLTE_SUCCESS;
  }
  
clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_sync_free(q);
  }
  return ret; 
}

uint32_t srslte_ue_sync_sf_len(srslte_ue_sync_t *q) {
  return q->frame_len;
}

void srslte_ue_sync_free(srslte_ue_sync_t *q) {
  if (q->input_buffer) {
    free(q->input_buffer);
  }
  if (q->do_agc) {
    srslte_agc_free(&q->agc);
  }
  if (!q->file_mode) {
    srslte_sync_free(&q->sfind);
    srslte_sync_free(&q->strack);    
  } else {
    srslte_filesource_free(&q->file_source);
  }
  bzero(q, sizeof(srslte_ue_sync_t));
}

void srslte_ue_sync_get_last_timestamp(srslte_ue_sync_t *q, srslte_timestamp_t *timestamp) {
  memcpy(timestamp, &q->last_timestamp, sizeof(srslte_timestamp_t));
}

uint32_t srslte_ue_sync_peak_idx(srslte_ue_sync_t *q) {
  return q->peak_idx;
}

srslte_ue_sync_state_t srslte_ue_sync_get_state(srslte_ue_sync_t *q) {
  return q->state;
}
uint32_t srslte_ue_sync_get_sfidx(srslte_ue_sync_t *q) {
  return q->sf_idx;    
}

void srslte_ue_sync_cfo_i_detec_en(srslte_ue_sync_t *q, bool enable) {
  srslte_sync_cfo_i_detec_en(&q->strack, enable);
  srslte_sync_cfo_i_detec_en(&q->sfind, enable);  
}

float srslte_ue_sync_get_cfo(srslte_ue_sync_t *q) {
  return 15000 * srslte_sync_get_cfo(&q->strack);
}

void srslte_ue_sync_set_cfo(srslte_ue_sync_t *q, float cfo) {  
  srslte_sync_set_cfo(&q->sfind, cfo/15000);
  srslte_sync_set_cfo(&q->strack, cfo/15000);
}

float srslte_ue_sync_get_sfo(srslte_ue_sync_t *q) {
  return q->mean_sfo/5e-3;
}

int srslte_ue_sync_get_last_sample_offset(srslte_ue_sync_t *q) {
  return q->last_sample_offset; 
}

void srslte_ue_sync_set_sample_offset_correct_period(srslte_ue_sync_t *q, uint32_t nof_subframes) {
  q->sample_offset_correct_period = nof_subframes; 
}

void srslte_ue_sync_set_sfo_ema(srslte_ue_sync_t *q, float ema_coefficient) {
  q->sfo_ema = ema_coefficient; 
}

void srslte_ue_sync_decode_sss_on_track(srslte_ue_sync_t *q, bool enabled) {
  q->decode_sss_on_track = enabled; 
}

void srslte_ue_sync_set_N_id_2(srslte_ue_sync_t *q, uint32_t N_id_2) {
  if (!q->file_mode) {
    srslte_ue_sync_reset(q);
    srslte_sync_set_N_id_2(&q->strack, N_id_2);
    srslte_sync_set_N_id_2(&q->sfind, N_id_2);    
  }
}

void srslte_ue_sync_set_agc_period(srslte_ue_sync_t *q, uint32_t period) {
  q->agc_period = period; 
}

static int find_peak_ok(srslte_ue_sync_t *q, cf_t *input_buffer) {

  
  if (srslte_sync_sss_detected(&q->sfind)) {    
    /* Get the subframe index (0 or 5) */
    q->sf_idx = srslte_sync_get_sf_idx(&q->sfind) + q->nof_recv_sf;
  } else {
    DEBUG("Found peak at %d, SSS not detected\n", q->peak_idx);
  }
  
  q->frame_find_cnt++;  
  DEBUG("Found peak %d at %d, value %.3f, Cell_id: %d CP: %s\n", 
       q->frame_find_cnt, q->peak_idx, 
       srslte_sync_get_last_peak_value(&q->sfind), q->cell.id, srslte_cp_string(q->cell.cp));       

  if (q->frame_find_cnt >= q->nof_avg_find_frames || q->peak_idx < 2*q->fft_size) {
    INFO("Realigning frame, reading %d samples\n", q->peak_idx+q->sf_len/2);
    /* Receive the rest of the subframe so that we are subframe aligned */
    if (q->recv_callback(q->stream, input_buffer, q->peak_idx+q->sf_len/2, &q->last_timestamp) < 0) {
      return SRSLTE_ERROR;
    }

    /* Reset variables */ 
    q->frame_ok_cnt = 0;
    q->frame_no_cnt = 0;
    q->frame_total_cnt = 0;       
    q->frame_find_cnt = 0; 
    q->mean_sample_offset = 0; 
    
    /* Goto Tracking state */
    q->state = SF_TRACK;     
    
    /* Initialize track state CFO */
    q->strack.mean_cfo = q->sfind.mean_cfo;
    q->strack.cfo_i    = q->sfind.cfo_i; 
  }
    
  return 0;
}

static int track_peak_ok(srslte_ue_sync_t *q, uint32_t track_idx) {
  
   /* Make sure subframe idx is what we expect */
  if ((q->sf_idx != srslte_sync_get_sf_idx(&q->strack)) && 
       q->decode_sss_on_track                           && 
       srslte_sync_sss_detected(&q->strack)) 
  {
    INFO("Warning: Expected SF idx %d but got %d! (%d frames)\n", 
           q->sf_idx, srslte_sync_get_sf_idx(&q->strack), q->frame_no_cnt);
    q->frame_no_cnt++;
    if (q->frame_no_cnt >= TRACK_MAX_LOST) {
      INFO("\n%d frames lost. Going back to FIND\n", (int) q->frame_no_cnt);
      q->state = SF_FIND;
    }
  } else {
    q->frame_no_cnt = 0;    
  }
  
  // Get sampling time offset 
  q->last_sample_offset = ((int) track_idx - (int) q->strack.max_offset/2 - (int) q->strack.fft_size); 
  
  // Adjust sampling time every q->sample_offset_correct_period subframes
  uint32_t frame_idx = 0; 
  if (q->sample_offset_correct_period) {
    frame_idx = q->frame_ok_cnt%q->sample_offset_correct_period;
    q->mean_sample_offset += (float) q->last_sample_offset/q->sample_offset_correct_period;
  } else {    
    q->mean_sample_offset = q->last_sample_offset; 
  }

  // Compute cumulative moving average time offset */
  if (!frame_idx) {
    // Adjust RF sampling time based on the mean sampling offset
    q->next_rf_sample_offset = (int) round(q->mean_sample_offset);
    
    // Reset PSS averaging if correcting every a period longer than 1
    if (q->sample_offset_correct_period > 1) {
      srslte_sync_reset(&q->strack);
    }
    
    // Compute SFO based on mean sample offset 
    if (q->sample_offset_correct_period) {
      q->mean_sample_offset /= q->sample_offset_correct_period;
    }
    q->mean_sfo = SRSLTE_VEC_EMA(q->mean_sample_offset, q->mean_sfo, q->sfo_ema);

    if (q->next_rf_sample_offset) {
      INFO("Time offset adjustment: %d samples (%.2f), mean SFO: %.2f Hz, %.5f samples/5-sf, ema=%f, length=%d\n", 
           q->next_rf_sample_offset, q->mean_sample_offset,
           srslte_ue_sync_get_sfo(q), 
           q->mean_sfo, q->sfo_ema, q->sample_offset_correct_period);    
    }
    q->mean_sample_offset = 0; 
  }

  /* If the PSS peak is beyond the frame (we sample too slowly), 
    discard the offseted samples to align next frame */
  if (q->next_rf_sample_offset > 0 && q->next_rf_sample_offset < MAX_TIME_OFFSET) {
    DEBUG("Positive time offset %d samples.\n", q->next_rf_sample_offset);
    if (q->recv_callback(q->stream, dummy, (uint32_t) q->next_rf_sample_offset, &q->last_timestamp) < 0) {
      fprintf(stderr, "Error receiving from USRP\n");
      return SRSLTE_ERROR; 
    }
    q->next_rf_sample_offset = 0;
  } 
  
  q->peak_idx = q->sf_len/2 + q->last_sample_offset;  
  q->frame_ok_cnt++;
  
  return 1;
}

static int track_peak_no(srslte_ue_sync_t *q) {
  
  /* if we missed too many PSS go back to FIND and consider this frame unsynchronized */
  q->frame_no_cnt++; 
  if (q->frame_no_cnt >= TRACK_MAX_LOST) {
    INFO("\n%d frames lost. Going back to FIND\n", (int) q->frame_no_cnt);
    q->state = SF_FIND;
    return 0; 
  } else {
    INFO("Tracking peak not found. Peak %.3f, %d lost\n", 
         srslte_sync_get_last_peak_value(&q->strack), (int) q->frame_no_cnt);    
    /*
    printf("Saving files: pss_corr (%d), input (%d)\n", q->strack.pss.frame_size, SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
    srslte_vec_save_file("pss_corr", q->strack.pss.conv_output_avg, q->strack.pss.frame_size*sizeof(float));
    srslte_vec_save_file("input", q->input_buffer, SRSLTE_SF_LEN_PRB(q->cell.nof_prb)*sizeof(cf_t));
    exit(-1);
    */
    return 1;
  }

}

static int receive_samples(srslte_ue_sync_t *q, cf_t *input_buffer) {
  
  /* A negative time offset means there are samples in our buffer for the next subframe, 
  because we are sampling too fast. 
  */
  if (q->next_rf_sample_offset < 0) {
    q->next_rf_sample_offset = -q->next_rf_sample_offset;
  }

  /* Get N subframes from the USRP getting more samples and keeping the previous samples, if any */  
  if (q->recv_callback(q->stream, &input_buffer[q->next_rf_sample_offset], q->frame_len - q->next_rf_sample_offset, &q->last_timestamp) < 0) {
    return SRSLTE_ERROR;
  }
  
  /* reset time offset */
  q->next_rf_sample_offset = 0;

  return SRSLTE_SUCCESS; 
}

bool first_track = true; 

int srslte_ue_sync_get_buffer(srslte_ue_sync_t *q, cf_t **sf_symbols) {
  int ret = srslte_ue_sync_zerocopy(q, q->input_buffer);
  if (sf_symbols) {
    *sf_symbols = q->input_buffer;
  }
  return ret; 

}

/* Returns 1 if the subframe is synchronized in time, 0 otherwise */
int srslte_ue_sync_zerocopy(srslte_ue_sync_t *q, cf_t *input_buffer) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  uint32_t track_idx; 
  
  if (q            != NULL   &&
      input_buffer != NULL)
  {
    
    if (q->file_mode) {
      int n = srslte_filesource_read(&q->file_source, input_buffer, q->sf_len);
      if (n < 0) {
        fprintf(stderr, "Error reading input file\n");
        return SRSLTE_ERROR; 
      }
      if (n == 0) {
        srslte_filesource_seek(&q->file_source, 0);
        q->sf_idx = 9; 
        int n = srslte_filesource_read(&q->file_source, input_buffer, q->sf_len);
        if (n < 0) {
          fprintf(stderr, "Error reading input file\n");
          return SRSLTE_ERROR; 
        }
      }
      if (q->correct_cfo) {
        srslte_cfo_correct(&q->file_cfo_correct, 
                    input_buffer, 
                    input_buffer, 
                    q->file_cfo / 15000 / q->fft_size);               
                    
      }
      q->sf_idx++;
      if (q->sf_idx == 10) {
        q->sf_idx = 0;
      }
      INFO("Reading %d samples. sf_idx = %d\n", q->sf_len, q->sf_idx);
      ret = 1;
    } else {
      if (receive_samples(q, input_buffer)) {
        fprintf(stderr, "Error receiving samples\n");
        return SRSLTE_ERROR;
      }

      switch (q->state) {
        case SF_FIND:     
          switch(srslte_sync_find(&q->sfind, input_buffer, 0, &q->peak_idx)) {
            case SRSLTE_SYNC_ERROR: 
              ret = SRSLTE_ERROR; 
              fprintf(stderr, "Error finding correlation peak (%d)\n", ret);
              return SRSLTE_ERROR;
            case SRSLTE_SYNC_FOUND: 
              ret = find_peak_ok(q, input_buffer);
              break;
            case SRSLTE_SYNC_FOUND_NOSPACE:
              /* If a peak was found but there is not enough space for SSS/CP detection, discard a few samples */
              printf("No space for SSS/CP detection. Realigning frame...\n");
              q->recv_callback(q->stream, dummy_offset_buffer, q->frame_len/2, NULL); 
              srslte_sync_reset(&q->sfind);
              ret = SRSLTE_SUCCESS; 
              break;       
            default:
              ret = SRSLTE_SUCCESS;
              break;
          }          
          if (q->do_agc) {
            srslte_agc_process(&q->agc, input_buffer, q->sf_len);        
          }
          
        break;
        case SF_TRACK:
         
          ret = 1;
          
          srslte_sync_sss_en(&q->strack, q->decode_sss_on_track);
          
          q->sf_idx = (q->sf_idx + q->nof_recv_sf) % 10;

          /* Every SF idx 0 and 5, find peak around known position q->peak_idx */
          if (q->sf_idx == 0 || q->sf_idx == 5) {

            if (q->do_agc && (q->agc_period == 0 || 
                             (q->agc_period && (q->frame_total_cnt%q->agc_period) == 0))) 
            {
              srslte_agc_process(&q->agc, input_buffer, q->sf_len);        
            }

            #ifdef MEASURE_EXEC_TIME
            struct timeval t[3];
            gettimeofday(&t[1], NULL);
            #endif
            
            track_idx = 0; 
            
            /* Track PSS/SSS around the expected PSS position 
             * In tracking phase, the subframe carrying the PSS is always the last one of the frame
             */
            switch(srslte_sync_find(&q->strack, input_buffer, 
                                    q->frame_len - q->sf_len/2 - q->fft_size - q->strack.max_offset/2, 
                                    &track_idx)) 
            {
              case SRSLTE_SYNC_ERROR:
                ret = SRSLTE_ERROR; 
                fprintf(stderr, "Error tracking correlation peak\n");
                return SRSLTE_ERROR;
              case SRSLTE_SYNC_FOUND: 
                ret = track_peak_ok(q, track_idx);
                break;
              case SRSLTE_SYNC_FOUND_NOSPACE:                 
                // It's very very unlikely that we fall here because this event should happen at FIND phase only 
                ret = 0; 
                q->state = SF_FIND; 
                printf("Warning: No space for SSS/CP while in tracking phase\n"); 
                break;
              case SRSLTE_SYNC_NOFOUND: 
                ret = track_peak_no(q); 
                break;
            }
            
            #ifdef MEASURE_EXEC_TIME
            gettimeofday(&t[2], NULL);
            get_time_interval(t);
            q->mean_exec_time = (float) SRSLTE_VEC_CMA((float) t[0].tv_usec, q->mean_exec_time, q->frame_total_cnt);
            #endif

            if (ret == SRSLTE_ERROR) {
              fprintf(stderr, "Error processing tracking peak\n");
              q->state = SF_FIND; 
              return SRSLTE_SUCCESS;
            } 
                      
            q->frame_total_cnt++;       
          } else {
            if (q->correct_cfo) {
              srslte_cfo_correct(&q->sfind.cfocorr, 
                          input_buffer, 
                          input_buffer, 
                          -srslte_sync_get_cfo(&q->strack) / q->fft_size);               
                          
            }            
          }          
        break;
      }
      
    }
  }  
  return ret; 
}

void srslte_ue_sync_reset(srslte_ue_sync_t *q) {

  if (!q->file_mode) {
    srslte_sync_reset(&q->sfind);
    srslte_sync_reset(&q->strack);
  } else {
    q->sf_idx = 9;
  }
  q->state = SF_FIND;
  q->frame_ok_cnt = 0;
  q->frame_no_cnt = 0;
  q->frame_total_cnt = 0; 
  q->mean_sample_offset = 0.0;
  q->next_rf_sample_offset = 0;
  q->frame_find_cnt = 0; 
  #ifdef MEASURE_EXEC_TIME
  q->mean_exec_time = 0;
  #endif      
}

