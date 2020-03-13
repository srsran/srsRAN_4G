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
#include <srslte/config.h>
#include <srslte/phy/utils/bit.h>
#include <srslte/phy/utils/vector.h>
#include <stdlib.h>
#include <strings.h>

#include <srslte/phy/ue/ue_mib_sl.h>
#include <srslte/phy/utils/debug.h>

int srslte_ue_mib_sl_set(srslte_ue_mib_sl_t* q,
                          uint32_t            nof_prb,
                          uint32_t            tdd_config,
                          uint32_t            direct_frame_number,
                          uint32_t            direct_subframe_number,
                          bool                in_coverage)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL) {

    switch (nof_prb) {
      case 6:
        q->sl_bandwidth_r12 = 0;
        break;
      case 15:
        q->sl_bandwidth_r12 = 1;
        break;
      case 25:
        q->sl_bandwidth_r12 = 2;
        break;
      case 50:
        q->sl_bandwidth_r12 = 3;
        break;
      case 75:
        q->sl_bandwidth_r12 = 4;
        break;
      case 100:
        q->sl_bandwidth_r12 = 5;
        break;
      default:
        printf("Invalid bandwidth\n");
        return SRSLTE_ERROR;
    }
    q->tdd_config_sl_r12          = tdd_config;
    q->direct_frame_number_r12    = direct_frame_number;
    q->direct_subframe_number_r12 = direct_subframe_number;
    q->in_coverage_r12            = in_coverage;

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

void srslte_ue_mib_sl_pack(srslte_ue_mib_sl_t *q, uint8_t *msg)
{
  bzero(msg, sizeof(uint8_t) * SRSLTE_MIB_SL_MAX_LEN);
  srslte_bit_unpack(q->sl_bandwidth_r12, &msg, 3);
  srslte_bit_unpack(q->tdd_config_sl_r12, &msg, 3);
  srslte_bit_unpack(q->direct_frame_number_r12, &msg, 10);
  srslte_bit_unpack(q->direct_subframe_number_r12, &msg, 4);
  srslte_bit_unpack((uint32_t)q->in_coverage_r12, &msg, 1);
}

void srlste_ue_mib_sl_unpack(srslte_ue_mib_sl_t *q, uint8_t *msg)
{
  q->sl_bandwidth_r12           = srslte_bit_pack(&msg, 3);
  q->tdd_config_sl_r12          = srslte_bit_pack(&msg, 3);
  q->direct_frame_number_r12    = srslte_bit_pack(&msg, 10);
  q->direct_subframe_number_r12 = srslte_bit_pack(&msg, 4);
  q->in_coverage_r12            = (bool)srslte_bit_pack(&msg, 1);
}

void srslte_ue_mib_sl_free(srslte_ue_mib_sl_t* q)
{
  if (q != NULL) {
    bzero(q, sizeof(srslte_ue_mib_sl_t));
  }
}