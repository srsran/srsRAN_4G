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

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"

uint32_t nof_prb    = 25;
uint32_t nof_frames = 20;

int   time_adv_samples = 0;
float tone_offset_hz   = 1e6;
float rf_rx_gain = 40, srslte_rf_tx_gain = 40, rf_freq = 2.4e9;
char* rf_args         = "";
char* output_filename = NULL;
char* input_filename  = NULL;

void usage(char* prog)
{
  printf("Usage: %s -o [rx_signal_file]\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-f RF TX/RX frequency [Default %.2f MHz]\n", rf_freq / 1e6);
  printf("\t-g RF RX gain [Default %.1f dB]\n", rf_rx_gain);
  printf("\t-G RF TX gain [Default %.1f dB]\n", srslte_rf_tx_gain);
  printf("\t-t Single tone offset (Hz) [Default %f]\n", tone_offset_hz);
  printf("\t-T Time advance samples [Default %d]\n", time_adv_samples);
  printf("\t-i File name to read signal from [Default single tone]\n");
  printf("\t-p Number of UL RB [Default %d]\n", nof_prb);
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "ioafgGptT")) != -1) {
    switch (opt) {
      case 'a':
        rf_args = argv[optind];
        break;
      case 'o':
        output_filename = argv[optind];
        break;
      case 'i':
        input_filename = argv[optind];
        break;
      case 't':
        tone_offset_hz = strtof(argv[optind], NULL);
        break;
      case 'T':
        time_adv_samples = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 'g':
        rf_rx_gain = strtof(argv[optind], NULL);
        break;
      case 'G':
        srslte_rf_tx_gain = strtof(argv[optind], NULL);
        break;
      case 'p':
        nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        if (!srslte_nofprb_isvalid(nof_prb)) {
          ERROR("Invalid number of UL RB %d\n", nof_prb);
          exit(-1);
        }
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (!output_filename) {
    usage(argv[0]);
    exit(-1);
  }
  if (time_adv_samples < 0) {
    printf("Time advance must be positive\n");
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  uint32_t flen = srslte_sampling_freq_hz(nof_prb) / 1000;

  cf_t* rx_buffer = srslte_vec_cf_malloc(flen * nof_frames);
  if (!rx_buffer) {
    perror("malloc");
    exit(-1);
  }

  cf_t* tx_buffer = srslte_vec_cf_malloc((flen + time_adv_samples));
  if (!tx_buffer) {
    perror("malloc");
    exit(-1);
  }
  srslte_vec_cf_zero(tx_buffer, flen + time_adv_samples);

  cf_t* zeros = srslte_vec_cf_malloc(flen);
  if (!zeros) {
    perror("calloc");
    exit(-1);
  }
  srslte_vec_cf_zero(zeros, flen);

  float time_adv_sec = (float)time_adv_samples / srslte_sampling_freq_hz(nof_prb);

  // Send through RF
  srslte_rf_t rf;
  printf("Opening RF device...\n");
  if (srslte_rf_open(&rf, rf_args)) {
    ERROR("Error opening rf\n");
    exit(-1);
  }

  int srate = srslte_sampling_freq_hz(nof_prb);
  srslte_rf_set_rx_srate(&rf, (double)srate);
  srslte_rf_set_tx_srate(&rf, (double)srate);

  srslte_rf_set_rx_gain(&rf, rf_rx_gain);
  srslte_rf_set_tx_gain(&rf, srslte_rf_tx_gain);
  srslte_rf_set_rx_freq(&rf, 0, rf_freq);

  printf("Subframe len:   %d samples\n", flen);
  printf("Time advance:   %f us\n", time_adv_sec * 1e6);
  printf("Set TX/RX rate: %.2f MHz\n", (float)srate / 1000000);
  printf("Set RX gain:    %.1f dB\n", rf_rx_gain);
  printf("Set TX gain:    %.1f dB\n", srslte_rf_tx_gain);
  printf("Set TX/RX freq: %.2f MHz\n", rf_freq / 1000000);
  srslte_rf_set_tx_freq(&rf, 0, rf_freq);

  sleep(1);

  if (input_filename) {
    srslte_vec_load_file(input_filename, &tx_buffer[time_adv_samples], flen * sizeof(cf_t));
  } else {
    for (int i = 0; i < flen - time_adv_samples; i++) {
      tx_buffer[i + time_adv_samples] = 0.3 * cexpf(_Complex_I * 2 * M_PI * tone_offset_hz * ((float)i / (float)srate));
    }
    srslte_vec_save_file("srslte_rf_txrx_tone", tx_buffer, flen * sizeof(cf_t));
  }

  srslte_timestamp_t tstamp;

  srslte_rf_start_rx_stream(&rf, false);
  uint32_t nframe = 0;

  while (nframe < nof_frames) {
    printf("Rx subframe %d\n", nframe);
    srslte_rf_recv_with_time(&rf, &rx_buffer[flen * nframe], flen, true, &tstamp.full_secs, &tstamp.frac_secs);
    nframe++;
    if (nframe == 9) {
      srslte_timestamp_add(&tstamp, 0, 2e-3 - time_adv_sec);
      srslte_rf_send_timed2(&rf, tx_buffer, flen + time_adv_samples, tstamp.full_secs, tstamp.frac_secs, true, true);
      printf("Transmitting Signal\n");
    }
  }

  srslte_vec_save_file(output_filename, &rx_buffer[10 * flen], flen * sizeof(cf_t));

  free(tx_buffer);
  free(rx_buffer);

  printf("Done\n");
  exit(0);
}
