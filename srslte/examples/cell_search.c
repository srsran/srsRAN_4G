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

#include "srslte/cuhd/cuhd_utils.h"


#ifndef DISABLE_UHD
#include "srslte/cuhd/cuhd.h"
#endif

#define MHZ             1000000
#define SAMP_FREQ       1920000
#define FLEN            9600
#define FLEN_PERIOD     0.005

#define MAX_EARFCN 1000


int band = -1;
int earfcn_start=-1, earfcn_end = -1;

cell_search_cfg_t config = {
  50,   // maximum number of 5ms frames to capture for MIB decoding
  50,   // maximum number of 5ms frames to capture for PSS correlation
  4.0,   // early-stops cell detection if mean PSR is above this value
  0     // 0 or negative to disable AGC 
}; 

struct cells {
  srslte_cell_t cell;
  float freq; 
  int dl_earfcn;
  float power;
};
struct cells results[1024]; 

float uhd_gain = 70.0;
char *uhd_args=""; 

void usage(char *prog) {
  printf("Usage: %s [agsendtvb] -b band\n", prog);
  printf("\t-a UHD args [Default %s]\n", uhd_args);
  printf("\t-g UHD gain [Default %.2f dB]\n", uhd_gain);
  printf("\t-s earfcn_start [Default All]\n");
  printf("\t-e earfcn_end [Default All]\n");
  printf("\t-n nof_frames_total [Default 100]\n");
  printf("\t-t threshold [Default %.2f]\n",config.threshold);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "agsendtvb")) != -1) {
    switch(opt) {
    case 'a':
      uhd_args = argv[optind];
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
      config.max_frames_pss = atoi(argv[optind]);
      break;
    case 't':
      config.threshold = atof(argv[optind]);
      break;
    case 'g':
      uhd_gain = atof(argv[optind]);
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

int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples, srslte_timestamp_t *t) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}

bool go_exit = false; 

void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}

int main(int argc, char **argv) {
  int n; 
  void *uhd;
  srslte_ue_cellsearch_t cs; 
  srslte_ue_cellsearch_result_t found_cells[3]; 
  int nof_freqs; 
  srslte_earfcn_t channels[MAX_EARFCN];
  uint32_t freq;
  uint32_t n_found_cells=0;
  
  parse_args(argc, argv);
    
  if (!config.init_agc) {
    printf("Opening UHD device...\n");
    if (cuhd_open(uhd_args, &uhd)) {
      fprintf(stderr, "Error opening uhd\n");
      exit(-1);
    }  
    cuhd_set_rx_gain(uhd, uhd_gain);
  } else {
    printf("Opening UHD device with threaded RX Gain control ...\n");
    if (cuhd_open_th(uhd_args, &uhd, false)) {
      fprintf(stderr, "Error opening uhd\n");
      exit(-1);
    }
    cuhd_set_rx_gain(uhd, 50);      
  }

  cuhd_set_master_clock_rate(uhd, 30.72e6);        

  // Supress UHD messages
  cuhd_suppress_stdout();
  
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
  
    /* set uhd_freq */
    cuhd_set_rx_freq(uhd, (double) channels[freq].fd * MHZ);
    cuhd_rx_wait_lo_locked(uhd);
    INFO("Set uhd_freq to %.3f MHz\n", (double) channels[freq].fd * MHZ/1000000);
    
    printf("[%3d/%d]: EARFCN %d Freq. %.2f MHz looking for PSS.\n", freq, nof_freqs,
                      channels[freq].id, channels[freq].fd);fflush(stdout);
    
    if (SRSLTE_VERBOSE_ISINFO()) {
      printf("\n");
    }
      
    bzero(found_cells, 3*sizeof(srslte_ue_cellsearch_result_t));
      
    if (srslte_ue_cellsearch_init(&cs, cuhd_recv_wrapper, uhd)) {
      fprintf(stderr, "Error initiating UE cell detect\n");
      exit(-1);
    }
    
    if (config.max_frames_pss) {
      srslte_ue_cellsearch_set_nof_frames_to_scan(&cs, config.max_frames_pss);
    }
    if (config.threshold) {
      srslte_ue_cellsearch_set_threshold(&cs, config.threshold);
    }
    if (config.init_agc) {
      srslte_ue_sync_start_agc(&cs.ue_sync, cuhd_set_rx_gain, config.init_agc);    
    }

    INFO("Setting sampling frequency %.2f MHz for PSS search\n", SRSLTE_CS_SAMP_FREQ/1000000);
    cuhd_set_rx_srate(uhd, SRSLTE_CS_SAMP_FREQ);
    INFO("Starting receiver...\n", 0);
    cuhd_start_rx_stream(uhd);
    
    n = srslte_ue_cellsearch_scan(&cs, found_cells, NULL); 
    if (n < 0) {
      fprintf(stderr, "Error searching cell\n");
      exit(-1);
    } else if (n > 0) {
      for (int i=0;i<3;i++) {
        if (found_cells[i].psr > config.threshold/2) {
          srslte_cell_t cell;
          cell.id = found_cells[i].cell_id; 
          cell.cp = found_cells[i].cp; 
          int ret = cuhd_mib_decoder(uhd, &config, &cell);
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
    
  cuhd_close(uhd);
  exit(0);
}


