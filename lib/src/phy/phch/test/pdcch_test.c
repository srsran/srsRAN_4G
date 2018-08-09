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
#include <unistd.h>
#include <srslte/phy/common/phy_common.h>
#include <srslte/phy/phch/ra.h>
#include <srslte/phy/phch/dci.h>
#include <srslte/phy/phch/pdcch.h>

#include "srslte/srslte.h"

srslte_cell_t cell = {
  .nof_prb = 6,
  .nof_ports = 1,
  .id = 1,
  .cp = SRSLTE_CP_NORM,
  .phich_resources = SRSLTE_PHICH_R_1,
  .phich_length = SRSLTE_PHICH_NORM
};

uint32_t cfi = 1;
uint32_t nof_rx_ant = 1;
bool print_dci_table;

void usage(char *prog) {
  printf("Usage: %s [cfpndv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-A nof_rx_ant [Default %d]\n", nof_rx_ant);
  printf("\t-d Print DCI table [Default %s]\n", print_dci_table?"yes":"no");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "cfpndvA")) != -1) {
    switch (opt) {
    case 'p':
      cell.nof_ports = (uint32_t) atoi(argv[optind]);
      break;
    case 'f':
      cfi = (uint32_t) atoi(argv[optind]);
      break;
    case 'n':
      cell.nof_prb = (uint32_t) atoi(argv[optind]);
      break;
    case 'c':
      cell.id = (uint32_t) atoi(argv[optind]);
      break;
    case 'A':
      nof_rx_ant = (uint32_t) atoi(argv[optind]);
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
  int x[5];
  const srslte_dci_format_t formats[] = { SRSLTE_DCI_FORMAT0, SRSLTE_DCI_FORMAT1, SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1C, SRSLTE_DCI_FORMAT2A };
  const int prb[6] = { 6, 15, 25, 50, 75, 100 };
  const int dci_sz[6][5] = { { 21, 19, 21, 8, 28 }, { 22, 23, 22, 10 , 31}, { 25, 27,
      25, 12 , 36}, { 27, 31, 27, 13 , 41}, { 27, 33, 27, 14 , 42}, { 28, 39, 28, 15, 48 }};


  printf("Testing DCI payload sizes...\n");
  printf("  PRB\t0\t1\t1A\t1C\t2A\n");
  for (i = 0; i < 6; i++) {
    int n = prb[i];
    for (j = 0; j < 5; j++) {
      x[j] = srslte_dci_format_sizeof(formats[j], (uint32_t) n, 1);
      if (x[j] != dci_sz[i][j]) {
        fprintf(stderr, "Invalid DCI payload size for %s\n",
            srslte_dci_format_string(formats[j]));
        return -1;
      }
    }
    printf("  %2d:\t%2d\t%2d\t%2d\t%2d\t%2d\n", n, x[0], x[1], x[2], x[3], x[4]);
  }
  printf("Ok\n");
  
  if (print_dci_table) {
    printf("dci_sz_table[101][4] = {\n");
    for (i=0;i<=100;i++) {
      printf("  {");
      for (j=0;j<4;j++) {
        printf("%d",srslte_dci_format_sizeof(formats[j], (uint32_t) i, 1));
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

typedef struct {
  srslte_dci_msg_t dci_tx, dci_rx;
  srslte_dci_location_t dci_location;
  srslte_dci_format_t dci_format;
  srslte_ra_dl_dci_t ra_dl_tx;
  srslte_ra_dl_dci_t ra_dl_rx;
} testcase_dci_t;

int main(int argc, char **argv) {
  srslte_pdcch_t pdcch_tx, pdcch_rx;
  testcase_dci_t testcases[10];
  srslte_ra_dl_dci_t ra_dl;
  srslte_regs_t regs;
  int i, j, k;
  cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  int nof_re;
  cf_t *tx_slot_symbols[SRSLTE_MAX_PORTS], *rx_slot_symbols[SRSLTE_MAX_PORTS];
  int nof_dcis;

  bzero(&testcases, sizeof(testcase_dci_t)*10);

  int ret = -1;

  parse_args(argc, argv);

  nof_re = SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;

  if (test_dci_payload_size()) {
    exit(-1);
  }

  /* init memory */
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    for (j = 0; j < SRSLTE_MAX_PORTS; j++) {
      ce[i][j] = malloc(sizeof(cf_t) * nof_re);
      if (!ce[i][j]) {
        perror("malloc");
        exit(-1);
      }
      for (k = 0; k < nof_re; k++) {
        //ce[i][j][k] = (i == j) ? 1 : 0;
        ce[i][j][k] = ((float)rand()/(float)RAND_MAX) + _Complex_I*((float)rand()/(float)RAND_MAX);
      }
    }
    tx_slot_symbols[i] = malloc(sizeof(cf_t) * nof_re);
    if (!tx_slot_symbols[i]) {
      perror("malloc");
      exit(-1);
    }
    bzero(tx_slot_symbols[i], sizeof(cf_t) * nof_re);

    rx_slot_symbols[i] = malloc(sizeof(cf_t) * nof_re);
    if (!rx_slot_symbols[i]) {
      perror("malloc");
      exit(-1);
    }
    bzero(rx_slot_symbols[i], sizeof(cf_t) * nof_re);
  }

  if (srslte_regs_init(&regs, cell)) {
    fprintf(stderr, "Error initiating regs\n");
    exit(-1);
  }

  if (srslte_pdcch_init_enb(&pdcch_tx, cell.nof_prb)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }
  if (srslte_pdcch_set_cell(&pdcch_tx, &regs, cell)) {
    fprintf(stderr, "Error setting cell in PDCCH object\n");
    exit(-1);
  }

  if (srslte_pdcch_init_ue(&pdcch_rx, cell.nof_prb, nof_rx_ant)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }
  if (srslte_pdcch_set_cell(&pdcch_rx, &regs, cell)) {
    fprintf(stderr, "Error setting cell in PDCCH object\n");
    exit(-1);
  }

  /* Resource allocate init */
  nof_dcis = 0;
  bzero(&ra_dl, sizeof(srslte_ra_dl_dci_t));
  ra_dl.harq_process = 0;
  ra_dl.mcs_idx = 5;
  ra_dl.ndi = 0;
  ra_dl.rv_idx = 0;
  ra_dl.alloc_type = SRSLTE_RA_ALLOC_TYPE0;
  ra_dl.type0_alloc.rbg_bitmask = 0x5;
  ra_dl.tb_en[0] = true;

  /* Format 1 Test case */
  testcases[nof_dcis].dci_format = SRSLTE_DCI_FORMAT1;
  testcases[nof_dcis].ra_dl_tx = ra_dl;
  nof_dcis++;

  /* Format 1 Test case */
  ra_dl.mcs_idx = 15;
  testcases[nof_dcis].dci_format = SRSLTE_DCI_FORMAT1;
  testcases[nof_dcis].ra_dl_tx = ra_dl;
  nof_dcis++;

  /* Tx Diversity Test case */
  if (cell.nof_ports > 1) {
    ra_dl.mcs_idx_1 = 0;
    ra_dl.rv_idx_1 = 0;
    ra_dl.ndi_1 = false;
    ra_dl.tb_en[1] = false;
    testcases[nof_dcis].dci_format = SRSLTE_DCI_FORMAT2A;
    testcases[nof_dcis].ra_dl_tx = ra_dl;
    nof_dcis++;
  }

  /* CDD Spatial Multiplexing Test case */
  if (cell.nof_ports > 1) {
    ra_dl.mcs_idx_1 = 28;
    ra_dl.rv_idx_1 = 1;
    ra_dl.ndi_1 = false;
    ra_dl.tb_en[1] = true;
    testcases[nof_dcis].dci_format = SRSLTE_DCI_FORMAT2A;
    testcases[nof_dcis].ra_dl_tx = ra_dl;
    nof_dcis++;
  }

  /* Execute Rx */
  for (i=0;i<nof_dcis;i++) {
    srslte_dci_msg_pack_pdsch(&testcases[i].ra_dl_tx, testcases[i].dci_format, &testcases[i].dci_tx,
                              cell.nof_prb, cell.nof_ports, false);
    srslte_dci_location_set(&testcases[i].dci_location, 0, (uint32_t) i);

    if (srslte_pdcch_encode(&pdcch_tx, &testcases[i].dci_tx, testcases[i].dci_location, (uint16_t) (1234 + i),
                            tx_slot_symbols, 0, cfi)) {
      fprintf(stderr, "Error encoding DCI message\n");
      goto quit;
    }
  }

  /* Apply channel */
  for (j = 0; j < nof_rx_ant; j++) {
    for (k = 0; k < nof_re; k++) {
      for (i = 0; i < cell.nof_ports; i++) {
        rx_slot_symbols[j][k] += tx_slot_symbols[i][k]*ce[i][j][k];
      }
    }
  }

  /* Execute 'Rx' */
  if (srslte_pdcch_extract_llr_multi(&pdcch_rx, rx_slot_symbols, ce, 0, 0, cfi)) {
    fprintf(stderr, "Error extracting LLRs\n");
    goto quit;
  }

  /* Decode DCIs */
  for (i=0;i<nof_dcis;i++) {
    uint16_t crc_rem;
    if (srslte_pdcch_decode_msg(&pdcch_rx, &testcases[i].dci_rx, &testcases[i].dci_location, testcases[i].dci_format, cfi, &crc_rem)) {
      fprintf(stderr, "Error decoding DCI message\n");
      goto quit;
    }
    if (srslte_dci_msg_unpack_pdsch(&testcases[i].dci_rx, &testcases[i].ra_dl_rx, cell.nof_prb, cell.nof_ports, false)) {
      fprintf(stderr, "Error unpacking DCI message\n");
      goto quit;
    }
    if (crc_rem >= 1234 && crc_rem < 1234 + nof_dcis) {
      crc_rem -= 1234;
    } else {
      printf("Received invalid DCI CRC 0x%x\n", crc_rem);
      goto quit;
    }
  }

  /* Compare Tx and Rx */
  for (i = 0; i < nof_dcis; i++) {
    if (memcmp(testcases[i].dci_tx.data, testcases[i].dci_rx.data, testcases[i].dci_tx.nof_bits)) {
      printf("Error in DCI %d: Received data does not match\n", i);
      goto quit;
    }
    if (memcmp(&testcases[i].ra_dl_tx, &testcases[i].ra_dl_rx, sizeof(srslte_ra_dl_dci_t))) {
      printf("Error in RA %d: Received data does not match\n", i);
      printf("     Field    |    Tx    |    Rx    \n");
      printf("--------------+----------+----------\n");
      printf(" harq_process | %8d | %8d\n", testcases[i].ra_dl_tx.harq_process, testcases[i].ra_dl_rx.harq_process);
      printf("      mcs_idx | %8d | %8d\n", testcases[i].ra_dl_tx.mcs_idx, testcases[i].ra_dl_rx.mcs_idx);
      printf("       rv_idx | %8d | %8d\n", testcases[i].ra_dl_tx.rv_idx, testcases[i].ra_dl_rx.rv_idx);
      printf("          ndi | %8d | %8d\n", testcases[i].ra_dl_tx.ndi, testcases[i].ra_dl_rx.ndi);
      printf("    mcs_idx_1 | %8d | %8d\n", testcases[i].ra_dl_tx.mcs_idx_1, testcases[i].ra_dl_rx.mcs_idx_1);
      printf("     rv_idx_1 | %8d | %8d\n", testcases[i].ra_dl_tx.rv_idx_1, testcases[i].ra_dl_rx.rv_idx_1);
      printf("        ndi_1 | %8d | %8d\n", testcases[i].ra_dl_tx.ndi_1, testcases[i].ra_dl_rx.ndi_1);
      printf("   tb_cw_swap | %8d | %8d\n", testcases[i].ra_dl_tx.tb_cw_swap, testcases[i].ra_dl_rx.tb_cw_swap);
      printf("      sram_id | %8d | %8d\n", testcases[i].ra_dl_tx.sram_id, testcases[i].ra_dl_rx.sram_id);
      printf("        pinfo | %8d | %8d\n", testcases[i].ra_dl_tx.pinfo, testcases[i].ra_dl_rx.pinfo);
      printf("        pconf | %8d | %8d\n", testcases[i].ra_dl_tx.pconf, testcases[i].ra_dl_rx.pconf);
      printf(" power_offset | %8d | %8d\n", testcases[i].ra_dl_tx.power_offset, testcases[i].ra_dl_rx.power_offset);
      printf("    tpc_pucch | %8d | %8d\n", testcases[i].ra_dl_tx.tpc_pucch, testcases[i].ra_dl_rx.tpc_pucch);
      printf("     tb_en[0] | %8d | %8d\n", testcases[i].ra_dl_tx.tb_en[0], testcases[i].ra_dl_rx.tb_en[0]);
      printf("     tb_en[1] | %8d | %8d\n", testcases[i].ra_dl_tx.tb_en[1], testcases[i].ra_dl_rx.tb_en[1]);
      printf("    dci_is_1a | %8d | %8d\n", testcases[i].ra_dl_tx.dci_is_1a, testcases[i].ra_dl_rx.dci_is_1a);
      printf("    dci_is_1c | %8d | %8d\n", testcases[i].ra_dl_tx.dci_is_1c, testcases[i].ra_dl_rx.dci_is_1c);
      goto quit;
    }
  }
  ret = 0;

quit: 
  srslte_pdcch_free(&pdcch_tx);
  srslte_pdcch_free(&pdcch_rx);
  srslte_regs_free(&regs);

  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    for (j = 0; j < SRSLTE_MAX_PORTS; j++) {
      free(ce[i][j]);
    }
    free(tx_slot_symbols[i]);
    free(rx_slot_symbols[i]);
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  srslte_dft_exit();
  exit(ret);
}
