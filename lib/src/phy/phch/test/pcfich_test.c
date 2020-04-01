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
#include <unistd.h>

#include "srslte/srslte.h"

srslte_cell_t cell = {
    6,                 // nof_prb
    1,                 // nof_ports
    1000,              // cell_id
    SRSLTE_CP_NORM,    // cyclic prefix
    SRSLTE_PHICH_NORM, // PHICH length
    SRSLTE_PHICH_R_1,  // PHICH resources
    SRSLTE_FDD,

};

void usage(char* prog)
{
  printf("Usage: %s [cpv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-p nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n nof_prb [Default %d]\n", cell.nof_prb);
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
  srslte_chest_dl_res_t chest_res;
  srslte_pcfich_t       pcfich;
  srslte_regs_t         regs;
  int                   i, j;
  int                   nof_re;
  cf_t*                 slot_symbols[SRSLTE_MAX_PORTS];
  uint32_t              cfi, nsf;
  int                   cid, max_cid;
  float                 corr_res;

  parse_args(argc, argv);

  nof_re = SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;

  /* init memory */
  srslte_chest_dl_res_init(&chest_res, cell.nof_prb);
  srslte_chest_dl_res_set_identity(&chest_res);
  srslte_chest_dl_res_set_identity(&chest_res);

  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    slot_symbols[i] = srslte_vec_cf_malloc(nof_re);
    if (!slot_symbols[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  if (cell.id == 1000) {
    cid     = 0;
    max_cid = 503;
  } else {
    cid     = cell.id;
    max_cid = cell.id;
  }

  srslte_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);

  while (cid <= max_cid) {
    cell.id = cid;

    printf("Testing CellID=%d...\n", cid);

    if (srslte_regs_init(&regs, cell)) {
      ERROR("Error initiating regs\n");
      exit(-1);
    }

    if (srslte_pcfich_init(&pcfich, 1)) {
      ERROR("Error creating PBCH object\n");
      exit(-1);
    }
    if (srslte_pcfich_set_cell(&pcfich, &regs, cell)) {
      ERROR("Error creating PBCH object\n");
      exit(-1);
    }

    for (nsf = 0; nsf < 10; nsf++) {
      dl_sf.tti = nsf;

      for (cfi = 1; cfi < 4; cfi++) {
        dl_sf.cfi = cfi;
        srslte_pcfich_encode(&pcfich, &dl_sf, slot_symbols);

        /* combine outputs */
        for (i = 1; i < cell.nof_ports; i++) {
          for (j = 0; j < nof_re; j++) {
            slot_symbols[0][j] += slot_symbols[i][j];
          }
        }
        if (srslte_pcfich_decode(&pcfich, &dl_sf, &chest_res, slot_symbols, &corr_res) < 0) {
          exit(-1);
        }
        INFO("cfi_tx: %d, cfi_rx: %d, ns: %d, distance: %f\n", cfi, dl_sf.cfi, nsf, corr_res);
      }
    }
    srslte_pcfich_free(&pcfich);
    srslte_regs_free(&regs);
    cid++;
  }
  srslte_chest_dl_res_free(&chest_res);
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    free(slot_symbols[i]);
  }
  printf("OK\n");
  exit(0);
}
