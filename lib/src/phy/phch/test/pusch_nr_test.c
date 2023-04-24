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

#include "srsran/phy/phch/pusch_nr.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/phch/ra_ul_nr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/phy/utils/vector.h"
#include <complex.h>
#include <getopt.h>

static srsran_carrier_nr_t carrier      = SRSRAN_DEFAULT_CARRIER_NR;
static uint32_t            n_prb        = 0;  // Set to 0 for steering
static uint32_t            mcs          = 30; // Set to 30 for steering
static srsran_sch_cfg_nr_t pusch_cfg    = {};
static uint16_t            rnti         = 0x1234;
static uint32_t            nof_ack_bits = 0;
static uint32_t            nof_csi_bits = 0;

void usage(char* prog)
{
  printf("Usage: %s [pTL] \n", prog);
  printf("\t-p Number of grant PRB, set to 0 for steering [Default %d]\n", n_prb);
  printf("\t-m MCS PRB, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-T Provide MCS table (64qam, 256qam, 64qamLowSE) [Default %s]\n",
         srsran_mcs_table_to_str(pusch_cfg.sch_cfg.mcs_table));
  printf("\t-L Provide number of layers [Default %d]\n", carrier.max_mimo_layers);
  printf("\t-A Provide a number of HARQ-ACK bits [Default %d]\n", nof_ack_bits);
  printf("\t-C Provide a number of CSI bits [Default %d]\n", nof_csi_bits);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "pmTLACv")) != -1) {
    switch (opt) {
      case 'p':
        n_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        mcs = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'T':
        pusch_cfg.sch_cfg.mcs_table = srsran_mcs_table_from_str(argv[optind]);
        break;
      case 'L':
        carrier.max_mimo_layers = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'A':
        nof_ack_bits = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'C':
        nof_csi_bits = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int                   ret      = SRSRAN_ERROR;
  srsran_pusch_nr_t     pusch_tx = {};
  srsran_pusch_nr_t     pusch_rx = {};
  srsran_chest_dl_res_t chest    = {};
  srsran_random_t       rand_gen = srsran_random_init(1234);

  srsran_pusch_data_nr_t data_tx                          = {};
  srsran_pusch_res_nr_t  data_rx                          = {};
  cf_t*                  sf_symbols[SRSRAN_MAX_LAYERS_NR] = {};

  // Set default PUSCH configuration
  pusch_cfg.sch_cfg.mcs_table = srsran_mcs_table_64qam;

  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    goto clean_exit;
  }

  srsran_pusch_nr_args_t pusch_args = {};
  pusch_args.sch.disable_simd       = false;
  pusch_args.measure_evm            = true;

  if (srsran_pusch_nr_init_ue(&pusch_tx, &pusch_args) < SRSRAN_SUCCESS) {
    ERROR("Error initiating PUSCH for Tx");
    goto clean_exit;
  }

  if (srsran_pusch_nr_init_gnb(&pusch_rx, &pusch_args) < SRSRAN_SUCCESS) {
    ERROR("Error initiating SCH NR for Rx");
    goto clean_exit;
  }

  if (srsran_pusch_nr_set_carrier(&pusch_tx, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  if (srsran_pusch_nr_set_carrier(&pusch_rx, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  for (uint32_t i = 0; i < carrier.max_mimo_layers; i++) {
    sf_symbols[i] = srsran_vec_cf_malloc(SRSRAN_SLOT_LEN_RE_NR(carrier.nof_prb));
    if (sf_symbols[i] == NULL) {
      ERROR("Error malloc");
      goto clean_exit;
    }
  }

  for (uint32_t i = 0; i < pusch_tx.max_cw; i++) {
    data_tx.payload[i]    = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
    data_rx.tb[i].payload = srsran_vec_u8_malloc(SRSRAN_SLOT_MAX_NOF_BITS_NR);
    if (data_tx.payload[i] == NULL || data_rx.tb[i].payload == NULL) {
      ERROR("Error malloc");
      goto clean_exit;
    }
  }

  srsran_softbuffer_tx_t softbuffer_tx = {};
  srsran_softbuffer_rx_t softbuffer_rx = {};

  if (srsran_softbuffer_tx_init_guru(&softbuffer_tx, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
      SRSRAN_SUCCESS) {
    ERROR("Error init soft-buffer");
    goto clean_exit;
  }

  if (srsran_softbuffer_rx_init_guru(&softbuffer_rx, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
      SRSRAN_SUCCESS) {
    ERROR("Error init soft-buffer");
    goto clean_exit;
  }

  // Use grant default A time resources with m=0
  if (srsran_ra_ul_nr_pusch_time_resource_default_A(carrier.scs, 0, &pusch_cfg.grant) < SRSRAN_SUCCESS) {
    ERROR("Error loading default grant");
    goto clean_exit;
  }

  // Set PUSCH grant without considering any procedure
  pusch_cfg.grant.nof_dmrs_cdm_groups_without_data = 1; // No need for MIMO
  pusch_cfg.grant.nof_layers                       = carrier.max_mimo_layers;
  pusch_cfg.grant.dci_format                       = srsran_dci_format_nr_1_0;
  pusch_cfg.grant.rnti                             = rnti;

  uint32_t n_prb_start = 1;
  uint32_t n_prb_end   = carrier.nof_prb + 1;
  if (n_prb > 0) {
    n_prb_start = SRSRAN_MIN(n_prb, n_prb_end - 1);
    n_prb_end   = SRSRAN_MIN(n_prb + 1, n_prb_end);
  }

  uint32_t mcs_start = 0;
  uint32_t mcs_end   = pusch_cfg.sch_cfg.mcs_table == srsran_mcs_table_256qam ? 28 : 29;
  if (mcs < mcs_end) {
    mcs_start = SRSRAN_MIN(mcs, mcs_end - 1);
    mcs_end   = SRSRAN_MIN(mcs + 1, mcs_end);
  }

  srsran_sch_hl_cfg_nr_t sch_hl_cfg = {};
  sch_hl_cfg.scaling                = 1.0f;
  sch_hl_cfg.beta_offsets.fix_ack   = 12.625f;
  sch_hl_cfg.beta_offsets.fix_csi1  = 2.25f;
  sch_hl_cfg.beta_offsets.fix_csi2  = 2.25f;

  if (srsran_chest_dl_res_init(&chest, carrier.nof_prb) < SRSRAN_SUCCESS) {
    ERROR("Initiating chest");
    goto clean_exit;
  }

  for (n_prb = n_prb_start; n_prb < n_prb_end; n_prb++) {
    for (mcs = mcs_start; mcs < mcs_end; mcs++) {
      for (uint32_t n = 0; n < SRSRAN_MAX_PRB_NR; n++) {
        pusch_cfg.grant.prb_idx[n] = (n < n_prb);
      }
      pusch_cfg.grant.nof_prb = n_prb;

      pusch_cfg.grant.dci_format = srsran_dci_format_nr_0_0;
      if (srsran_ra_nr_fill_tb(&pusch_cfg, &pusch_cfg.grant, mcs, &pusch_cfg.grant.tb[0]) < SRSRAN_SUCCESS) {
        ERROR("Error filling tb");
        goto clean_exit;
      }

      // Generate SCH payload
      for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
        // Skip TB if no allocated
        if (data_tx.payload[tb] == NULL) {
          continue;
        }

        for (uint32_t i = 0; i < pusch_cfg.grant.tb[tb].tbs; i++) {
          data_tx.payload[tb][i] = (uint8_t)srsran_random_uniform_int_dist(rand_gen, 0, UINT8_MAX);
        }
        pusch_cfg.grant.tb[tb].softbuffer.tx = &softbuffer_tx;
      }

      // Generate HARQ ACK bits
      if (nof_ack_bits > 0) {
        pusch_cfg.uci.ack.count = nof_ack_bits;
        for (uint32_t i = 0; i < nof_ack_bits; i++) {
          data_tx.uci.ack[i] = (uint8_t)srsran_random_uniform_int_dist(rand_gen, 0, 1);
        }
      }

      // Generate CSI report bits
      uint8_t csi_report_tx[SRSRAN_UCI_NR_MAX_CSI1_BITS] = {};
      uint8_t csi_report_rx[SRSRAN_UCI_NR_MAX_CSI1_BITS] = {};
      if (nof_csi_bits > 0) {
        pusch_cfg.uci.csi[0].cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_NONE;
        pusch_cfg.uci.csi[0].K_csi_rs     = nof_csi_bits;
        pusch_cfg.uci.nof_csi             = 1;
        data_tx.uci.csi[0].none           = csi_report_tx;
        for (uint32_t i = 0; i < nof_csi_bits; i++) {
          csi_report_tx[i] = (uint8_t)srsran_random_uniform_int_dist(rand_gen, 0, 1);
        }

        data_rx.uci.csi[0].none = csi_report_rx;
      }

      if (srsran_ra_ul_set_grant_uci_nr(&carrier, &sch_hl_cfg, &pusch_cfg.uci, &pusch_cfg) < SRSRAN_SUCCESS) {
        ERROR("Setting UCI");
        goto clean_exit;
      }

      if (srsran_pusch_nr_encode(&pusch_tx, &pusch_cfg, &pusch_cfg.grant, &data_tx, sf_symbols) < SRSRAN_SUCCESS) {
        ERROR("Error encoding");
        goto clean_exit;
      }

      for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
        pusch_cfg.grant.tb[tb].softbuffer.rx = &softbuffer_rx;
        srsran_softbuffer_rx_reset(pusch_cfg.grant.tb[tb].softbuffer.rx);
      }

      for (uint32_t i = 0; i < pusch_cfg.grant.tb->nof_re; i++) {
        chest.ce[0][0][i] = 1.0f;
      }
      chest.nof_re = pusch_cfg.grant.tb->nof_re;

      if (srsran_pusch_nr_decode(&pusch_rx, &pusch_cfg, &pusch_cfg.grant, &chest, sf_symbols, &data_rx) <
          SRSRAN_SUCCESS) {
        ERROR("Error encoding");
        goto clean_exit;
      }

      if (data_rx.evm[0] > 0.001f) {
        ERROR("Error PUSCH EVM is too high %f", data_rx.evm[0]);
        goto clean_exit;
      }

      // Check symbols Mean Square Error (MSE)
      uint32_t nof_re = srsran_ra_dl_nr_slot_nof_re(&pusch_cfg, &pusch_cfg.grant);
      if (nof_re * pusch_cfg.grant.nof_layers > 0) {
        float mse     = 0.0f;
        float mse_tmp = 0.0f;
        for (uint32_t i = 0; i < pusch_cfg.grant.nof_layers; i++) {
          for (uint32_t j = 0; j < nof_re; j++) {
            mse_tmp = cabsf(pusch_tx.d[i][j] - pusch_rx.d[i][j]);
            mse += mse_tmp * mse_tmp;
          }
        }
        mse = mse / (nof_re * pusch_cfg.grant.nof_layers);
        if (mse > 0.001) {
          ERROR("MSE error (%f) is too high", mse);
          for (uint32_t i = 0; i < pusch_cfg.grant.nof_layers; i++) {
            printf("d_tx[%d]=", i);
            srsran_vec_fprint_c(stdout, pusch_tx.d[i], nof_re);
            printf("d_rx[%d]=", i);
            srsran_vec_fprint_c(stdout, pusch_rx.d[i], nof_re);
          }
          goto clean_exit;
        }
      }

      // Check Received SCH LLR match
      if (pusch_rx.G_ulsch > 0) {
        for (uint32_t i = 0; i < pusch_rx.G_ulsch; i++) {
          uint8_t rx_bit = (((int8_t*)pusch_rx.g_ulsch)[i]) < 0 ? 1 : 0;
          if (rx_bit == 0) {
            pusch_rx.g_ulsch[i] = pusch_tx.g_ulsch[i];
          } else {
            pusch_rx.g_ulsch[i] = rx_bit;
          }
        }
        if (memcmp(pusch_tx.g_ulsch, pusch_rx.g_ulsch, pusch_tx.G_ulsch) != 0) {
          printf("g_ulsch_tx=");
          srsran_vec_fprint_byte(stdout, pusch_tx.g_ulsch, pusch_tx.G_ulsch);
          printf("g_ulsch_rx=");
          srsran_vec_fprint_byte(stdout, pusch_rx.g_ulsch, pusch_tx.G_ulsch);
          //            srsran_vec_fprint_bs(stdout, (int8_t*)pusch_rx.g_ulsch, pusch_rx.G_ulsch);

          goto clean_exit;
        }
      }

      // Validate UL-SCH CRC check
      if (!data_rx.tb[0].crc) {
        ERROR("Failed to match CRC; n_prb=%d; mcs=%d; TBS=%d;", n_prb, mcs, pusch_cfg.grant.tb[0].tbs);
        goto clean_exit;
      }

      // Validate UL-SCH payload
      if (memcmp(data_tx.payload[0], data_rx.tb[0].payload, pusch_cfg.grant.tb[0].tbs / 8) != 0) {
        ERROR("Failed to match Tx/Rx data; n_prb=%d; mcs=%d; TBS=%d;", n_prb, mcs, pusch_cfg.grant.tb[0].tbs);
        printf("Tx data: ");
        srsran_vec_fprint_byte(stdout, data_tx.payload[0], pusch_cfg.grant.tb[0].tbs / 8);
        printf("Rx data: ");
        srsran_vec_fprint_byte(stdout, data_tx.payload[0], pusch_cfg.grant.tb[0].tbs / 8);
        goto clean_exit;
      }

      // Validate UCI is decoded successfully
      if (nof_ack_bits > 0 || nof_csi_bits > 0) {
        if (!data_rx.uci.valid) {
          ERROR("UCI data was not decoded ok");
          goto clean_exit;
        }
      }

      // Validate HARQ-ACK is decoded successfully
      if (nof_ack_bits > 0) {
        if (memcmp(data_tx.uci.ack, data_rx.uci.ack, nof_ack_bits) != 0) {
          ERROR("UCI HARQ-ACK bits are unmatched");
          printf("Tx data: ");
          srsran_vec_fprint_byte(stdout, data_tx.uci.ack, nof_ack_bits);
          printf("Rx data: ");
          srsran_vec_fprint_byte(stdout, data_rx.uci.ack, nof_ack_bits);
          goto clean_exit;
        }
      }

      // Validate CSI is decoded successfully
      if (nof_csi_bits > 0) {
        if (memcmp(data_tx.uci.csi[0].none, data_rx.uci.csi[0].none, nof_csi_bits) != 0) {
          ERROR("UCI CSI bits are unmatched");
          printf("Tx data: ");
          srsran_vec_fprint_byte(stdout, data_tx.uci.csi[0].none, nof_csi_bits);
          printf("Rx data: ");
          srsran_vec_fprint_byte(stdout, data_rx.uci.csi[0].none, nof_csi_bits);
          goto clean_exit;
        }
      }

      if (get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO) {
        char str[512];
        srsran_pusch_nr_rx_info(&pusch_rx, &pusch_cfg, &pusch_cfg.grant, &data_rx, str, (uint32_t)sizeof(str));

        char str_extra[2048];
        srsran_sch_cfg_nr_info(&pusch_cfg, str_extra, (uint32_t)sizeof(str_extra));
        INFO("PUSCH: %s\n%s", str, str_extra);
      }

      printf("n_prb=%d; mcs=%d; TBS=%d; EVM=%f; PASSED!\n", n_prb, mcs, pusch_cfg.grant.tb[0].tbs, data_rx.evm[0]);
    }
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  srsran_chest_dl_res_free(&chest);
  srsran_random_free(rand_gen);
  srsran_pusch_nr_free(&pusch_tx);
  srsran_pusch_nr_free(&pusch_rx);
  for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    if (data_tx.payload[i]) {
      free(data_tx.payload[i]);
    }
    if (data_rx.tb[i].payload) {
      free(data_rx.tb[i].payload);
    }
  }
  for (uint32_t i = 0; i < SRSRAN_MAX_LAYERS_NR; i++) {
    if (sf_symbols[i]) {
      free(sf_symbols[i]);
    }
  }
  srsran_softbuffer_tx_free(&softbuffer_tx);
  srsran_softbuffer_rx_free(&softbuffer_rx);

  return ret;
}
