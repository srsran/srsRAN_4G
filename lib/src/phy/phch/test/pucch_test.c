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
#include <sys/time.h>
#include <unistd.h>

#include "srslte/srslte.h"

srslte_cell_t cell = {
    25,                 // nof_prb
    1,                  // nof_ports
    1,                  // cell_id
    SRSLTE_CP_NORM,     // cyclic prefix
    SRSLTE_PHICH_NORM,  // PHICH length
    SRSLTE_PHICH_R_1_6, // PHICH resources
    SRSLTE_FDD,

};

uint32_t subframe      = 0;
bool     test_cqi_only = false;

void usage(char* prog)
{
  printf("Usage: %s [csNnv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-n nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-q Test CQI encoding/decoding only [Default %s].\n", test_cqi_only ? "yes" : "no");
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "csNnqv")) != -1) {
    switch (opt) {
      case 's':
        subframe = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'q':
        test_cqi_only = true;
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

int test_uci_cqi_pucch(void)
{
  int                                   ret                                  = SRSLTE_SUCCESS;
  __attribute__((aligned(256))) uint8_t o_bits[SRSLTE_UCI_MAX_CQI_LEN_PUCCH] = {0};
  __attribute__((aligned(256))) uint8_t e_bits[SRSLTE_UCI_CQI_CODED_PUCCH_B] = {0};
  __attribute__((aligned(256))) int16_t e_symb[SRSLTE_UCI_CQI_CODED_PUCCH_B] = {0};
  __attribute__((aligned(256))) uint8_t d_bits[SRSLTE_UCI_MAX_CQI_LEN_PUCCH] = {0};

  srslte_uci_cqi_pucch_t uci_cqi_pucch = {0};

  srslte_uci_cqi_pucch_init(&uci_cqi_pucch);

  for (uint32_t nof_bits = 1; nof_bits <= SRSLTE_UCI_MAX_CQI_LEN_PUCCH - 1; nof_bits++) {
    for (uint32_t cqi = 0; cqi < (1 << nof_bits); cqi++) {
      uint32_t recv;

      uint8_t* ptr = o_bits;
      srslte_bit_unpack(cqi, &ptr, nof_bits);

      srslte_uci_encode_cqi_pucch(o_bits, nof_bits, e_bits);
      // srslte_uci_encode_cqi_pucch_from_table(&uci_cqi_pucch, o_bits, nof_bits, e_bits);
      for (int i = 0; i < SRSLTE_UCI_CQI_CODED_PUCCH_B; i++) {
        e_symb[i] = 2 * e_bits[i] - 1;
      }

      srslte_uci_decode_cqi_pucch(&uci_cqi_pucch, e_symb, d_bits, nof_bits);

      ptr  = d_bits;
      recv = srslte_bit_pack(&ptr, nof_bits);

      if (recv != cqi) {
        printf("Error! cqi = %d (len: %d), %X!=%X \n", cqi, nof_bits, cqi, recv);
        if (srslte_verbose) {
          printf("original: ");
          srslte_vec_fprint_b(stdout, o_bits, nof_bits);
          printf(" decoded: ");
          srslte_vec_fprint_b(stdout, d_bits, nof_bits);
        }
        ret = SRSLTE_ERROR;
      }
    }
  }

  srslte_uci_cqi_pucch_free(&uci_cqi_pucch);

  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }

  return ret;
}

int main(int argc, char** argv)
{
  srslte_pucch_t        pucch;
  srslte_pucch_cfg_t    pucch_cfg;
  srslte_refsignal_ul_t dmrs;
  cf_t*                 sf_symbols = NULL;
  cf_t                  pucch_dmrs[2 * SRSLTE_NRE * 3];
  int                   ret = -1;

  parse_args(argc, argv);

  if (test_cqi_only) {
    return test_uci_cqi_pucch();
  }

  if (srslte_pucch_init_ue(&pucch)) {
    ERROR("Error creating PDSCH object\n");
    exit(-1);
  }
  if (srslte_pucch_set_cell(&pucch, cell)) {
    ERROR("Error creating PDSCH object\n");
    exit(-1);
  }
  if (srslte_refsignal_ul_init(&dmrs, cell.nof_prb)) {
    ERROR("Error creating PDSCH object\n");
    exit(-1);
  }
  if (srslte_refsignal_ul_set_cell(&dmrs, cell)) {
    ERROR("Error creating PDSCH object\n");
    exit(-1);
  }

  bzero(&pucch_cfg, sizeof(srslte_pucch_cfg_t));

  if (srslte_pucch_set_rnti(&pucch, 11)) {
    ERROR("Error setting C-RNTI\n");
    goto quit;
  }

  sf_symbols = srslte_vec_cf_malloc(SRSLTE_NOF_RE(cell));
  if (!sf_symbols) {
    goto quit;
  }

  srslte_ul_sf_cfg_t ul_sf;
  ZERO_OBJECT(ul_sf);

  srslte_pucch_format_t format;
  for (format = 0; format <= SRSLTE_PUCCH_FORMAT_3; format++) {
    for (uint32_t d = 1; d <= 3; d++) {
      for (uint32_t ncs = 0; ncs < 8; ncs += d) {
        for (uint32_t n_pucch = 1; n_pucch < 130; n_pucch += 50) {

          struct timeval t[3];

          pucch_cfg.delta_pucch_shift = d;
          pucch_cfg.group_hopping_en  = false;
          pucch_cfg.N_cs              = ncs;
          pucch_cfg.n_rb_2            = 0;
          pucch_cfg.format            = format;
          pucch_cfg.n_pucch           = n_pucch;
          pucch_cfg.rnti              = 11;

          ul_sf.tti = subframe;

          srslte_uci_data_t uci_data;
          ZERO_OBJECT(uci_data);

          switch (format) {
            case SRSLTE_PUCCH_FORMAT_1:
              uci_data.value.scheduling_request = true;
              break;
            case SRSLTE_PUCCH_FORMAT_1A:
            case SRSLTE_PUCCH_FORMAT_2A:
              uci_data.value.ack.ack_value[0] = 1;
              uci_data.cfg.ack[0].nof_acks    = 1;
              break;
            case SRSLTE_PUCCH_FORMAT_1B:
            case SRSLTE_PUCCH_FORMAT_2B:
            case SRSLTE_PUCCH_FORMAT_3:
              uci_data.value.ack.ack_value[0] = 1;
              uci_data.value.ack.ack_value[1] = 1;
              uci_data.cfg.ack[0].nof_acks    = 2;
              break;
            default:
              break;
          }
          if (format >= SRSLTE_PUCCH_FORMAT_2) {
            uci_data.cfg.cqi.data_enable = true;
          }

          gettimeofday(&t[1], NULL);
          if (srslte_pucch_encode(&pucch, &ul_sf, &pucch_cfg, &uci_data.value, sf_symbols)) {
            ERROR("Error encoding PUCCH\n");
            goto quit;
          }

          if (srslte_refsignal_dmrs_pucch_gen(&dmrs, &ul_sf, &pucch_cfg, pucch_dmrs)) {
            ERROR("Error encoding PUCCH\n");
            goto quit;
          }
          if (srslte_refsignal_dmrs_pucch_put(&dmrs, &pucch_cfg, pucch_dmrs, sf_symbols)) {
            ERROR("Error encoding PUCCH\n");
            goto quit;
          }
          gettimeofday(&t[2], NULL);
          get_time_interval(t);
          INFO("format %d, n_pucch: %d, ncs: %d, d: %d, t_exec=%ld us\n", format, n_pucch, ncs, d, t[0].tv_usec);
        }
      }
    }
  }

  ret = 0;
quit:
  srslte_pucch_free(&pucch);
  srslte_refsignal_ul_free(&dmrs);

  if (sf_symbols) {
    free(sf_symbols);
  }
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
