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
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#include "srslte/srslte.h"

#define MSE_THRESHOLD	0.0005

int nof_symbols = 1000;
int nof_layers = 1, nof_tx_ports = 1, nof_rx_ports = 1, nof_re = 1;
char *mimo_type_name = NULL;

void usage(char *prog) {
  printf(
      "Usage: %s -m [single|diversity|multiplex|cdd] -l [nof_layers] -p [nof_tx_ports]\n"
          " -r [nof_rx_ports]\n", prog);
  printf("\t-n num_symbols [Default %d]\n", nof_symbols);
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "mplnr")) != -1) {
    switch (opt) {
    case 'n':
      nof_symbols = atoi(argv[optind]);
      break;
    case 'p':
      nof_tx_ports = atoi(argv[optind]);
      break;
    case 'r':
      nof_rx_ports = atoi(argv[optind]);
      break;
    case 'l':
      nof_layers = atoi(argv[optind]);
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

void populate_channel_cdd(cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], uint32_t n) {
  int i, j, k;

  for (i = 0; i < nof_tx_ports; i++) {
    for (j = 0; j < nof_rx_ports; j++) {
      for (k = 0; k < n; k++) {
        h[i][j][k] = (float) rand() / RAND_MAX + ((float) rand() / RAND_MAX) * _Complex_I;
      }
    }
  }
}

void populate_channel_diversity(cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], uint32_t n) {
  int i, j, k, l;

  for (i = 0; i < nof_tx_ports; i++) {
    for (j = 0; j < nof_rx_ports; j++) {
      for (k = 0; k < n / nof_layers; k++) {
        cf_t hsymb = (float) rand() / RAND_MAX + ((float) rand() / RAND_MAX) * _Complex_I;
        for (l = 0; l < nof_layers; l++) {
          // assume the channel is the same for all symbols
          h[i][j][k * nof_layers + l] = hsymb;
        }
      }
    }
  }
}

void populate_channel_single(cf_t *h) {
  int i;

  for (i = 0; i < nof_re; i++) {
    h[i] = (float) rand() / RAND_MAX + ((float) rand() / RAND_MAX) * _Complex_I;
  }
}

void populate_channel(srslte_mimo_type_t type, cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS]) {
  switch (type) {
    case SRSLTE_MIMO_TYPE_CDD:
      populate_channel_cdd(h, (uint32_t) nof_re);
      break;
    case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
      populate_channel_diversity(h, (uint32_t) nof_re);
      break;
    case SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX:
      fprintf(stderr, "Error: not implemented channel emulator\n");
      exit(-1);
      //break;
    case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
    default:
      populate_channel_single(h[0][0]);
  }
}

int main(int argc, char **argv) {
  int i, j, k;
  float mse;
  cf_t *x[SRSLTE_MAX_LAYERS], *r[SRSLTE_MAX_PORTS], *y[SRSLTE_MAX_PORTS], *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
      *xr[SRSLTE_MAX_LAYERS];
  srslte_mimo_type_t type;
  
  parse_args(argc, argv);

  /* Check input ranges */
  if (nof_tx_ports > SRSLTE_MAX_PORTS || nof_rx_ports > SRSLTE_MAX_PORTS || nof_layers > SRSLTE_MAX_LAYERS) {
    fprintf(stderr, "Invalid number of layers or ports\n");
    exit(-1);
  }

  /* Parse MIMO Type */
  if (srslte_str2mimotype(mimo_type_name, &type)) {
    fprintf(stderr, "Invalid MIMO type %s\n", mimo_type_name);
    exit(-1);
  }

  /* Check scenario conditions are OK */
  switch (type) {
    case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
      nof_re = nof_layers*nof_symbols;
      break;
    case SRSLTE_MIMO_TYPE_CDD:
      nof_re = nof_symbols*nof_tx_ports/nof_layers;
      if (nof_rx_ports != 2 || nof_tx_ports != 2) {
        fprintf(stderr, "CDD nof_tx_ports=%d nof_rx_ports=%d is not currently supported\n", nof_tx_ports, nof_rx_ports);
        exit(-1);
      }
      break;
    default:
      nof_re = nof_symbols*nof_layers;
  }

  /* Allocate x and xr (received symbols) in memory for each layer */
  for (i = 0; i < nof_layers; i++) {
    /* Source data */
    x[i] = srslte_vec_malloc(sizeof(cf_t) * nof_symbols);
    if (!x[i]) {
      perror("srslte_vec_malloc");
      exit(-1);
    }

    /* Sink data */
    xr[i] = srslte_vec_malloc(sizeof(cf_t) * nof_symbols);
    if (!xr[i]) {
      perror("srslte_vec_malloc");
      exit(-1);
    }
  }

  /* Allocate y in memory for tx each port */
  for (i = 0; i < nof_tx_ports; i++) {
    y[i] = srslte_vec_malloc(sizeof(cf_t) * nof_re);
    if (!y[i]) {
      perror("srslte_vec_malloc");
      exit(-1);
    }
  }

  /* Allocate h in memory for each cross channel and layer */
  for (i = 0; i < nof_tx_ports; i++) {
    for (j = 0; j < nof_rx_ports; j++) {
      h[i][j] = srslte_vec_malloc(sizeof(cf_t) * nof_re);
      if (!h[i][j]) {
        perror("srslte_vec_malloc");
        exit(-1);
      }
    }
  }

  /* Allocate r */
  for (i = 0; i < nof_rx_ports; i++) {
    r[i] = srslte_vec_malloc(sizeof(cf_t) * nof_re);
    if (!r[i]) {
      perror("srslte_vec_malloc");
      exit(-1);
    }
  }

  /* Generate source random data */
  for (i = 0; i < nof_layers; i++) {
    for (j = 0; j < nof_symbols; j++) {
      x[i][j] = (2 * (rand() % 2) - 1 + (2 * (rand() % 2) - 1) * _Complex_I) / sqrt(2);
    }
  }

  /* Execute Precoding (Tx) */
  if (srslte_precoding_type(x, y, nof_layers, nof_tx_ports, nof_symbols, type) < 0) {
    fprintf(stderr, "Error layer mapper encoder\n");
    exit(-1);
  }

  /* generate channel */
  populate_channel(type, h);

  /* pass signal through channel
   (we are in the frequency domain so it's a multiplication) */
  for (i = 0; i < nof_rx_ports; i++) {
    for (k = 0; k < nof_re; k++) {
      r[i][k] = (cf_t) (0.0 + 0.0 * _Complex_I);
      for (j = 0; j < nof_tx_ports; j++) {
        r[i][k] += y[j][k] * h[j][i][k];
      }
    }
  }

  /* predecoding / equalization */
  struct timeval t[3];
  gettimeofday(&t[1], NULL);
  srslte_predecoding_type_multi(r, h, xr, nof_rx_ports, nof_tx_ports, nof_layers,
                                nof_re, type, 0);
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("Execution Time: %ld us\n", t[0].tv_usec);
  
  /* check errors */
  mse = 0;
  for (i = 0; i < nof_layers; i++) {
    for (j = 0; j < nof_symbols; j++) {
      mse += cabsf(xr[i][j] - x[i][j]);
    }
  }
  printf("MSE: %f\n", mse/ nof_layers / nof_symbols );
  if (mse / nof_layers / nof_symbols > MSE_THRESHOLD) {
    exit(-1);
  } 

  /* Free all data */
  for (i = 0; i < nof_layers; i++) {
    free(x[i]);
    free(xr[i]);
  }

  for (i = 0; i < nof_rx_ports; i++) {
    free(r[i]);
  }

  for (i = 0; i < nof_rx_ports; i++) {
    for (j = 0; j < nof_tx_ports; j++) {
      free(h[j][i]);
    }
  }

  printf("Ok\n");
  exit(0); 
}
