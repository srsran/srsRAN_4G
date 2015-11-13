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
#include "srslte/io/filesink.h"

static bool keep_running = true;
char *output_file_name;
char *uhd_args="";
float uhd_gain=40.0, uhd_freq=-1.0, uhd_rate=0.96e6;
int nof_samples = -1;

void int_handler(int dummy) {
  keep_running = false;
}

void usage(char *prog) {
  printf("Usage: %s [agrnv] -f rx_frequency_hz -o output_file\n", prog);
  printf("\t-a UHD args [Default %s]\n", uhd_args);
  printf("\t-g UHD Gain [Default %.2f dB]\n", uhd_gain);
  printf("\t-r UHD Rate [Default %.6f Hz]\n", uhd_rate);
  printf("\t-n nof_samples [Default %d]\n", nof_samples);
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "agrnvfo")) != -1) {
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
    case 'r':
      uhd_rate = atof(argv[optind]);
      break;
    case 'f':
      uhd_freq = atof(argv[optind]);
      break;
    case 'n':
      nof_samples = atoi(argv[optind]);
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (uhd_freq < 0) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char **argv) {
  cf_t *buffer; 
  int sample_count, n;
  void *uhd;
  srslte_filesink_t sink;
  int32_t buflen;

  signal(SIGINT, int_handler);

  parse_args(argc, argv);
  
  buflen = 4800;
  sample_count = 0;
  
  buffer = malloc(sizeof(cf_t) * buflen);
  if (!buffer) {
    perror("malloc");
    exit(-1);
  }

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

  printf("Set RX freq: %.2f MHz\n", cuhd_set_rx_freq(uhd, uhd_freq) / 1000000);
  printf("Set RX gain: %.2f dB\n", cuhd_set_rx_gain(uhd, uhd_gain));
  float srate = cuhd_set_rx_srate(uhd, uhd_rate); 
  if (srate != uhd_rate) {
    if (srate < 10e6) {          
      cuhd_set_master_clock_rate(uhd, 4*uhd_rate);        
    } else {
      cuhd_set_master_clock_rate(uhd, uhd_rate);        
    }
    srate = cuhd_set_rx_srate(uhd, uhd_rate);
    if (srate != uhd_rate) {
      fprintf(stderr, "Errror setting samplign frequency %.2f MHz\n", uhd_rate*1e-6);
      exit(-1);
    }
  }

  printf("Correctly RX rate: %.2f MHz\n", srate*1e-6);
  cuhd_rx_wait_lo_locked(uhd);
  cuhd_start_rx_stream(uhd);
  
  
  while((sample_count < nof_samples || nof_samples == -1)
        && keep_running){
    n = cuhd_recv(uhd, buffer, buflen, 1);
    if (n < 0) {
      fprintf(stderr, "Error receiving samples\n");
      exit(-1);
    }
    
    srslte_filesink_write(&sink, buffer, buflen);
    sample_count += buflen;
  }
  
  srslte_filesink_free(&sink);
  free(buffer);
  cuhd_close(uhd);

  printf("Ok - wrote %d samples\n", sample_count);
  exit(0);
}
