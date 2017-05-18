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
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <complex.h>

#include "srslte/phch/prach.h"

#define MAX_LEN  70176


uint32_t N_ifft_ul        = 128;
uint32_t preamble_format  = 0;
uint32_t root_seq_idx     = 0;
uint32_t zero_corr_zone   = 1;
uint32_t n_seqs           = 64;

void usage(char *prog) {
  printf("Usage: %s\n", prog);
  printf("\t-N Uplink IFFT size [Default 128]\n");
  printf("\t-f Preamble format [Default 0]\n");
  printf("\t-r Root sequence index [Default 0]\n");
  printf("\t-z Zero correlation zone config [Default 1]\n");
  printf("\t-n Number of sequences used for each test [Default 64]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "Nfrzn")) != -1) {
    switch (opt) {
    case 'N':
      N_ifft_ul = atoi(argv[optind]);
      break;
    case 'f':
      preamble_format = atoi(argv[optind]);
      break;
    case 'r':
      root_seq_idx = atoi(argv[optind]);
      break;
    case 'z':
      zero_corr_zone = atoi(argv[optind]);
      break;
    case 'n':
      n_seqs = atoi(argv[optind]);
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}

int main(int argc, char **argv) {
  parse_args(argc, argv);

  srslte_prach_t *p = (srslte_prach_t*)malloc(sizeof(srslte_prach_t));

  bool high_speed_flag      = false;

  cf_t preamble[MAX_LEN];
  memset(preamble, 0, sizeof(cf_t)*MAX_LEN);
  cf_t preamble_sum[MAX_LEN];
  memset(preamble_sum, 0, sizeof(cf_t)*MAX_LEN);

  srslte_prach_init(p,
             N_ifft_ul,
             preamble_format,
             root_seq_idx,
             high_speed_flag,
             zero_corr_zone);

  uint32_t seq_index = 0;
  uint32_t frequency_offset = 0;

  uint32_t indices[64];
  uint32_t n_indices = 0;
  for(int i=0;i<64;i++)
    indices[i] = 0;

  srslte_prach_set_detect_factor(p, 10);
  
  for(seq_index=0;seq_index<n_seqs;seq_index++)
  {
    srslte_prach_gen(p,
              seq_index,
              frequency_offset,
              preamble);

    for(int i=0;i<p->N_cp+p->N_seq;i++)
    {
      preamble_sum[i] += preamble[i];
    }
  }

  uint32_t prach_len = p->N_seq;
  if(preamble_format == 2 || preamble_format == 3)
    prach_len /= 2;
  srslte_prach_detect(p, 0, &preamble_sum[p->N_cp], prach_len, indices, &n_indices);

  if(n_indices != n_seqs)
    return -1;

  for(int i=0;i<n_seqs;i++)
  {
    if(indices[i] != i)
      return -1;
  }

  srslte_prach_free(p);
  free(p);

	printf("Done\n");
	exit(0);
}
