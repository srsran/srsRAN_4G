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

#include "srslte/srslte.h"

int nof_prb = -1;
srslte_cp_t cp = SRSLTE_CP_NORM;
int nof_repetitions = 128;

static double elapsed_us(struct timeval *ts_start, struct timeval *ts_end) {
  if (ts_end->tv_usec > ts_start->tv_usec) {
    return ((double) ts_end->tv_sec - (double) ts_start->tv_sec) * 1000000 +
           (double) ts_end->tv_usec - (double) ts_start->tv_usec;
  } else {
    return ((double) ts_end->tv_sec - (double) ts_start->tv_sec - 1) * 1000000 +
           ((double) ts_end->tv_usec + 1000000) - (double) ts_start->tv_usec;
  }
}

void usage(char *prog) {
  printf("Usage: %s\n", prog);
  printf("\t-n nof_prb [Default All]\n");
  printf("\t-e extended cyclic prefix [Default Normal]\n");
  printf("\t-r nof_repetitions [Default %d]\n", nof_repetitions);
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "ner")) != -1) {
    switch (opt) {
    case 'n':
      nof_prb = atoi(argv[optind]);
      break;
    case 'e':
      cp = SRSLTE_CP_EXT;
      break;
    case 'r':
      nof_repetitions = atoi(argv[optind]);
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}


int main(int argc, char **argv) {
  struct timeval start, end;
  srslte_ofdm_t fft, ifft;
  cf_t *input, *outfft, *outifft;
  float mse;
  int n_prb, max_prb, n_re;
  int i;

  parse_args(argc, argv);

  if (nof_prb == -1) {
    n_prb = 6;
    max_prb = 100;
  } else {
    n_prb = nof_prb;
    max_prb = nof_prb;
  }
  while(n_prb <= max_prb) {
    n_re = SRSLTE_CP_NSYMB(cp) * n_prb * SRSLTE_NRE;

    printf("Running test for %d PRB, %d RE... ", n_prb, n_re);fflush(stdout);

    input = srslte_vec_malloc(sizeof(cf_t) * n_re * 2);
    if (!input) {
      perror("malloc");
      exit(-1);
    }
    outfft = srslte_vec_malloc(sizeof(cf_t) * n_re * 2);
    if (!outfft) {
      perror("malloc");
      exit(-1);
    }
    outifft = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SLOT_LEN(srslte_symbol_sz(n_prb)) * 2);
    if (!outifft) {
      perror("malloc");
      exit(-1);
    }
    bzero(outifft, sizeof(cf_t) * SRSLTE_SLOT_LEN(srslte_symbol_sz(n_prb)) * 2);

    if (srslte_ofdm_rx_init(&fft, cp, outifft, outfft, n_prb)) {
      fprintf(stderr, "Error initializing FFT\n");
      exit(-1);
    }
    srslte_ofdm_set_normalize(&fft, true);

    if (srslte_ofdm_tx_init(&ifft, cp, input, outifft, n_prb)) {
      fprintf(stderr, "Error initializing iFFT\n");
      exit(-1);
    }
    srslte_ofdm_set_normalize(&ifft, true);

    for (i=0;i<n_re;i++) {
      input[i] = 100 * ((float) rand() / (float) RAND_MAX + I * ((float) rand() / (float) RAND_MAX));
      //input[i] = 100;
    }

  gettimeofday(&start, NULL);
  for (int i = 0; i < nof_repetitions; i++) {
      srslte_ofdm_tx_slot(&ifft, 0);
  }
  gettimeofday(&end, NULL);\
  printf(" Tx@%.1fMsps", (float)(SRSLTE_SLOT_LEN(srslte_symbol_sz(n_prb))*nof_repetitions)/elapsed_us(&start, &end));

  gettimeofday(&start, NULL);
  for (int i = 0; i < nof_repetitions; i++) {
    srslte_ofdm_rx_slot(&fft, 0);
  }
  gettimeofday(&end, NULL);\
  printf(" Rx@%.1fMsps", (float)(SRSLTE_SLOT_LEN(srslte_symbol_sz(n_prb))*nof_repetitions)/elapsed_us(&start, &end));

    /* compute MSE */
    mse = 0.0f;
    for (i=0;i<n_re;i++) {
      cf_t error = input[i] - outfft[i];
      mse += (__real__ error * __real__ error + __imag__ error * __imag__ error)/cabsf(input[i]);
      if (mse > 1.0f) printf("%04d. %+.1f%+.1fi Vs. %+.1f%+.1f %+.1f%+.1f (mse=%f)\n", i, __real__ input[i], __imag__ input[i], __real__ outifft[i], __imag__ outifft[i], __real__ outfft[i], __imag__ outfft[i], mse);
    }
    /*for (i=0;i<n_re;i++) {
      mse += cabsf(input[i] - outfft[i]);
    }*/
    printf(" MSE=%.6f\n", mse);

    if (mse >= 0.07) {
      printf("MSE too large\n");
      exit(-1);
    }

    srslte_ofdm_rx_free(&fft);
    srslte_ofdm_tx_free(&ifft);

    free(input);
    free(outfft);
    free(outifft);

    n_prb++;
  }

  srslte_dft_exit();

  exit(0);
}
