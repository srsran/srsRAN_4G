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

#include "srsran/phy/phch/ra_dl_nr.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/phch/sch_nr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <getopt.h>
#include <srsran/phy/utils/random.h>

static srsran_carrier_nr_t carrier = SRSRAN_DEFAULT_CARRIER_NR;

static uint32_t            n_prb     = 0;  // Set to 0 for steering
static uint32_t            mcs       = 30; // Set to 30 for steering
static uint32_t            rv        = 4;  // Set to 30 for steering
static srsran_sch_cfg_nr_t pdsch_cfg = {};

static void usage(char* prog)
{
  printf("Usage: %s [prTL] \n", prog);
  printf("\t-P Number of carrier PRB [Default %d]\n", carrier.nof_prb);
  printf("\t-p Number of grant PRB, set to 0 for steering [Default %d]\n", n_prb);
  printf("\t-r Redundancy version, set to 4 or higher for steering [Default %d]\n", rv);
  printf("\t-m MCS PRB, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-T Provide MCS table (64qam, 256qam, 64qamLowSE) [Default %s]\n",
         srsran_mcs_table_to_str(pdsch_cfg.sch_cfg.mcs_table));
  printf("\t-L Provide number of layers [Default %d]\n", carrier.max_mimo_layers);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "PpmTLvr")) != -1) {
    switch (opt) {
      case 'P':
        carrier.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        n_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        mcs = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        rv = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'T':
        pdsch_cfg.sch_cfg.mcs_table = srsran_mcs_table_from_str(argv[optind]);
        break;
      case 'L':
        carrier.max_mimo_layers = (uint32_t)strtol(argv[optind], NULL, 10);
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
  int             ret       = SRSRAN_ERROR;
  srsran_sch_nr_t sch_nr_tx = {};
  srsran_sch_nr_t sch_nr_rx = {};
  srsran_random_t rand_gen  = srsran_random_init(1234);

  uint8_t* data_tx = srsran_vec_u8_malloc(1024 * 1024);
  uint8_t* encoded = srsran_vec_u8_malloc(1024 * 1024 * 8);
  int8_t*  llr     = srsran_vec_i8_malloc(1024 * 1024 * 8);
  uint8_t* data_rx = srsran_vec_u8_malloc(1024 * 1024);

  // Set default PDSCH configuration
  pdsch_cfg.sch_cfg.mcs_table = srsran_mcs_table_64qam;

  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    goto clean_exit;
  }

  if (data_tx == NULL || data_rx == NULL) {
    goto clean_exit;
  }

  srsran_sch_nr_args_t args   = {};
  args.disable_simd           = false;
  args.decoder_use_flooded    = false;
  args.decoder_scaling_factor = 0.8;
  args.max_nof_iter           = 20;
  if (srsran_sch_nr_init_tx(&sch_nr_tx, &args) < SRSRAN_SUCCESS) {
    ERROR("Error initiating SCH NR for Tx");
    goto clean_exit;
  }

  if (srsran_sch_nr_init_rx(&sch_nr_rx, &args) < SRSRAN_SUCCESS) {
    ERROR("Error initiating SCH NR for Rx");
    goto clean_exit;
  }

  if (srsran_sch_nr_set_carrier(&sch_nr_tx, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
  }

  if (srsran_sch_nr_set_carrier(&sch_nr_rx, &carrier)) {
    ERROR("Error setting SCH NR carrier");
    goto clean_exit;
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
  pdsch_cfg.grant.S          = 1;
  pdsch_cfg.grant.L          = 13;
  pdsch_cfg.grant.k          = 0;
  pdsch_cfg.grant.nof_layers = carrier.max_mimo_layers;
  pdsch_cfg.grant.dci_format = srsran_dci_format_nr_1_0;

  uint32_t n_prb_start = 1;
  uint32_t n_prb_end   = carrier.nof_prb + 1;
  if (n_prb > 0) {
    n_prb_start = SRSRAN_MIN(n_prb, n_prb_end - 1);
    n_prb_end   = SRSRAN_MIN(n_prb + 1, n_prb_end);
  }

  uint32_t rv_start = 0;
  uint32_t rv_end   = 4;
  if (rv < 4) {
    rv_start = rv;
    rv_end   = rv + 1;
  }

  uint32_t mcs_start = 0;
  uint32_t mcs_end   = pdsch_cfg.sch_cfg.mcs_table == srsran_mcs_table_256qam ? 28 : 29;
  if (mcs < mcs_end) {
    mcs_start = SRSRAN_MIN(mcs, mcs_end - 1);
    mcs_end   = SRSRAN_MIN(mcs + 1, mcs_end);
  }

  for (n_prb = n_prb_start; n_prb < n_prb_end; n_prb++) {
    for (mcs = mcs_start; mcs < mcs_end; mcs++) {
      for (rv = rv_start; rv < rv_end; rv++) {
        for (uint32_t n = 0; n < SRSRAN_MAX_PRB_NR; n++) {
          pdsch_cfg.grant.prb_idx[n] = (n < n_prb);
        }
        pdsch_cfg.grant.nof_dmrs_cdm_groups_without_data = 1; // No need for MIMO

        srsran_sch_tb_t tb = {};
        tb.rv              = rv;
        if (srsran_ra_nr_fill_tb(&pdsch_cfg, &pdsch_cfg.grant, mcs, &tb) < SRSRAN_SUCCESS) {
          ERROR("Error filing tb");
          goto clean_exit;
        }

        for (uint32_t i = 0; i < tb.tbs; i++) {
          data_tx[i] = (uint8_t)srsran_random_uniform_int_dist(rand_gen, 0, UINT8_MAX);
        }

        tb.softbuffer.tx = &softbuffer_tx;

        if (srsran_dlsch_nr_encode(&sch_nr_tx, &pdsch_cfg.sch_cfg, &tb, data_tx, encoded) < SRSRAN_SUCCESS) {
          ERROR("Error encoding");
          goto clean_exit;
        }

        for (uint32_t i = 0; i < tb.nof_bits; i++) {
          llr[i] = encoded[i] ? -10 : +10;
        }

        tb.softbuffer.rx = &softbuffer_rx;
        srsran_softbuffer_rx_reset(tb.softbuffer.rx);

        srsran_sch_tb_res_nr_t res = {};
        res.payload                = data_rx;
        if (srsran_dlsch_nr_decode(&sch_nr_rx, &pdsch_cfg.sch_cfg, &tb, llr, &res) < SRSRAN_SUCCESS) {
          ERROR("Error encoding");
          goto clean_exit;
        }

        if (rv == 0) {
          if (!res.crc) {
            ERROR("Failed to match CRC; n_prb=%d; mcs=%d; TBS=%d;", n_prb, mcs, tb.tbs);
            goto clean_exit;
          }

          if (memcmp(data_tx, data_rx, tb.tbs / 8) != 0) {
            ERROR("Failed to match Tx/Rx data; n_prb=%d; mcs=%d; TBS=%d;", n_prb, mcs, tb.tbs);
            printf("Tx data: ");
            srsran_vec_fprint_byte(stdout, data_tx, tb.tbs / 8);
            printf("Rx data: ");
            srsran_vec_fprint_byte(stdout, data_rx, tb.tbs / 8);
            goto clean_exit;
          }
        }

        INFO("n_prb=%d; mcs=%d; rv=%d TBS=%d; PASSED!\n", n_prb, mcs, rv, tb.tbs);
      }
    }
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  srsran_random_free(rand_gen);
  srsran_sch_nr_free(&sch_nr_tx);
  srsran_sch_nr_free(&sch_nr_rx);
  if (data_tx) {
    free(data_tx);
  }
  if (data_rx) {
    free(data_rx);
  }
  if (llr) {
    free(llr);
  }
  if (encoded) {
    free(encoded);
  }
  srsran_softbuffer_tx_free(&softbuffer_tx);
  srsran_softbuffer_rx_free(&softbuffer_rx);

  return ret;
}
