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

#include "srslte/srslte.h"
#include "srslte/cuhd/cuhd.h"
#include "srslte/cuhd/cuhd_utils.h"

cell_search_cfg_t cell_detect_config = {
  5000, // maximum number of frames to receive for MIB decoding
  50, // maximum number of frames to receive for PSS correlation 
  10.0,
  50.0
};

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  int nof_subframes;
  bool disable_plots;
  int force_N_id_2;
  char *uhd_args; 
  float uhd_freq; 
  float uhd_gain;
}prog_args_t;

void args_default(prog_args_t *args) {
  args->nof_subframes = -1; 
  args->force_N_id_2 = -1; // Pick the best
  args->uhd_args = "";
  args->uhd_freq = -1.0;
  args->uhd_gain = 50; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [aglnv] -f rx_frequency (in Hz)\n", prog);
  printf("\t-a UHD args [Default %s]\n", args->uhd_args);
  printf("\t-g UHD RX gain [Default %.2f dB]\n", args->uhd_gain);
  printf("\t-l Force N_id_2 [Default best]\n");
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

int  parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "aglnvf")) != -1) {
    switch (opt) {
    case 'a':
      args->uhd_args = argv[optind];
      break;
    case 'g':
      args->uhd_gain = atof(argv[optind]);
      break;
    case 'f':
      args->uhd_freq = atof(argv[optind]);
      break;
    case 'n':
      args->nof_subframes = atoi(argv[optind]);
      break;
    case 'l':
      args->force_N_id_2 = atoi(argv[optind]);
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(args, argv[0]);
      return -1;
    }
  }
  if (args->uhd_freq < 0) {
    usage(args, argv[0]);
    return -1;
  }
  return 0;
}
/**********************************************************************/

/* TODO: Do something with the output data */
uint8_t data[1000000];

bool go_exit = false; 
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}

int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples, srslte_timestamp_t *q) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}

enum receiver_state { DECODE_MIB, DECODE_SIB, MEASURE} state; 

#define MAX_SINFO 10
#define MAX_NEIGHBOUR_CELLS     128

int main(int argc, char **argv) {
  int ret; 
  cf_t *sf_buffer; 
  prog_args_t prog_args; 
  srslte_cell_t cell;  
  int64_t sf_cnt;
  srslte_ue_sync_t ue_sync; 
  srslte_ue_mib_t ue_mib; 
  void *uhd; 
  srslte_ue_dl_t ue_dl; 
  srslte_ofdm_t fft; 
  srslte_chest_dl_t chest; 
  uint32_t nframes=0;
  uint32_t nof_trials = 0; 
  uint32_t sfn = 0; // system frame number
  int n; 
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];
  uint32_t sfn_offset; 
  float rssi_utra=0,rssi=0, rsrp=0, rsrq=0, snr=0;
  cf_t *ce[SRSLTE_MAX_PORTS];

  if (parse_args(&prog_args, argc, argv)) {
    exit(-1);
  }

  if (prog_args.uhd_gain > 0) {
    printf("Opening UHD device...\n");
    if (cuhd_open(prog_args.uhd_args, &uhd)) {
      fprintf(stderr, "Error opening uhd\n");
      exit(-1);
    }
    cuhd_set_rx_gain(uhd, prog_args.uhd_gain);      
  } else {
    printf("Opening UHD device with threaded RX Gain control ...\n");
    if (cuhd_open_th(prog_args.uhd_args, &uhd, false)) {
      fprintf(stderr, "Error opening uhd\n");
      exit(-1);
    }
    cuhd_set_rx_gain(uhd, 50);      
  }

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);
  signal(SIGINT, sig_int_handler);

  cuhd_set_master_clock_rate(uhd, 30.72e6);        

  /* set receiver frequency */
  cuhd_set_rx_freq(uhd, (double) prog_args.uhd_freq);
  cuhd_rx_wait_lo_locked(uhd);
  printf("Tunning receiver to %.3f MHz\n", (double ) prog_args.uhd_freq/1000000);
  
  uint32_t ntrial=0; 
  do {
    ret = cuhd_search_and_decode_mib(uhd, &cell_detect_config, prog_args.force_N_id_2, &cell);
    if (ret < 0) {
      fprintf(stderr, "Error searching for cell\n");
      exit(-1); 
    } else if (ret == 0 && !go_exit) {
      printf("Cell not found after %d trials. Trying again (Press Ctrl+C to exit)\n", ntrial++);
    }      
  } while (ret == 0 && !go_exit); 
  
  if (go_exit) {
    exit(0);
  }
  
  /* set sampling frequency */
    int srate = srslte_sampling_freq_hz(cell.nof_prb);    
    if (srate != -1) {  
      if (srate < 10e6) {          
        cuhd_set_master_clock_rate(uhd, 4*srate);        
      } else {
        cuhd_set_master_clock_rate(uhd, srate);        
      }
      printf("Setting sampling rate %.2f MHz\n", (float) srate/1000000);
      float srate_uhd = cuhd_set_rx_srate(uhd, (double) srate);
      if (srate_uhd != srate) {
        fprintf(stderr, "Could not set sampling rate\n");
        exit(-1);
      }
    } else {
      fprintf(stderr, "Invalid number of PRB %d\n", cell.nof_prb);
      exit(-1);
    }

  INFO("Stopping UHD and flushing buffer...\n",0);
  cuhd_stop_rx_stream(uhd);
  cuhd_flush_buffer(uhd);
  
  if (srslte_ue_sync_init(&ue_sync, cell, cuhd_recv_wrapper, uhd)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    return -1; 
  }
  if (srslte_ue_dl_init(&ue_dl, cell)) { 
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    return -1;
  }
  if (srslte_ue_mib_init(&ue_mib, cell)) {
    fprintf(stderr, "Error initaiting UE MIB decoder\n");
    return -1;
  }
  
  /* Configure downlink receiver for the SI-RNTI since will be the only one we'll use */
  srslte_ue_dl_set_rnti(&ue_dl, SRSLTE_SIRNTI); 

  /* Initialize subframe counter */
  sf_cnt = 0;
    
  if (srslte_ofdm_rx_init(&fft, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initiating FFT\n");
    return -1;
  }
  if (srslte_chest_dl_init(&chest, cell)) {
    fprintf(stderr, "Error initiating channel estimator\n");
    return -1;
  }
  
  int sf_re = SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);

  cf_t *sf_symbols = srslte_vec_malloc(sf_re * sizeof(cf_t));

  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    ce[i] = srslte_vec_malloc(sizeof(cf_t) * sf_re);
  }
  
  cuhd_start_rx_stream(uhd);
  
  float rx_gain_offset = 0;

  /* Main loop */
  while ((sf_cnt < prog_args.nof_subframes || prog_args.nof_subframes == -1) && !go_exit) {
    
    ret = srslte_ue_sync_get_buffer(&ue_sync, &sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error calling srslte_ue_sync_work()\n");
    }

        
    /* srslte_ue_sync_get_buffer returns 1 if successfully read 1 aligned subframe */
    if (ret == 1) {
      switch (state) {
        case DECODE_MIB:
          if (srslte_ue_sync_get_sfidx(&ue_sync) == 0) {
            srslte_pbch_decode_reset(&ue_mib.pbch);
            n = srslte_ue_mib_decode(&ue_mib, sf_buffer, bch_payload, NULL, &sfn_offset);
            if (n < 0) {
              fprintf(stderr, "Error decoding UE MIB\n");
              return -1;
            } else if (n == SRSLTE_UE_MIB_FOUND) {   
              srslte_pbch_mib_unpack(bch_payload, &cell, &sfn);
              printf("Decoded MIB. SFN: %d, offset: %d\n", sfn, sfn_offset);
              sfn = (sfn + sfn_offset)%1024; 
              state = DECODE_SIB; 
            }
          }
          break;
        case DECODE_SIB:
          /* We are looking for SI Blocks, search only in appropiate places */
          if ((srslte_ue_sync_get_sfidx(&ue_sync) == 5 && (sfn%2)==0)) {
            n = srslte_ue_dl_decode_rnti_rv(&ue_dl, sf_buffer, data, srslte_ue_sync_get_sfidx(&ue_sync), SRSLTE_SIRNTI,
                                 ((int) ceilf((float)3*(((sfn)/2)%4)/2))%4);
            if (n < 0) {
              fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
              return -1;
            } else if (n == 0) {
              printf("CFO: %+6.4f KHz, SFO: %+6.4f Khz, NOI: %.2f, PDCCH-Det: %.3f\r",
                      srslte_ue_sync_get_cfo(&ue_sync)/1000, srslte_ue_sync_get_sfo(&ue_sync)/1000, 
                      srslte_sch_average_noi(&ue_dl.pdsch.dl_sch),
                      (float) ue_dl.nof_detected/nof_trials);                
              nof_trials++; 
            } else {
              printf("Decoded SIB1. Payload: ");
              srslte_vec_fprint_byte(stdout, data, n/8);;
              state = MEASURE;
            }
          }
        break;
        
      case MEASURE:
        
        if (srslte_ue_sync_get_sfidx(&ue_sync) == 5) {
          /* Run FFT for all subframe data */
          srslte_ofdm_rx_sf(&fft, sf_buffer, sf_symbols);
          
          srslte_chest_dl_estimate(&chest, sf_symbols, ce, srslte_ue_sync_get_sfidx(&ue_sync));
                  
          rssi = SRSLTE_VEC_EMA(srslte_vec_avg_power_cf(sf_buffer,SRSLTE_SF_LEN(srslte_symbol_sz(cell.nof_prb))),rssi,0.05);
          rssi_utra = SRSLTE_VEC_EMA(srslte_chest_dl_get_rssi(&chest),rssi_utra,0.05);
          rsrq = SRSLTE_VEC_EMA(srslte_chest_dl_get_rsrq(&chest),rsrq,0.05);
          rsrp = SRSLTE_VEC_EMA(srslte_chest_dl_get_rsrp(&chest),rsrp,0.05);      
          snr = SRSLTE_VEC_EMA(srslte_chest_dl_get_snr(&chest),snr,0.05);      
          
          nframes++;          
        } 
        
        
        if ((nframes%100) == 0 || rx_gain_offset == 0) {
          if (cuhd_has_rssi(uhd)) {
            rx_gain_offset = 10*log10(rssi)-cuhd_get_rssi(uhd);
          } else {
            rx_gain_offset = cuhd_get_rx_gain(uhd);
          }
        }
        
        // Plot and Printf
        if ((nframes%10) == 0) {

          printf("CFO: %+8.4f KHz, SFO: %+8.4f Khz, RSSI: %5.1f dBm, RSSI/ref-symbol: %+5.1f dBm, "
                 "RSRP: %+5.1f dBm, RSRQ: %5.1f dB, SNR: %5.1f dB\r",
                srslte_ue_sync_get_cfo(&ue_sync)/1000, srslte_ue_sync_get_sfo(&ue_sync)/1000, 
                10*log10(rssi*1000) - rx_gain_offset,                                  
                10*log10(rssi_utra*1000)- rx_gain_offset, 
                10*log10(rsrp*1000) - rx_gain_offset, 
                10*log10(rsrq), 10*log10(snr));                
          if (srslte_verbose != SRSLTE_VERBOSE_NONE) {
            printf("\n");
          }
        }
        break;
      }
      if (srslte_ue_sync_get_sfidx(&ue_sync) == 9) {
        sfn++; 
        if (sfn == 1024) {
          sfn = 0; 
        }
      }
    } else if (ret == 0) {
      printf("Finding PSS... Peak: %8.1f, FrameCnt: %d, State: %d\r", 
        srslte_sync_get_peak_value(&ue_sync.sfind), 
        ue_sync.frame_total_cnt, ue_sync.state);      
    }
   
        
    sf_cnt++;                  
  } // Main loop

  srslte_ue_sync_free(&ue_sync);
  cuhd_close(uhd);
  printf("\nBye\n");
  exit(0);
}



