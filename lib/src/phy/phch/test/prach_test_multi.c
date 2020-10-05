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

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <strings.h>
#include "srslte/srslte.h"
#include <time.h>
#include "srslte/phy/phch/prach.h"
#include "srslte/phy/utils/debug.h"
char* input_file_name = NULL;
#define PRACH_SRATE 1048750

#define MAX_LEN 70176
int offset                = -1;
uint32_t nof_prb          = 6;
uint32_t preamble_format  = 0;
uint32_t root_seq_idx     = 0;
uint32_t zero_corr_zone   = 1;
uint32_t n_seqs           = 64;
uint32_t num_ra_preambles = 0; // use default

bool freq_domain_offset_calc       = false;
bool test_successive_cancellation  = false;
bool test_offset_calculation       = false;
bool stagger_prach_power_and_phase = false;
// this will work best with one or two simultaenous prach
srslte_filesource_t    fsrc;

void usage(char* prog)
{
  printf("Usage: %s\n", prog);
  printf("\t-N Uplink number of PRB [Default %d]\n", nof_prb);
  printf("\t-f Preamble format [Default 0]\n");
  printf("\t-r Root sequence index [Default 0]\n");
  printf("\t-z Zero correlation zone config [Default 1]\n");
  printf("\t-n Number of sequences used for each test [Default 64]\n");
  printf("\t-S stagger_prach_power_and_phase [Default false]\n");
  printf("\t-s test_successive_cancellation  [Default false]\n");
  printf("\t-O test_offset_calculation  [Default false]\n");
  printf("\t-F freq_domain_offset_calc [Default false]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "NfrznioSsOF")) != -1) {
    switch (opt) {
      case 'N':
        nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        preamble_format = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        root_seq_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'z':
        zero_corr_zone = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        n_seqs = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'o':
        offset =  (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        test_successive_cancellation = true;
        break;
      case 'O':
        test_offset_calculation = true;
        break;
      case 'S':
        stagger_prach_power_and_phase = true;
        break;
      case 'F':
        freq_domain_offset_calc = true;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}
// this function staggers power and phase of the different PRACH signals for more realisitc testing
void stagger_prach_powers(srslte_prach_t *prach, cf_t *preamble, cf_t* preamble_sum, int freq_offset, int n_seqs, int *offsets) {

  for (int seq_index = 0; seq_index < n_seqs; seq_index++) {
    srslte_prach_gen(prach, seq_index, freq_offset, preamble);
    if (seq_index == 0) {
      srslte_vec_sc_prod_ccc(preamble, cexpf(_Complex_I * 0.5), preamble, prach->N_cp + prach->N_seq);
      srslte_vec_sc_prod_cfc(preamble, 1, preamble, prach->N_cp + prach->N_seq);
    }
    if (seq_index == 1) {
      srslte_vec_sc_prod_ccc(preamble, cexpf(_Complex_I * 1), preamble, prach->N_cp + prach->N_seq);
      srslte_vec_sc_prod_cfc(preamble, 0.8, preamble, prach->N_cp + prach->N_seq);
    }
    if (seq_index == 2) {
      srslte_vec_sc_prod_ccc(preamble, cexpf(_Complex_I * 0.1), preamble, prach->N_cp + prach->N_seq);
      srslte_vec_sc_prod_cfc(preamble, 0.05, preamble, prach->N_cp + prach->N_seq);
    }
    if (seq_index == 3) {
      srslte_vec_sc_prod_ccc(preamble, cexpf(_Complex_I * 0.9), preamble, prach->N_cp + prach->N_seq);
      srslte_vec_sc_prod_cfc(preamble, 0.7, preamble, prach->N_cp + prach->N_seq);
    }
    if (seq_index == 4) {
      srslte_vec_sc_prod_ccc(preamble, cexpf(_Complex_I * 0.3), preamble, prach->N_cp + prach->N_seq);
      srslte_vec_sc_prod_cfc(preamble, 0.15, preamble, prach->N_cp + prach->N_seq);
    }
    if (seq_index == 5) {
      srslte_vec_sc_prod_cfc(preamble, 0.15, preamble, prach->N_cp + prach->N_seq);
    }
    int off = (offset == -1) ? offsets[seq_index] : offset;
    for (int i = 0; i < prach->N_cp + prach->N_seq; i++) {
      preamble_sum[i + off] += preamble[i];
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);
  srslte_prach_t prach;

  bool high_speed_flag = false;
  srand(0);
  cf_t preamble[MAX_LEN];
  memset(preamble, 0, sizeof(cf_t) * MAX_LEN);
  cf_t preamble_sum[MAX_LEN];
  memset(preamble_sum, 0, sizeof(cf_t) * MAX_LEN);
  int offsets[64];
  memset(offsets, 0, sizeof(int) * 64);
  float t_offsets[64];
  srslte_prach_cfg_t prach_cfg;
  ZERO_OBJECT(prach_cfg);
  if(input_file_name) {
    if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
      ERROR("Error opening file %s\n", input_file_name);
      exit(-1);
    }
  }
  prach_cfg.config_idx                     = preamble_format;
  prach_cfg.hs_flag                        = high_speed_flag;
  prach_cfg.freq_offset                    = 0;
  prach_cfg.root_seq_idx                   = root_seq_idx;
  prach_cfg.zero_corr_zone                 = zero_corr_zone;
  prach_cfg.num_ra_preambles               = num_ra_preambles;
  prach_cfg.enable_successive_cancellation = test_successive_cancellation;
  prach_cfg.enable_freq_domain_offset_calc = freq_domain_offset_calc;

  int  srate = srslte_sampling_freq_hz(nof_prb);
  int  divisor = srate / PRACH_SRATE;
  if (test_offset_calculation || test_successive_cancellation || stagger_prach_power_and_phase) {
    if (n_seqs > 6) {
      n_seqs = 6;
    }
    prach_cfg.zero_corr_zone     = 0;
    prach_cfg.num_ra_preambles   = 8;
    printf("limiting number of preambles to 6\n");
    if (test_offset_calculation) {
      for (int i = 0; i < 6; i++) {
        offsets[i] = (rand() % 50);
      }
    }
  }

  if (srslte_prach_init(&prach, srslte_symbol_sz(nof_prb))) {
    return -1;
  }

  if (srslte_prach_set_cfg(&prach, &prach_cfg, nof_prb)) {
    ERROR("Error initiating PRACH object\n");
    return -1;
  }

  uint32_t seq_index        = 0;

  uint32_t indices[64];
  uint32_t n_indices = 0;
  for (int i = 0; i < 64; i++)
    indices[i] = 0;

  srslte_prach_set_detect_factor(&prach, 10);
  if (stagger_prach_power_and_phase) {
    stagger_prach_powers(&prach, preamble, preamble_sum, prach_cfg.freq_offset, n_seqs, offsets);
  } else {
    for (seq_index = 0; seq_index < n_seqs; seq_index++) {
      srslte_prach_gen(&prach, seq_index, prach_cfg.freq_offset, preamble);
      int off = (offset == -1) ? offsets[seq_index] : offset;
      for (int i = prach.N_cp; i < prach.N_cp + prach.N_seq; i++) {
        preamble_sum[i + off] += preamble[i];
      }
    }
  }

  if (input_file_name) {
    srslte_filesource_read(&fsrc, &preamble_sum[prach.N_cp],  prach.N_seq);
  }

  uint32_t prach_len = prach.N_seq;
  if (preamble_format == 2 || preamble_format == 3) {
    prach_len /= 2;
  }
  struct timeval t[3];
  gettimeofday(&t[1], NULL);
  srslte_prach_detect_offset(&prach, 0, &preamble_sum[prach.N_cp], prach_len, indices, t_offsets , NULL, &n_indices);
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("texec=%ld us\n", t[0].tv_usec);
  int err = 0;
  if (n_indices != n_seqs) {
    printf("n_indices %d n_seq %d\n", n_indices, n_seqs);
    err++;
  }
  for (int i = 0; i < n_indices; i++) {
    if (test_offset_calculation) {
      int error =  (int)(t_offsets[i] * srate) - offsets[i];
      if (abs(error) > divisor) {
        printf("preamble %d has incorrect offset calculated as %d, should be %d\n",
               indices[i],
               (int)(t_offsets[i] * srate),
               offsets[i]);
        err++;
      }
    }
  }
  if (err){
    return -1;
  }

  srslte_prach_free(&prach);
  printf("Done\n");
  exit(0);
}
