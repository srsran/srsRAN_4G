/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "srsran/srsran.h"

#define MAX_LEN 70176

static bool     is_nr            = false;
static uint32_t nof_prb          = 50;
static uint32_t config_idx       = 3;
static uint32_t root_seq_idx     = 0;
static uint32_t zero_corr_zone   = 15;
static uint32_t num_ra_preambles = 0; // use default

static void usage(char* prog)
{
  printf("Usage: %s\n", prog);
  printf("\t-n Uplink number of PRB [Default %d]\n", nof_prb);
  printf("\t-f Preamble format [Default 0]\n");
  printf("\t-r Root sequence index [Default 0]\n");
  printf("\t-z Zero correlation zone config [Default 1]\n");
  printf("\t-N Toggle LTE/NR operation, zero for LTE, non-zero for NR [Default %s]\n", is_nr ? "NR" : "LTE");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "nfrzN")) != -1) {
    switch (opt) {
      case 'n':
        nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        config_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        root_seq_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'z':
        zero_corr_zone = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'N':
        is_nr = (uint32_t)strtol(argv[optind], NULL, 10) > 0;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);
  srsran_prach_t prach;

  bool high_speed_flag = false;

  cf_t preamble[MAX_LEN];
  memset(preamble, 0, sizeof(cf_t) * MAX_LEN);

  srsran_prach_cfg_t prach_cfg;
  ZERO_OBJECT(prach_cfg);
  prach_cfg.is_nr            = is_nr;
  prach_cfg.config_idx       = config_idx;
  prach_cfg.hs_flag          = high_speed_flag;
  prach_cfg.freq_offset      = 0;
  prach_cfg.root_seq_idx     = root_seq_idx;
  prach_cfg.zero_corr_zone   = zero_corr_zone;
  prach_cfg.num_ra_preambles = num_ra_preambles;

  if (srsran_prach_init(&prach, srsran_symbol_sz(nof_prb))) {
    return -1;
  }

  struct timeval t[3] = {};
  gettimeofday(&t[1], NULL);
  if (srsran_prach_set_cfg(&prach, &prach_cfg, nof_prb)) {
    ERROR("Error initiating PRACH object");
    return -1;
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("It took %ld microseconds to configure\n", t[0].tv_usec + t[0].tv_sec * 1000000UL);

  uint32_t seq_index = 0;
  uint32_t indices[64];
  uint32_t n_indices = 0;
  for (int i = 0; i < 64; i++)
    indices[i] = 0;

  for (seq_index = 0; seq_index < 64; seq_index++) {
    srsran_prach_gen(&prach, seq_index, 0, preamble);

    uint32_t prach_len = prach.N_seq;

    gettimeofday(&t[1], NULL);
    srsran_prach_detect(&prach, 0, &preamble[prach.N_cp], prach_len, indices, &n_indices);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    printf("texec=%ld us\n", t[0].tv_usec);
    if (n_indices != 1 || indices[0] != seq_index)
      return -1;
  }

  srsran_prach_free(&prach);

  printf("Done\n");
  exit(0);
}
