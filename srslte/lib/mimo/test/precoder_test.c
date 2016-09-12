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

#define MSE_THRESHOLD	0.00001

int nof_symbols = 1000;
int nof_layers = 1, nof_ports = 1;
char *mimo_type_name = NULL;

void usage(char *prog) {
  printf(
      "Usage: %s -m [single|diversity|multiplex] -l [nof_layers] -p [nof_ports]\n",
      prog);
  printf("\t-n num_symbols [Default %d]\n", nof_symbols);
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "mpln")) != -1) {
    switch (opt) {
    case 'n':
      nof_symbols = atoi(argv[optind]);
      break;
    case 'p':
      nof_ports = atoi(argv[optind]);
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

int main(int argc, char **argv) {
  int i, j;
  float mse;
  cf_t *x[SRSLTE_MAX_LAYERS], *r[SRSLTE_MAX_PORTS], *y[SRSLTE_MAX_PORTS], *h[SRSLTE_MAX_PORTS],
      *xr[SRSLTE_MAX_LAYERS];
  srslte_mimo_type_t type;
  
  parse_args(argc, argv);

  if (nof_ports > SRSLTE_MAX_PORTS || nof_layers > SRSLTE_MAX_LAYERS) {
    fprintf(stderr, "Invalid number of layers or ports\n");
    exit(-1);
  }

  if (srslte_str2mimotype(mimo_type_name, &type)) {
    fprintf(stderr, "Invalid MIMO type %s\n", mimo_type_name);
    exit(-1);
  }

  for (i = 0; i < nof_layers; i++) {
    x[i] = srslte_vec_malloc(sizeof(cf_t) * nof_symbols);
    if (!x[i]) {
      perror("srslte_vec_malloc");
      exit(-1);
    }
    xr[i] = srslte_vec_malloc(sizeof(cf_t) * nof_symbols);
    if (!xr[i]) {
      perror("srslte_vec_malloc");
      exit(-1);
    }
  }
  for (i = 0; i < nof_ports; i++) {
    y[i] = srslte_vec_malloc(sizeof(cf_t) * nof_symbols * nof_layers);
    // TODO: The number of symbols per port is different in spatial multiplexing.
    if (!y[i]) {
      perror("srslte_vec_malloc");
      exit(-1);
    }
    h[i] = srslte_vec_malloc(sizeof(cf_t) * nof_symbols * nof_layers);
    if (!h[i]) {
      perror("srslte_vec_malloc");
      exit(-1);
    }
  }

  /* only 1 receiver antenna supported now */
  r[0] = srslte_vec_malloc(sizeof(cf_t) * nof_symbols * nof_layers);
  if (!r[0]) {
    perror("srslte_vec_malloc");
    exit(-1);
  }

  /* generate random data */
  for (i = 0; i < nof_layers; i++) {
    for (j = 0; j < nof_symbols; j++) {
      x[i][j] = (2*(rand()%2)-1+(2*(rand()%2)-1)*_Complex_I)/sqrt(2);
    }
  }
  
  /* precoding */
  if (srslte_precoding_type(x, y, nof_layers, nof_ports, nof_symbols, type) < 0) {
    fprintf(stderr, "Error layer mapper encoder\n");
    exit(-1);
  }

  /* generate channel */
  for (i = 0; i < nof_ports; i++) {
    for (j = 0; j < nof_symbols; j++) {
      h[i][nof_layers*j] = (float) rand()/RAND_MAX+((float) rand()/RAND_MAX)*_Complex_I;
      // assume the channel is time-invariant in nlayer consecutive symbols
      for (int k=0;k<nof_layers;k++) {
        h[i][nof_layers*j+k] = h[i][nof_layers*j];              
      }
    }
  }

  /* pass signal through channel
   (we are in the frequency domain so it's a multiplication) */
  /* there's only one receiver antenna, signals from different transmitter
   * ports are simply combined at the receiver
   */
  for (j = 0; j < nof_symbols * nof_layers; j++) {
    r[0][j] = 0;
    for (i = 0; i < nof_ports; i++) {
      r[0][j] += y[i][j] * h[i][j];
    }
  }
    
  /* predecoding / equalization */
  struct timeval t[3];
  gettimeofday(&t[1], NULL);
  if (srslte_predecoding_type(r[0], h, xr, nof_ports, nof_layers,
      nof_symbols * nof_layers, type, 0) < 0) {
    fprintf(stderr, "Error layer mapper encoder\n");
    exit(-1);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("Execution Time: %d us\n", t[0].tv_usec);
  
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

  for (i = 0; i < nof_layers; i++) {
    free(x[i]);
    free(xr[i]);
  }
  for (i = 0; i < nof_ports; i++) {
    free(y[i]);
    free(h[i]);
  }

  free(r[0]);
  
  printf("Ok\n");
  exit(0); 
}
