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

#include "srslte/srslte.h"


srslte_cell_t cell = {
  6,            // nof_prb
  1,            // nof_ports
  1000,         // cell_id
  SRSLTE_CP_NORM,       // cyclic prefix
  SRSLTE_PHICH_R_1,          // PHICH resources      
  SRSLTE_PHICH_NORM    // PHICH length
};

srslte_phich_resources_t phich_res = SRSLTE_PHICH_R_1;
srslte_phich_length_t phich_length = SRSLTE_PHICH_NORM;

void usage(char *prog) {
  printf("Usage: %s [cpvgel]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-g phich ng factor: 1/6, 1/2, 1, 2 [Default 1]\n");
  printf("\t-e phich extended length [Default normal]\n");
  printf("\t-l extended cyclic prefix [Default normal]\n");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "cpnvgel")) != -1) {
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
    case 'g':
      if (!strcmp(argv[optind], "1/6")) {
        phich_res = SRSLTE_PHICH_R_1_6;
      } else if (!strcmp(argv[optind], "1/2")) {
        phich_res = SRSLTE_PHICH_R_1_2;
      } else if (!strcmp(argv[optind], "1")) {
        phich_res = SRSLTE_PHICH_R_1;
      } else if (!strcmp(argv[optind], "2")) {
        phich_res = SRSLTE_PHICH_R_2;
      } else {
        fprintf(stderr, "Invalid phich ng factor %s. Setting to default.\n", argv[optind]);
      }
      break;
    case 'e':
      phich_length = SRSLTE_PHICH_EXT;
      break;
    case 'l':
      cell.cp = SRSLTE_CP_EXT;
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}


int main(int argc, char **argv) {
  srslte_phich_t phich;
  srslte_regs_t regs;
  int i, j;
  cf_t *ce[SRSLTE_MAX_PORTS];
  int nof_re;
  cf_t *slot_symbols[SRSLTE_MAX_PORTS];
  uint8_t ack[50][SRSLTE_PHICH_NORM_NSEQUENCES], ack_rx;
  uint32_t nsf; 
  float distance;
  int cid, max_cid;
  uint32_t ngroup, nseq, max_nseq;

  parse_args(argc,argv);

  max_nseq = SRSLTE_CP_ISNORM(cell.cp)?SRSLTE_PHICH_NORM_NSEQUENCES:SRSLTE_PHICH_EXT_NSEQUENCES;

  nof_re = SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;

  /* init memory */
  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    ce[i] = malloc(sizeof(cf_t) * nof_re);
    if (!ce[i]) {
      perror("malloc");
      exit(-1);
    }
    for (j=0;j<nof_re;j++) {
      ce[i][j] = 1;
    }
    slot_symbols[i] = malloc(sizeof(cf_t) * nof_re);
    if (!slot_symbols[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  if (cell.id == 1000) {
    cid = 0;
    max_cid = 503;
  } else {
    cid = cell.id;
    max_cid = cell.id;
  }
  while(cid <= max_cid) {
    cell.id = cid;
    
    printf("Testing CellID=%d...\n", cid);

    if (srslte_regs_init(&regs, cell)) {
      fprintf(stderr, "Error initiating regs\n");
      exit(-1);
    }

    if (srslte_phich_init(&phich, &regs, cell)) {
      fprintf(stderr, "Error creating PBCH object\n");
      exit(-1);
    }

    for (nsf=0;nsf<10;nsf++) {

      srslte_phich_reset(&phich, slot_symbols);

      /* Transmit all PHICH groups and sequence numbers */
      for (ngroup=0;ngroup<srslte_phich_ngroups(&phich);ngroup++) {
        for (nseq=0;nseq<max_nseq;nseq++) {

          ack[ngroup][nseq] = rand()%2;

          srslte_phich_encode(&phich, ack[ngroup][nseq], ngroup, nseq, nsf, slot_symbols);
        }
      }
      /* combine outputs */
      for (i=1;i<cell.nof_ports;i++) {
        for (j=0;j<nof_re;j++) {
          slot_symbols[0][j] += slot_symbols[i][j];
        }
      }

      /* Receive all PHICH groups and sequence numbers */
      for (ngroup=0;ngroup<srslte_phich_ngroups(&phich);ngroup++) {
        for (nseq=0;nseq<max_nseq;nseq++) {

          if (srslte_phich_decode(&phich, slot_symbols[0], ce, 0, ngroup, nseq, nsf, &ack_rx, &distance)<0) {
            printf("Error decoding ACK\n");
            exit(-1);
          }
          INFO("%d/%d, ack_tx: %d, ack_rx: %d, ns: %d, distance: %f\n",
              ngroup, nseq, ack[ngroup][nseq], ack_rx, nsf, distance);
          if (ack[ngroup][nseq] != ack_rx) {
            printf("Invalid received ACK: %d!=%d\n", ack[ngroup][nseq], ack_rx);
            exit(-1);
          }
          if (distance < 1.5) {
            printf("Error\n");
            exit(-1);
          }
        }
      }
    }
    srslte_phich_free(&phich);
    srslte_regs_free(&regs);
    cid++;
  }

  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    free(ce[i]);
    free(slot_symbols[i]);
  }
  printf("OK\n");
  exit(0);
}
