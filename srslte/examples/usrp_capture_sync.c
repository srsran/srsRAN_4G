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
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <stdbool.h>

#include "srslte/srslte.h"
#include "srslte/cuhd/cuhd.h"

static bool keep_running = true;
char *output_file_name = NULL;
char *uhd_args="";
float uhd_gain=60.0, uhd_freq=-1.0;
int nof_prb = 6;
int nof_subframes = -1;
int N_id_2 = -1; 

void int_handler(int dummy) {
  keep_running = false;
}

void usage(char *prog) {
  printf("Usage: %s [agrnv] -l N_id_2 -f rx_frequency_hz -o output_file\n", prog);
  printf("\t-a UHD args [Default %s]\n", uhd_args);
  printf("\t-g UHD Gain [Default %.2f dB]\n", uhd_gain);
  printf("\t-p nof_prb [Default %d]\n", nof_prb);
  printf("\t-n nof_subframes [Default %d]\n", nof_subframes);
  printf("\t-v verbose\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "agpnvfol")) != -1) {
    switch (opt) {
    case 'o':
      output_file_name = argv[optind];
      break;
    case 'a':
      uhd_args = argv[optind];
      break;
    case 'g':
      uhd_gain = atof(argv[optind]);
      break;
    case 'p':
      nof_prb = atoi(argv[optind]);
      break;
    case 'f':
      uhd_freq = atof(argv[optind]);
      break;
    case 'n':
      nof_subframes = atoi(argv[optind]);
      break;
    case 'l':
      N_id_2 = atoi(argv[optind]);
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (uhd_freq < 0 || N_id_2 == -1 || output_file_name == NULL) {
    usage(argv[0]);
    exit(-1);
  }
}

int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples, srslte_timestamp_t *t) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}

int main(int argc, char **argv) {
  cf_t *buffer; 
  int n;
  void *uhd;
  srslte_filesink_t sink;
  srslte_ue_sync_t ue_sync; 
  srslte_cell_t cell; 

  signal(SIGINT, int_handler);

  parse_args(argc, argv);
  
  srslte_filesink_init(&sink, output_file_name, SRSLTE_COMPLEX_FLOAT_BIN);

  printf("Opening UHD device...\n");
  if (cuhd_open(uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }
  cuhd_set_master_clock_rate(uhd, 30.72e6);        

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);

  printf("Set RX freq: %.6f MHz\n", cuhd_set_rx_freq(uhd, uhd_freq) / 1000000);
  printf("Set RX gain: %.1f dB\n", cuhd_set_rx_gain(uhd, uhd_gain));
    int srate = srslte_sampling_freq_hz(nof_prb);    
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
      fprintf(stderr, "Invalid number of PRB %d\n", nof_prb);
      exit(-1);
    }
  cuhd_rx_wait_lo_locked(uhd);
  cuhd_start_rx_stream(uhd);

  cell.cp = SRSLTE_CP_NORM; 
  cell.id = N_id_2;
  cell.nof_prb = nof_prb; 
  cell.nof_ports = 1; 
  
  if (srslte_ue_sync_init(&ue_sync, cell, cuhd_recv_wrapper, uhd)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1); 
  }
 
  uint32_t subframe_count = 0;
  bool start_capture = false; 
  bool stop_capture = false; 
  while((subframe_count < nof_subframes || nof_subframes == -1)
        && !stop_capture)
  {
    n = srslte_ue_sync_get_buffer(&ue_sync, &buffer);
    if (n < 0) {
      fprintf(stderr, "Error receiving samples\n");
      exit(-1);
    }
    if (n == 1) {
      if (!start_capture) {
        if (srslte_ue_sync_get_sfidx(&ue_sync) == 9) {
          start_capture = true; 
        }        
      } else {
        printf("Writing to file %6d subframes...\r", subframe_count);
        srslte_filesink_write(&sink, buffer, SRSLTE_SF_LEN_PRB(nof_prb));
        subframe_count++;                              
      }      
    }
    if (!keep_running) {
      if (!start_capture || (start_capture && srslte_ue_sync_get_sfidx(&ue_sync) == 9)) {
        stop_capture = true; 
      }
    }
  }
  
  srslte_filesink_free(&sink);
  cuhd_close(uhd);
  srslte_ue_sync_free(&ue_sync);

  printf("Ok - wrote %d subframes\n", subframe_count);
  exit(0);
}
