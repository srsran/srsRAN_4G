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
#include <unistd.h>

#include "srslte/srslte.h"

srslte_cell_t cell = {.nof_prb         = 6,
                      .nof_ports       = 1,
                      .id              = 1,
                      .cp              = SRSLTE_CP_NORM,
                      .phich_resources = SRSLTE_PHICH_R_1,
                      .phich_length    = SRSLTE_PHICH_NORM};

uint32_t         cfi        = 1;
uint32_t         nof_rx_ant = 1;
bool             print_dci_table;
srslte_dci_cfg_t dci_cfg = {};

void usage(char* prog)
{
  printf("Usage: %s [cfpndxv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-A nof_rx_ant [Default %d]\n", nof_rx_ant);
  printf("\t-d Print DCI table [Default %s]\n", print_dci_table ? "yes" : "no");
  printf("\t-x Enable/Disable Cross-scheduling [Default %s]\n", dci_cfg.cif_enabled ? "enabled" : "disabled");
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cfpndvAx")) != -1) {
    switch (opt) {
      case 'p':
        cell.nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        cfi = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'A':
        nof_rx_ant = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        print_dci_table = true;
        break;
      case 'x':
        dci_cfg.cif_enabled ^= true;
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

int test_dci_payload_size()
{
  int                       i, j;
  int                       x[5];
  const srslte_dci_format_t formats[] = {
      SRSLTE_DCI_FORMAT0, SRSLTE_DCI_FORMAT1, SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1C, SRSLTE_DCI_FORMAT2A};
  const int prb[6]       = {6, 15, 25, 50, 75, 100};
  const int dci_sz[6][5] = {{21, 19, 21, 8, 28},
                            {22, 23, 22, 10, 31},
                            {25, 27, 25, 12, 36},
                            {27, 31, 27, 13, 41},
                            {27, 33, 27, 14, 42},
                            {28, 39, 28, 15, 48}};

  srslte_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);

  srslte_cell_t cell_test;
  ZERO_OBJECT(cell_test);
  cell_test.nof_ports = 1;

  ZERO_OBJECT(dci_cfg);

  printf("Testing DCI payload sizes...\n");
  printf("  PRB\t0\t1\t1A\t1C\t2A\n");
  for (i = 0; i < 6; i++) {
    int n             = prb[i];
    cell_test.nof_prb = n;

    for (j = 0; j < 5; j++) {
      x[j] = srslte_dci_format_sizeof(&cell_test, &dl_sf, &dci_cfg, formats[j]);
      if (x[j] != dci_sz[i][j]) {
        ERROR("Invalid DCI payload size for %s and %d PRB. Is %d and should be %d\n",
              srslte_dci_format_string(formats[j]),
              n,
              x[j],
              dci_sz[i][j]);
        return -1;
      }
    }
    printf("  %2d:\t%2d\t%2d\t%2d\t%2d\t%2d\n", n, x[0], x[1], x[2], x[3], x[4]);
  }
  printf("Ok\n");

  if (print_dci_table) {
    printf("dci_sz_table[101][4] = {\n");
    for (i = 0; i <= 100; i++) {
      printf("  {");
      for (j = 0; j < 4; j++) {
        cell_test.nof_prb = i;
        printf("%d", srslte_dci_format_sizeof(&cell, &dl_sf, &dci_cfg, formats[j]));
        if (j < 3) {
          printf(", ");
        }
      }
      if (i < 100) {
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
  srslte_dci_msg_t      dci_tx, dci_rx;
  srslte_dci_location_t dci_location;
  srslte_dci_format_t   dci_format;
  srslte_dci_dl_t       ra_dl_tx;
  srslte_dci_dl_t       ra_dl_rx;
} testcase_dci_t;

int main(int argc, char** argv)
{

  srslte_chest_dl_res_t chest_dl_res;
  srslte_pdcch_t        pdcch_tx, pdcch_rx;
  testcase_dci_t        testcases[10];
  srslte_regs_t         regs;
  int                   i;
  int                   nof_re;
  cf_t*                 slot_symbols[SRSLTE_MAX_PORTS];
  int                   nof_dcis;

  bzero(&testcases, sizeof(testcase_dci_t) * 10);

  int ret = -1;

  parse_args(argc, argv);

  nof_re = SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;

  if (test_dci_payload_size()) {
    exit(-1);
  }

  /* init memory */

  srslte_chest_dl_res_init(&chest_dl_res, cell.nof_prb);
  srslte_chest_dl_res_set_identity(&chest_dl_res);

  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    slot_symbols[i] = srslte_vec_cf_malloc(nof_re);
    if (!slot_symbols[i]) {
      perror("malloc");
      exit(-1);
    }
    srslte_vec_cf_zero(slot_symbols[i], nof_re);
  }

  if (srslte_regs_init(&regs, cell)) {
    ERROR("Error initiating regs\n");
    exit(-1);
  }

  if (srslte_pdcch_init_enb(&pdcch_tx, cell.nof_prb)) {
    ERROR("Error creating PDCCH object\n");
    exit(-1);
  }
  if (srslte_pdcch_set_cell(&pdcch_tx, &regs, cell)) {
    ERROR("Error setting cell in PDCCH object\n");
    exit(-1);
  }

  if (srslte_pdcch_init_ue(&pdcch_rx, cell.nof_prb, nof_rx_ant)) {
    ERROR("Error creating PDCCH object\n");
    exit(-1);
  }
  if (srslte_pdcch_set_cell(&pdcch_rx, &regs, cell)) {
    ERROR("Error setting cell in PDCCH object\n");
    exit(-1);
  }

  /* Resource allocate init */
  nof_dcis = 0;
  srslte_dci_dl_t dci;
  ZERO_OBJECT(dci);
  dci.pid                     = 5;
  dci.tb[0].mcs_idx           = 5;
  dci.tb[0].ndi               = 0;
  dci.tb[0].rv                = 1;
  dci.alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci.type0_alloc.rbg_bitmask = 0x5;
  dci.cif_present             = dci_cfg.cif_enabled;
  if (dci_cfg.cif_enabled) {
    dci.cif = (uint32_t)(random() & 0x7);
  }

  /* Format 1 Test case */
  if (cell.nof_ports == 1) {
    testcases[nof_dcis].dci_format = SRSLTE_DCI_FORMAT1;
    if (dci_cfg.cif_enabled) {
      dci.cif = (uint32_t)(random() & 0x7);
    }
    testcases[nof_dcis].ra_dl_tx = dci;
    nof_dcis++;

    /* Format 1 Test case */
    dci.tb[0].mcs_idx              = 15;
    testcases[nof_dcis].dci_format = SRSLTE_DCI_FORMAT1;
    if (dci_cfg.cif_enabled) {
      dci.cif = (uint32_t)(random() & 0x7);
    }
    testcases[nof_dcis].ra_dl_tx = dci;
    nof_dcis++;
  }

  /* Tx Diversity Test case */
  if (cell.nof_ports > 1) {
    dci.tb[1].mcs_idx              = 13;
    dci.tb[1].rv                   = 3;
    dci.tb[1].ndi                  = true;
    testcases[nof_dcis].dci_format = SRSLTE_DCI_FORMAT2A;
    if (dci_cfg.cif_enabled) {
      dci.cif = (uint32_t)(random() & 0x7);
    }
    testcases[nof_dcis].ra_dl_tx = dci;
    nof_dcis++;
  }

  /* CDD Spatial Multiplexing Test case */
  if (cell.nof_ports > 1) {
    dci.tb[1].mcs_idx              = 28;
    dci.tb[1].rv                   = 1;
    dci.tb[1].ndi                  = false;
    testcases[nof_dcis].dci_format = SRSLTE_DCI_FORMAT2;
    if (dci_cfg.cif_enabled) {
      dci.cif = (uint32_t)(random() & 0x7);
    }
    testcases[nof_dcis].ra_dl_tx = dci;
    nof_dcis++;
  }

  srslte_dci_cfg_t dci_cfg;
  ZERO_OBJECT(dci_cfg);

  srslte_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);
  dl_sf.cfi = cfi;

  for (int s = 0; s < 10; s++) {
    dl_sf.tti = s;
    printf("Encoding %d DCIs for sf_idx=%d\n", nof_dcis, s);
    /* Execute Rx */
    for (i = 0; i < nof_dcis; i++) {
      testcases[i].ra_dl_tx.rnti   = (uint16_t)(1234 + i);
      testcases[i].ra_dl_tx.format = testcases[i].dci_format;

      srslte_dci_msg_pack_pdsch(&cell, &dl_sf, &dci_cfg, &testcases[i].ra_dl_tx, &testcases[i].dci_tx);
      srslte_dci_location_set(&testcases[i].dci_location, 0, (uint32_t)i);

      testcases[i].dci_tx.format   = testcases[i].dci_format;
      testcases[i].dci_tx.location = testcases[i].dci_location;

      // Enable just 1 TB per default
      if (testcases[i].dci_format < SRSLTE_DCI_FORMAT2) {
        for (int j = 1; j < SRSLTE_MAX_CODEWORDS; j++) {
          SRSLTE_DCI_TB_DISABLE(testcases[i].ra_dl_tx.tb[j]);
        }
      }

      if (srslte_pdcch_encode(&pdcch_tx, &dl_sf, &testcases[i].dci_tx, slot_symbols)) {
        ERROR("Error encoding DCI message\n");
        goto quit;
      }
    }

    /* Execute 'Rx' */
    if (srslte_pdcch_extract_llr(&pdcch_rx, &dl_sf, &chest_dl_res, slot_symbols)) {
      ERROR("Error extracting LLRs\n");
      goto quit;
    }

    /* Decode DCIs */
    for (i = 0; i < nof_dcis; i++) {
      testcases[i].dci_rx.format   = testcases[i].dci_format;
      testcases[i].dci_rx.location = testcases[i].dci_location;
      if (srslte_pdcch_decode_msg(&pdcch_rx, &dl_sf, &dci_cfg, &testcases[i].dci_rx)) {
        ERROR("Error decoding DCI message\n");
        goto quit;
      }
      if (srslte_dci_msg_unpack_pdsch(&cell, &dl_sf, &dci_cfg, &testcases[i].dci_rx, &testcases[i].ra_dl_rx)) {
        ERROR("Error unpacking DCI message\n");
        goto quit;
      }
      if (testcases[i].dci_rx.rnti >= 1234 && testcases[i].dci_rx.rnti < 1234 + nof_dcis) {
        testcases[i].dci_rx.rnti -= 1234;
      } else {
        printf("Received invalid DCI CRC %d\n", testcases[i].dci_rx.rnti);
        goto quit;
      }
    }

    /* Compare Tx and Rx */
    for (i = 0; i < nof_dcis; i++) {
      if (memcmp(testcases[i].dci_tx.payload, testcases[i].dci_rx.payload, testcases[i].dci_tx.nof_bits)) {
        printf("Error in DCI %d: Received data does not match\n", i);
        goto quit;
      }
#if SRSLTE_DCI_HEXDEBUG
      // Ignore Hex str
      bzero(testcases[i].ra_dl_rx.hex_str, sizeof(testcases[i].ra_dl_rx.hex_str));
      testcases[i].ra_dl_rx.nof_bits = 0;
#endif
      // Ignore DCI location
      testcases[i].ra_dl_rx.location = testcases[i].ra_dl_tx.location;
      // Ignore cw_idx
      for (int j = 0; j < SRSLTE_MAX_CODEWORDS; j++) {
        testcases[i].ra_dl_rx.tb[j].cw_idx = testcases[i].ra_dl_tx.tb[j].cw_idx;
      }
      if (memcmp(&testcases[i].ra_dl_tx, &testcases[i].ra_dl_rx, sizeof(srslte_dci_dl_t))) {
        uint8_t* x = (uint8_t*)&testcases[i].ra_dl_rx;
        uint8_t* y = (uint8_t*)&testcases[i].ra_dl_tx;
        for (int j = 0; j < sizeof(srslte_dci_dl_t); j++) {
          if (x[j] != y[j]) {
            printf("error in byte %d, rx=%d, tx=%d\n", j, x[j], y[j]);
          }
        }
        printf("tx: ");
        srslte_vec_fprint_byte(stdout, (uint8_t*)&testcases[i].ra_dl_tx, sizeof(srslte_dci_dl_t));
        printf("rx: ");
        srslte_vec_fprint_byte(stdout, (uint8_t*)&testcases[i].ra_dl_rx, sizeof(srslte_dci_dl_t));
        printf("Error in RA %d: Received data does not match\n", i);
        printf("     Field    |    Tx    |    Rx    \n");
        printf("--------------+----------+----------\n");
        if (testcases[i].ra_dl_tx.cif) {
          printf("          cif | %8d | %8d\n", testcases[i].ra_dl_tx.cif, testcases[i].ra_dl_rx.cif);
        }
        printf(" harq_process | %8d | %8d\n", testcases[i].ra_dl_tx.pid, testcases[i].ra_dl_rx.pid);
        printf("      mcs_idx | %8d | %8d\n", testcases[i].ra_dl_tx.tb[0].mcs_idx, testcases[i].ra_dl_rx.tb[0].mcs_idx);
        printf("       rv_idx | %8d | %8d\n", testcases[i].ra_dl_tx.tb[0].rv, testcases[i].ra_dl_rx.tb[0].rv);
        printf("          ndi | %8d | %8d\n", testcases[i].ra_dl_tx.tb[0].ndi, testcases[i].ra_dl_rx.tb[0].ndi);
        printf("    mcs_idx_1 | %8d | %8d\n", testcases[i].ra_dl_tx.tb[1].mcs_idx, testcases[i].ra_dl_rx.tb[1].mcs_idx);
        printf("     rv_idx_1 | %8d | %8d\n", testcases[i].ra_dl_tx.tb[1].rv, testcases[i].ra_dl_rx.tb[1].rv);
        printf("        ndi_1 | %8d | %8d\n", testcases[i].ra_dl_tx.tb[1].ndi, testcases[i].ra_dl_rx.tb[1].ndi);
        printf("   tb_cw_swap | %8d | %8d\n", testcases[i].ra_dl_tx.tb_cw_swap, testcases[i].ra_dl_rx.tb_cw_swap);
        printf("      sram_id | %8d | %8d\n", testcases[i].ra_dl_tx.sram_id, testcases[i].ra_dl_rx.sram_id);
        printf("        pinfo | %8d | %8d\n", testcases[i].ra_dl_tx.pinfo, testcases[i].ra_dl_rx.pinfo);
        printf("        pconf | %8d | %8d\n", testcases[i].ra_dl_tx.pconf, testcases[i].ra_dl_rx.pconf);
        printf(" power_offset | %8d | %8d\n", testcases[i].ra_dl_tx.power_offset, testcases[i].ra_dl_rx.power_offset);
        printf("    tpc_pucch | %8d | %8d\n", testcases[i].ra_dl_tx.tpc_pucch, testcases[i].ra_dl_rx.tpc_pucch);
        goto quit;
      }
    }
  }
  ret = 0;

quit:
  srslte_pdcch_free(&pdcch_tx);
  srslte_pdcch_free(&pdcch_rx);
  srslte_chest_dl_res_free(&chest_dl_res);
  srslte_regs_free(&regs);

  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    free(slot_symbols[i]);
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
