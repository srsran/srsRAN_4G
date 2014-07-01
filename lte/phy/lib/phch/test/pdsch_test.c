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
#include <sys/time.h>

#include "liblte/phy/phy.h"

lte_cell_t cell = {
  6,            // nof_prb
  1,            // nof_ports
  1,            // cell_id
  CPNORM        // cyclic prefix
};

uint32_t cfi = 1;
uint32_t tbs = -1;
uint32_t subframe = 1;
ra_mod_t modulation = BPSK;

void usage(char *prog) {
  printf("Usage: %s [cell.cpnfvmt] -l TBS \n", prog);
  printf("\t-m modulation (1: BPSK, 2: QPSK, 3: QAM16, 4: QAM64) [Default BPSK]\n");
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "lcell.cpnfvmts")) != -1) {
    switch(opt) {
    case 'm':
      switch(atoi(argv[optind])) {
      case 1:
        modulation = BPSK;
        break;
      case 2:
        modulation = QPSK;
        break;
      case 4:
        modulation = QAM16;
        break;
      case 6:
        modulation = QAM64;
        break;
      default:
        fprintf(stderr, "Invalid modulation %d. Possible values: "
            "(1: BPSK, 2: QPSK, 3: QAM16, 4: QAM64)\n", atoi(argv[optind]));
        break;
      }
      break;
    case 's':
      subframe = atoi(argv[optind]);
      break;
    case 'l':
      tbs = atoi(argv[optind]);
      break;
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
  if (tbs == -1) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char **argv) {
  pdsch_t pdsch;
  int i, j;
  char *data = NULL;
  cf_t *ce[MAX_PORTS];
  int nof_re;
  cf_t *slot_symbols[MAX_PORTS];
  int ret = -1;
  struct timeval t[3];
  ra_mcs_t mcs;
  ra_prb_t prb_alloc;

  parse_args(argc,argv);

  nof_re = 2 * CPNORM_NSYMB * cell.nof_prb * RE_X_RB;

  mcs.tbs = tbs;
  mcs.mod = modulation;
  prb_alloc.slot[0].nof_prb = cell.nof_prb;
  for (i=0;i<prb_alloc.slot[0].nof_prb;i++) {
    prb_alloc.slot[0].prb_idx[i] = i;
  }
  memcpy(&prb_alloc.slot[1], &prb_alloc.slot[0], sizeof(ra_prb_slot_t));

  ra_prb_get_re_dl(&prb_alloc, cell.nof_prb, cell.nof_ports, 2, CPNORM);

  /* init memory */
  for (i=0;i<cell.nof_ports;i++) {
    ce[i] = malloc(sizeof(cf_t) * nof_re);
    if (!ce[i]) {
      perror("malloc");
      goto quit;
    }
    for (j=0;j<nof_re;j++) {
      ce[i][j] = 1;
    }
    slot_symbols[i] = calloc(sizeof(cf_t) , nof_re);
    if (!slot_symbols[i]) {
      perror("malloc");
      goto quit;
    }
  }

  data = malloc(sizeof(char) * mcs.tbs);
  if (!data) {
    perror("malloc");
    goto quit;
  }

  if (pdsch_init(&pdsch, 1234, cell)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    goto quit;
  }

  for (i=0;i<mcs.tbs;i++) {
    data[i] = rand()%2;
  }

  pdsch_encode(&pdsch, data, slot_symbols, subframe, mcs, &prb_alloc);

  /* combine outputs */
  for (i=0;i<cell.nof_ports;i++) {
    for (j=0;j<nof_re;j++) {
      if (i > 0) {
        slot_symbols[0][j] += slot_symbols[i][j];
      }
      ce[i][j] = 1;
    }
  }
  
  gettimeofday(&t[1], NULL);
  int r = pdsch_decode(&pdsch, slot_symbols[0], ce, data, subframe, mcs, &prb_alloc);
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  if (r) {
    printf("Error decoding\n");
    ret = -1;
  } else {
    printf("DECODED OK in %d:%d (%.2f Mbps)\n", (int) t[0].tv_sec, (int) t[0].tv_usec, (float) mcs.tbs/t[0].tv_usec);
  }
  ret = 0;
quit:
  pdsch_free(&pdsch);

  for (i=0;i<cell.nof_ports;i++) {
    if (ce[i]) {
      free(ce[i]);
    }
    if (slot_symbols[i]) {
      free(slot_symbols[i]);
    }
  }
  if (data) {
    free(data);
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
