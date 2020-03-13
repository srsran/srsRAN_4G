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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srslte/srslte.h"

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      printf("[%s][Line %d]. Fail at %s\n", __FUNCTION__, __LINE__, #cond);                                            \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

int fdd_tests()
{
  srslte_cell_t      cell;
  srslte_pucch_cfg_t pucch_cfg;
  srslte_uci_cfg_t   uci_cfg;
  srslte_uci_value_t uci_value;
  uint8_t            b[SRSLTE_UCI_MAX_ACK_BITS] = {};

  ZERO_OBJECT(cell);
  cell.cp         = SRSLTE_CP_NORM;
  cell.frame_type = SRSLTE_FDD;
  cell.nof_prb    = 50;

  ZERO_OBJECT(pucch_cfg);
  pucch_cfg.n_pucch_sr = 3;
  pucch_cfg.N_pucch_1  = 7;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      pucch_cfg.n1_pucch_an_cs[i][j] = j * 11 + i * 13;
    }
  }
  ZERO_OBJECT(uci_value);

  // Format 1
  ZERO_OBJECT(uci_cfg);
  uci_value.scheduling_request = true;
  srslte_ue_ul_pucch_resource_selection(&cell, &pucch_cfg, &uci_cfg, &uci_value, b);
  TESTASSERT(pucch_cfg.format == SRSLTE_PUCCH_FORMAT_1);
  TESTASSERT(pucch_cfg.n_pucch == pucch_cfg.n_pucch_sr);

  // Format 1A with and without SR
  for (int i = 0; i < 2; i++) {
    uci_value.scheduling_request = i == 0;
    ZERO_OBJECT(uci_cfg);
    uci_cfg.ack[0].nof_acks = 1;
    uci_cfg.ack[0].ncce[0]  = 13;
    srslte_ue_ul_pucch_resource_selection(&cell, &pucch_cfg, &uci_cfg, &uci_value, b);
    TESTASSERT(pucch_cfg.format == SRSLTE_PUCCH_FORMAT_1A);
    if (i == 0) {
      TESTASSERT(pucch_cfg.n_pucch == pucch_cfg.n_pucch_sr);
    } else {
      TESTASSERT(pucch_cfg.n_pucch == uci_cfg.ack[0].ncce[0] + pucch_cfg.N_pucch_1);
    }
  }

  // Format 1B with and without SR, MIMO
  pucch_cfg.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL;
  for (int i = 0; i < 2; i++) {
    uci_value.scheduling_request = i == 0;
    uci_value.ack.ack_value[0]   = 1; // To force a different resource than n_pucch_0 in case of CS incorrectly selected
    uci_value.ack.ack_value[1]   = 1;
    ZERO_OBJECT(uci_cfg);
    uci_cfg.ack[0].nof_acks = 2;
    uci_cfg.ack[0].ncce[0]  = 13;
    srslte_ue_ul_pucch_resource_selection(&cell, &pucch_cfg, &uci_cfg, &uci_value, b);
    TESTASSERT(pucch_cfg.format == SRSLTE_PUCCH_FORMAT_1B);
    if (i == 0) {
      TESTASSERT(pucch_cfg.n_pucch == pucch_cfg.n_pucch_sr);
    } else {
      TESTASSERT(pucch_cfg.n_pucch == uci_cfg.ack[0].ncce[0] + pucch_cfg.N_pucch_1);
    }
  }

  // Format 1B-CS, no SR, 2 CA SISO
  uci_value.scheduling_request = 0;
  ZERO_OBJECT(uci_cfg);
  pucch_cfg.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS;
  uci_cfg.ack[0].nof_acks          = 1;
  uci_cfg.ack[1].nof_acks          = 1;
  uci_cfg.ack[0].ncce[0]           = 13;
  uci_cfg.ack[1].ncce[0]           = 4;

  // Both on Pcell

  // ACK/ACK, n_pucch = n_pucch_1
  uci_value.ack.ack_value[0] = 1;
  uci_value.ack.ack_value[1] = 1;
  srslte_ue_ul_pucch_resource_selection(&cell, &pucch_cfg, &uci_cfg, &uci_value, b);
  TESTASSERT(pucch_cfg.format == SRSLTE_PUCCH_FORMAT_1B);
  TESTASSERT(pucch_cfg.n_pucch == uci_cfg.ack[1].ncce[0] + pucch_cfg.N_pucch_1);

  // ACK/DTX, n_pucch = n_pucch_0
  uci_value.ack.ack_value[0] = 1;
  uci_value.ack.ack_value[1] = 2;
  srslte_ue_ul_pucch_resource_selection(&cell, &pucch_cfg, &uci_cfg, &uci_value, b);
  TESTASSERT(pucch_cfg.format == SRSLTE_PUCCH_FORMAT_1B);
  TESTASSERT(pucch_cfg.n_pucch == uci_cfg.ack[0].ncce[0] + pucch_cfg.N_pucch_1);

  // Each on its serving cell, n_pucch_1 is explicit
  uci_cfg.ack[1].grant_cc_idx  = 1;
  uci_cfg.ack[1].tpc_for_pucch = 3;
  uci_value.ack.ack_value[0]   = 1;
  uci_value.ack.ack_value[1]   = 1;
  srslte_ue_ul_pucch_resource_selection(&cell, &pucch_cfg, &uci_cfg, &uci_value, b);
  TESTASSERT(pucch_cfg.format == SRSLTE_PUCCH_FORMAT_1B);
  TESTASSERT(pucch_cfg.n_pucch == pucch_cfg.n1_pucch_an_cs[uci_cfg.ack[1].tpc_for_pucch][0]);

  // PCell scheduled on Scell
  uci_cfg.ack[0].grant_cc_idx  = 1;
  uci_cfg.ack[0].tpc_for_pucch = 2;
  uci_value.ack.ack_value[0]   = 1;
  uci_value.ack.ack_value[1]   = 2;
  srslte_ue_ul_pucch_resource_selection(&cell, &pucch_cfg, &uci_cfg, &uci_value, b);
  TESTASSERT(pucch_cfg.format == SRSLTE_PUCCH_FORMAT_1B);
  TESTASSERT(pucch_cfg.n_pucch == pucch_cfg.n1_pucch_an_cs[uci_cfg.ack[0].tpc_for_pucch][0]);

  // MIMO has the same logic of resource selection, no need to test for now

  // Format 1B-CS with SR
  ZERO_OBJECT(uci_cfg);
  uci_value.scheduling_request = true;
  uci_cfg.ack[0].nof_acks      = 1;
  uci_cfg.ack[1].nof_acks      = 1;
  srslte_ue_ul_pucch_resource_selection(&cell, &pucch_cfg, &uci_cfg, &uci_value, b);
  TESTASSERT(pucch_cfg.format == SRSLTE_PUCCH_FORMAT_1B);

  return 0;
}

int main(int argc, char** argv)
{

  // Test only until Format1B - CS
  TESTASSERT(fdd_tests() == 0);

  printf("Ok\n");
  exit(0);
}
