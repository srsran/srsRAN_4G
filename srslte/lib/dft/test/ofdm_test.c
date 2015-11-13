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

void usage(char *prog) {
  printf("Usage: %s\n", prog);
  printf("\t-n nof_prb [Default All]\n");
  printf("\t-e extended cyclic prefix [Default Normal]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "ne")) != -1) {
    switch (opt) {
    case 'n':
      nof_prb = atoi(argv[optind]);
      break;
    case 'e':
      cp = SRSLTE_CP_EXT;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}


int main(int argc, char **argv) {
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

    input = malloc(sizeof(cf_t) * n_re);
    if (!input) {
      perror("malloc");
      exit(-1);
    }
    outfft = malloc(sizeof(cf_t) * SRSLTE_SLOT_LEN(srslte_symbol_sz(n_prb)));
    if (!outfft) {
      perror("malloc");
      exit(-1);
    }
    outifft = malloc(sizeof(cf_t) * n_re);
    if (!outifft) {
      perror("malloc");
      exit(-1);
    }

    if (srslte_ofdm_rx_init(&fft, cp, n_prb)) {
      fprintf(stderr, "Error initializing FFT\n");
      exit(-1);
    }
    srslte_dft_plan_set_norm(&fft.fft_plan, true);

    if (srslte_ofdm_tx_init(&ifft, cp, n_prb)) {
      fprintf(stderr, "Error initializing iFFT\n");
      exit(-1);
    }
    srslte_dft_plan_set_norm(&ifft.fft_plan, true);

    for (i=0;i<n_re;i++) {
      input[i] = 100 * ((float) rand()/RAND_MAX + (float) I*rand()/RAND_MAX);
    }

    srslte_ofdm_tx_slot(&ifft, input, outfft);
    srslte_ofdm_rx_slot(&fft, outfft, outifft);

    /* compute MSE */

    mse = 0;
    for (i=0;i<n_re;i++) {
      mse += cabsf(input[i] - outifft[i]);
    }
    printf("MSE=%f\n", mse);

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
  exit(0);
}
