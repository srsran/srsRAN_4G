/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <strings.h>

#include "srslte/phy/phch/mib_sl.h"
#include "srslte/phy/utils/bit.h"

int srslte_mib_sl_init(srslte_mib_sl_t* q, srslte_sl_tm_t tm)
{
  if (tm == SRSLTE_SIDELINK_TM1 || tm == SRSLTE_SIDELINK_TM2) {
    q->mib_sl_len = SRSLTE_MIB_SL_LEN;
  } else if (tm == SRSLTE_SIDELINK_TM3 || tm == SRSLTE_SIDELINK_TM4) {
    q->mib_sl_len = SRSLTE_MIB_SL_V2X_LEN;
  } else {
    return SRSLTE_ERROR;
  }

  q->sl_bandwidth_r12           = 0;
  q->direct_frame_number_r12    = 0;
  q->direct_subframe_number_r12 = 0;
  q->in_coverage_r12            = false;
  q->tdd_config_sl_r12          = 0;

  return SRSLTE_SUCCESS;
}

int srslte_mib_sl_set(srslte_mib_sl_t* q,
                      uint32_t         nof_prb,
                      uint32_t         tdd_config,
                      uint32_t         direct_frame_number,
                      uint32_t         direct_subframe_number,
                      bool             in_coverage)
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

void srslte_mib_sl_pack(srslte_mib_sl_t* q, uint8_t* msg)
{
  bzero(msg, sizeof(uint8_t) * SRSLTE_MIB_SL_MAX_LEN);
  srslte_bit_unpack(q->sl_bandwidth_r12, &msg, 3);
  srslte_bit_unpack(q->tdd_config_sl_r12, &msg, 3);
  srslte_bit_unpack(q->direct_frame_number_r12, &msg, 10);
  srslte_bit_unpack(q->direct_subframe_number_r12, &msg, 4);
  srslte_bit_unpack((uint32_t)q->in_coverage_r12, &msg, 1);
}

void srslte_mib_sl_unpack(srslte_mib_sl_t* q, uint8_t* msg)
{
  q->sl_bandwidth_r12           = srslte_bit_pack(&msg, 3);
  q->tdd_config_sl_r12          = srslte_bit_pack(&msg, 3);
  q->direct_frame_number_r12    = srslte_bit_pack(&msg, 10);
  q->direct_subframe_number_r12 = srslte_bit_pack(&msg, 4);
  q->in_coverage_r12            = (bool)srslte_bit_pack(&msg, 1);
}

void srslte_mib_sl_printf(FILE* f, srslte_mib_sl_t* q)
{
  fprintf(f, " - Bandwidth:              %i\n", q->sl_bandwidth_r12);
  fprintf(f, " - Direct Frame Number:    %i\n", q->direct_frame_number_r12);
  fprintf(f, " - Direct Subframe Number: %i\n", q->direct_subframe_number_r12);
  fprintf(f, " - TDD config:             %i\n", q->tdd_config_sl_r12);
  fprintf(f, " - In coverage:            %s\n", q->in_coverage_r12 ? "yes" : "no");
}

void srslte_mib_sl_free(srslte_mib_sl_t* q)
{
  if (q != NULL) {
    bzero(q, sizeof(srslte_mib_sl_t));
  }
}
