/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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
#include <time.h>
#include <stdbool.h>

#include "srslte/srslte.h"

int nof_tx_bits = -1, nof_rx_bits = -1;
int nof_filler_bits = -1; 
int rv_idx = 0;

void usage(char *prog) {
  printf("Usage: %s -t nof_tx_bits -r nof_rx_bits [-i rv_idx -f nof_filler_bits]\n", prog);
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "trif")) != -1) {
    switch (opt) {
    case 'f':
      nof_filler_bits = atoi(argv[optind]);
      break;
    case 't':
      nof_tx_bits = atoi(argv[optind]);
      break;
    case 'r':
      nof_rx_bits = atoi(argv[optind]);
      break;
    case 'i':
      rv_idx = atoi(argv[optind]);
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (nof_tx_bits == -1) {
    usage(argv[0]);
    exit(-1);
  }
  if (nof_rx_bits == -1) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char **argv) {
  int i;
  uint8_t *bits, *bits_out, *rm_bits, *w_buff_c;
  float *rm_symbols, *unrm_symbols, *w_buff_f;
  int nof_errors;

  parse_args(argc, argv);

  bits = malloc(sizeof(uint8_t) * nof_tx_bits);
  if (!bits) {
    perror("malloc");
    exit(-1);
  }
  bits_out = malloc(sizeof(uint8_t) * nof_tx_bits);
  if (!bits_out) {
    perror("malloc");
    exit(-1);
  }
  w_buff_c = malloc(sizeof(uint8_t) * nof_tx_bits * 10);
  if (!w_buff_c) {
    perror("malloc");
    exit(-1);
  }
  rm_bits = malloc(sizeof(uint8_t) * nof_rx_bits);
  if (!rm_bits) {
    perror("malloc");
    exit(-1);
  }
  rm_symbols = malloc(sizeof(float) * nof_rx_bits);
  if (!rm_symbols) {
    perror("malloc");
    exit(-1);
  }
  w_buff_f = malloc(sizeof(float) * nof_rx_bits * 10);
  if (!w_buff_f) {
    perror("malloc");
    exit(-1);
  }
  unrm_symbols = malloc(sizeof(float) * nof_tx_bits);
  if (!unrm_symbols) {
    perror("malloc");
    exit(-1);
  }

  for (i = 0; i < nof_tx_bits; i++) {
    bits[i] = rand() % 2;
  }
  
  for (i=0;i<nof_filler_bits;i++) {
    bits[3*i+0] = SRSLTE_TX_NULL;
    bits[3*i+1] = SRSLTE_TX_NULL;
  }
  
  bzero(w_buff_c, nof_tx_bits * 10 * sizeof(uint8_t));
  bzero(w_buff_f, nof_rx_bits * 10 * sizeof(float));
  
  printf("BITS: ");
  srslte_vec_fprint_b(stdout, bits, nof_tx_bits);

  srslte_rm_turbo_tx(w_buff_c, nof_tx_bits * 10, bits, nof_tx_bits, rm_bits, nof_rx_bits, rv_idx);

  printf("RM: ");
  srslte_vec_fprint_b(stdout, rm_bits, nof_rx_bits);
 
  for (i = 0; i < nof_rx_bits; i++) {
    rm_symbols[i] = (float) rm_bits[i] ? 1 : -1;
  }

  srslte_rm_turbo_rx(w_buff_f, nof_rx_bits * 10, rm_symbols, nof_rx_bits, unrm_symbols, nof_tx_bits,
      rv_idx, nof_filler_bits);

  printf("UMRM: ");
  srslte_vec_fprint_f(stdout, unrm_symbols, nof_tx_bits);

  for (i=0;i<nof_tx_bits;i++) {
    bits_out[i] = unrm_symbols[i]>0?1:0;
  }
  printf("BITS: ");
  srslte_vec_fprint_b(stdout, bits_out, nof_tx_bits);
  printf("BITS: ");
  srslte_vec_fprint_b(stdout, bits, nof_tx_bits);

  nof_errors = 0;
  for (i = 0; i < nof_tx_bits; i++) {
    if (unrm_symbols[i] > 0 && ((unrm_symbols[i] > 0) != bits[i])) {
      nof_errors++;
    }
  }

  free(bits);
  free(rm_bits);
  free(rm_symbols);
  free(unrm_symbols);
  free(bits_out);
  
  if (nof_errors) {
    printf("nof_errors=%d\n", nof_errors);
    exit(-1);
  }

  printf("Ok\n");
  exit(0);
}
