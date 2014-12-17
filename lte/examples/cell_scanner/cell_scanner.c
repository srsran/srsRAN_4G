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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include "liblte/rrc/rrc.h"
#include "liblte/phy/phy.h"
#include "liblte/cuhd/cuhd.h"
#include "../cuhd_utils.h"
#include "cell_scanner.h"

#define B210_DEFAULT_GAIN         40.0
#define B210_DEFAULT_GAIN_CORREC  80.0 // Gain of the Rx chain when the gain is set to 40

int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}

int cell_scanner_init(cell_scanner_t *q, cell_scanner_config_t *config) 
{
  bzero(q, sizeof(cell_scanner_t));
  
  memcpy(&q->config, config, sizeof(cell_scanner_config_t));
  
  printf("Opening UHD device...\n");
  if (cuhd_open(q->config.uhd_args, &q->uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    return -1;
  }
  /* Set receiver gain */
  cuhd_set_rx_gain(q->uhd, B210_DEFAULT_GAIN);

  return 0;
}

void cell_scanner_close(cell_scanner_t *q) {
    cuhd_close(q->uhd);
}

int cell_scanner_all_cells(cell_scanner_t *q, float frequency, cell_scanner_result_t *result) 
{
  return cell_scanner_cell(q, frequency, -1, result);
}

#define MAX_SINFO 10

int cell_scanner_cell(cell_scanner_t *q, float frequency, int N_id_2, cell_scanner_result_t *result) 
{
  int ret; 
  cf_t *sf_buffer; 
  lte_cell_t cell;  
  int64_t sf_cnt;
  ue_sync_t ue_sync; 
  ue_mib_t ue_mib; 
  ue_dl_t ue_dl; 
  lte_fft_t fft; 
  chest_dl_t chest; 
  uint32_t nframes_measure=0;
  uint32_t nof_trials = 0; 
  uint32_t sfn = 0; // system frame number
  int n; 
  uint8_t bch_payload[BCH_PAYLOAD_LEN], bch_payload_unpacked[BCH_PAYLOAD_LEN];
  uint32_t sfn_offset; 
  uint8_t data[1024];
  uint8_t data_unpacked[1024];

  bzero(result, sizeof(cell_scanner_result_t));
  
  /* set receiver frequency */
  cuhd_set_rx_freq(q->uhd, (double) frequency);
  cuhd_rx_wait_lo_locked(q->uhd);
  printf("Tunning receiver to %.3f MHz\n", (double ) frequency/1000000);

  cell_search_cfg_t cfg; 
  cfg.max_frames_pss = q->config.pss_max_frames;
  cfg.max_frames_pbch = q->config.pbch_max_frames;
  cfg.threshold = q->config.cell_detect_early_stop_threshold;
  
  ret = cuhd_search_and_decode_mib(q->uhd, &cfg, N_id_2, &cell);
  if (ret < 0) {
    fprintf(stderr, "Error searching cell\n");
    exit(-1); 
  } else if (ret == 0) {
    printf("Cell not found\n");
    exit(0);
  }
  
  /* set sampling frequency */
  int srate = lte_sampling_freq_hz(cell.nof_prb);
  if (srate != -1) {  
    cuhd_set_rx_srate(q->uhd, (double) srate);      
  } else {
    fprintf(stderr, "Invalid number of PRB %d\n", cell.nof_prb);
    return LIBLTE_ERROR;
  }

  INFO("Stopping UHD and flushing buffer...\n",0);
  cuhd_stop_rx_stream(q->uhd);
  cuhd_flush_buffer(q->uhd);
  
  if (ue_sync_init(&ue_sync, cell, cuhd_recv_wrapper, q->uhd)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1); 
  }
  if (ue_dl_init(&ue_dl, cell, 1234)) { 
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    exit(-1);
  }
  if (ue_mib_init(&ue_mib, cell)) {
    fprintf(stderr, "Error initaiting UE MIB decoder\n");
    exit(-1);
  }
  
  /* Configure downlink receiver for the SI-RNTI since will be the only one we'll use */
  ue_dl_set_rnti(&ue_dl, SIRNTI); 

  /* Initialize subframe counter */
  sf_cnt = 0;
    
  if (lte_fft_init(&fft, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initiating FFT\n");
    return -1;
  }
  if (chest_dl_init(&chest, cell)) {
    fprintf(stderr, "Error initiating channel estimator\n");
    return -1;
  }
  
  int sf_re = SF_LEN_RE(cell.nof_prb, cell.cp);
  cf_t *sf_symbols = vec_malloc(sf_re * sizeof(cf_t));

  cf_t *ce[MAX_PORTS];
  for (int i=0;i<MAX_PORTS;i++) {
    ce[i] = vec_malloc(sizeof(cf_t) * sf_re);
  }

  cuhd_start_rx_stream(q->uhd);
    
  memcpy(&result->phy_cell, &cell, sizeof(lte_cell_t));
  
  chest_dl_t *chest_ptr = &ue_dl.chest; 
  bool mib_decoded = false; 
  bool sib_decoded = false; 
  
  /* Main loop */
  while (sf_cnt < q->config.measure_avg_nof_frames) {
    
    ret = ue_sync_get_buffer(&ue_sync, &sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error calling ue_sync_work()\n");
    }
        
    /* ue_sync_get_buffer returns 1 if successfully read 1 aligned subframe */
    if (ret == 1) {
      if (!mib_decoded) {
        if (ue_sync_get_sfidx(&ue_sync) == 0) {
          pbch_decode_reset(&ue_mib.pbch);
          n = ue_mib_decode(&ue_mib, sf_buffer, bch_payload_unpacked, NULL, &sfn_offset);
          if (n < 0) {
            fprintf(stderr, "Error decoding UE MIB\n");
            exit(-1);
          } else if (n == MIB_FOUND) {             
            bit_unpack_vector(bch_payload_unpacked, bch_payload, BCH_PAYLOAD_LEN);
            bcch_bch_unpack(bch_payload, BCH_PAYLOAD_LEN, &cell, &sfn);
            printf("Decoded MIB. SFN: %d, offset: %d\n", sfn, sfn_offset);
            sfn = (sfn + sfn_offset)%1024; 
            mib_decoded = true; 
          }
        }        
      }
                    
      /* We are looking for SI Blocks, search only in appropiate places */
      if (mib_decoded && !sib_decoded && 
        (ue_sync_get_sfidx(&ue_sync) == 5 && (sfn%2)==0)) 
      {
        n = ue_dl_decode_sib(&ue_dl, sf_buffer, data, ue_sync_get_sfidx(&ue_sync), 
                              ((int) ceilf((float)3*(((sfn)/2)%4)/2))%4);
        if (n < 0) {
          fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
          exit(-1);
        } else if (n == 0) {
          nof_trials++; 
        } else {
          bit_unpack_vector(data, data_unpacked, n);
          void *dlsch_msg = bcch_dlsch_unpack(data_unpacked, n);
          if (dlsch_msg) {
            printf("\n");fflush(stdout);
            cell_access_info_t cell_info; 
            bcch_dlsch_sib1_get_cell_access_info(dlsch_msg, &cell_info);
            printf("Decoded SIB1. Cell ID: 0x%x\n", cell_info.cell_id);
            
            result->cell_id = cell_info.cell_id;
            
            bcch_dlsch_fprint(dlsch_msg, stdout);        
            
            sib_decoded = true; 
          }
        }
      } else {
        chest_ptr = &chest; 
        /* Run FFT for all subframe data */
        lte_fft_run_sf(&fft, sf_buffer, sf_symbols);            
        chest_dl_estimate(&chest, sf_symbols, ce, ue_sync_get_sfidx(&ue_sync));            
      }
            
      result->rssi = VEC_CMA(vec_avg_power_cf(sf_buffer,SF_LEN(lte_symbol_sz(cell.nof_prb))),
                            result->rssi,nframes_measure);
      result->rsrq = VEC_EMA(chest_dl_get_rsrq(chest_ptr),result->rsrq,0.01);
      result->rsrp = VEC_CMA(chest_dl_get_rsrp(chest_ptr),result->rsrp,nframes_measure);      
      result->snr = VEC_EMA(chest_dl_get_snr(chest_ptr),result->snr,0.01);      
      nframes_measure++;
      
      // Plot and Printf
      if ((nframes_measure%10) == 0) {
        printf("CFO: %+8.4f KHz, SFO: %+8.4f Khz, RSSI: %5.1f dBm, "
              "RSRP: %+5.1f dBm, RSRQ: %5.1f dB, SNR: %5.1f dB\r",
              ue_sync_get_cfo(&ue_sync)/1000, ue_sync_get_sfo(&ue_sync)/1000, 
              10*log10(result->rssi*1000)-B210_DEFAULT_GAIN_CORREC, 
              10*log10(result->rsrp*1000)-B210_DEFAULT_GAIN_CORREC, 
              10*log10(result->rsrq), 10*log10(result->snr));                
        if (verbose != VERBOSE_NONE) {
          printf("\n");
        }        
      }
      if (ue_sync_get_sfidx(&ue_sync) == 9) {
        sfn++; 
        if (sfn == 1024) {
          sfn = 0; 
        }
      }
    } else if (ret == 0) {
      printf("Finding PSS... Peak: %8.1f, FrameCnt: %d, State: %d\r", 
        sync_get_peak_value(&ue_sync.sfind), 
        ue_sync.frame_total_cnt, ue_sync.state);      
    }
    sf_cnt++;                  
  } // Main loop
  
  // Correct RSRP and RSSI measurements 
  result->rssi /= pow(10, 8);
  result->rsrp /= pow(10, 8);

  ue_sync_free(&ue_sync);
  
  return 0; 
}



