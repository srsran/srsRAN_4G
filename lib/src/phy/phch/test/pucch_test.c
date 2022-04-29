/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
#include <sys/time.h>
#include <unistd.h>

#include "srsran/srsran.h"

static srsran_cell_t cell = {
    25,                 // nof_prb
    1,                  // nof_ports
    1,                  // cell_id
    SRSRAN_CP_NORM,     // cyclic prefix
    SRSRAN_PHICH_NORM,  // PHICH length
    SRSRAN_PHICH_R_1_6, // PHICH resources
    SRSRAN_FDD,

};

static uint32_t subframe      = 0;
static bool     test_cqi_only = false;
static float    snr_db        = 20.0f;

static void usage(char* prog)
{
  printf("Usage: %s [csNnv]\n", prog);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-n nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-q Test CQI encoding/decoding only [Default %s].\n", test_cqi_only ? "yes" : "no");
  printf("\t-S Signal to Noise Ratio in dB [Default %.2f].\n", snr_db);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "csNnqSv")) != -1) {
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
      case 'S':
        snr_db = strtof(argv[optind], NULL);
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

int test_uci_cqi_pucch(void)
{
  int                                   ret                                  = SRSRAN_SUCCESS;
  __attribute__((aligned(256))) uint8_t o_bits[SRSRAN_UCI_MAX_CQI_LEN_PUCCH] = {0};
  __attribute__((aligned(256))) uint8_t e_bits[SRSRAN_UCI_CQI_CODED_PUCCH_B] = {0};
  __attribute__((aligned(256))) int16_t e_symb[SRSRAN_CQI_MAX_BITS]          = {0};
  __attribute__((aligned(256))) uint8_t d_bits[SRSRAN_UCI_MAX_CQI_LEN_PUCCH] = {0};

  srsran_uci_cqi_pucch_t uci_cqi_pucch = {0};

  srsran_uci_cqi_pucch_init(&uci_cqi_pucch);

  for (uint32_t nof_bits = 1; nof_bits <= SRSRAN_UCI_MAX_CQI_LEN_PUCCH - 1; nof_bits++) {
    for (uint32_t cqi = 0; cqi < (1 << nof_bits); cqi++) {
      uint32_t recv;

      uint8_t* ptr = o_bits;
      srsran_bit_unpack(cqi, &ptr, nof_bits);

      srsran_uci_encode_cqi_pucch(o_bits, nof_bits, e_bits);
      // srsran_uci_encode_cqi_pucch_from_table(&uci_cqi_pucch, o_bits, nof_bits, e_bits);
      for (int i = 0; i < SRSRAN_UCI_CQI_CODED_PUCCH_B; i++) {
        e_symb[i] = 2 * e_bits[i] - 1;
      }

      srsran_uci_decode_cqi_pucch(&uci_cqi_pucch, e_symb, d_bits, nof_bits);

      ptr  = d_bits;
      recv = srsran_bit_pack(&ptr, nof_bits);

      if (recv != cqi) {
        printf("Error! cqi = %d (len: %d), %X!=%X \n", cqi, nof_bits, cqi, recv);
        if (get_srsran_verbose_level()) {
          printf("original: ");
          srsran_vec_fprint_b(stdout, o_bits, nof_bits);
          printf(" decoded: ");
          srsran_vec_fprint_b(stdout, d_bits, nof_bits);
        }
        ret = SRSRAN_ERROR;
      }
    }
  }

  srsran_uci_cqi_pucch_free(&uci_cqi_pucch);

  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }

  return ret;
}

int main(int argc, char** argv)
{
  srsran_pucch_t        pucch_ue   = {};
  srsran_pucch_t        pucch_enb  = {};
  srsran_pucch_cfg_t    pucch_cfg  = {};
  srsran_refsignal_ul_t dmrs       = {};
  cf_t*                 sf_symbols = NULL;
  cf_t                  pucch_dmrs[2 * SRSRAN_NRE * 3];
  int                   ret       = -1;
  srsran_chest_ul_t     chest     = {};
  srsran_chest_ul_res_t chest_res = {};
  srsran_channel_awgn_t awgn      = {};

  parse_args(argc, argv);

  if (test_cqi_only) {
    return test_uci_cqi_pucch();
  }

  if (srsran_pucch_init_ue(&pucch_ue)) {
    ERROR("Error creating PDSCH object");
    exit(-1);
  }
  if (srsran_pucch_set_cell(&pucch_ue, cell)) {
    ERROR("Error creating PDSCH object");
    exit(-1);
  }
  if (srsran_pucch_init_enb(&pucch_enb)) {
    ERROR("Error creating PDSCH object");
    exit(-1);
  }
  if (srsran_pucch_set_cell(&pucch_enb, cell)) {
    ERROR("Error creating PDSCH object");
    exit(-1);
  }
  if (srsran_refsignal_ul_set_cell(&dmrs, cell)) {
    ERROR("Error creating PDSCH object");
    exit(-1);
  }

  if (srsran_chest_ul_init(&chest, cell.nof_prb) < SRSRAN_SUCCESS) {
    ERROR("Error initiating channel estimator");
    goto quit;
  }

  if (srsran_chest_ul_res_init(&chest_res, cell.nof_prb) < SRSRAN_SUCCESS) {
    ERROR("Error initiating channel estimator result");
    goto quit;
  }

  if (srsran_chest_ul_set_cell(&chest, cell) < SRSRAN_SUCCESS) {
    ERROR("Error setting channel estimator cell");
    goto quit;
  }

  if (srsran_channel_awgn_init(&awgn, 0x1234) < SRSRAN_SUCCESS) {
    ERROR("Error initiating AWGN");
    goto quit;
  }
  if (srsran_channel_awgn_set_n0(&awgn, -snr_db) < SRSRAN_SUCCESS) {
    ERROR("Error setting AWGN");
    goto quit;
  }

  sf_symbols = srsran_vec_cf_malloc(SRSRAN_NOF_RE(cell));
  if (!sf_symbols) {
    goto quit;
  }

  srsran_ul_sf_cfg_t ul_sf;
  ZERO_OBJECT(ul_sf);

  srsran_pucch_format_t format;
  for (format = 0; format < SRSRAN_PUCCH_FORMAT_ERROR; format++) {
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

          srsran_uci_data_t uci_data;
          ZERO_OBJECT(uci_data);

          switch (format) {
            case SRSRAN_PUCCH_FORMAT_1:
              uci_data.value.scheduling_request = true;
              break;
            case SRSRAN_PUCCH_FORMAT_1A:
            case SRSRAN_PUCCH_FORMAT_2A:
              uci_data.value.ack.ack_value[0] = 1;
              uci_data.cfg.ack[0].nof_acks    = 1;
              break;
            case SRSRAN_PUCCH_FORMAT_1B:
            case SRSRAN_PUCCH_FORMAT_2B:
            case SRSRAN_PUCCH_FORMAT_3:
              uci_data.value.ack.ack_value[0] = 1;
              uci_data.value.ack.ack_value[1] = 1;
              uci_data.cfg.ack[0].nof_acks    = 2;
              break;
            default:
              break;
          }
          if (format >= SRSRAN_PUCCH_FORMAT_2) {
            uci_data.cfg.cqi.data_enable = true;
          }

          // Encode PUCCH signals
          gettimeofday(&t[1], NULL);
          if (srsran_pucch_encode(&pucch_ue, &ul_sf, &pucch_cfg, &uci_data.value, sf_symbols)) {
            ERROR("Error encoding PUCCH");
            goto quit;
          }

          if (srsran_refsignal_dmrs_pucch_gen(&dmrs, &ul_sf, &pucch_cfg, pucch_dmrs)) {
            ERROR("Error encoding PUCCH");
            goto quit;
          }
          if (srsran_refsignal_dmrs_pucch_put(&dmrs, &pucch_cfg, pucch_dmrs, sf_symbols)) {
            ERROR("Error encoding PUCCH");
            goto quit;
          }
          gettimeofday(&t[2], NULL);
          get_time_interval(t);
          uint64_t t_enc = t[0].tv_usec + t[0].tv_sec * 1000000UL;

          // Run AWGN channel
          srsran_channel_awgn_run_c(&awgn, sf_symbols, sf_symbols, SRSRAN_NOF_RE(cell));

          // Decode PUCCH signals
          gettimeofday(&t[1], NULL);
          if (srsran_chest_ul_estimate_pucch(&chest, &ul_sf, &pucch_cfg, sf_symbols, &chest_res) < SRSRAN_SUCCESS) {
            ERROR("Error estimating PUCCH channel");
            goto quit;
          }

          srsran_pucch_res_t res = {};
          if (srsran_pucch_decode(&pucch_enb, &ul_sf, &pucch_cfg, &chest_res, sf_symbols, &res) < SRSRAN_SUCCESS) {
            ERROR("Error decoding PUCCH");
            goto quit;
          }
          gettimeofday(&t[2], NULL);
          get_time_interval(t);
          uint64_t t_dec = t[0].tv_usec + t[0].tv_sec * 1000000UL;

          // Check EPRE and RSRP are +/- 1 dB and SNR measurements are +/- 3dB
          if (fabsf(chest_res.epre_dBfs) > 1.0 || fabsf(chest_res.rsrp_dBfs) > 1.0 ||
              fabsf(chest_res.snr_db - snr_db) > 3.0) {
            ERROR("Invalid EPRE (%+.2f), RSRP (%+.2f) or SNR (%+.2f)",
                  chest_res.epre_dBfs,
                  chest_res.rsrp_dBfs,
                  chest_res.snr_db);
            goto quit;
          }

          INFO("format %d, n_pucch: %d, ncs: %d, d: %d, t_encode=%" PRIu64 " us, t_decode=%" PRIu64
               " us, EPRE=%+.1f dBfs, RSRP=%+.1f dBfs, SNR=%+.1f dBfs\n",
               format,
               n_pucch,
               ncs,
               d,
               t_enc,
               t_dec,
               chest_res.epre_dBfs,
               chest_res.rsrp_dBfs,
               chest_res.snr_db);
        }
      }
    }
  }

  ret = 0;
quit:
  srsran_pucch_free(&pucch_ue);
  srsran_pucch_free(&pucch_enb);
  srsran_chest_ul_free(&chest);
  srsran_chest_ul_res_free(&chest_res);
  srsran_channel_awgn_free(&awgn);
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
