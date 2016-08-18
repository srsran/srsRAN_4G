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
#include <sys/time.h>
#include <complex.h>

#include "srslte/srslte.h"

#define MAX_LEN  70176


uint32_t N_ifft_ul        = 1536;
uint32_t config_idx       = 3;
uint32_t root_seq_idx     = 0;
uint32_t zero_corr_zone   = 15;

void usage(char *prog) {
  printf("Usage: %s\n", prog);
  printf("\t-N Uplink IFFT size [Default %d]\n", N_ifft_ul);
  printf("\t-f Preamble format [Default 0]\n");
  printf("\t-r Root sequence index [Default 0]\n");
  printf("\t-z Zero correlation zone config [Default 1]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "Nfrz")) != -1) {
    switch (opt) {
    case 'N':
      N_ifft_ul = atoi(argv[optind]);
      break;
    case 'f':
      config_idx = atoi(argv[optind]);
      break;
    case 'r':
      root_seq_idx = atoi(argv[optind]);
      break;
    case 'z':
      zero_corr_zone = atoi(argv[optind]);
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

  srslte_prach_init(p,
             N_ifft_ul,
             config_idx,
             root_seq_idx,
             high_speed_flag,
             zero_corr_zone);

  uint32_t seq_index = 0;
  uint32_t frequency_offset = 0;

  uint32_t indices[64];
  uint32_t n_indices = 0;
  for(int i=0;i<64;i++)
    indices[i] = 0;

  for(seq_index=0;seq_index<64;seq_index++)
  {
    srslte_prach_gen(p,
              seq_index,
              frequency_offset,
              preamble);

    uint32_t prach_len = p->N_seq;
    
    struct timeval t[3];
    gettimeofday(&t[1], NULL);
    srslte_prach_detect(p, frequency_offset, &preamble[p->N_cp], prach_len, indices, &n_indices);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    printf("texec=%d us\n", t[0].tv_usec);
    if(n_indices != 1 || indices[0] != seq_index)
      return -1;
  }

  srslte_prach_free(p);
  free(p);

  printf("Done\n");
  exit(0);
}
