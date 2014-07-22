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


#include "liblte/phy/phch/ue_sync.h"

#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"

#define MAX_TIME_OFFSET 128
cf_t dummy[MAX_TIME_OFFSET];

#define CURRENT_FFTSIZE   lte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SF_LEN(CURRENT_FFTSIZE, q->cell.cp)

#define CURRENT_SLOTLEN_RE SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SF_LEN_RE(q->cell.nof_prb, q->cell.cp)

#define MAXIMUM_SFLEN       SF_LEN(2048, CPNORM)
#define MAXIMUM_SFLEN_RE    SF_LEN_RE(110, CPNORM)

static int mib_decoder_initialize(ue_sync_t *q);
static void mib_decoder_free(ue_sync_t *q);


static void update_threshold(ue_sync_t *q) {
  int symbol_sz = lte_symbol_sz(q->cell.nof_prb);
  if (symbol_sz > 0) {
    switch (symbol_sz) {
      case 128:
        sync_set_threshold(&q->s, 10000, 1000);
        break;
      case 256:
        sync_set_threshold(&q->s, 20000, 2000);
        break;
      case 512:
        sync_set_threshold(&q->s, 30000, 3000);
        break;
      case 1024:
        sync_set_threshold(&q->s, 40000, 4000);
        break;
      case 2048:
        sync_set_threshold(&q->s, 50000, 5000);
    }  
  }
}

int ue_sync_init(ue_sync_t *q, 
                    double (set_rate_callback)(void*, double),
                    int (recv_callback)(void*, void*, uint32_t),
                    void *stream_handler) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (q                    != NULL && 
      stream_handler       != NULL && 
      set_rate_callback    != NULL &&
      recv_callback        != NULL)
  {
    ret = LIBLTE_ERROR;
    
    bzero(q, sizeof(ue_sync_t));

    ue_sync_reset(q);

    q->cell.nof_prb = SYNC_PBCH_NOF_PRB;
    q->cell.nof_ports = SYNC_PBCH_NOF_PORTS; 
    q->cell.id = 0; 
    q->cell.cp = CPNORM;
    
    q->pbch_decoded = false; 
    q->pbch_initialized = false; 
    q->pbch_decoder_enabled = true; 
    q->pbch_decode_always = false; 
    q->decode_sss_on_track = false; 
    q->change_srate = true; 
    q->nof_mib_decodes = DEFAULT_NOF_MIB_DECODES;
    q->stream = stream_handler;
    q->recv_callback = recv_callback;
    q->set_rate_callback = set_rate_callback;
    
    INFO("Setting sampling frequency 1.92 MHz\n",0);
    q->set_rate_callback(q->stream, 1920000.0);
    
    if (agc_init(&q->agc)) {
      goto clean_exit;
    }
    
    if(sync_init(&q->s, CURRENT_SFLEN, CURRENT_FFTSIZE, CURRENT_FFTSIZE)) {
      goto clean_exit;
    }
    
    sync_pss_det_absolute(&q->s);
    
    if (cfo_init(&q->cfocorr, MAXIMUM_SFLEN)) {
      fprintf(stderr, "Error initiating CFO\n");
      goto clean_exit;
    }
   
    q->input_buffer = vec_malloc(3 * MAXIMUM_SFLEN * sizeof(cf_t));
    if (!q->input_buffer) {
      perror("malloc");
      goto clean_exit;
    }
   
    q->receive_buffer = vec_malloc(3 * MAXIMUM_SFLEN * sizeof(cf_t));
    if (!q->receive_buffer) {
      perror("malloc");
      goto clean_exit;
    }
    
    q->sf_symbols = vec_malloc(MAXIMUM_SFLEN_RE * sizeof(cf_t));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit; 
    }
    for (int i=0;i<SYNC_PBCH_NOF_PORTS;i++) {
      q->ce[i] = vec_malloc(MAXIMUM_SFLEN_RE * sizeof(cf_t));
      if (!q->ce[i]) {
        perror("malloc");
        goto clean_exit; 
      }
    }
   
    update_threshold(q);
    
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
  if (q->receive_buffer) {
    free(q->receive_buffer);
  }
  if (q->sf_symbols) {
    free(q->sf_symbols);
  }
  for (int i=0;i<SYNC_PBCH_NOF_PORTS;i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
  }
  mib_decoder_free(q);
  cfo_free(&q->cfocorr);
  sync_free(&q->s);
  agc_free(&q->agc);
}

lte_cell_t ue_sync_get_cell(ue_sync_t *q) {
  return q->cell;
}

pbch_mib_t ue_sync_get_mib(ue_sync_t *q) {
  return q->mib;
}

uint32_t ue_sync_peak_idx(ue_sync_t *q) {
  return q->peak_idx;
}

ue_sync_state_t ue_sync_get_state(ue_sync_t *q) {
  return q->state;
}

void ue_sync_change_srate(ue_sync_t *q, bool enabled) {
  q->change_srate = enabled; 
}

static int update_srate(ue_sync_t *q) {
  struct timeval t[3];
    
  gettimeofday(&t[1], NULL);
  if (sync_realloc(&q->s, CURRENT_SFLEN, CURRENT_FFTSIZE, CURRENT_FFTSIZE)) {
    fprintf(stderr, "Error realloc'ing SYNC\n");
    return LIBLTE_ERROR;
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  
  if (q->nof_mib_decodes > 1) {
    mib_decoder_free(q);  
    if (mib_decoder_initialize(q)) {
      fprintf(stderr, "Error reinitializing MIB decoder\n");
      return LIBLTE_ERROR; 
    }
  }  

  // Finally set the new sampling rate
  q->set_rate_callback(q->stream, (float) lte_sampling_freq_hz(q->cell.nof_prb));
 
  update_threshold(q);
   
  ue_sync_reset(q);
  INFO("Set sampling rate %.2f MHz, fft_size=%d, sf_len=%d Threshold=%.2f/%.2f Texec=%d us\n", 
       (float) lte_sampling_freq_hz(q->cell.nof_prb)/1000000, 
       CURRENT_FFTSIZE, CURRENT_SFLEN, q->s.find_threshold, q->s.track_threshold, (int) t[0].tv_usec);
  
  return LIBLTE_SUCCESS;
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

bool ue_sync_is_mib_decoded(ue_sync_t *q) {
  return q->pbch_decoded;
}

void ue_sync_pbch_enable(ue_sync_t *q, bool enabled) {
  q->pbch_decoder_enabled = enabled; 
}

void ue_sync_pbch_always(ue_sync_t *q, bool enabled) {
  q->pbch_decode_always = enabled;
}

void ue_sync_decode_sss_on_track(ue_sync_t *q, bool enabled) {
  q->decode_sss_on_track = enabled; 
}

void ue_sync_set_nof_pbch_decodes(ue_sync_t *q, uint32_t nof_pbch_decodes) {
  q->nof_mib_decodes = nof_pbch_decodes;
}

static int mib_decoder_initialize(ue_sync_t *q) {
    
  if (lte_fft_init(&q->fft, q->cell.cp, q->cell.nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    return -1;
  }
  if (chest_init_LTEDL(&q->chest, q->cell)) {
    fprintf(stderr, "Error initializing reference signal\n");
    return -1;
  }
  if (pbch_init(&q->pbch, q->cell)) {
    fprintf(stderr, "Error initiating PBCH\n");
    return -1;
  }
  q->pbch_initialized = 1;
  DEBUG("PBCH initiated cell_id=%d\n", q->cell.id);
  
  return 0;
}

static void mib_decoder_free(ue_sync_t *q) {
  chest_free(&q->chest);
  pbch_free(&q->pbch);
  lte_fft_free(&q->fft);
}

static int mib_decoder_run(ue_sync_t *q) {
  int ret; 
  
  /* Run FFT for the second slot */
  lte_fft_run_sf(&q->fft, q->input_buffer, q->sf_symbols);

  /* Get channel estimates of slot #1 for each port */
  chest_ce_sf(&q->chest, q->sf_symbols, q->ce, 0);
  
  if (q->pbch_last_trial && 
    (q->frame_total_cnt - q->pbch_last_trial > 2)) 
  {
    pbch_decode_reset(&q->pbch);
    INFO("Resetting PBCH decoder: last trial %d, now is %d\n", 
         q->pbch_last_trial, q->frame_total_cnt);
    q->pbch_last_trial = 0;
  }
  
  if (pbch_decode(&q->pbch, q->sf_symbols, q->ce, &q->mib) == 1) {        
    q->frame_number = q->mib.sfn; 
    q->cell.nof_ports = q->mib.nof_ports;
    
    if (!q->pbch_decoded) {
      printf("\n\nMIB decoded:\n");
      pbch_mib_fprint(stdout, &q->mib, q->cell.id);
      
      if (q->cell.nof_prb != q->mib.nof_prb) {
        q->cell.nof_prb = q->mib.nof_prb;
        if (q->change_srate) {
          ret = update_srate(q);        
        }
      }
    } else { 
      INFO("MIB decoded #%d SFN: %d\n", q->pbch_decoded, q->mib.sfn);
    }
    q->pbch_decoded++;
    
    pbch_decode_reset(&q->pbch);

  } else {
    INFO("MIB not decoded: %d\n", q->frame_total_cnt/2);
    q->pbch_last_trial = q->frame_total_cnt;
  }
  
  return ret; 
}

static int find_peak_ok(ue_sync_t *q) {
  int ret; 
  
  if (q->peak_idx < CURRENT_SFLEN) {
    /* Receive the rest of the next subframe */
    if (q->recv_callback(q->stream, &q->input_buffer[CURRENT_SFLEN], q->peak_idx+CURRENT_SFLEN/2) < 0) {
      return LIBLTE_ERROR;
    }
  }
  
  if (sync_sss_detected(&q->s)) {
    ret = sync_get_cell_id(&q->s);
    if (ret >= 0) {
      q->cell.id = (uint32_t) ret;
      q->cell.cp = sync_get_cp(&q->s);
    }
    
    /* Get the subframe index (0 or 5) */
    q->sf_idx = sync_get_slot_id(&q->s)/2;
   
    /* Reset variables */ 
    q->frame_ok_cnt = 0;
    q->frame_no_cnt = 0;
    q->frame_total_cnt = 0;       

    /* Goto Tracking state */
    q->state = SF_TRACK;      
    ret = LIBLTE_SUCCESS;
  
    INFO("Found peak at %d, value %.3f, SF_idx: %d, Cell_id: %d CP: %s\n", 
        q->peak_idx, sync_get_peak_value(&q->s), q->sf_idx, q->cell.id, lte_cp_string(q->cell.cp));       
    
    if (q->peak_idx < CURRENT_SFLEN) {
      q->sf_idx++;
    }
    
    return ret;
  } else {
    INFO("Found peak at %d, SSS not detected\n", q->peak_idx);
    return 0;
  }
}

int track_peak_ok(ue_sync_t *q, uint32_t track_idx) {
  int ret = LIBLTE_SUCCESS; 
  
   /* Make sure subframe idx is what we expect */
  if ((q->sf_idx != sync_get_slot_id(&q->s)/2) && q->decode_sss_on_track) {
    INFO("\nWarning: Expected SF idx %d but got %d!\n", 
          q->sf_idx, sync_get_slot_id(&q->s)/2);
    q->sf_idx = sync_get_slot_id(&q->s)/2;
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
    q->cur_cfo = EXPAVERAGE(sync_get_cfo(&q->s), q->cur_cfo, q->frame_ok_cnt);
    
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
    INFO("Tracking peak not found. Peak %.3f, %d lost\n", sync_get_peak_value(&q->s), (int) q->frame_no_cnt);    
  }

  return LIBLTE_SUCCESS;
}

static int receive_samples(ue_sync_t *q) {
  
  if (q->cell.nof_prb >= 6 && q->cell.nof_prb <= 100) {
    /* A negative time offset means there are samples in our buffer for the next subframe, 
    because we are sampling too fast. 
    */
    if (q->time_offset < 0) {
      q->time_offset = -q->time_offset;
    } 
    /* copy last part of the last subframe (use move since there could be overlapping) */
    memcpy(q->receive_buffer, &q->input_buffer[CURRENT_SFLEN-q->time_offset], q->time_offset*sizeof(cf_t));

    /* Get 1 subframe from the USRP getting more samples and keeping the previous samples, if any */  
    if (q->recv_callback(q->stream, &q->receive_buffer[q->time_offset], CURRENT_SFLEN - q->time_offset) < 0) {
      return LIBLTE_ERROR;
    }
    
    /* reset time offset */
    q->time_offset = 0;

    return LIBLTE_SUCCESS; 
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
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
    
    agc_process(&q->agc, q->receive_buffer, q->input_buffer, CURRENT_SFLEN);
    
    switch (q->state) {
      case SF_AGC: 
        q->frame_total_cnt++;
        if (q->frame_total_cnt >= AGC_NOF_FRAMES) {
          q->state = SF_FIND; 
          q->frame_total_cnt = 0; 
        }
        ret = 0;
        break;
      case SF_FIND:
        q->s.sss_en = true; 
        
        /* Find peak and cell id */
        ret = sync_find(&q->s, q->input_buffer, &q->peak_idx);
        if (ret < 0) {
          fprintf(stderr, "Error finding correlation peak (%d)\n", ret);
          return -1;
        }
        
        DEBUG("Find PAR=%.2f\n", sync_get_peak_value(&q->s));
        
        if (ret == 1) {
          ret = find_peak_ok(q);
          /* Initialize PBCH decoder */
          if (ret == LIBLTE_SUCCESS) {
            if (!q->pbch_initialized && q->pbch_decoder_enabled) {
              ret = mib_decoder_initialize(q);
              if (ret < 0) {
                fprintf(stderr, "Error initializing MIB decoder\n");
              }
            }
          } else if (ret < 0) {
            if (ret < 0) {
              fprintf(stderr, "Error processing find peak \n");
            }
          }
        } else if (q->peak_idx != 0) {
          uint32_t rlen; 
          if (q->peak_idx < CURRENT_SFLEN/2) {
            rlen = CURRENT_SFLEN/2-q->peak_idx;
          } else {
            rlen = q->peak_idx;
          }
          if (q->recv_callback(q->stream, q->receive_buffer, rlen) < 0) {
            return LIBLTE_ERROR;
          }
        }
      break;
      case SF_TRACK:
        ret = LIBLTE_SUCCESS;
        
        q->s.sss_en = q->decode_sss_on_track; 
        
        q->sf_idx = (q->sf_idx + 1) % 10;

        DEBUG("TRACK: SF=%d FrameCNT: %d\n", q->sf_idx, q->frame_total_cnt);
        
        /* Every SF idx 0 and 5, find peak around known position q->peak_idx */
        if (q->sf_idx == 0 || q->sf_idx == 5) {

          #ifdef MEASURE_EXEC_TIME
          gettimeofday(&t[1], NULL);
          #endif
          
          track_idx = 0; 
          
          /* track pss around the middle of the subframe, where the PSS is */
          ret = sync_track(&q->s, q->input_buffer, CURRENT_SFLEN/2-CURRENT_FFTSIZE, &track_idx);
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
                (int) q->frame_total_cnt, sync_get_peak_value(&q->s), q->sf_idx, track_idx, q->time_offset, sync_get_cfo(&q->s));
          
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
        
        /* At subframe 0, try to decode PBCH if not yet decoded */
        if (q->sf_idx == 0) {
          if(q->pbch_decoder_enabled                     &&
            (q->pbch_decoded < q->nof_mib_decodes || q->pbch_decode_always)) 
          {
            mib_decoder_run(q);
          } else {
            q->mib.sfn = (q->mib.sfn + 1) % 1024;
          }
        }

        if (ret == LIBLTE_SUCCESS) {
          if (q->pbch_decoder_enabled) {
            if (q->pbch_decoded >= q->nof_mib_decodes) {
              ret = 1; 
            } else {
              ret = 0;
            }
          } else {
            ret = 1;
          }
        }
      break;
    }
  }  
  DEBUG("UE SYNC returns %d\n", ret);
  return ret; 
}

void ue_sync_reset(ue_sync_t *q) {
  q->state = SF_AGC;
    
  q->pbch_last_trial = 0; 
  q->frame_ok_cnt = 0;
  q->frame_no_cnt = 0;
  q->frame_total_cnt = 0; 
  q->cur_cfo = 0;
  q->mean_time_offset = 0;
  q->time_offset = 0;
  #ifdef MEASURE_EXEC_TIME
  q->mean_exec_time = 0;
  #endif
  
  pbch_decode_reset(&q->pbch);
}

