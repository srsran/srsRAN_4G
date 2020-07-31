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

static uint32_t     nof_frames = 10;
static uint32_t     num_bits   = 1000;
static srslte_mod_t modulation = SRSLTE_MOD_NITEMS;

void usage(char* prog)
{
  printf("Usage: %s [nfv] -m modulation (1: BPSK, 2: QPSK, 4: QAM16, 6: QAM64)\n", prog);
  printf("\t-n num_bits [Default %d]\n", num_bits);
  printf("\t-f nof_frames [Default %d]\n", nof_frames);
  printf("\t-v srslte_verbose [Default None]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "nmvf")) != -1) {
    switch (opt) {
      case 'n':
        num_bits = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        nof_frames = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
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
            ERROR("Invalid modulation %d. Possible values: "
                  "(1: BPSK, 2: QPSK, 4: QAM16, 6: QAM64)\n",
                  (int)strtol(argv[optind], NULL, 10));
            break;
        }
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (modulation == SRSLTE_MOD_NITEMS) {
    usage(argv[0]);
    exit(-1);
  }
}

float mse_threshold()
{
  switch (modulation) {
    case SRSLTE_MOD_BPSK:
      return 1.0e-6;
    case SRSLTE_MOD_QPSK:
      return 1.0e-6;
    case SRSLTE_MOD_16QAM:
      return 0.11;
    case SRSLTE_MOD_64QAM:
      return 0.19;
    case SRSLTE_MOD_256QAM:
      return 0.3;
    default:
      return -1.0f;
  }
}

int main(int argc, char** argv)
{
  int                  i;
  srslte_modem_table_t mod;
  uint8_t *            input, *output;
  cf_t*                symbols;
  float*               llr;
  short*               llr_s;
  int8_t*              llr_b;

  parse_args(argc, argv);

  /* initialize objects */
  if (srslte_modem_table_lte(&mod, modulation)) {
    ERROR("Error initializing modem table\n");
    exit(-1);
  }

  /* check that num_bits is multiple of num_bits x symbol */
  num_bits = mod.nbits_x_symbol * (num_bits / mod.nbits_x_symbol);

  /* allocate buffers */
  input = srslte_vec_u8_malloc(num_bits);
  if (!input) {
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

  llr = srslte_vec_f_malloc(num_bits);
  if (!llr) {
    perror("malloc");
    exit(-1);
  }

  llr_s = srslte_vec_i16_malloc(num_bits);
  if (!llr_s) {
    perror("malloc");
    exit(-1);
  }

  llr_b = srslte_vec_i8_malloc(num_bits);
  if (!llr_b) {
    perror("malloc");
    exit(-1);
  }

  /* generate random data */
  srand(0);

  int            ret = -1;
  struct timeval t[3];
  float          mean_texec   = 0.0;
  float          mean_texec_s = 0.0;
  float          mean_texec_b = 0.0;
  for (int n = 0; n < nof_frames; n++) {
    for (i = 0; i < num_bits; i++) {
      input[i] = rand() % 2;
    }

    /* modulate */
    srslte_mod_modulate(&mod, input, symbols, num_bits);

    gettimeofday(&t[1], NULL);
    srslte_demod_soft_demodulate(modulation, symbols, llr, num_bits / mod.nbits_x_symbol);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);

    /* compute exponentially averaged execution time */
    if (n > 0) {
      mean_texec = SRSLTE_VEC_CMA((float)t[0].tv_usec, mean_texec, n - 1);
    }

    gettimeofday(&t[1], NULL);
    srslte_demod_soft_demodulate_s(modulation, symbols, llr_s, num_bits / mod.nbits_x_symbol);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);

    if (n > 0) {
      mean_texec_s = SRSLTE_VEC_CMA((float)t[0].tv_usec, mean_texec_s, n - 1);
    }

    gettimeofday(&t[1], NULL);
    srslte_demod_soft_demodulate_b(modulation, symbols, llr_b, num_bits / mod.nbits_x_symbol);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);

    if (n > 0) {
      mean_texec_b = SRSLTE_VEC_CMA((float)t[0].tv_usec, mean_texec_b, n - 1);
    }

    if (SRSLTE_VERBOSE_ISDEBUG()) {
      printf("bits=");
      srslte_vec_fprint_b(stdout, input, num_bits);

      printf("symbols=");
      srslte_vec_fprint_c(stdout, symbols, num_bits / mod.nbits_x_symbol);

      printf("llr=");
      srslte_vec_fprint_f(stdout, llr, num_bits);

      printf("llr_s=");
      srslte_vec_fprint_s(stdout, llr_s, num_bits);

      printf("llr_b=");
      srslte_vec_fprint_bs(stdout, llr_b, num_bits);
    }

    // Check demodulation errors
    for (int i = 0; i < num_bits; i++) {
      if (input[i] != (llr[i] > 0 ? 1 : 0)) {
        printf("Error in bit %d\n", i);
        goto clean_exit;
      }
    }
  }
  ret = 0;

clean_exit:
  free(llr_b);
  free(llr_s);
  free(llr);
  free(symbols);
  free(output);
  free(input);

  srslte_modem_table_free(&mod);

  printf("Mean Throughput: %.2f/%.2f/%.2f. Mbps ExTime: %.2f/%.2f/%.2f us\n",
         num_bits / mean_texec,
         num_bits / mean_texec_s,
         num_bits / mean_texec_b,
         mean_texec,
         mean_texec_s,
         mean_texec_b);
  exit(ret);
}
