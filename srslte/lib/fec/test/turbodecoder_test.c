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
#include <time.h>

#include <sys/time.h>
#include <time.h>
#include "srslte/srslte.h"

#include "turbodecoder_test.h"



uint32_t frame_length = 1000, nof_frames = 100;
float ebno_db = 100.0;
uint32_t seed = 0;
int K = -1;

#define MAX_ITERATIONS  4
int nof_iterations = MAX_ITERATIONS;
int test_known_data = 0;
int test_errors = 0;

#define SNR_POINTS      8
#define SNR_MIN         0.0
#define SNR_MAX         4.0

void usage(char *prog) {
  printf("Usage: %s [nlesv]\n", prog);
  printf(
      "\t-k Test with known data (ignores frame_length) [Default disabled]\n");
  printf("\t-i nof_iterations [Default %d]\n", nof_iterations);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-l frame_length [Default %d]\n", frame_length);
  printf("\t-e ebno in dB [Default scan]\n");
  printf("\t-t test: check errors on exit [Default disabled]\n");
  printf("\t-s seed [Default 0=time]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "inlstvekt")) != -1) {
    switch (opt) {
    case 'n':
      nof_frames = atoi(argv[optind]);
      break;
    case 'k':
      test_known_data = 1;
      break;
    case 't':
      test_errors = 1;
      break;
    case 'i':
      nof_iterations = atoi(argv[optind]);
      break;
    case 'l':
      frame_length = atoi(argv[optind]);
      break;
    case 'e':
      ebno_db = atof(argv[optind]);
      break;
    case 's':
      seed = (uint32_t) strtoul(argv[optind], NULL, 0);
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

void output_matlab(float ber[MAX_ITERATIONS][SNR_POINTS], int snr_points) {
  int i, j;
  FILE *f = fopen("turbocoder_snr.m", "w");
  if (!f) {
    perror("fopen");
    exit(-1);
  }
  fprintf(f, "ber=[");
  for (j = 0; j < MAX_ITERATIONS; j++) {
    for (i = 0; i < snr_points; i++) {
      fprintf(f, "%g ", ber[j][i]);
    }
    fprintf(f, ";\n");
  }
  fprintf(f, "];\n");
  fprintf(f, "snr=linspace(%g,%g-%g/%d,%d);\n", SNR_MIN, SNR_MAX, SNR_MAX,
      snr_points, snr_points);
  fprintf(f, "semilogy(snr,ber,snr,0.5*erfc(sqrt(10.^(snr/10))));\n");
  fprintf(f,
      "legend('1 iter','2 iter', '3 iter', '4 iter', 'theory-uncoded');");
  fprintf(f, "grid on;\n");
  fclose(f);
}

int main(int argc, char **argv) {
  uint32_t frame_cnt;
  float *llr;
  uint8_t *llr_c;
  uint8_t *data_tx, *data_rx, *symbols;
  uint32_t i, j;
  float var[SNR_POINTS];
  uint32_t snr_points;
  float ber[MAX_ITERATIONS][SNR_POINTS];
  uint32_t errors[100];
  uint32_t coded_length;
  struct timeval tdata[3];
  float mean_usec;
  srslte_tdec_t tdec;
  srslte_tcod_t tcod;

  parse_args(argc, argv);

  if (!seed) {
    seed = time(NULL);
  }
  srand(seed);

  if (test_known_data) {
    frame_length = KNOWN_DATA_LEN;
  } else {
    frame_length = srslte_cb_size(srslte_find_cb_index(frame_length));
  }

  coded_length = 3 * (frame_length) + SRSLTE_TCOD_TOTALTAIL;

  printf("  Frame length: %d\n", frame_length);
  if (ebno_db < 100.0) {
    printf("  EbNo: %.2f\n", ebno_db);
  }

  data_tx = srslte_vec_malloc(frame_length * sizeof(uint8_t));
  if (!data_tx) {
    perror("malloc");
    exit(-1);
  }

  data_rx = srslte_vec_malloc(frame_length * sizeof(uint8_t));
  if (!data_rx) {
    perror("malloc");
    exit(-1);
  }

  symbols = srslte_vec_malloc(coded_length * sizeof(uint8_t));
  if (!symbols) {
    perror("malloc");
    exit(-1);
  }
  llr = srslte_vec_malloc(coded_length * sizeof(float));
  if (!llr) {
    perror("malloc");
    exit(-1);
  }
  llr_c = srslte_vec_malloc(coded_length * sizeof(uint8_t));
  if (!llr_c) {
    perror("malloc");
    exit(-1);
  }

  if (srslte_tcod_init(&tcod, frame_length)) {
    fprintf(stderr, "Error initiating Turbo coder\n");
    exit(-1);
  }

  if (srslte_tdec_init(&tdec, frame_length)) {
    fprintf(stderr, "Error initiating Turbo decoder\n");
    exit(-1);
  }

  float ebno_inc, esno_db;
  ebno_inc = (SNR_MAX - SNR_MIN) / SNR_POINTS;
  if (ebno_db == 100.0) {
    snr_points = SNR_POINTS;
    for (i = 0; i < snr_points; i++) {
      ebno_db = SNR_MIN + i * ebno_inc;
      esno_db = ebno_db + 10 * log10((double) 1 / 3);
      var[i] = sqrt(1 / (pow(10, esno_db / 10)));
    }
  } else {
    esno_db = ebno_db + 10 * log10((double) 1 / 3);
    var[0] = sqrt(1 / (pow(10, esno_db / 10)));
    snr_points = 1;
  }
  for (i = 0; i < snr_points; i++) {
    mean_usec = 0;
    frame_cnt = 0;
    bzero(errors, sizeof(int) * MAX_ITERATIONS);
    while (frame_cnt < nof_frames) {

      /* generate data_tx */
      for (j = 0; j < frame_length; j++) {
        if (test_known_data) {
          data_tx[j] = known_data[j];
        } else {
          data_tx[j] = rand() % 2;
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
        llr[j] = symbols[j] ? sqrt(2) : -sqrt(2);
      }

      srslte_ch_awgn_f(llr, llr, var[i], coded_length);

      /* decoder */
      srslte_tdec_reset(&tdec, frame_length);

      uint32_t t;
      if (nof_iterations == -1) {
        t = MAX_ITERATIONS;
      } else {
        t = nof_iterations;
      }
      for (j = 0; j < t; j++) {

        if (!j)
          gettimeofday(&tdata[1], NULL); // Only measure 1 iteration
        srslte_tdec_iteration(&tdec, llr, frame_length);
        srslte_tdec_decision(&tdec, data_rx, frame_length);
        if (!j)
          gettimeofday(&tdata[2], NULL);
        if (!j)
          get_time_interval(tdata);
        if (!j)
          mean_usec = (float) mean_usec * 0.9 + (float) tdata[0].tv_usec * 0.1;

        /* check errors */
        errors[j] += srslte_bit_diff(data_tx, data_rx, frame_length);
        if (j < MAX_ITERATIONS) {
          ber[j][i] = (float) errors[j] / (frame_cnt * frame_length);
        }
      }
      frame_cnt++;
      printf("Eb/No: %3.2f %10d/%d   ",
      SNR_MIN + i * ebno_inc, frame_cnt, nof_frames);
      printf("BER: %.2e  ", (float) errors[j - 1] / (frame_cnt * frame_length));
      printf("%3.1f Mbps (%6.2f usec)", (float) frame_length / mean_usec,
          mean_usec);
      printf("\r");

    }
    printf("\n");

    if (snr_points == 1) {
      if (test_known_data && seed == KNOWN_DATA_SEED
          && ebno_db == KNOWN_DATA_EBNO && frame_cnt == KNOWN_DATA_NFRAMES) {
        for (j = 0; j < MAX_ITERATIONS; j++) {
          if (errors[j] > known_data_errors[j]) {
            fprintf(stderr, "Expected %d errors but got %d\n",
                known_data_errors[j], errors[j]);
            exit(-1);
          } else {
            printf("Iter %d ok\n", j + 1);
          }
        }
      } else {
        for (j = 0; j < MAX_ITERATIONS; j++) {
          printf("BER: %g\t%u errors\n",
              (float) errors[j] / (frame_cnt * frame_length), errors[j]);
          if (test_errors) {
            if (errors[j]
                > get_expected_errors(frame_cnt, seed, j + 1, frame_length,
                    ebno_db)) {
              fprintf(stderr, "Expected %d errors but got %d\n",
                  get_expected_errors(frame_cnt, seed, j + 1, frame_length,
                      ebno_db), errors[j]);
              exit(-1);
            } else {
              printf("Iter %d ok\n", j + 1);
            }
          }
        }
      }
    }
  }

  free(data_tx);
  free(symbols);
  free(llr);
  free(llr_c);
  free(data_rx);

  srslte_tdec_free(&tdec);
  srslte_tcod_free(&tcod);

  printf("\n");
  output_matlab(ber, snr_points);
  printf("Done\n");
  exit(0);
}
