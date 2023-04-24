/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */
#include "srsran/phy/fec/block/block.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/support/srsran_test.h"
#include <memory.h>
#include <srsran/phy/utils/vector.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

static uint32_t        seed            = 0x1234;
static uint32_t        nof_repetitions = 1;
static uint32_t        E               = SRSRAN_FEC_BLOCK_SIZE;
static uint32_t        A               = 100;
static srsran_random_t random_gen      = NULL;

void usage(char* prog)
{
  printf("Usage: %s [Rv]\n", prog);
  printf("\t-R Number of repetitions [Default %d]\n", nof_repetitions);
  printf("\t-E Number of encoded bits [Default %d]\n", E);
  printf("\t-v increase verbose [Default %d]\n", get_srsran_verbose_level());
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "REv")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'E':
        E = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int test(uint32_t block_size)
{
  struct timeval t[3]                               = {};
  uint8_t        tx[SRSRAN_FEC_BLOCK_MAX_NOF_BITS]  = {};
  uint8_t        rx[SRSRAN_FEC_BLOCK_MAX_NOF_BITS]  = {};
  uint8_t        encoded[4 * SRSRAN_FEC_BLOCK_SIZE] = {};
  int16_t        llr_i16[4 * SRSRAN_FEC_BLOCK_SIZE] = {};
  int8_t         llr_i8[4 * SRSRAN_FEC_BLOCK_SIZE]  = {};

  // Generate random data
  for (uint32_t i = 0; i < block_size; i++) {
    tx[i] = (uint8_t)srsran_random_uniform_int_dist(random_gen, 0, 1);
  }

  gettimeofday(&t[1], NULL);
  for (uint32_t r = 0; r < nof_repetitions; r++) {
    srsran_block_encode(tx, block_size, encoded, E);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  uint64_t t_encode_us = t[0].tv_sec * 1000000 + t[0].tv_usec;

  for (uint32_t i = 0; i < E; i++) {
    int32_t llr = (encoded[i] == 0) ? -A : +A;
    llr_i16[i]  = (int16_t)llr;
    llr_i8[i]   = (int8_t)llr;
  }

  int32_t corr_i16 = 0;
  gettimeofday(&t[1], NULL);

  for (uint32_t r = 0; r < nof_repetitions; r++) {
    corr_i16 = srsran_block_decode_i16(llr_i16, E, rx, block_size);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  TESTASSERT(corr_i16 == E * A);
  TESTASSERT(memcmp(tx, rx, block_size) == 0);
  uint64_t t_decode_i16_us = t[0].tv_sec * 1000000 + t[0].tv_usec;

  gettimeofday(&t[1], NULL);
  int32_t corr_i8 = 0;
  for (uint32_t r = 0; r < nof_repetitions; r++) {
    corr_i8 = srsran_block_decode_i8(llr_i8, E, rx, block_size);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  TESTASSERT(corr_i8 == E * A);
  TESTASSERT(memcmp(tx, rx, block_size) == 0);
  uint64_t t_decode_i8_us = t[0].tv_sec * 1000000 + t[0].tv_usec;

  double total_bits = (double)(block_size * nof_repetitions);
  INFO("Block size %d PASSED! Encoder: %.1f us / %.1f Mbps; 16 bit Decoder: %.1f us / %.2f Mbps; 8 bit decoder: %.1f / "
       "%.2f Mbps",
       block_size,
       t_encode_us / (double)nof_repetitions,
       total_bits / (double)t_encode_us,
       t_decode_i16_us / (double)nof_repetitions,
       total_bits / (double)t_decode_i16_us,
       t_decode_i8_us / (double)nof_repetitions,
       total_bits / (double)t_decode_i8_us);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);
  random_gen = srsran_random_init(seed);

  for (uint32_t block_size = 3; block_size <= SRSRAN_FEC_BLOCK_MAX_NOF_BITS; block_size++) {
    if (test(block_size) < SRSRAN_SUCCESS) {
      break;
    }
  }

  srsran_random_free(random_gen);
}
