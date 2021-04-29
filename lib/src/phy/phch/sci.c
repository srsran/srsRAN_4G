/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include <strings.h>

#include "srsran/phy/phch/sci.h"
#include "srsran/phy/utils/bit.h"

int srsran_sci_init(srsran_sci_t*                         q,
                    const srsran_cell_sl_t*               cell,
                    const srsran_sl_comm_resource_pool_t* sl_comm_resource_pool)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL && cell != NULL && sl_comm_resource_pool != NULL) {
    ret = SRSRAN_ERROR;

    bzero(q, sizeof(srsran_sci_t));

    q->nof_prb = cell->nof_prb;
    q->tm      = cell->tm;

    if (cell->tm == SRSRAN_SIDELINK_TM1 || cell->tm == SRSRAN_SIDELINK_TM2) {
      q->format  = SRSRAN_SCI_FORMAT0;
      q->sci_len = srsran_sci_format0_sizeof(cell->nof_prb);

    } else if (cell->tm == SRSRAN_SIDELINK_TM3 || cell->tm == SRSRAN_SIDELINK_TM4) {
      q->format           = SRSRAN_SCI_FORMAT1;
      q->sci_len          = SRSRAN_SCI_TM34_LEN;
      q->size_sub_channel = sl_comm_resource_pool->size_sub_channel;
      q->num_sub_channel  = sl_comm_resource_pool->num_sub_channel;

    } else {
      return SRSRAN_ERROR;
    }
    ret = SRSRAN_SUCCESS;
  }

  return ret;
}

int srsran_sci_format0_pack(srsran_sci_t* q, uint8_t* output)
{
  if (!q) {
    printf("Error packing SCI Format 0\n");
    return SRSRAN_ERROR;
  }

  bzero(output, sizeof(uint8_t) * SRSRAN_SCI_MAX_LEN);
  srsran_bit_unpack((uint32_t)q->freq_hopping_flag, &output, 1);
  if (q->freq_hopping_flag) {
    printf("Frequency Hopping in Sidelink is not supported\n");
    return SRSRAN_ERROR;
  } else {
    srsran_bit_unpack(q->riv, &output, (uint32_t)ceil(log2(((q->nof_prb) * (q->nof_prb + 1) / 2))));
  }
  srsran_bit_unpack(q->trp_idx, &output, 7);
  srsran_bit_unpack(q->mcs_idx, &output, 5);
  srsran_bit_unpack(q->timing_advance, &output, 11);
  srsran_bit_unpack(q->N_sa_id, &output, 8);

  return SRSRAN_SUCCESS;
}

int srsran_sci_format1_pack(srsran_sci_t* q, uint8_t* output)
{
  if (!q) {
    printf("Error packing SCI Format 1\n");
    return SRSRAN_ERROR;
  }

  bzero(output, sizeof(uint8_t) * SRSRAN_SCI_MAX_LEN);
  srsran_bit_unpack(q->priority, &output, 3);
  srsran_bit_unpack(q->resource_reserv, &output, 4);
  srsran_bit_unpack(q->riv, &output, (uint32_t)ceil(log2(((q->num_sub_channel) * (q->num_sub_channel + 1) / 2))));
  srsran_bit_unpack(q->time_gap, &output, 4);
  srsran_bit_unpack(q->mcs_idx, &output, 5);
  srsran_bit_unpack(q->retransmission, &output, 1);

  return SRSRAN_SUCCESS;
}

int srsran_sci_format0_unpack(srsran_sci_t* q, uint8_t* input)
{
  if (!q) {
    printf("Error unpacking SCI Format 0\n");
    return SRSRAN_ERROR;
  }

  // Sanity check: avoid SCIs with all 0s
  uint32_t i = 0;
  for (; i < q->sci_len; i++) {
    if (input[i] != 0) {
      break;
    }
  }
  if (i == q->sci_len) {
    return SRSRAN_ERROR;
  }

  q->freq_hopping_flag = (bool)srsran_bit_pack(&input, 1);
  if (q->freq_hopping_flag) {
    printf("Frequency Hopping in Sidelink is not supported\n");
    return SRSRAN_ERROR;
  } else {
    q->riv = srsran_bit_pack(&input, (uint32_t)ceil(log2(((q->nof_prb) * (q->nof_prb + 1) / 2))));
  }
  q->trp_idx        = srsran_bit_pack(&input, 7);
  q->mcs_idx        = srsran_bit_pack(&input, 5);
  q->timing_advance = srsran_bit_pack(&input, 11);
  q->N_sa_id        = srsran_bit_pack(&input, 8);

  // Sanity check
  if (q->mcs_idx >= 29) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_sci_format1_unpack(srsran_sci_t* q, uint8_t* input)
{
  if (!q) {
    printf("Error unpacking SCI Format 1\n");
    return SRSRAN_ERROR;
  }

  // Sanity check: avoid SCIs with all 0s
  uint32_t i = 0;
  for (; i < q->sci_len; i++) {
    if (input[i] != 0) {
      break;
    }
  }
  if (i == q->sci_len) {
    return SRSRAN_ERROR;
  }

  q->priority        = srsran_bit_pack(&input, 3);
  q->resource_reserv = srsran_bit_pack(&input, 4);
  q->riv      = srsran_bit_pack(&input, (uint32_t)ceil(log2(((q->num_sub_channel) * (q->num_sub_channel + 1) / 2))));
  q->time_gap = srsran_bit_pack(&input, 4);
  q->mcs_idx  = srsran_bit_pack(&input, 5);
  q->retransmission      = srsran_bit_pack(&input, 1);
  q->transmission_format = srsran_bit_pack(&input, 1);

  // Sanity check
  if (q->mcs_idx >= 29) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_sci_info(const srsran_sci_t* q, char* str, uint32_t len)
{
  uint32_t n = 0;
  n          = srsran_print_check(str, len, n, "SCI%i: riv=%i, mcs=%i", q->format, q->riv, q->mcs_idx);

  if (q->format == SRSRAN_SCI_FORMAT0) {
    n = srsran_print_check(str,
                           SRSRAN_SCI_MSG_MAX_LEN,
                           n,
                           ", trp_idx=%i, t_adv=%i, n_sa_id=%i, freqhoppflg=%s",
                           q->trp_idx,
                           q->timing_advance,
                           q->N_sa_id,
                           q->freq_hopping_flag ? "true" : "false");
  } else if (q->format == SRSRAN_SCI_FORMAT1) {
    n = srsran_print_check(str,
                           SRSRAN_SCI_MSG_MAX_LEN,
                           n,
                           ", priority=%i, res_rsrv=%i, t_gap=%i, rtx=%i, txformat=%d",
                           q->priority,
                           q->resource_reserv,
                           q->time_gap,
                           q->retransmission,
                           q->transmission_format);
  }
}

void srsran_sci_free(srsran_sci_t* q)
{
  if (q != NULL) {
    bzero(q, sizeof(srsran_sci_t));
  }
}
