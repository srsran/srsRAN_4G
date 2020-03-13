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

#include <strings.h>
#include "srslte/phy/phch/sci.h"
#include "srslte/phy/utils/bit.h"

int srslte_sci_init(srslte_sci_t*  q,
                    uint32_t       nof_prb,
                    srslte_sl_tm_t tm,
                    uint32_t       size_sub_channel,
                    uint32_t       num_sub_channel)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    ret = SRSLTE_ERROR;

    bzero(q, sizeof(srslte_sci_t));

    q->nof_prb = nof_prb;
    q->tm      = tm;

    if (tm == SRSLTE_SIDELINK_TM1 || tm == SRSLTE_SIDELINK_TM2) {
      q->format = SRSLTE_SCI_FORMAT0;
      q->sci_len = srslte_sci_format0_sizeof(nof_prb);

    } else if (tm == SRSLTE_SIDELINK_TM3 || tm == SRSLTE_SIDELINK_TM4) {
      q->format = SRSLTE_SCI_FORMAT1;
      q->sci_len          = SRSLTE_SCI_TM34_LEN;
      q->size_sub_channel = size_sub_channel;
      q->num_sub_channel  = num_sub_channel;

    } else {
      return SRSLTE_ERROR;
    }
    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

int srslte_sci_format0_pack(srslte_sci_t* q, uint8_t* output)
{
  if (!q) {
    printf("Error packing SCI Format 0\n");
    return SRSLTE_ERROR;
  }

  bzero(output, sizeof(uint8_t) * SRSLTE_SCI_MAX_LEN);
  srslte_bit_unpack((uint32_t)q->freq_hopping_flag, &output, 1);
  if (q->freq_hopping_flag) {
    printf("Frequency Hopping in Sidelink is not supported\n");
    return SRSLTE_ERROR;
  } else {
    srslte_bit_unpack(q->riv, &output, (uint32_t)ceil(log2(((q->nof_prb) * (q->nof_prb + 1) / 2))));
  }
  srslte_bit_unpack(q->trp_idx, &output, 7);
  srslte_bit_unpack(q->mcs_idx, &output, 5);
  srslte_bit_unpack(q->timing_advance, &output, 11);
  srslte_bit_unpack(q->N_sa_id, &output, 8);

  return SRSLTE_SUCCESS;
}

int srslte_sci_format1_pack(srslte_sci_t* q, uint8_t* output)
{
  if (!q) {
    printf("Error packing SCI Format 1\n");
    return SRSLTE_ERROR;
  }

  bzero(output, sizeof(uint8_t) * SRSLTE_SCI_MAX_LEN);
  srslte_bit_unpack(q->priority, &output, 3);
  srslte_bit_unpack(q->resource_reserv, &output, 4);
  srslte_bit_unpack(q->riv, &output, (uint32_t)ceil(log2(((q->num_sub_channel) * (q->num_sub_channel + 1) / 2))));
  srslte_bit_unpack(q->time_gap, &output, 4);
  srslte_bit_unpack(q->mcs_idx, &output, 5);
  srslte_bit_unpack(q->retransmission, &output, 1);

  return SRSLTE_SUCCESS;
}

int srslte_sci_format0_unpack(srslte_sci_t* q, uint8_t* input)
{
  if (!q) {
    printf("Error unpacking SCI Format 0\n");
    return SRSLTE_ERROR;
  }

  q->freq_hopping_flag = (bool)srslte_bit_pack(&input, 1);
  if (q->freq_hopping_flag) {
    printf("Frequency Hopping in Sidelink is not supported\n");
    return SRSLTE_ERROR;
  } else {
    q->riv = srslte_bit_pack(&input, (uint32_t)ceil(log2(((q->nof_prb) * (q->nof_prb + 1) / 2))));
  }
  q->trp_idx        = srslte_bit_pack(&input, 7);
  q->mcs_idx        = srslte_bit_pack(&input, 5);
  q->timing_advance = srslte_bit_pack(&input, 11);
  q->N_sa_id        = srslte_bit_pack(&input, 8);

  return SRSLTE_SUCCESS;
}

int srslte_sci_format1_unpack(srslte_sci_t* q, uint8_t* input)
{
  if (!q) {
    printf("Error unpacking SCI Format 1\n");
    return SRSLTE_ERROR;
  }

  q->priority        = srslte_bit_pack(&input, 3);
  q->resource_reserv = srslte_bit_pack(&input, 4);
  q->riv      = srslte_bit_pack(&input, (uint32_t)ceil(log2(((q->num_sub_channel) * (q->num_sub_channel + 1) / 2))));
  q->time_gap = srslte_bit_pack(&input, 4);
  q->mcs_idx  = srslte_bit_pack(&input, 5);
  q->retransmission = srslte_bit_pack(&input, 1);

  return SRSLTE_SUCCESS;
}

void srslte_sci_info(char* str, srslte_sci_t* q)
{
  uint32_t n = snprintf(str, 20, "SCI%i: riv=%i, mcs=%i", q->format, q->riv, q->mcs_idx);

  if (q->format == SRSLTE_SCI_FORMAT0) {
    n = srslte_print_check(str,
                           SRSLTE_SCI_MSG_MAX_LEN,
                           n,
                           ", trp_idx=%i, t_adv=%i, n_sa_id=%i, freqhoppflg=%s\n",
                           q->trp_idx,
                           q->timing_advance,
                           q->N_sa_id,
                           q->freq_hopping_flag ? "true" : "false");
  } else if (q->format == SRSLTE_SCI_FORMAT1) {
    n = srslte_print_check(str,
                           SRSLTE_SCI_MSG_MAX_LEN,
                           n,
                           ", priority=%i, res_rsrv=%i, t_gap=%i, rtx=%i\n",
                           q->priority,
                           q->resource_reserv,
                           q->time_gap,
                           q->retransmission);
  }
}

void srslte_sci_free(srslte_sci_t* q)
{
  if (q != NULL) {
    bzero(q, sizeof(srslte_sci_t));
  }
}

uint32_t srslte_sci_format0_sizeof(uint32_t nof_prb)
{
  // 3GPP TS 36.212 5.4.3.1
  uint32_t n = 0;

  // Frequency hopping flag – 1 bit
  n += 1;

  // Resource block assignment and hopping resource allocation
  n += (uint32_t)ceil(log((nof_prb * (nof_prb + 1)) / 2.0) / log(2));

  // Time resource pattern – 7 bits
  n += 7;

  // Modulation and coding scheme – 5 bit
  n += 5;

  // Timing advance indication – 11 bits
  n += 11;

  // Group destination ID – 8 bits
  n += 8;

  return n;
}