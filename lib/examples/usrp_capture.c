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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include <stdbool.h>

#include "srslte/phy/io/filesink.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"

static bool keep_running = true;
char*       output_file_name;
char*       rf_args = "";
float       rf_gain = 40.0, rf_freq = -1.0, rf_rate = 0.96e6;
int         nof_samples     = -1;
int         nof_rx_antennas = 1;

void int_handler(int dummy)
{
  keep_running = false;
}

void usage(char* prog)
{
  printf("Usage: %s [agrnv] -f rx_frequency_hz -o output_file\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-r RF Rate [Default %.6f Hz]\n", rf_rate);
  printf("\t-n nof_samples [Default %d]\n", nof_samples);
  printf("\t-A nof_rx_antennas [Default %d]\n", nof_rx_antennas);
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "agrnvfoA")) != -1) {
    switch (opt) {
      case 'o':
        output_file_name = argv[optind];
        break;
      case 'a':
        rf_args = argv[optind];
        break;
      case 'g':
        rf_gain = strtof(argv[optind], NULL);
        break;
      case 'r':
        rf_rate = strtof(argv[optind], NULL);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 'n':
        nof_samples = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'A':
        nof_rx_antennas = (int)strtol(argv[optind], NULL, 10);
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

int main(int argc, char** argv)
{
  cf_t*             buffer[SRSLTE_MAX_PORTS];
  int               sample_count, n;
  srslte_rf_t       rf;
  srslte_filesink_t sink;
  uint32_t          buflen;

  signal(SIGINT, int_handler);

  parse_args(argc, argv);

  buflen       = 4800;
  sample_count = 0;

  for (int i = 0; i < nof_rx_antennas; i++) {
    buffer[i] = srslte_vec_cf_malloc(buflen);
    if (!buffer[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  srslte_filesink_init(&sink, output_file_name, SRSLTE_COMPLEX_FLOAT_BIN);

  printf("Opening RF device...\n");
  if (srslte_rf_open_multi(&rf, rf_args, nof_rx_antennas)) {
    ERROR("Error opening rf\n");
    exit(-1);
  }

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);

  srslte_rf_set_rx_gain(&rf, rf_gain);
  srslte_rf_set_rx_freq(&rf, nof_rx_antennas, rf_freq);
  printf("Set RX freq: %.2f MHz\n", rf_freq / 1000000);
  printf("Set RX gain: %.2f dB\n", rf_gain);
  float srate = srslte_rf_set_rx_srate(&rf, rf_rate);
  if (srate != rf_rate) {
    srate = srslte_rf_set_rx_srate(&rf, rf_rate);
    if (srate != rf_rate) {
      ERROR("Error setting samplign frequency %.2f MHz\n", rf_rate * 1e-6);
      exit(-1);
    }
  }

  printf("Correctly RX rate: %.2f MHz\n", srate * 1e-6);
  srslte_rf_start_rx_stream(&rf, false);

  while ((sample_count < nof_samples || nof_samples == -1) && keep_running) {
    n = srslte_rf_recv_with_time_multi(&rf, (void**)buffer, buflen, true, NULL, NULL);
    if (n < 0) {
      ERROR("Error receiving samples\n");
      exit(-1);
    }

    srslte_filesink_write_multi(&sink, (void**)buffer, buflen, nof_rx_antennas);
    sample_count += buflen;
  }

  for (int i = 0; i < nof_rx_antennas; i++) {
    if (buffer[i]) {
      free(buffer[i]);
    }
  }

  srslte_filesink_free(&sink);
  srslte_rf_close(&rf);

  printf("Ok - wrote %d samples\n", sample_count);
  exit(0);
}
