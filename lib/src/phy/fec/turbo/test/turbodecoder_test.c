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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srslte/srslte.h"
#include <srslte/phy/utils/random.h>
#include <sys/time.h>
#include <time.h>

#include "turbodecoder_test.h"

uint32_t frame_length = 1000, nof_frames = 100;
float    ebno_db = 100.0;
uint32_t seed    = 0;
int      K       = -1;

#define MAX_ITERATIONS 10
int nof_cb          = 1;
int nof_iterations  = MAX_ITERATIONS;
int test_known_data = 0;
int test_errors     = 0;
int nof_repetitions = 1;

srslte_tdec_impl_type_t tdec_type;

#define SNR_POINTS 4
#define SNR_MIN 1.0
#define SNR_MAX 8.0

void usage(char* prog)
{
  printf("Usage: %s [kcinNledts]\n", prog);
  printf("\t-k Test with known data (ignores frame_length) [Default disabled]\n");
  printf("\t-c nof_cb in parallel [Default %d]\n", nof_cb);
  printf("\t-i nof_iterations [Default %d]\n", nof_iterations);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-N nof_repetitions [Default %d]\n", nof_repetitions);
  printf("\t-l frame_length [Default %d]\n", frame_length);
  printf("\t-e ebno in dB [Default scan]\n");
  printf("\t-d Decoder implementation type: 0: Generic, 1: SSE, 2: SSE-window\n");
  printf("\t-t test: check errors on exit [Default disabled]\n");
  printf("\t-s seed [Default 0=time]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "kcinNledts")) != -1) {
    switch (opt) {
      case 'c':
        nof_cb = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_frames = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'N':
        nof_repetitions = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'k':
        test_known_data = 1;
        break;
      case 't':
        test_errors = 1;
        break;
      case 'i':
        nof_iterations = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'l':
        frame_length = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        tdec_type = (srslte_tdec_impl_type_t)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        ebno_db = strtof(argv[optind], NULL);
        break;
      case 's':
        seed = (uint32_t)strtoul(argv[optind], NULL, 0);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  srslte_random_t random_gen = srslte_random_init(0);
  uint32_t        frame_cnt;
  float*          llr;
  short*          llr_s;
  uint8_t*        llr_c;
  uint8_t *       data_tx, *data_rx, *data_rx_bytes, *symbols;
  uint32_t        i, j;
  float           var[SNR_POINTS];
  uint32_t        snr_points;
  uint32_t        errors = 0;
  uint32_t        coded_length;
  struct timeval  tdata[3];
  float           mean_usec;
  srslte_tdec_t   tdec;
  srslte_tcod_t   tcod;

  parse_args(argc, argv);

  if (!seed) {
    seed = time(NULL);
  }
  srand(seed);

  if (test_known_data) {
    frame_length = KNOWN_DATA_LEN;
  } else {
    frame_length = srslte_cbsegm_cbsize(srslte_cbsegm_cbindex(frame_length));
  }

  coded_length = 3 * (frame_length) + SRSLTE_TCOD_TOTALTAIL;

  printf("  Frame length: %d\n", frame_length);
  if (ebno_db < 100.0) {
    printf("  EbNo: %.2f\n", ebno_db);
  }

  data_tx = srslte_vec_u8_malloc(frame_length);
  if (!data_tx) {
    perror("malloc");
    exit(-1);
  }

  data_rx = srslte_vec_u8_malloc(frame_length);
  if (!data_rx) {
    perror("malloc");
    exit(-1);
  }
  data_rx_bytes = srslte_vec_u8_malloc(frame_length);
  if (!data_rx_bytes) {
    perror("malloc");
    exit(-1);
  }

  symbols = srslte_vec_u8_malloc(coded_length);
  if (!symbols) {
    perror("malloc");
    exit(-1);
  }
  llr = srslte_vec_f_malloc(coded_length);
  if (!llr) {
    perror("malloc");
    exit(-1);
  }
  llr_s = srslte_vec_i16_malloc(coded_length);
  if (!llr_s) {
    perror("malloc");
    exit(-1);
  }
  llr_c = srslte_vec_u8_malloc(coded_length);
  if (!llr_c) {
    perror("malloc");
    exit(-1);
  }

  if (srslte_tcod_init(&tcod, frame_length)) {
    ERROR("Error initiating Turbo coder\n");
    exit(-1);
  }

#ifdef HAVE_NEON
  tdec_type = SRSLTE_TDEC_NEON_WINDOW;
#else
  // tdec_type = SRSLTE_TDEC_SSE_WINDOW;
#endif
  if (srslte_tdec_init_manual(&tdec, frame_length, tdec_type)) {
    ERROR("Error initiating Turbo decoder\n");
    exit(-1);
  }

  srslte_tdec_force_not_sb(&tdec);

  float ebno_inc, esno_db;
  ebno_inc = (SNR_MAX - SNR_MIN) / SNR_POINTS;
  if (ebno_db == 100.0) {
    snr_points = SNR_POINTS;
    for (i = 0; i < snr_points; i++) {
      ebno_db = SNR_MIN + i * ebno_inc;
      esno_db = ebno_db + srslte_convert_power_to_dB(1.0f / 3.0f);
      var[i]  = srslte_convert_dB_to_amplitude(-esno_db);
    }
  } else {
    esno_db    = ebno_db + srslte_convert_power_to_dB(1.0f / 3.0f);
    var[0]     = srslte_convert_dB_to_amplitude(-esno_db);
    snr_points = 1;
  }
  for (i = 0; i < snr_points; i++) {

    mean_usec = 0;
    errors    = 0;
    frame_cnt = 0;
    while (frame_cnt < nof_frames) {
      /* generate data_tx */
      for (j = 0; j < frame_length; j++) {
        if (test_known_data) {
          data_tx[j] = known_data[j];
        } else {
          data_tx[j] = srslte_random_uniform_int_dist(random_gen, 0, 1);
        }
      }

      /* coded BER */
      if (test_known_data) {
        for (j = 0; j < coded_length; j++) {
          symbols[j] = known_data_encoded[j];
        }
      } else {
        srslte_tcod_encode(&tcod, data_tx, symbols, frame_length);
      }

      for (j = 0; j < coded_length; j++) {
        llr[j] = symbols[j] ? 1 : -1;
      }
      srslte_ch_awgn_f(llr, llr, var[i], coded_length);

      for (j = 0; j < coded_length; j++) {
        llr_s[j] = (int16_t)(100 * llr[j]);
      }

      /* decoder */
      srslte_tdec_new_cb(&tdec, frame_length);

      uint32_t t;
      if (nof_iterations == -1) {
        t = MAX_ITERATIONS;
      } else {
        t = nof_iterations;
      }

      gettimeofday(&tdata[1], NULL);
      for (int k = 0; k < nof_repetitions; k++) {
        srslte_tdec_run_all(&tdec, llr_s, data_rx_bytes, t, frame_length);
      }
      gettimeofday(&tdata[2], NULL);
      get_time_interval(tdata);
      mean_usec = (tdata[0].tv_sec * 1e6 + tdata[0].tv_usec) / nof_repetitions;

      frame_cnt++;
      uint32_t errors_this = 0;
      srslte_bit_unpack_vector(data_rx_bytes, data_rx, frame_length);

      errors_this = srslte_bit_diff(data_tx, data_rx, frame_length);
      // printf("error[%d]=%d\n", cb, errors_this);
      errors += errors_this;
      printf("Eb/No: %2.2f %10d/%d   ", SNR_MIN + i * ebno_inc, frame_cnt, nof_frames);
      printf("BER: %.2e  ", (float)errors / (nof_cb * frame_cnt * frame_length));
      printf("%3.1f Mbps (%6.2f usec)", (float)(nof_cb * frame_length) / mean_usec, mean_usec);
      printf("\r");
    }
    printf("\n");
  }

  printf("\n");
  if (snr_points == 1) {
    if (errors) {
      printf("%d Errors\n", errors / nof_cb);
    }
  }

  free(data_rx_bytes);
  free(data_tx);
  free(symbols);
  free(llr);
  free(llr_c);
  free(llr_s);
  free(data_rx);

  srslte_tdec_free(&tdec);
  srslte_tcod_free(&tcod);
  srslte_random_free(random_gen);

  printf("\n");
  printf("Done\n");
  exit(0);
}
