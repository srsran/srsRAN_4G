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

int cell_id = 1;
int nof_prb = 6;
int nof_ports = 1;
int cfi = 1;

void usage(char *prog) {
  printf("Usage: %s [cpv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell_id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p nof_ports [Default %d]\n", nof_ports);
  printf("\t-n nof_prb [Default %d]\n", nof_prb);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "cpnfv")) != -1) {
    switch (opt) {
    case 'p':
      nof_ports = atoi(argv[optind]);
      break;
    case 'f':
      cfi = atoi(argv[optind]);
      break;
    case 'n':
      nof_prb = atoi(argv[optind]);
      break;
    case 'c':
      cell_id = atoi(argv[optind]);
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

int test_dci_payload_size() {
  int i, j;
  int x[4];
  const dci_format_t formats[4] = { Format0, Format1, Format1A, Format1C };
  const int prb[6] = { 6, 15, 25, 50, 75, 100 };
  const int dci_sz[6][5] = { { 21, 19, 21, 8 }, { 22, 23, 22, 10 }, { 25, 27,
      25, 12 }, { 27, 31, 27, 13 }, { 27, 33, 27, 14 }, { 28, 39, 28, 15 } };

  printf("Testing DCI payload sizes...\n");
  printf("  PRB\t0\t1\t1A\t1C\n");
  for (i = 0; i < 6; i++) {
    int n = prb[i];
    for (j = 0; j < 4; j++) {
      x[j] = dci_format_sizeof(formats[j], n);
      if (x[j] != dci_sz[i][j]) {
        fprintf(stderr, "Invalid DCI payload size for %s\n",
            dci_format_string(formats[j]));
        return -1;
      }
    }
    printf("  %2d:\t%2d\t%2d\t%2d\t%2d\n", n, x[0], x[1], x[2], x[3]);
  }
  printf("Ok\n");
  return 0;
}

int main(int argc, char **argv) {
  pdcch_t pdcch;
  dci_t dci_tx, dci_rx;
  ra_pdsch_t ra_dl;
  regs_t regs;
  int i, j;
  cf_t *ce[MAX_PORTS];
  int nof_re;
  cf_t *slot_symbols[MAX_PORTS];
  int nof_dcis;
  int ret = -1;

  parse_args(argc, argv);

  nof_re = CPNORM_NSYMB * nof_prb * RE_X_RB;

  if (test_dci_payload_size()) {
    exit(-1);
  }

  /* init memory */
  for (i = 0; i < MAX_PORTS; i++) {
    ce[i] = malloc(sizeof(cf_t) * nof_re);
    if (!ce[i]) {
      perror("malloc");
      exit(-1);
    }
    for (j = 0; j < nof_re; j++) {
      ce[i][j] = 1;
    }
    slot_symbols[i] = malloc(sizeof(cf_t) * nof_re);
    if (!slot_symbols[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  if (regs_init(&regs, cell_id, nof_prb, nof_ports, R_1, PHICH_NORM, CPNORM)) {
    fprintf(stderr, "Error initiating regs\n");
    exit(-1);
  }

  if (regs_set_cfi(&regs, cfi)) {
    fprintf(stderr, "Error setting CFI\n");
    exit(-1);
  }

  if (pdcch_init(&pdcch, &regs, nof_prb, nof_ports, cell_id, CPNORM)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }
  if (pdcch_set_cfi(&pdcch, cfi)) {
    fprintf(stderr, "Error setting CFI %d\n", cfi);
    return -1;    
  }


  dci_init(&dci_tx, 2);
  bzero(&ra_dl, sizeof(ra_pdsch_t));
  ra_dl.harq_process = 0;
  //ra_pdsch_set_mcs_index(&ra_dl, 6);
  ra_pdsch_set_mcs(&ra_dl, QAM16, 5);
  ra_dl.ndi = 0;
  ra_dl.rv_idx = 0;
  ra_dl.alloc_type = alloc_type0;
  ra_dl.type0_alloc.rbg_bitmask = 0x5;

  dci_msg_pack_pdsch(&ra_dl, &dci_tx.msg[0], Format1, nof_prb, false);
  dci_msg_candidate_set(&dci_tx.msg[0], 0, 0, 1234);
  dci_tx.nof_dcis++;

  ra_pdsch_set_mcs(&ra_dl, QAM16, 15);
  dci_msg_pack_pdsch(&ra_dl, &dci_tx.msg[1], Format1, nof_prb, false);
  dci_msg_candidate_set(&dci_tx.msg[1], 0, 1, 1234);
  dci_tx.nof_dcis++;

  pdcch_encode(&pdcch, &dci_tx, slot_symbols, 0);

  /* combine outputs */
  for (i = 1; i < nof_ports; i++) {
    for (j = 0; j < nof_re; j++) {
      slot_symbols[0][j] += slot_symbols[i][j];
    }
  }

  pdcch_init_search_ue(&pdcch, 1234);

  dci_init(&dci_rx, 2);
  nof_dcis = pdcch_decode(&pdcch, slot_symbols[0], ce, &dci_rx, 0);
  if (nof_dcis < 0) {
    printf("Error decoding\n");
  } else if (nof_dcis == dci_tx.nof_dcis) {
    for (i = 0; i < nof_dcis; i++) {
      if (dci_tx.msg[i].location.L != dci_rx.msg[i].location.L
          || dci_tx.msg[i].location.ncce != dci_rx.msg[i].location.ncce
          || dci_tx.msg[i].location.nof_bits != dci_rx.msg[i].location.nof_bits
          || dci_tx.msg[i].location.rnti != dci_rx.msg[i].location.rnti) {
        printf("Error in DCI %d: Received location does not match\n", i);
        dci_candidate_fprint(stdout, &dci_tx.msg[i].location);
        dci_candidate_fprint(stdout, &dci_rx.msg[i].location);
        goto quit;
      }

      if (memcmp(dci_tx.msg[i].data, dci_rx.msg[i].data,
          dci_tx.msg[i].location.nof_bits)) {
        printf("Error in DCI %d: Received data does not match\n", i);
        goto quit;
      }
    }
  } else {
    printf("Transmitted %d DCIs but got %d\n", dci_tx.nof_dcis, nof_dcis);
    goto quit;
  }
  ret = 0;
  quit: pdcch_free(&pdcch);
  regs_free(&regs);
  dci_free(&dci_tx);
  dci_free(&dci_rx);

  for (i = 0; i < MAX_PORTS; i++) {
    free(ce[i]);
    free(slot_symbols[i]);
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
