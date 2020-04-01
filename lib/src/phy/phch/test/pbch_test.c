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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srslte/srslte.h"

srslte_cell_t cell = {
    6,                 // nof_prb
    1,                 // nof_ports
    1,                 // cell_id
    SRSLTE_CP_NORM,    // cyclic prefix
    SRSLTE_PHICH_NORM, // PHICH length
    SRSLTE_PHICH_R_1,  // PHICH resources
    SRSLTE_FDD,

};

void usage(char* prog)
{
  printf("Usage: %s [cpv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cpnv")) != -1) {
    switch (opt) {
      case 'p':
        cell.nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
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

int main(int argc, char** argv)
{
  srslte_chest_dl_res_t chest_dl_res;
  srslte_pbch_t         pbch;
  uint8_t               bch_payload_tx[SRSLTE_BCH_PAYLOAD_LEN], bch_payload_rx[SRSLTE_BCH_PAYLOAD_LEN];
  int                   i, j, k;
  int                   nof_re;
  cf_t*                 sf_symbols[SRSLTE_MAX_PORTS];
  uint32_t              nof_rx_ports;

  parse_args(argc, argv);

  nof_re = SRSLTE_SF_LEN_RE(cell.nof_prb, SRSLTE_CP_NORM);

  /* init memory */
  srslte_chest_dl_res_init(&chest_dl_res, cell.nof_prb);
  for (i = 0; i < cell.nof_ports; i++) {
    for (j = 0; j < cell.nof_ports; j++) {
      for (k = 0; k < nof_re; k++) {
        chest_dl_res.ce[i][j][k] = 1;
      }
    }
    sf_symbols[i] = srslte_vec_cf_malloc(nof_re);
    if (!sf_symbols[i]) {
      perror("malloc");
      exit(-1);
    }
  }
  if (srslte_pbch_init(&pbch)) {
    ERROR("Error creating PBCH object\n");
    exit(-1);
  }
  if (srslte_pbch_set_cell(&pbch, cell)) {
    ERROR("Error creating PBCH object\n");
    exit(-1);
  }

  srand(time(NULL));
  for (i = 0; i < SRSLTE_BCH_PAYLOAD_LEN; i++) {
    bch_payload_tx[i] = rand() % 2;
  }

  srslte_pbch_encode(&pbch, bch_payload_tx, sf_symbols, 0);

  /* combine outputs */
  for (i = 1; i < cell.nof_ports; i++) {
    for (j = 0; j < nof_re; j++) {
      sf_symbols[0][j] += sf_symbols[i][j];
    }
  }

  srslte_pbch_decode_reset(&pbch);
  if (1 != srslte_pbch_decode(&pbch, &chest_dl_res, sf_symbols, bch_payload_rx, &nof_rx_ports, NULL)) {
    printf("Error decoding\n");
    exit(-1);
  }

  srslte_pbch_free(&pbch);

  for (i = 0; i < cell.nof_ports; i++) {
    free(sf_symbols[i]);
  }

  srslte_chest_dl_res_free(&chest_dl_res);

  printf("Tx ports: %d - Rx ports: %d\n", cell.nof_ports, nof_rx_ports);
  printf("Tx payload: ");
  srslte_vec_fprint_hex(stdout, bch_payload_tx, SRSLTE_BCH_PAYLOAD_LEN);
  printf("Rx payload: ");
  srslte_vec_fprint_hex(stdout, bch_payload_rx, SRSLTE_BCH_PAYLOAD_LEN);

  if (nof_rx_ports == cell.nof_ports &&
      !memcmp(bch_payload_rx, bch_payload_tx, sizeof(uint8_t) * SRSLTE_BCH_PAYLOAD_LEN)) {
    printf("OK\n");
    exit(0);
  } else {
    printf("Error\n");
    exit(-1);
  }
}
