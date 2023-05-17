/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

#include "srsran/srsran.h"

srsran_cell_t cell = {
    6,                 // nof_prb
    1,                 // nof_ports
    1,                 // cell_id
    SRSRAN_CP_NORM,    // cyclic prefix
    SRSRAN_PHICH_NORM, // PHICH length
    SRSRAN_PHICH_R_1,  // PHICH resources
    SRSRAN_FDD,

};

void usage(char* prog)
{
  printf("Usage: %s [cpv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
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
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  srsran_chest_dl_res_t chest_dl_res;
  srsran_pbch_t         pbch;
  uint8_t               bch_payload_tx[SRSRAN_BCH_PAYLOAD_LEN], bch_payload_rx[SRSRAN_BCH_PAYLOAD_LEN];
  int                   i, j, k;
  int                   nof_re;
  cf_t*                 sf_symbols[SRSRAN_MAX_PORTS];
  uint32_t              nof_rx_ports;
  srsran_random_t       random_gen = srsran_random_init(0x1234);

  parse_args(argc, argv);

  nof_re = SRSRAN_SF_LEN_RE(cell.nof_prb, SRSRAN_CP_NORM);

  /* init memory */
  srsran_chest_dl_res_init(&chest_dl_res, cell.nof_prb);
  for (i = 0; i < cell.nof_ports; i++) {
    for (j = 0; j < cell.nof_ports; j++) {
      for (k = 0; k < nof_re; k++) {
        chest_dl_res.ce[i][j][k] = 1;
      }
    }
    sf_symbols[i] = srsran_vec_cf_malloc(nof_re);
    if (!sf_symbols[i]) {
      perror("malloc");
      exit(-1);
    }
  }
  if (srsran_pbch_init(&pbch)) {
    ERROR("Error creating PBCH object");
    exit(-1);
  }
  if (srsran_pbch_set_cell(&pbch, cell)) {
    ERROR("Error creating PBCH object");
    exit(-1);
  }

  for (i = 0; i < SRSRAN_BCH_PAYLOAD_LEN; i++) {
    bch_payload_tx[i] = (uint8_t)srsran_random_uniform_int_dist(random_gen, 0, 1);
  }

  srsran_pbch_encode(&pbch, bch_payload_tx, sf_symbols, 0);

  /* combine outputs */
  for (i = 1; i < cell.nof_ports; i++) {
    for (j = 0; j < nof_re; j++) {
      sf_symbols[0][j] += sf_symbols[i][j];
    }
  }

  srsran_pbch_decode_reset(&pbch);
  if (1 != srsran_pbch_decode(&pbch, &chest_dl_res, sf_symbols, bch_payload_rx, &nof_rx_ports, NULL)) {
    printf("Error decoding\n");
    exit(-1);
  }

  srsran_pbch_free(&pbch);

  for (i = 0; i < cell.nof_ports; i++) {
    free(sf_symbols[i]);
  }

  srsran_chest_dl_res_free(&chest_dl_res);
  srsran_random_free(random_gen);

  printf("Tx ports: %d - Rx ports: %d\n", cell.nof_ports, nof_rx_ports);
  printf("Tx payload: ");
  srsran_vec_fprint_hex(stdout, bch_payload_tx, SRSRAN_BCH_PAYLOAD_LEN);
  printf("Rx payload: ");
  srsran_vec_fprint_hex(stdout, bch_payload_rx, SRSRAN_BCH_PAYLOAD_LEN);

  if (nof_rx_ports == cell.nof_ports &&
      !memcmp(bch_payload_rx, bch_payload_tx, sizeof(uint8_t) * SRSRAN_BCH_PAYLOAD_LEN)) {
    printf("OK\n");
    exit(0);
  } else {
    printf("Error\n");
    exit(-1);
  }
}
