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

#include "srslte/srslte.h"
#include "crc_test.h"

int num_bits = 5001, srslte_crc_length = 24;
uint32_t srslte_crc_poly = 0x1864CFB;
uint32_t seed = 1;

void usage(char *prog) {
  printf("Usage: %s [nlps]\n", prog);
  printf("\t-n num_bits [Default %d]\n", num_bits);
  printf("\t-l srslte_crc_length [Default %d]\n", srslte_crc_length);
  printf("\t-p srslte_crc_poly (Hex) [Default 0x%x]\n", srslte_crc_poly);
  printf("\t-s seed [Default 0=time]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "nlps")) != -1) {
    switch (opt) {
    case 'n':
      num_bits = atoi(argv[optind]);
      break;
    case 'l':
      srslte_crc_length = atoi(argv[optind]);
      break;
    case 'p':
      srslte_crc_poly = (uint32_t) strtoul(argv[optind], NULL, 16);
      break;
    case 's':
      seed = (uint32_t) strtoul(argv[optind], NULL, 0);
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}

int main(int argc, char **argv) {
  int i;
  uint8_t *data;
  uint32_t srslte_crc_word, expected_word;
  srslte_crc_t srslte_crc_p;

  parse_args(argc, argv);

  data = malloc(sizeof(uint8_t) * (num_bits + srslte_crc_length * 2));
  if (!data) {
    perror("malloc");
    exit(-1);
  }

  if (!seed) {
    seed = time(NULL);
  }
  srand(seed);

  // Generate data
  for (i = 0; i < num_bits; i++) {
    data[i] = rand() % 2;
  }

  //Initialize CRC params and tables
  if (srslte_crc_init(&srslte_crc_p, srslte_crc_poly, srslte_crc_length)) {
    exit(-1);
  }

  // generate CRC word
  srslte_crc_word = srslte_crc_checksum(&srslte_crc_p, data, num_bits);

  free(data);

  // check if generated word is as expected
  if (get_expected_word(num_bits, srslte_crc_length, srslte_crc_poly, seed,
      &expected_word)) {
    fprintf(stderr, "Test parameters not defined in test_results.h\n");
    exit(-1);
  }
  exit(expected_word != srslte_crc_word);
}
