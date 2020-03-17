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
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      printf("[%s][Line %d]. Fail at %s\n", __FUNCTION__, __LINE__, #cond);                                            \
      return -1;                                                                                                       \
    }                                                                                                                  \
  } while (false)

int fdd_tests(uint32_t max_cc)
{
  srslte_ue_dl_t     ue_dl     = {};
  srslte_dl_sf_cfg_t sf_cfg_dl = {}; // This is used for TDD only

  uint32_t test_cnt = 0;

  // Force FDD
  ue_dl.cell.frame_type = SRSLTE_FDD;

  for (uint32_t nof_cc = 1; nof_cc <= max_cc; nof_cc++) {
    for (uint32_t nof_tb = 1; nof_tb <= SRSLTE_MAX_CODEWORDS; nof_tb++) {
      for (uint32_t nof_active_cc = 1; nof_active_cc <= nof_cc; nof_active_cc++) {
        for (uint32_t nof_active_tb = 1; nof_active_tb <= nof_tb; nof_active_tb++) {
          srslte_pdsch_ack_t ack_info = {};
          ack_info.nof_cc             = nof_cc;
          ack_info.transmission_mode  = nof_tb == 1 ? SRSLTE_TM1 : SRSLTE_TM4;

          // Check different modes?
          ack_info.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS;

          for (uint32_t cc_idx = 0; cc_idx < nof_cc; cc_idx++) {
            ack_info.cc[cc_idx].M                   = 1; // always 1 in FDD
            ack_info.cc[cc_idx].m[0].present        = cc_idx < nof_active_cc;
            ack_info.cc[cc_idx].m[0].resource.n_cce = cc_idx + 1;
            if (ack_info.cc[cc_idx].m[0].present) {
              for (uint32_t j = 0; j < nof_tb; j++) {
                ack_info.cc[cc_idx].m[0].value[j] = j < nof_active_tb ? 1 : 2;
              }
            } else {
              memset(ack_info.cc[cc_idx].m[0].value, 2, SRSLTE_MAX_CODEWORDS);
            }
          }

          for (uint32_t sr_enabled = 0; sr_enabled < 2; sr_enabled++) { // 0: true, 1: false, otherwise: break
            for (uint8_t cqi_enabled = 0; cqi_enabled < 2; cqi_enabled++) {
              for (uint8_t simul_cqi_ack = 0; simul_cqi_ack < cqi_enabled + 1; simul_cqi_ack++) {
                for (uint8_t pusch_enabled = 0; pusch_enabled < 2; pusch_enabled++) {
                  srslte_uci_data_t uci_data = {};

                  ack_info.is_pusch_available = (pusch_enabled > 0);
                  ack_info.simul_cqi_ack      = (simul_cqi_ack > 0);

                  // Generate ACK/NACK bits
                  srslte_uci_data_reset(&uci_data);
                  uci_data.value.scheduling_request = (sr_enabled > 0);
                  uci_data.cfg.cqi.data_enable      = (cqi_enabled > 0);

                  printf("FDD Test %d: TM=%d, CC=%d, nof_active_tb=%d, nof_active_cc=%d, sr_request=%s, cqi_data=%s, "
                         "simul_cqi_ack=%s, is_pusch=%s\n",
                         test_cnt++,
                         ack_info.transmission_mode + 1,
                         nof_cc,
                         nof_active_tb,
                         nof_active_cc,
                         sr_enabled ? "yes" : "no",
                         cqi_enabled ? "yes" : "no",
                         simul_cqi_ack ? "yes" : "no",
                         pusch_enabled ? "yes" : "no");

                  srslte_ue_dl_gen_ack(&ue_dl.cell, &sf_cfg_dl, &ack_info, &uci_data);

                  // Check output
                  if (nof_cc == 1) {
                    TESTASSERT(uci_data.cfg.ack[0].nof_acks == nof_active_tb);
                  } else if (uci_data.value.scheduling_request && !ack_info.is_pusch_available) {
                    TESTASSERT(uci_data.cfg.ack[0].nof_acks == 1);
                  } else if (cqi_enabled && !pusch_enabled) {
                    // No PUSCH, CSI and HARQ ACK collision
                    if (nof_active_cc == 1 && simul_cqi_ack) {
                      // CSI shall not be dropped
                      TESTASSERT(uci_data.cfg.cqi.data_enable);

                      // Serving cell only HARQ ACK is multiplexed
                      TESTASSERT(uci_data.cfg.ack[0].nof_acks == nof_active_tb);

                      // Other serving cells HARQ shall not be multiplexed
                      for (int i = 1; i < nof_cc; i++) {
                        TESTASSERT(uci_data.cfg.ack[i].nof_acks == 0);
                      }
                    } else {
                      // CSI shall be dropped
                      TESTASSERT(!uci_data.cfg.cqi.data_enable);

                      // Acknowledgments as usual
                      for (int i = 0; i < nof_cc; i++) {
                        TESTASSERT(uci_data.cfg.ack[i].nof_acks == nof_tb);
                      }
                    }
                  } else {
                    for (int i = 0; i < nof_cc; i++) {
                      TESTASSERT(uci_data.cfg.ack[i].nof_acks == nof_tb);
                    }
                  }
                  uint32_t k = 0;
                  for (uint32_t i = 0; i < nof_cc; i++) {
                    TESTASSERT(uci_data.cfg.ack[i].ncce[0] == i + 1);
                    for (uint32_t j = 0; j < uci_data.cfg.ack[i].nof_acks; j++) {
                      TESTASSERT(uci_data.value.ack.ack_value[k++]);
                    }
                  }
                  TESTASSERT(k == srslte_uci_cfg_total_ack(&uci_data.cfg));
                  TESTASSERT(uci_data.value.ack.ack_value[k] == 2);
                }
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

int main(int argc, char** argv)
{

  // Test only until Format1B - CS
  TESTASSERT(fdd_tests(2) == 0);

  printf("Ok\n");
  exit(0);
}
