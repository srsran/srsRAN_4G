/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srslte/phy/phch/ra_dl_nr.h"
#include "srslte/phy/phch/ra_nr.h"
#include "srslte/phy/phch/sch_nr.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include <getopt.h>
#include <srslte/phy/utils/random.h>

static srslte_carrier_nr_t carrier = {
    0,                 // cell_id
    0,                 // numerology
    SRSLTE_MAX_PRB_NR, // nof_prb
    0,                 // start
    1                  // max_mimo_layers
};

static uint32_t              n_prb       = 0;  // Set to 0 for steering
static uint32_t              mcs         = 30; // Set to 30 for steering
static srslte_sch_cfg_nr_t   pdsch_cfg   = {};
static srslte_sch_grant_nr_t pdsch_grant = {};

void usage(char* prog)
{
  printf("Usage: %s [pTL] \n", prog);
  printf("\t-p Number of grant PRB, set to 0 for steering [Default %d]\n", n_prb);
  printf("\t-m MCS PRB, set to >28 for steering [Default %d]\n", mcs);
  printf("\t-T Provide MCS table (64qam, 256qam, 64qamLowSE) [Default %s]\n",
         srslte_mcs_table_to_str(pdsch_cfg.sch_cfg.mcs_table));
  printf("\t-L Provide number of layers [Default %d]\n", carrier.max_mimo_layers);
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
        carrier.max_mimo_layers = (uint32_t)strtol(argv[optind], NULL, 10);
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
  int             ret       = SRSLTE_ERROR;
  srslte_sch_nr_t sch_nr_tx = {};
  srslte_sch_nr_t sch_nr_rx = {};
  srslte_random_t rand_gen  = srslte_random_init(1234);

  uint8_t* data_tx = srslte_vec_u8_malloc(1024 * 1024);
  uint8_t* encoded = srslte_vec_u8_malloc(1024 * 1024 * 8);
  int8_t*  llr     = srslte_vec_i8_malloc(1024 * 1024 * 8);
  uint8_t* data_rx = srslte_vec_u8_malloc(1024 * 1024);

  // Set default PDSCH configuration
  pdsch_cfg.sch_cfg.mcs_table = srslte_mcs_table_64qam;

  if (parse_args(argc, argv) < SRSLTE_SUCCESS) {
    goto clean_exit;
  }

  if (data_tx == NULL || data_rx == NULL) {
    goto clean_exit;
  }

  srslte_sch_nr_args_t args = {};
  args.disable_simd         = false;
  if (srslte_sch_nr_init_tx(&sch_nr_tx, &args) < SRSLTE_SUCCESS) {
    ERROR("Error initiating SCH NR for Tx\n");
    goto clean_exit;
  }

  if (srslte_sch_nr_init_rx(&sch_nr_rx, &args) < SRSLTE_SUCCESS) {
    ERROR("Error initiating SCH NR for Rx\n");
    goto clean_exit;
  }

  if (srslte_sch_nr_set_carrier(&sch_nr_tx, &carrier)) {
    ERROR("Error setting SCH NR carrier\n");
    goto clean_exit;
  }

  if (srslte_sch_nr_set_carrier(&sch_nr_rx, &carrier)) {
    ERROR("Error setting SCH NR carrier\n");
    goto clean_exit;
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
  if (srslte_ra_dl_nr_time_default_A(0, pdsch_cfg.dmrs_typeA.typeA_pos, &pdsch_grant) < SRSLTE_SUCCESS) {
    ERROR("Error loading default grant\n");
    goto clean_exit;
  }
  pdsch_grant.nof_layers = carrier.max_mimo_layers;
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

      srslte_sch_tb_t tb = {};
      if (srslte_ra_nr_fill_tb(&pdsch_cfg, &pdsch_grant, mcs, &tb) < SRSLTE_SUCCESS) {
        ERROR("Error filing tb\n");
        goto clean_exit;
      }

      for (uint32_t i = 0; i < tb.tbs; i++) {
        data_tx[i] = (uint8_t)srslte_random_uniform_int_dist(rand_gen, 0, UINT8_MAX);
      }

      tb.softbuffer.tx = &softbuffer_tx;

      if (srslte_dlsch_nr_encode(&sch_nr_tx, &pdsch_cfg.sch_cfg, &tb, data_tx, encoded) < SRSLTE_SUCCESS) {
        ERROR("Error encoding\n");
        goto clean_exit;
      }

      for (uint32_t i = 0; i < tb.nof_bits; i++) {
        llr[i] = encoded[i] ? -10 : +10;
      }

      tb.softbuffer.rx = &softbuffer_rx;
      srslte_softbuffer_rx_reset(tb.softbuffer.rx);

      bool crc = false;
      if (srslte_dlsch_nr_decode(&sch_nr_rx, &pdsch_cfg.sch_cfg, &tb, llr, data_rx, &crc) < SRSLTE_SUCCESS) {
        ERROR("Error encoding\n");
        goto clean_exit;
      }

      if (!crc) {
        ERROR("Failed to match CRC; n_prb=%d; mcs=%d; TBS=%d;\n", n_prb, mcs, tb.tbs);
        goto clean_exit;
      }

      if (memcmp(data_tx, data_rx, tb.tbs / 8) != 0) {
        ERROR("Failed to match Tx/Rx data; n_prb=%d; mcs=%d; TBS=%d;\n", n_prb, mcs, tb.tbs);
        printf("Tx data: ");
        srslte_vec_fprint_byte(stdout, data_tx, tb.tbs / 8);
        printf("Rx data: ");
        srslte_vec_fprint_byte(stdout, data_rx, tb.tbs / 8);
        goto clean_exit;
      }

      printf("n_prb=%d; mcs=%d; TBS=%d; PASSED!\n", n_prb, mcs, tb.tbs);
    }
  }

  ret = SRSLTE_SUCCESS;

clean_exit:
  srslte_random_free(rand_gen);
  srslte_sch_nr_free(&sch_nr_tx);
  srslte_sch_nr_free(&sch_nr_rx);
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
  srslte_softbuffer_tx_free(&softbuffer_tx);
  srslte_softbuffer_rx_free(&softbuffer_rx);

  return SRSLTE_SUCCESS;
}
