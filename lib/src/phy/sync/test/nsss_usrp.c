/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srslte/phy/io/filesink.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/sync/nsss.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

char*    rf_args = "";
float    rf_gain = 40.0, rf_freq = -1.0;
int      nof_frames   = -1;
uint32_t fft_size     = 128;
float    threshold    = 0.4;
bool     has_cfo_corr = true;
float    cfo_fixed    = 0.0;

srslte_nbiot_cell_t cell = {
    .base       = {.nof_prb = 1, .cp = SRSLTE_CP_NORM, .nof_ports = 1, .id = 0},
    .nbiot_prb  = 0,
    .n_id_ncell = 0,
};

void usage(char* prog)
{
  printf("Usage: %s [adgtvnp] -f rx_frequency_hz -i cell_id\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-C Disable CFO correction [Default %s]\n", has_cfo_corr ? "Enabled" : "Disabled");
  printf("\t-c Manual CFO offset [Default %.0f Hz]\n", cfo_fixed);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-t threshold [Default %.2f]\n", threshold);
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "agCctvfi")) != -1) {
    switch (opt) {
      case 'a':
        rf_args = argv[optind];
        break;
      case 'C':
        has_cfo_corr = false;
        break;
      case 'c':
        cfo_fixed = strtof(argv[optind], NULL);
        break;
      case 'g':
        rf_gain = strtof(argv[optind], NULL);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 't':
        threshold = strtof(argv[optind], NULL);
        break;
      case 'i':
        cell.n_id_ncell = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_frames = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (rf_freq < 0) {
    usage(argv[0]);
    exit(-1);
  }
}

bool go_exit = false;
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}

int main(int argc, char** argv)
{
  cf_t*               buffer;
  int                 n;
  srslte_rf_t         rf;
  srslte_nsss_synch_t nsss;
  float               nsss_peak_value;
  parse_args(argc, argv);

  signal(SIGINT, sig_int_handler);

  float srate     = 15000.0 * fft_size;
  int   input_len = srate * 10 / 1000 * 2; // capture two full frames to make sure we have one NSSS

  printf("Input length %d samples\n", input_len);

  printf("Opening RF device...\n");
  if (srslte_rf_open(&rf, rf_args)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }

  printf("Set RX rate: %.2f MHz\n", srslte_rf_set_rx_srate(&rf, srate) / 1000000);
  printf("Set RX gain: %.1f dB\n", srslte_rf_set_rx_gain(&rf, rf_gain));
  printf("Set RX freq: %.2f MHz\n", srslte_rf_set_rx_freq(&rf, 0, rf_freq) / 1000000);

  buffer = srslte_vec_cf_malloc(input_len * 2);
  if (!buffer) {
    perror("malloc");
    exit(-1);
  }

  if (srslte_nsss_synch_init(&nsss, input_len, fft_size)) {
    fprintf(stderr, "Error initializing NSSS object\n");
    exit(-1);
  }

  srslte_rf_start_rx_stream(&rf, false);

  printf("Receiving two full frames ..\n");
  n = srslte_rf_recv(&rf, buffer, input_len, 1);
  if (n != input_len) {
    fprintf(stderr, "Error receiving samples\n");
    exit(-1);
  }
  srslte_rf_close(&rf);

  // perform CFO correction
  if (has_cfo_corr) {
    srslte_cfo_t cfocorr;
    if (srslte_cfo_init(&cfocorr, input_len)) {
      fprintf(stderr, "Error initiating CFO\n");
      exit(-1);
    }
    srslte_cfo_set_tol(&cfocorr, 50.0 / (15000.0 * fft_size));
    srslte_cfo_correct(&cfocorr, buffer, buffer, -cfo_fixed / (15000 * fft_size));
    srslte_cfo_free(&cfocorr);
  }

  // try to find NSSS
  printf("Detecting cell id ..\n");
  uint32_t cell_id = SRSLTE_CELL_ID_UNKNOWN;
  uint32_t sfn_partial;
  srslte_nsss_sync_find(&nsss, buffer, &nsss_peak_value, &cell_id, &sfn_partial);
  printf("Cell id: %d, peak_value=%f\n", cell_id, nsss_peak_value);

  srslte_nsss_synch_free(&nsss);
  free(buffer);

  printf("Ok\n");
  exit(0);
}
