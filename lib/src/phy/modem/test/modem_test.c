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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srslte/srslte.h"

time_t         start, finish;
struct timeval x, y;

static uint32_t     num_bits   = 1000;
static srslte_mod_t modulation = SRSLTE_MOD_BPSK;

void usage(char* prog)
{
  printf("Usage: %s [nmse]\n", prog);
  printf("\t-n num_bits [Default %d]\n", num_bits);
  printf("\t-m modulation (1: BPSK, 2: QPSK, 4: QAM16, 6: QAM64, 8: QAM256) [Default BPSK]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "nm")) != -1) {
    switch (opt) {
      case 'n':
        num_bits = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        switch (strtol(argv[optind], NULL, 10)) {
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
          case 8:
            modulation = SRSLTE_MOD_256QAM;
            break;
          default:
            ERROR("Invalid modulation %ld. Possible values: "
                  "(1: BPSK, 2: QPSK, 4: QAM16, 6: QAM64, 8: QAM256)\n",
                  strtol(argv[optind], NULL, 10));
            break;
        }
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  int                  ret = SRSLTE_SUCCESS;
  int                  i;
  srslte_modem_table_t mod;
  uint8_t *            input, *input_bytes, *output;
  cf_t *               symbols, *symbols_bytes;
  float*               llr;

  parse_args(argc, argv);

  /* initialize objects */
  if (srslte_modem_table_lte(&mod, modulation)) {
    ERROR("Error initializing modem table\n");
    exit(-1);
  }

  srslte_modem_table_bytes(&mod);

  /* check that num_bits is multiple of num_bits x symbol */
  if (num_bits % mod.nbits_x_symbol) {
    ERROR("Error num_bits must be multiple of %d\n", mod.nbits_x_symbol);
    exit(-1);
  }

  /* allocate buffers */
  input = srslte_vec_u8_malloc(num_bits);
  if (!input) {
    perror("malloc");
    exit(-1);
  }
  input_bytes = srslte_vec_u8_malloc(num_bits / 8);
  if (!input_bytes) {
    perror("malloc");
    exit(-1);
  }
  output = srslte_vec_u8_malloc(num_bits);
  if (!output) {
    perror("malloc");
    exit(-1);
  }
  symbols = srslte_vec_cf_malloc(num_bits / mod.nbits_x_symbol);
  if (!symbols) {
    perror("malloc");
    exit(-1);
  }
  symbols_bytes = srslte_vec_cf_malloc(num_bits / mod.nbits_x_symbol);
  if (!symbols_bytes) {
    perror("malloc");
    exit(-1);
  }

  llr = srslte_vec_f_malloc(num_bits);
  if (!llr) {
    perror("malloc");
    exit(-1);
  }

  /* generate random data */
  for (i = 0; i < num_bits; i++) {
    input[i] = rand() % 2;
  }

  /* modulate */
  struct timeval t[3];
  gettimeofday(&t[1], NULL);
  int ntrials = 100;
  for (int i = 0; i < ntrials; i++) {
    srslte_mod_modulate(&mod, input, symbols, num_bits);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);

  printf("Bit: %ld us\n", t[0].tv_usec);

  /* Test packed implementation */
  srslte_bit_pack_vector(input, input_bytes, num_bits);
  gettimeofday(&t[1], NULL);
  for (int i = 0; i < ntrials; i++) {
    srslte_mod_modulate_bytes(&mod, input_bytes, symbols_bytes, num_bits);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);

  printf("Byte: %ld us\n", t[0].tv_usec);
  for (int i = 0; i < num_bits / mod.nbits_x_symbol; i++) {
    if (symbols[i] != symbols_bytes[i]) {
      printf("error in symbol %d\n", i);
      exit(-1);
    }
  }

  srslte_vec_f_zero(llr, num_bits / mod.nbits_x_symbol);

  printf("Symbols OK\n");
  /* demodulate */
  gettimeofday(&x, NULL);
  srslte_demod_soft_demodulate(modulation, symbols, llr, num_bits / mod.nbits_x_symbol);
  gettimeofday(&y, NULL);
  printf("\nElapsed time [us]: %ld\n", y.tv_usec - x.tv_usec);
  for (i = 0; i < num_bits; i++) {
    output[i] = llr[i] >= 0 ? 1 : 0;
  }

  /* check errors */
  for (i = 0; i < num_bits && ret == SRSLTE_SUCCESS; i++) {
    if (input[i] != output[i]) {
      ERROR("Error in bit %d\n", i);
      ret = SRSLTE_ERROR;
    }
  }

  free(llr);
  free(symbols);
  free(symbols_bytes);
  free(output);
  free(input);
  free(input_bytes);

  srslte_modem_table_free(&mod);

  exit(ret);
}
