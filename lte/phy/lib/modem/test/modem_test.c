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

#include "liblte/phy/phy.h"

int num_bits = 1000;
lte_mod_t modulation;
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
  modem_table_t mod;
  demod_hard_t demod_hard;
  demod_soft_t demod_soft;
  char *input, *output;
  cf_t *symbols;
  float *llr;

  parse_args(argc, argv);

  /* initialize objects */
  if (modem_table_lte(&mod, modulation, soft_output)) {
    fprintf(stderr, "Error initializing modem table\n");
    exit(-1);
  }

  /* check that num_bits is multiple of num_bits x symbol */
  if (num_bits % mod.nbits_x_symbol) {
    fprintf(stderr, "Error num_bits must be multiple of %d\n", mod.nbits_x_symbol);
    exit(-1);
  }

  if (soft_output) {
    demod_soft_init(&demod_soft);
    demod_soft_table_set(&demod_soft, &mod);
    demod_soft_alg_set(&demod_soft, soft_exact?EXACT:APPROX);
  } else {
    demod_hard_init(&demod_hard);
    demod_hard_table_set(&demod_hard, modulation);
  }

  /* allocate buffers */
  input = malloc(sizeof(char) * num_bits);
  if (!input) {
    perror("malloc");
    exit(-1);
  }
  output = malloc(sizeof(char) * num_bits);
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
  mod_modulate(&mod, input, symbols, num_bits);

  /* demodulate */
  if (soft_output) {
    demod_soft_demodulate(&demod_soft, symbols, llr, num_bits / mod.nbits_x_symbol);
    for (i=0;i<num_bits;i++) {
      output[i] = llr[i]>=0 ? 1 : 0;
    }
  } else {
    demod_hard_demodulate(&demod_hard, symbols, output, num_bits / mod.nbits_x_symbol);
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

  modem_table_free(&mod);

  printf("Ok\n");
  exit(0);
}
