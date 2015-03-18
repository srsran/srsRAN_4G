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
#include <stdbool.h>

#include "srslte/srslte.h"

int nof_frames = 10; 
int num_bits = 1000;
srslte_mod_t modulation = 10;

void usage(char *prog) {
  printf("Usage: %s [nfv] -m modulation (1: BPSK, 2: QPSK, 4: QAM16, 6: QAM64)\n", prog);
  printf("\t-n num_bits [Default %d]\n", num_bits);
  printf("\t-f nof_frames [Default %d]\n", nof_frames);
  printf("\t-v verbose [Default None]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "nmvf")) != -1) {
    switch (opt) {
    case 'n':
      num_bits = atoi(argv[optind]);
      break;
    case 'f':
      nof_frames = atoi(argv[optind]);
      break;
    case 'v':
      verbose++;
      break;
    case 'm':
      switch(atoi(argv[optind])) {
      case 1:
        modulation = LTE_BPSK;
        break;
      case 2:
        modulation = LTE_QPSK;
        break;
      case 4:
        modulation = LTE_QAM16;
        break;
      case 6:
        modulation = LTE_QAM64;
        break;
      default:
        fprintf(stderr, "Invalid modulation %d. Possible values: "
            "(1: BPSK, 2: QPSK, 4: QAM16, 6: QAM64)\n", atoi(argv[optind]));
        break;
      }
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (modulation == 10) {
    usage(argv[0]);
    exit(-1);
  }
}

float mse_threshold() {
  switch(modulation) {
    case LTE_BPSK: 
      return 1.0e-6;
    case LTE_QPSK:
      return 1.0e-6; 
    case LTE_QAM16: 
      return 0.11; 
    case LTE_QAM64:
      return 0.19;
    default:
      return -1.0;
  }
}

int main(int argc, char **argv) {
  int i;
  modem_table_t mod;
  demod_soft_t demod_soft;
  uint8_t *input, *output;
  cf_t *symbols;
  float *llr_exact, *llr_approx;

  parse_args(argc, argv);

  /* initialize objects */
  if (modem_table_lte(&mod, modulation, true)) {
    fprintf(stderr, "Error initializing modem table\n");
    exit(-1);
  }

  /* check that num_bits is multiple of num_bits x symbol */
  num_bits = mod.nbits_x_symbol * (num_bits / mod.nbits_x_symbol);

  demod_soft_init(&demod_soft, num_bits / mod.nbits_x_symbol);
  demod_soft_table_set(&demod_soft, &mod);
  demod_soft_sigma_set(&demod_soft, 2.0 / mod.nbits_x_symbol);


  /* allocate buffers */
  input = malloc(sizeof(uint8_t) * num_bits);
  if (!input) {
    perror("malloc");
    exit(-1);
  }
  output = malloc(sizeof(uint8_t) * num_bits);
  if (!output) {
    perror("malloc");
    exit(-1);
  }
  symbols = malloc(sizeof(cf_t) * num_bits / mod.nbits_x_symbol);
  if (!symbols) {
    perror("malloc");
    exit(-1);
  }

  llr_exact = malloc(sizeof(float) * num_bits);
  if (!llr_exact) {
    perror("malloc");
    exit(-1);
  }

  llr_approx = malloc(sizeof(float) * num_bits);
  if (!llr_approx) {
    perror("malloc");
    exit(-1);
  }

  /* generate random data */
  srand(0);
  
  int ret = -1;
  double mse;
  struct timeval t[3]; 
  float mean_texec = 0.0; 
  for (int n=0;n<nof_frames;n++) {
    for (i=0;i<num_bits;i++) {
      input[i] = rand()%2;
    }

    /* modulate */
    mod_modulate(&mod, input, symbols, num_bits);

    /* add noise */
    srslte_ch_awgn_c(symbols, symbols, srslte_ch_awgn_get_variance(5.0, mod.nbits_x_symbol), num_bits / mod.nbits_x_symbol);
    
    /* Compare exact with approximation algorithms */
    demod_soft_alg_set(&demod_soft, EXACT);
    demod_soft_demodulate(&demod_soft, symbols, llr_exact, num_bits / mod.nbits_x_symbol);
    
    demod_soft_alg_set(&demod_soft, APPROX);
    gettimeofday(&t[1], NULL);
    demod_soft_demodulate(&demod_soft, symbols, llr_approx, num_bits / mod.nbits_x_symbol);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    
    /* compute exponentially averaged execution time */
    if (n > 0) {
      mean_texec = VEC_CMA((float) t[0].tv_usec, mean_texec, n-1);      
    }
    
    /* check MSE */
    mse = 0.0;
    for (i=0;i<num_bits;i++) {
      float e = llr_exact[i] - llr_approx[i];
      mse += e*e;
    }
    mse/=num_bits;

    if (VERBOSE_ISDEBUG()) {
      printf("exact=");
      vec_fprint_f(stdout, llr_exact, num_bits);

      printf("approx=");
      vec_fprint_f(stdout, llr_approx, num_bits);
    }
    
    if (mse > mse_threshold()) {
        goto clean_exit; 
    }
  }
  ret = 0; 

clean_exit:  
  free(llr_exact);
  free(llr_approx);
  free(symbols);
  free(output);
  free(input);

  modem_table_free(&mod);
  demod_soft_free(&demod_soft);

  if (ret == 0) {
    printf("Ok Mean Throughput: %.2f. Mbps ExTime: %.2f us\n", num_bits/mean_texec, mean_texec);    
  } else {
    printf("Error: MSE too large (%f > %f)\n", mse, mse_threshold());
  }
  exit(ret);
}
