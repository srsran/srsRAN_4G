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
#include <signal.h>

#include <unistd.h>

#include "srslte/srslte.h"

#include "srslte/rf/rf_utils.h"


#ifndef DISABLE_RF
#include "srslte/rf/rf.h"
#endif

#define MHZ             1000000
#define SAMP_FREQ       1920000
#define FLEN            9600
#define FLEN_PERIOD     0.005

#define MAX_EARFCN 1000


int band = -1;
int earfcn_start=-1, earfcn_end = -1;


cell_search_cfg_t cell_detect_config = {
  SRSLTE_DEFAULT_MAX_FRAMES_PBCH,
  SRSLTE_DEFAULT_MAX_FRAMES_PSS, 
  SRSLTE_DEFAULT_NOF_VALID_PSS_FRAMES,
  0
};

struct cells {
  srslte_cell_t cell;
  float freq; 
  int dl_earfcn;
  float power;
};
struct cells results[1024]; 

float rf_gain = 70.0;
char *rf_args=""; 

void usage(char *prog) {
  printf("Usage: %s [agsendtvb] -b band\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-g RF gain [Default %.2f dB]\n", rf_gain);
  printf("\t-s earfcn_start [Default All]\n");
  printf("\t-e earfcn_end [Default All]\n");
  printf("\t-n nof_frames_total [Default 100]\n");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "agsendvb")) != -1) {
    switch(opt) {
    case 'a':
      rf_args = argv[optind];
      break;
    case 'b':
      band = atoi(argv[optind]);
      break;
    case 's':
      earfcn_start = atoi(argv[optind]);
      break;
    case 'e':
      earfcn_end = atoi(argv[optind]);
      break;
    case 'n':
      cell_detect_config.max_frames_pss = atoi(argv[optind]);
      break;
    case 'g':
      rf_gain = atof(argv[optind]);
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (band == -1) {
    usage(argv[0]);
    exit(-1);
  }
}

int srslte_rf_recv_wrapper(void *h, void *data, uint32_t nsamples, srslte_timestamp_t *t) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return srslte_rf_recv((srslte_rf_t*) h, data, nsamples, 1);
}

bool go_exit = false; 

void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}

double srslte_rf_set_rx_gain_wrapper(void *h, double f) {
  return srslte_rf_set_rx_gain((srslte_rf_t*) h, f);
}

int main(int argc, char **argv) {
  int n; 
  srslte_rf_t rf;
  srslte_ue_cellsearch_t cs; 
  srslte_ue_cellsearch_result_t found_cells[3]; 
  int nof_freqs; 
  srslte_earfcn_t channels[MAX_EARFCN];
  uint32_t freq;
  uint32_t n_found_cells=0;
  
  parse_args(argc, argv);
    
  printf("Opening RF device...\n");
  if (srslte_rf_open(&rf, rf_args)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }  
  if (!cell_detect_config.init_agc) {
    srslte_rf_set_rx_gain(&rf, rf_gain);
  } else {
    printf("Starting AGC thread...\n");
    if (srslte_rf_start_gain_thread(&rf, false)) {
      fprintf(stderr, "Error opening rf\n");
      exit(-1);
    }
    srslte_rf_set_rx_gain(&rf, 50);      
  }

  srslte_rf_set_master_clock_rate(&rf, 30.72e6);        

  // Supress RF messages
  srslte_rf_suppress_stdout(&rf);
  
  nof_freqs = srslte_band_get_fd_band(band, channels, earfcn_start, earfcn_end, MAX_EARFCN);
  if (nof_freqs < 0) {
    fprintf(stderr, "Error getting EARFCN list\n");
    exit(-1);
  }

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);
  signal(SIGINT, sig_int_handler);

  for (freq=0;freq<nof_freqs && !go_exit;freq++) {
  
    /* set rf_freq */
    srslte_rf_set_rx_freq(&rf, (double) channels[freq].fd * MHZ);
    srslte_rf_rx_wait_lo_locked(&rf);
    INFO("Set rf_freq to %.3f MHz\n", (double) channels[freq].fd * MHZ/1000000);
    
    printf("[%3d/%d]: EARFCN %d Freq. %.2f MHz looking for PSS.\n", freq, nof_freqs,
                      channels[freq].id, channels[freq].fd);fflush(stdout);
    
    if (SRSLTE_VERBOSE_ISINFO()) {
      printf("\n");
    }
      
    bzero(found_cells, 3*sizeof(srslte_ue_cellsearch_result_t));
      
    if (srslte_ue_cellsearch_init(&cs, cell_detect_config.max_frames_pss, srslte_rf_recv_wrapper, (void*) &rf)) {
      fprintf(stderr, "Error initiating UE cell detect\n");
      exit(-1);
    }
    
    if (cell_detect_config.max_frames_pss) {
      srslte_ue_cellsearch_set_nof_valid_frames(&cs, cell_detect_config.nof_valid_pss_frames);
    }
    if (cell_detect_config.init_agc) {
      srslte_ue_sync_start_agc(&cs.ue_sync, srslte_rf_set_rx_gain_wrapper, cell_detect_config.init_agc);    
    }

    INFO("Setting sampling frequency %.2f MHz for PSS search\n", SRSLTE_CS_SAMP_FREQ/1000000);
    srslte_rf_set_rx_srate(&rf, SRSLTE_CS_SAMP_FREQ);
    INFO("Starting receiver...\n", 0);
    srslte_rf_start_rx_stream(&rf);
    
    n = srslte_ue_cellsearch_scan(&cs, found_cells, NULL); 
    if (n < 0) {
      fprintf(stderr, "Error searching cell\n");
      exit(-1);
    } else if (n > 0) {
      for (int i=0;i<3;i++) {
        if (found_cells[i].psr > 10.0) {
          srslte_cell_t cell;
          cell.id = found_cells[i].cell_id; 
          cell.cp = found_cells[i].cp; 
          int ret = rf_mib_decoder(&rf, &cell_detect_config, &cell, NULL);
          if (ret < 0) {
            fprintf(stderr, "Error decoding MIB\n");
            exit(-1);
          }
          if (ret == SRSLTE_UE_MIB_FOUND) {
            printf("Found CELL ID %d. %d PRB, %d ports\n", 
                 cell.id, 
                 cell.nof_prb, 
                 cell.nof_ports);
            if (cell.nof_ports > 0) {
              memcpy(&results[n_found_cells].cell, &cell, sizeof(srslte_cell_t));
              results[n_found_cells].freq = channels[freq].fd; 
              results[n_found_cells].dl_earfcn = channels[freq].id;
              results[n_found_cells].power = found_cells[i].peak;
              n_found_cells++;
            }
          }          
        }
      }
    }    
  }
  
  printf("\n\nFound %d cells\n", n_found_cells);
  for (int i=0;i<n_found_cells;i++) {
    printf("Found CELL %.1f MHz, EARFCN=%d, PHYID=%d, %d PRB, %d ports, PSS power=%.1f dBm\n", 
           results[i].freq,
           results[i].dl_earfcn,
           results[i].cell.id, 
           results[i].cell.nof_prb, 
           results[i].cell.nof_ports, 
           10*log10(results[i].power));

  }
  
  printf("\nBye\n");
    
  srslte_rf_close(&rf);
  exit(0);
}


