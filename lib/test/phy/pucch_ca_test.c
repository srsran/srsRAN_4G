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

#include <srslte/common/test_common.h>
#include <srslte/phy/utils/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>

#include "srslte/srslte.h"

static int test_pucch_ca(srslte_ack_nack_feedback_mode_t ack_nack_feedback_mode,
                         uint32_t                        nof_prb,
                         const uint32_t*                 nof_tb,
                         uint16_t                        nof_carriers)
{
  srslte_pucch_cfg_t pucch_cfg = {};
  uint16_t           rnti      = 0x1234;
  srslte_cell_t      cell      = {
      nof_prb,            // nof_prb
      1,                  // nof_ports
      1,                  // cell_id
      SRSLTE_CP_NORM,     // cyclic prefix
      SRSLTE_PHICH_NORM,  // PHICH length
      SRSLTE_PHICH_R_1_6, // PHICH resources
      SRSLTE_FDD,
  };

  cf_t*                             buffer         = NULL;
  srslte_refsignal_dmrs_pusch_cfg_t dmrs_pusch_cfg = {}; // Use default
  srslte_ue_ul_t                    ue_ul          = {};
  srslte_ue_ul_cfg_t                ue_ul_cfg      = {};
  srslte_enb_ul_t                   enb_ul         = {};
  srslte_ul_sf_cfg_t                ul_sf          = {};
  srslte_pucch_res_t                pucch_res      = {};
  srslte_pusch_data_t               pusch_data     = {};

  // Basic default args
  pucch_cfg.delta_pucch_shift      = 1;                      // 1, 2, 3
  pucch_cfg.n_rb_2                 = 1;                      // 0, 1, ..., 98
  pucch_cfg.N_cs                   = 1;                      // 0, 1, ..., 7
  pucch_cfg.N_pucch_1              = 1;                      // 0, 1, ..., 2047
  pucch_cfg.ack_nack_feedback_mode = ack_nack_feedback_mode; // Normal, CS, PUCCH3

  // Set Channel Selection resources
  for (uint32_t i = 0, k = 6; i < SRSLTE_PUCCH_SIZE_AN_CS; i++) {
    for (uint32_t j = 0; j < SRSLTE_PUCCH_NOF_AN_CS; j++, k++) {
      pucch_cfg.n1_pucch_an_cs[i][j] = k;
    }
  }

  uint32_t base_ncce = 1;
  for (uint32_t i = 0; i < nof_carriers; base_ncce += nof_tb[i++]) {
    pucch_cfg.uci_cfg.ack[i].grant_cc_idx = 0;         // 0: PCell Scheduling; 1: SCell Scheduling
    pucch_cfg.uci_cfg.ack[i].ncce[0]      = base_ncce; // PDCCH Location, 0 is always fine
    pucch_cfg.uci_cfg.ack[i].nof_acks     = nof_tb[i]; // Number of transport blocks, 1 or 2
  }

  // Set derived parameters
  pucch_cfg.rnti = rnti;

  // Init buffers
  buffer = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB(cell.nof_prb));
  TESTASSERT(buffer);

  // Init UE
  TESTASSERT(!srslte_ue_ul_init(&ue_ul, buffer, cell.nof_prb));
  TESTASSERT(!srslte_ue_ul_set_cell(&ue_ul, cell));
  srslte_ue_ul_set_rnti(&ue_ul, rnti);

  // Init eNb
  TESTASSERT(!srslte_enb_ul_init(&enb_ul, buffer, cell.nof_prb));
  TESTASSERT(!srslte_enb_ul_set_cell(&enb_ul, cell, &dmrs_pusch_cfg, NULL));
  TESTASSERT(!srslte_enb_ul_add_rnti(&enb_ul, rnti));

  // The test itself starts here
  for (ul_sf.tti = 0; ul_sf.tti < (1U << (nof_carriers * 2U)); ul_sf.tti++) {

    // Generate new data
    pusch_data.uci.ack.valid = true;
    for (uint32_t i = 0, k = 0; i < nof_carriers; i++) {
      for (uint32_t j = 0; j < nof_tb[i]; j++, k++) {
        pusch_data.uci.ack.ack_value[k] = (ul_sf.tti >> k) & 1U;
      }
    }

    // Copy UL configuration
    ue_ul_cfg.ul_cfg.pucch = pucch_cfg;

    // Generate UL Signal
    TESTASSERT(srslte_ue_ul_encode(&ue_ul, &ul_sf, &ue_ul_cfg, &pusch_data) >= SRSLTE_SUCCESS);

    // Process UL signal
    srslte_enb_ul_fft(&enb_ul);

    TESTASSERT(!srslte_enb_ul_get_pucch(&enb_ul, &ul_sf, &pucch_cfg, &pucch_res));

    TESTASSERT(pucch_res.detected);
    TESTASSERT(pucch_res.uci_data.ack.valid);

    // Check results
    for (int i = 0, k = 0; i < nof_carriers; i++) {
      for (int j = 0; j < nof_tb[i]; j++, k++) {
        INFO("cc=%d; tb=%d; tx_ack=%d; rx_ack=%d;\n",
             i,
             j,
             pusch_data.uci.ack.ack_value[k],
             pucch_res.uci_data.ack.ack_value[k]);
        TESTASSERT(pusch_data.uci.ack.ack_value[k] == pucch_res.uci_data.ack.ack_value[k]);
      }
    }
  }

  // Free all
  srslte_ue_ul_free(&ue_ul);
  srslte_enb_ul_free(&enb_ul);
  free(buffer);

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  // Set PHY lib verbose to INFO
  srslte_verbose = SRSLTE_VERBOSE_INFO;

  uint32_t nof_tb_1[SRSLTE_MAX_CARRIERS] = {1, 1, 1, 1, 1};
  uint32_t nof_tb_2[SRSLTE_MAX_CARRIERS] = {2, 1, 1, 1, 1};
  uint32_t nof_tb_3[SRSLTE_MAX_CARRIERS] = {2, 2, 2, 2, 2};

  TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS, 6, nof_tb_1, 2));
  TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS, 6, nof_tb_2, 2));
  TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS, 6, nof_tb_3, 2));

  TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS, 100, nof_tb_1, 2));
  TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS, 100, nof_tb_2, 2));
  TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS, 100, nof_tb_3, 2));

  for (uint32_t i = 2; i < SRSLTE_PUCCH_FORMAT3_MAX_CARRIERS; i++) {
    TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3, 6, nof_tb_1, i));
    TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3, 6, nof_tb_2, i));
    TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3, 6, nof_tb_3, i));

    TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3, 100, nof_tb_1, i));
    TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3, 100, nof_tb_2, i));
    TESTASSERT(!test_pucch_ca(SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3, 100, nof_tb_3, i));
  }

  printf("Ok\n");

  return SRSLTE_SUCCESS;
}
