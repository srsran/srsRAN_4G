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
#include <srslte/phy/utils/random.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srslte/srslte.h"

int   nof_symbols = 1000;
int   nof_cw = 1, nof_layers = 1;
char* mimo_type_name = NULL;

void usage(char* prog)
{
  printf("Usage: %s -m [%s|%s|%s|%s] -c [nof_cw] -l [nof_layers]\n",
         prog,
         srslte_mimotype2str(0),
         srslte_mimotype2str(1),
         srslte_mimotype2str(2),
         srslte_mimotype2str(3));
  printf("\t-n num_symbols [Default %d]\n", nof_symbols);
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "mcln")) != -1) {
    switch (opt) {
      case 'n':
        nof_symbols = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        nof_cw = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'l':
        nof_layers = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        mimo_type_name = argv[optind];
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (!mimo_type_name) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  srslte_random_t    random = srslte_random_init(0);
  int                i, j, num_errors, symbols_layer;
  cf_t *             d[SRSLTE_MAX_CODEWORDS], *x[SRSLTE_MAX_LAYERS], *dp[SRSLTE_MAX_CODEWORDS];
  srslte_tx_scheme_t type;
  int                nof_symb_cw[SRSLTE_MAX_CODEWORDS];
  int                n[2];

  parse_args(argc, argv);

  if (srslte_str2mimotype(mimo_type_name, &type)) {
    ERROR("Invalid MIMO type %s\n", mimo_type_name);
    exit(-1);
  }

  if (nof_cw > 1) {
    n[0]           = nof_layers / nof_cw;
    n[1]           = nof_layers - n[0];
    nof_symb_cw[0] = nof_symbols * n[0];
    nof_symb_cw[1] = nof_symbols * n[1];
  } else {
    nof_symb_cw[0] = nof_symbols;
    nof_symb_cw[1] = 0;
  }

  for (i = 0; i < nof_cw; i++) {
    d[i] = srslte_vec_cf_malloc(nof_symb_cw[i]);
    if (!d[i]) {
      perror("malloc");
      exit(-1);
    }
    dp[i] = srslte_vec_cf_malloc(nof_symb_cw[i]);
    if (!dp[i]) {
      perror("malloc");
      exit(-1);
    }
  }
  for (i = 0; i < nof_layers; i++) {
    x[i] = srslte_vec_cf_malloc(nof_symbols);
    if (!x[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  /* generate random data */
  for (i = 0; i < nof_cw; i++) {
    for (j = 0; j < nof_symb_cw[i]; j++) {
      d[i][j] = srslte_random_uniform_complex_dist(random, -10, 10);
    }
  }

  /* layer mapping */
  if ((symbols_layer = srslte_layermap_type(d, x, nof_cw, nof_layers, nof_symb_cw, type)) < 0) {
    ERROR("Error layer mapper encoder\n");
    exit(-1);
  }

  /* layer de-mapping */
  if (srslte_layerdemap_type(x, dp, nof_layers, nof_cw, nof_symbols / nof_layers, nof_symb_cw, type) < 0) {
    ERROR("Error layer mapper encoder\n");
    exit(-1);
  }

  /* check errors */
  num_errors = 0;
  for (i = 0; i < nof_cw; i++) {
    for (j = 0; j < nof_symb_cw[i]; j++) {
      if (d[i][j] != dp[i][j]) {
        num_errors++;
      }
    }
  }

  for (i = 0; i < nof_cw; i++) {
    free(d[i]);
    free(dp[i]);
  }
  for (i = 0; i < nof_layers; i++) {
    free(x[i]);
  }

  srslte_random_free(random);

  if (num_errors) {
    printf("%d Errors\n", num_errors);
    exit(-1);
  } else {
    printf("Ok\n");
    exit(0);
  }
}
