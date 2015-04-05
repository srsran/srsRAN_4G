/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

time_t start, finish;
struct timeval x, y;

int num_bits = 1000;
srslte_mod_t modulation;
bool soft_output = false, soft_exact = false;

void usage(char *prog) {
  printf("Usage: %s [nmse]\n", prog);
  printf("\t-n num_bits [Default %d]\n", num_bits);
  printf("\t-m modulation (1: BPSK, 2: QPSK, 3: QAM16, 4: QAM64) [Default BPSK]\n");
  printf("\t-s soft outputs [Default hard]\n");
  printf("\t-e soft outputs exact algorithm [Default approx]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "nmse")) != -1) {
    switch (opt) {
    case 'n':
      num_bits = atoi(argv[optind]);
      break;
    case 's':
      soft_output = true;
      break;
    case 'e':
      soft_exact = true;
      break;
    case 'm':
      switch(atoi(argv[optind])) {
      case 1:
        modulation = SRSLTE_MOD_BPSK;
        break;
      case 2:
        modulation = SRSLTE_MOD_QPSK;
        break;
      case 4:
        modulation = SRSLTE_MOD_16QAM;
        break;
      case 6:
        modulation = SRSLTE_MOD_64QAM;
        break;
      default:
        fprintf(stderr, "Invalid modulation %d. Possible values: "
            "(1: BPSK, 2: QPSK, 3: QAM16, 4: QAM64)\n", atoi(argv[optind]));
        break;
      }
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}


int main(int argc, char **argv) {
  int i;
  srslte_modem_table_t mod;
  srslte_demod_hard_t demod_hard;
  srslte_demod_soft_t demod_soft;
  uint8_t *input, *output;
  cf_t *symbols;
  float *llr;

//  unsigned long strt, fin;
//  strt = x->tv_usec;
//  fin = y->tv_usec;
  
  parse_args(argc, argv);

  /* initialize objects */
  if (srslte_modem_table_lte(&mod, modulation, soft_output)) {
    fprintf(stderr, "Error initializing modem table\n");
    exit(-1);
  }

  /* check that num_bits is multiple of num_bits x symbol */
  if (num_bits % mod.nbits_x_symbol) {
    fprintf(stderr, "Error num_bits must be multiple of %d\n", mod.nbits_x_symbol);
    exit(-1);
  }

  if (soft_output) {
    srslte_demod_soft_init(&demod_soft, num_bits / mod.nbits_x_symbol);
    srslte_demod_soft_table_set(&demod_soft, &mod);
    srslte_demod_soft_alg_set(&demod_soft, soft_exact?SRSLTE_DEMOD_SOFT_ALG_EXACT:SRSLTE_DEMOD_SOFT_ALG_APPROX);
  } else {
    srslte_demod_hard_init(&demod_hard);
    srslte_demod_hard_table_set(&demod_hard, modulation);
  }

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

  llr = malloc(sizeof(float) * num_bits);
  if (!llr) {
    perror("malloc");
    exit(-1);
  }

  /* generate random data */
  srand(time(NULL));
  for (i=0;i<num_bits;i++) {
    input[i] = rand()%2;
  }

  /* modulate */
  srslte_mod_modulate(&mod, input, symbols, num_bits);

  /* demodulate */
  if (soft_output) {

    gettimeofday(&x, NULL);
    srslte_demod_soft_demodulate(&demod_soft, symbols, llr, num_bits / mod.nbits_x_symbol);
    gettimeofday(&y, NULL);
    printf("\nElapsed time [ns]: %d\n", (int) y.tv_usec - (int) x.tv_usec);
    
    for (i=0;i<num_bits;i++) {
      output[i] = llr[i]>=0 ? 1 : 0;
    }
  } else {
    srslte_demod_hard_demodulate(&demod_hard, symbols, output, num_bits / mod.nbits_x_symbol);
  }

  /* check errors */
  for (i=0;i<num_bits;i++) {
    if (input[i] != output[i]) {
      fprintf(stderr, "Error in bit %d\n", i);
      exit(-1);
    }
  }

  free(llr);
  free(symbols);
  free(output);
  free(input);

  srslte_modem_table_free(&mod);
  if (soft_output) {
    srslte_demod_soft_free(&demod_soft);    
  }

  printf("Ok\n");
  exit(0);
}
