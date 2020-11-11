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

#include "srslte/phy/enb/enb_dl_nr.h"
#include "srslte/phy/phch/ra_nr.h"
#include "srslte/phy/ue/ue_dl_nr.h"
#include "srslte/phy/ue/ue_dl_nr_data.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/random.h"
#include "srslte/phy/utils/vector.h"
#include <getopt.h>

static srslte_carrier_nr_t carrier = {
    0,   // cell_id
    0,   // numerology
    100, // nof_prb
    0    // start
};

static uint32_t                n_prb       = 0;  // Set to 0 for steering
static uint32_t                mcs         = 30; // Set to 30 for steering
static srslte_pdsch_cfg_nr_t   pdsch_cfg   = {};
static srslte_pdsch_grant_nr_t pdsch_grant = {};

void usage(char* prog)
{
  printf("Usage: %s [pTL] \n", prog);
  printf("\t-p Number of grant PRB, set to 0 for steering [Default %d]\n", n_prb);
  printf("\t-m MCS PRB, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-T Provide MCS table (64qam, 256qam, 64qamLowSE) [Default %s]\n",
         srslte_mcs_table_to_str(pdsch_cfg.sch_cfg.mcs_table));
  printf("\t-L Provide number of layers [Default %d]\n", pdsch_cfg.sch_cfg.max_mimo_layers);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "pmTLv")) != -1) {
    switch (opt) {
      case 'p':
        n_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        mcs = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'T':
        pdsch_cfg.sch_cfg.mcs_table = srslte_mcs_table_from_str(argv[optind]);
        break;
      case 'L':
        pdsch_cfg.sch_cfg.max_mimo_layers = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  int                   ret                      = SRSLTE_ERROR;
  srslte_enb_dl_nr_t    enb_dl                   = {};
  srslte_ue_dl_nr_t     ue_dl                    = {};
  srslte_pdsch_res_nr_t pdsch_res[SRSLTE_MAX_TB] = {};
  srslte_random_t       rand_gen                 = srslte_random_init(1234);
  srslte_dl_slot_cfg_t  slot                     = {};

  uint8_t* data_tx[SRSLTE_MAX_TB]        = {};
  uint8_t* data_rx[SRSLTE_MAX_CODEWORDS] = {};
  cf_t*    buffer                        = NULL;

  buffer = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB(carrier.nof_prb));
  if (buffer == NULL) {
    ERROR("Error malloc\n");
    goto clean_exit;
  }

  srslte_ue_dl_nr_args_t ue_dl_args = {};
  ue_dl_args.nof_rx_antennas        = 1;
  ue_dl_args.pdsch.sch.disable_simd = true;
  ue_dl_args.pdsch.measure_evm      = true;

  srslte_enb_dl_nr_args_t enb_dl_args = {};
  enb_dl_args.nof_tx_antennas         = 1;
  enb_dl_args.pdsch.sch.disable_simd  = true;

  // Set default PDSCH configuration
  pdsch_cfg.sch_cfg.mcs_table       = srslte_mcs_table_64qam;
  pdsch_cfg.sch_cfg.max_mimo_layers = 1;
  if (parse_args(argc, argv) < SRSLTE_SUCCESS) {
    goto clean_exit;
  }

  srslte_pdsch_args_t pdsch_args = {};
  pdsch_args.sch.disable_simd    = true;
  pdsch_args.measure_evm         = true;

  if (srslte_ue_dl_nr_init(&ue_dl, &buffer, &ue_dl_args)) {
    ERROR("Error UE DL\n");
    goto clean_exit;
  }

  if (srslte_enb_dl_nr_init(&enb_dl, &buffer, &enb_dl_args)) {
    ERROR("Error UE DL\n");
    goto clean_exit;
  }

  if (srslte_ue_dl_nr_set_carrier(&ue_dl, &carrier, &pdsch_cfg.sch_cfg)) {
    ERROR("Error setting SCH NR carrier\n");
    goto clean_exit;
  }

  if (srslte_enb_dl_nr_set_carrier(&enb_dl, &carrier, &pdsch_cfg.sch_cfg)) {
    ERROR("Error setting SCH NR carrier\n");
    goto clean_exit;
  }

  for (uint32_t i = 0; i < 1; i++) {
    data_tx[i] = srslte_vec_u8_malloc(SRSLTE_SLOT_MAX_NOF_BITS_NR);
    data_rx[i] = srslte_vec_u8_malloc(SRSLTE_SLOT_MAX_NOF_BITS_NR);
    if (data_tx[i] == NULL || data_rx[i] == NULL) {
      ERROR("Error malloc\n");
      goto clean_exit;
    }

    pdsch_res[i].payload = data_rx[i];
  }

  srslte_softbuffer_tx_t softbuffer_tx = {};
  srslte_softbuffer_rx_t softbuffer_rx = {};

  if (srslte_softbuffer_tx_init_guru(&softbuffer_tx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer\n");
    goto clean_exit;
  }

  if (srslte_softbuffer_rx_init_guru(&softbuffer_rx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer\n");
    goto clean_exit;
  }

  // Use grant default A time resources with m=0
  if (srslte_ue_dl_nr_pdsch_time_resource_default_A(0, pdsch_cfg.dmrs_cfg_typeA.typeA_pos, &pdsch_grant) <
      SRSLTE_SUCCESS) {
    ERROR("Error loading default grant\n");
    goto clean_exit;
  }
  pdsch_grant.nof_layers = pdsch_cfg.sch_cfg.max_mimo_layers;
  pdsch_grant.dci_format = srslte_dci_format_nr_1_0;

  uint32_t n_prb_start = 1;
  uint32_t n_prb_end   = carrier.nof_prb + 1;
  if (n_prb > 0) {
    n_prb_start = SRSLTE_MIN(n_prb, n_prb_end - 1);
    n_prb_end   = SRSLTE_MIN(n_prb + 1, n_prb_end);
  }

  uint32_t mcs_start = 0;
  uint32_t mcs_end   = pdsch_cfg.sch_cfg.mcs_table == srslte_mcs_table_256qam ? 28 : 29;
  if (mcs < mcs_end) {
    mcs_start = SRSLTE_MIN(mcs, mcs_end - 1);
    mcs_end   = SRSLTE_MIN(mcs + 1, mcs_end);
  }

  for (n_prb = n_prb_start; n_prb < n_prb_end; n_prb++) {
    for (mcs = mcs_start; mcs < mcs_end; mcs++) {

      for (uint32_t n = 0; n < SRSLTE_MAX_PRB_NR; n++) {
        pdsch_grant.prb_idx[n] = (n < n_prb);
      }

      if (srslte_ra_nr_fill_tb(&pdsch_cfg, &pdsch_grant, mcs, &pdsch_grant.tb[0]) < SRSLTE_SUCCESS) {
        ERROR("Error filing tb\n");
        goto clean_exit;
      }

      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        // Skip TB if no allocated
        if (data_tx[tb] == NULL) {
          continue;
        }

        for (uint32_t i = 0; i < pdsch_grant.tb[tb].tbs; i++) {
          data_tx[tb][i] = (uint8_t)srslte_random_uniform_int_dist(rand_gen, 0, UINT8_MAX);
        }
        pdsch_grant.tb[tb].softbuffer.tx = &softbuffer_tx;
      }

      if (srslte_enb_dl_nr_pdsch_put(&enb_dl, &slot, &pdsch_cfg, &pdsch_grant, data_tx) < SRSLTE_SUCCESS) {
        ERROR("Error encoding\n");
        goto clean_exit;
      }

      srslte_enb_dl_nr_gen_signal(&enb_dl);

      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        pdsch_grant.tb[tb].softbuffer.rx = &softbuffer_rx;
        srslte_softbuffer_rx_reset(pdsch_grant.tb[tb].softbuffer.rx);
      }

      srslte_ue_dl_nr_estimate_fft(&ue_dl);

      if (srslte_ue_dl_nr_pdsch_get(&ue_dl, &slot, &pdsch_cfg, &pdsch_grant, pdsch_res) < SRSLTE_SUCCESS) {
        ERROR("Error decoding\n");
        goto clean_exit;
      }

      if (pdsch_res->evm > 0.001f) {
        ERROR("Error PDSCH EVM is too high %f\n", pdsch_res->evm);
        goto clean_exit;
      }

      if (!pdsch_res[0].crc) {
        ERROR("Failed to match CRC; n_prb=%d; mcs=%d; TBS=%d;\n", n_prb, mcs, pdsch_grant.tb[0].tbs);
        goto clean_exit;
      }

      if (memcmp(data_tx[0], data_rx[0], pdsch_grant.tb[0].tbs / 8) != 0) {
        ERROR("Failed to match Tx/Rx data; n_prb=%d; mcs=%d; TBS=%d;\n", n_prb, mcs, pdsch_grant.tb[0].tbs);
        printf("Tx data: ");
        srslte_vec_fprint_byte(stdout, data_tx[0], pdsch_grant.tb[0].tbs / 8);
        printf("Rx data: ");
        srslte_vec_fprint_byte(stdout, data_rx[0], pdsch_grant.tb[0].tbs / 8);
        goto clean_exit;
      }

      printf("n_prb=%d; mcs=%d; TBS=%d; EVM=%f; PASSED!\n", n_prb, mcs, pdsch_grant.tb[0].tbs, pdsch_res[0].evm);
    }
  }

  ret = SRSLTE_SUCCESS;

clean_exit:
  srslte_random_free(rand_gen);
  srslte_enb_dl_nr_free(&enb_dl);
  srslte_ue_dl_nr_free(&ue_dl);
  for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (data_tx[i]) {
      free(data_tx[i]);
    }
    if (data_rx[i]) {
      free(data_rx[i]);
    }
  }
  if (buffer) {
    free(buffer);
  }
  srslte_softbuffer_tx_free(&softbuffer_tx);
  srslte_softbuffer_rx_free(&softbuffer_rx);

  return ret;
}
