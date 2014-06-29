/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
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

#include "liblte/phy/phy.h"

lte_cell_t cell = {
  6,            // nof_prb
  1,            // nof_ports
  1,            // cell_id
  CPNORM        // cyclic prefix
};

void usage(char *prog) {
  printf("Usage: %s [cpv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "cpnv")) != -1) {
    switch(opt) {
    case 'p':
      cell.nof_ports = atoi(argv[optind]);
      break;
    case 'n':
      cell.nof_prb = atoi(argv[optind]);
      break;
    case 'c':
      cell.id = atoi(argv[optind]);
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}


int main(int argc, char **argv) {
  pbch_t pbch;
  pbch_mib_t mib_tx, mib_rx;
  int i, j;
  cf_t *ce[MAX_PORTS];
  int nof_re;
  cf_t *sf_symbols[MAX_PORTS];

  parse_args(argc,argv);

  nof_re = 2 * CPNORM_NSYMB * cell.nof_prb * RE_X_RB;

  /* init memory */
  for (i=0;i<cell.nof_ports;i++) {
    ce[i] = malloc(sizeof(cf_t) * nof_re);
    if (!ce[i]) {
      perror("malloc");
      exit(-1);
    }
    for (j=0;j<nof_re;j++) {
      ce[i][j] = 1;
    }
    sf_symbols[i] = malloc(sizeof(cf_t) * nof_re);
    if (!sf_symbols[i]) {
      perror("malloc");
      exit(-1);
    }

  }
  if (pbch_init(&pbch, cell)) {
    fprintf(stderr, "Error creating PBCH object\n");
    exit(-1);
  }

  mib_tx.nof_ports = cell.nof_ports;
  mib_tx.nof_prb = 50;
  mib_tx.phich_length = PHICH_EXT;
  mib_tx.phich_resources = R_1_6;
  mib_tx.sfn = 124;

  pbch_encode(&pbch, &mib_tx, sf_symbols);

  /* combine outputs */
  for (i=1;i<cell.nof_ports;i++) {
    for (j=0;j<nof_re;j++) {
      sf_symbols[0][j] += sf_symbols[i][j];
    }
  }

  pbch_decode_reset(&pbch);
  if (1 != pbch_decode(&pbch, sf_symbols[0], ce, &mib_rx)) {
    printf("Error decoding\n");
    exit(-1);
  }

  pbch_free(&pbch);

  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
    free(sf_symbols[i]);
  }

  if (!memcmp(&mib_tx, &mib_rx, sizeof(pbch_mib_t))) {
    printf("OK\n");
    exit(0);
  } else {
    pbch_mib_fprint(stdout, &mib_rx);
    exit(-1);
  }
}
