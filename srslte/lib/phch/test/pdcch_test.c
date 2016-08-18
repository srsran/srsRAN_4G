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
  1,            // cell_id
  SRSLTE_CP_NORM,       // cyclic prefix
  SRSLTE_PHICH_R_1,          // PHICH resources      
  SRSLTE_PHICH_NORM    // PHICH length
};

uint32_t cfi = 1;
bool print_dci_table; 

void usage(char *prog) {
  printf("Usage: %s [cfpndv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-d Print DCI table [Default %s]\n", print_dci_table?"yes":"no");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "cfpndv")) != -1) {
    switch (opt) {
    case 'p':
      cell.nof_ports = atoi(argv[optind]);
      break;
    case 'f':
      cfi = atoi(argv[optind]);
      break;
    case 'n':
      cell.nof_prb = atoi(argv[optind]);
      break;
    case 'c':
      cell.id = atoi(argv[optind]);
      break;
    case 'd':
      print_dci_table = true;
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

int test_dci_payload_size() {
  int i, j;
  int x[4];
  const srslte_dci_format_t formats[4] = { SRSLTE_DCI_FORMAT0, SRSLTE_DCI_FORMAT1, SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1C };
  const int prb[6] = { 6, 15, 25, 50, 75, 100 };
  const int dci_sz[6][5] = { { 21, 19, 21, 8 }, { 22, 23, 22, 10 }, { 25, 27,
      25, 12 }, { 27, 31, 27, 13 }, { 27, 33, 27, 14 }, { 28, 39, 28, 15 } };

  printf("Testing DCI payload sizes...\n");
  printf("  PRB\t0\t1\t1A\t1C\n");
  for (i = 0; i < 6; i++) {
    int n = prb[i];
    for (j = 0; j < 4; j++) {
      x[j] = srslte_dci_format_sizeof(formats[j], n, 1);
      if (x[j] != dci_sz[i][j]) {
        fprintf(stderr, "Invalid DCI payload size for %s\n",
            srslte_dci_format_string(formats[j]));
        return -1;
      }
    }
    printf("  %2d:\t%2d\t%2d\t%2d\t%2d\n", n, x[0], x[1], x[2], x[3]);
  }
  printf("Ok\n");
  
  if (print_dci_table) {
    printf("dci_sz_table[101][4] = {\n");
    for (i=0;i<=100;i++) {
      printf("  {");
      for (int j=0;j<4;j++) {
        printf("%d",srslte_dci_format_sizeof(formats[j], i, 1));
        if (j<3) {
          printf(", ");
        }
      }
      if (i<100) {
        printf("},\n");
      } else {
        printf("}\n");
      }
    }
    printf("};\n");
  }
  return 0;
}

int main(int argc, char **argv) {
  srslte_pdcch_t pdcch;
  srslte_dci_msg_t dci_tx[2], dci_rx[2], dci_tmp;
  srslte_dci_location_t dci_locations[2];
  srslte_ra_dl_dci_t ra_dl;
  srslte_regs_t regs;
  int i, j;
  cf_t *ce[SRSLTE_MAX_PORTS];
  int nof_re;
  cf_t *slot_symbols[SRSLTE_MAX_PORTS];
  int nof_dcis; 

  int ret = -1;

  parse_args(argc, argv);

  nof_re = SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;

  if (test_dci_payload_size()) {
    exit(-1);
  }

  /* init memory */
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
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

  if (srslte_regs_init(&regs, cell)) {
    fprintf(stderr, "Error initiating regs\n");
    exit(-1);
  }

  if (srslte_regs_set_cfi(&regs, cfi)) {
    fprintf(stderr, "Error setting CFI\n");
    exit(-1);
  }

  if (srslte_pdcch_init(&pdcch, &regs, cell)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }

  nof_dcis = 2;
  bzero(&ra_dl, sizeof(srslte_ra_dl_dci_t));
  ra_dl.harq_process = 0;
  ra_dl.mcs_idx = 5;
  ra_dl.ndi = 0;
  ra_dl.rv_idx = 0;
  ra_dl.alloc_type = SRSLTE_RA_ALLOC_TYPE0;
  ra_dl.type0_alloc.rbg_bitmask = 0x5;

  srslte_dci_msg_pack_pdsch(&ra_dl, SRSLTE_DCI_FORMAT1, &dci_tx[0], cell.nof_prb, false);
  srslte_dci_location_set(&dci_locations[0], 0, 0);

  ra_dl.mcs_idx = 15;
  srslte_dci_msg_pack_pdsch(&ra_dl, SRSLTE_DCI_FORMAT1, &dci_tx[1], cell.nof_prb, false);
  srslte_dci_location_set(&dci_locations[1], 0, 1);
  
  for (i=0;i<nof_dcis;i++) {
    if (srslte_pdcch_encode(&pdcch, &dci_tx[i], dci_locations[i], 1234+i, slot_symbols, 0, cfi)) {
      fprintf(stderr, "Error encoding DCI message\n");
      goto quit;
    }
  }

  srslte_vec_fprint_b(stdout, dci_tx[0].data, dci_tx[0].nof_bits);
  /* combine outputs */
  for (i = 1; i < cell.nof_ports; i++) {
    for (j = 0; j < nof_re; j++) {
      slot_symbols[0][j] += slot_symbols[i][j];
    }
  }

  for (i=0;i<2;i++) {
    if (srslte_pdcch_extract_llr(&pdcch, slot_symbols[0], ce, 0, 0, cfi)) {
      fprintf(stderr, "Error extracting LLRs\n");
      goto quit;
    }
    uint16_t crc_rem; 
    if (srslte_pdcch_decode_msg(&pdcch, &dci_tmp, &dci_locations[i], SRSLTE_DCI_FORMAT1, &crc_rem)) {
      fprintf(stderr, "Error decoding DCI message\n");
      goto quit;
    }      
    if (crc_rem >= 1234 && crc_rem < 1234 + nof_dcis) {
      crc_rem -= 1234;
        memcpy(&dci_rx[crc_rem], &dci_tmp, sizeof(srslte_dci_msg_t));
    } else {
      printf("Received invalid DCI CRC 0x%x\n", crc_rem);
      goto quit;
    }
  }
  for (i = 0; i < nof_dcis; i++) {
    if (memcmp(dci_tx[i].data, dci_rx[i].data, dci_tx[i].nof_bits)) {
      printf("Error in DCI %d: Received data does not match\n", i);
      goto quit;
    }
  }
  ret = 0;

quit: 
  srslte_pdcch_free(&pdcch);
  srslte_regs_free(&regs);

  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
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
