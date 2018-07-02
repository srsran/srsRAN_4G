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
#include <unistd.h>

#include "srslte/srslte.h"

srslte_cell_t cell = {
    .nof_prb = 100,
    .nof_ports = 1,
    .id = 1,
    .cp = SRSLTE_CP_NORM,
    .phich_resources = SRSLTE_PHICH_R_1,
    .phich_length = SRSLTE_PHICH_NORM
};

uint32_t tm = 0;
srslte_mimo_type_t mimo_type = SRSLTE_MIMO_TYPE_SINGLE_ANTENNA;
uint32_t sf_idx = 5;
uint32_t cfi = 3;
uint32_t nof_rx_ant = 1;
uint32_t nof_subframes = 10;
uint16_t rnti = 0x1234;
bool print_dci_table;

void usage(char *prog) {
  printf("Usage: %s [cfpndvs]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-f cfi [Default %d]\n", cfi);
  printf("\t-p cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-s number of subframes to simulate [Default %d]\n", nof_subframes);
  printf("\t-d Print DCI table [Default %s]\n", print_dci_table ? "yes" : "no");
  printf("\t-x MIMO Type: single, diversity, cdd, multiplex [Default %s]\n", srslte_mimotype2str(mimo_type));
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "cfpndvsx")) != -1) {
    switch (opt) {
      case 'x':
        if (srslte_str2mimotype(argv[optind], &mimo_type)) {
          fprintf(stderr, "'%s' is not a valid MIMO type\n", argv[optind]);
          usage(argv[0]);
          exit(SRSLTE_ERROR);
        }
        if (mimo_type == SRSLTE_MIMO_TYPE_SINGLE_ANTENNA) {
          cell.nof_ports = 1;
          nof_rx_ant = 1;
          tm = 0;
        } else {
          cell.nof_ports = 2;
          nof_rx_ant = 2;
          if (mimo_type == SRSLTE_MIMO_TYPE_TX_DIVERSITY) {
            tm = 2;
          }
        }
        break;
      case 'f':
        cfi = (uint32_t) atoi(argv[optind]);
        break;
      case 'p':
        cell.nof_prb = (uint32_t) atoi(argv[optind]);
        break;
      case 'c':
        cell.id = (uint32_t) atoi(argv[optind]);
        break;
      case 's':
        nof_subframes = (uint32_t) atoi(argv[optind]);
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

int prbset_num = 1, last_prbset_num = 1;
int prbset_orig = 0;

unsigned int
reverse(register unsigned int x) {
  x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
  x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
  x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
  x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
  return ((x >> 16) | (x << 16));

}

uint32_t prbset_to_bitmask() {
  uint32_t mask = 0;
  int nb = (int) ceilf((float) cell.nof_prb / srslte_ra_type0_P(cell.nof_prb));
  for (int i = 0; i < nb; i++) {
    if (i >= prbset_orig && i < prbset_orig + prbset_num) {
      mask = mask | (0x1 << i);
    }
  }
  return reverse(mask) >> (32 - nb);
}

int main(int argc, char **argv) {
  srslte_enb_dl_t enb_dl = {};
  srslte_ue_dl_t ue_dl = {};
  srslte_softbuffer_tx_t *softbuffer_tx[SRSLTE_MAX_TB] = {};
  srslte_softbuffer_rx_t *softbuffer_rx[SRSLTE_MAX_TB] = {};
  uint8_t *data_tx[SRSLTE_MAX_TB] = {};
  uint8_t *data_rx[SRSLTE_MAX_TB] = {};
  uint32_t count_failures = 0;

  int ret = -1;

  parse_args(argc, argv);

  cf_t *signal_buffer[SRSLTE_MAX_PORTS] = {NULL};

  /*
   * Allocate Memory
   */
  for (int i = 0; i < cell.nof_ports; i++) {
    signal_buffer[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
    if (!signal_buffer[i]) {
      fprintf(stderr, "Error allocating buffer\n");
      goto quit;
    }
  }

  for (int i = 0; i < SRSLTE_MAX_TB; i++) {
    softbuffer_tx[i] = (srslte_softbuffer_tx_t *) calloc(sizeof(srslte_softbuffer_tx_t), 1);
    if (!softbuffer_tx[i]) {
      fprintf(stderr, "Error allocating softbuffer_tx\n");
      goto quit;
    }

    if (srslte_softbuffer_tx_init(softbuffer_tx[i], cell.nof_prb)) {
      fprintf(stderr, "Error initiating softbuffer_tx\n");
      goto quit;
    }

    softbuffer_rx[i] = (srslte_softbuffer_rx_t *) calloc(sizeof(srslte_softbuffer_rx_t), 1);
    if (!softbuffer_rx[i]) {
      fprintf(stderr, "Error allocating softbuffer_rx\n");
      goto quit;
    }

    if (srslte_softbuffer_rx_init(softbuffer_rx[i], cell.nof_prb)) {
      fprintf(stderr, "Error initiating softbuffer_rx\n");
      goto quit;
    }

    data_tx[i] = srslte_vec_malloc(sizeof(uint8_t) * 150000);
    if (!data_tx[i]) {
      fprintf(stderr, "Error allocating data tx\n");
      goto quit;
    }

    data_rx[i] = srslte_vec_malloc(sizeof(uint8_t) * 150000);
    if (!data_rx[i]) {
      fprintf(stderr, "Error allocating data tx\n");
      goto quit;
    }
  }

  /*
   * Initialise eNb
   */
  if (srslte_enb_dl_init(&enb_dl, signal_buffer, cell.nof_prb)) {
    fprintf(stderr, "Error initiating eNb downlink\n");
    goto quit;
  }

  if (srslte_enb_dl_set_cell(&enb_dl, cell)) {
    fprintf(stderr, "Error setting eNb DL cell\n");
    goto quit;
  }

  srslte_enb_dl_set_cfi(&enb_dl, cfi);
  srslte_enb_dl_set_power_allocation(&enb_dl, 0.0f, 0.0f); /* Default: none */

  /*
   * Initialise UE
   */
  if (srslte_ue_dl_init(&ue_dl, signal_buffer, cell.nof_prb, nof_rx_ant)) {
    fprintf(stderr, "Error initiating UE downlink\n");
    goto quit;
  }

  if (srslte_ue_dl_set_cell(&ue_dl, cell)) {
    fprintf(stderr, "Error setting UE downlink cell\n");
    goto quit;
  }

  srslte_ue_dl_set_rnti(&ue_dl, rnti);

  /*
   * Loop
   */
  for (uint32_t sf_idx = 0; sf_idx < nof_subframes; sf_idx++) {
    bool acks[SRSLTE_MAX_TB] = {};

    /* Run eNodeB */
    srslte_enb_dl_clear_sf(&enb_dl);

    srslte_enb_dl_put_base(&enb_dl, sf_idx);

    srslte_ra_dl_dci_t dci = {};
    srslte_dci_format_t dci_format = SRSLTE_DCI_FORMAT1A;
    srslte_ra_dl_grant_t grant = {};

    /* Pupulate TB Common */
    dci.harq_process = 0;

    /* Pupulate TB0 */
    dci.mcs_idx = 0;
    dci.ndi = 0;
    dci.rv_idx = 0;
    dci.tb_en[0] = true;

    if (mimo_type == SRSLTE_MIMO_TYPE_CDD || mimo_type == SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX) {
      dci_format = SRSLTE_DCI_FORMAT2B;

      /* Pupulate TB1 */
      dci.mcs_idx_1 = 0;
      dci.ndi_1 = 0;
      dci.rv_idx_1 = 0;
      dci.tb_en[1] = true;

      /* Pupulate Allocation */
      dci.alloc_type = SRSLTE_RA_ALLOC_TYPE0;
      dci.alloc_type = SRSLTE_RA_ALLOC_TYPE0;
      dci.type0_alloc.rbg_bitmask = prbset_to_bitmask();
    } else {
      dci_format = SRSLTE_DCI_FORMAT1A;
      dci.alloc_type = SRSLTE_RA_ALLOC_TYPE2;
      dci.type2_alloc.riv = 0;
      dci.type2_alloc.L_crb = 4;
      dci.type2_alloc.RB_start = 0;
      dci.type2_alloc.n_prb1a = 1;
      dci.type2_alloc.n_gap = 0;
      dci.type2_alloc.mode = 0;
    }

    dci.dci_is_1a = (dci_format == SRSLTE_DCI_FORMAT1A);
    dci.dci_is_1c = (dci_format == SRSLTE_DCI_FORMAT1C);

    srslte_ra_dl_dci_to_grant(&dci, cell.nof_prb, rnti, &grant);

    srslte_dci_location_t location = {
        .ncce = 0,
        .L = 2
    };

    for (int t = 0; t < SRSLTE_RA_DL_GRANT_NOF_TB(&grant); t++) {
      for (int i = 0; i < grant.mcs->tbs; i++) {
        data_tx[t][i] = (uint8_t) (rand() & 0xff);
      }
    }

    if (srslte_enb_dl_put_pdcch_dl(&enb_dl,
                                   &dci,
                                   dci_format,
                                   location,
                                   rnti,
                                   sf_idx % 10) < 0) {
      fprintf(stderr, "Error putting PDCCH\n");
      goto quit;
    }

    if (srslte_enb_dl_put_pdsch(&enb_dl,
                                &grant,
                                softbuffer_tx,
                                rnti,
                                (int[SRSLTE_MAX_CODEWORDS]) {dci.rv_idx, dci.rv_idx_1},
                                sf_idx % 10,
                                data_tx,
                                mimo_type) < 0) {
      fprintf(stderr, "Error putting PDSCH\n");
      goto quit;
    }

    srslte_enb_dl_gen_signal(&enb_dl);

    /* Run UE */
    int n = srslte_ue_dl_decode(&ue_dl, data_rx, tm, sf_idx, acks);
    if (n < 0) {
      fprintf(stderr, "Error decoding PDSCH\n");
      goto quit;
    }

    for (int i = 0; i < SRSLTE_RA_DL_GRANT_NOF_TB(&grant); i++) {
      if (!acks[i]) {
        INFO("UE Failed decoding subframe %d\n", sf_idx);
        count_failures++;
      }
    }
  }

  printf("Finished! The UE failed decoding %d of %d.\n", count_failures, nof_subframes);
  if (!count_failures) {
    ret = SRSLTE_SUCCESS;
  }

  quit:
  srslte_enb_dl_free(&enb_dl);
  srslte_ue_dl_free(&ue_dl);

  for (
      int i = 0;
      i < cell.
          nof_ports;
      i++) {
    if (signal_buffer[i]) {
      free(signal_buffer[i]);
    }
  }

  for (
      int i = 0;
      i < SRSLTE_MAX_TB; i++) {
    if (softbuffer_tx[i]) {
      srslte_softbuffer_tx_free(softbuffer_tx[i]);
      free(softbuffer_tx[i]);
    }

    if (softbuffer_rx[i]) {
      srslte_softbuffer_rx_free(softbuffer_rx[i]);
      free(softbuffer_rx[i]);
    }

    if (data_tx[i]) {
      free(data_tx[i]);
    }

    if (data_rx[i]) {
      free(data_rx[i]);
    }
  }

  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  srslte_dft_exit();
  exit(ret);
}
