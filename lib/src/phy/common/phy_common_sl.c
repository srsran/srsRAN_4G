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

#include <string.h>

#include "srslte/phy/common/phy_common_sl.h"
#include "srslte/phy/common/sequence.h"
#include "srslte/phy/utils/debug.h"

int srslte_sl_group_hopping_f_gh(uint32_t f_gh[SRSLTE_NSLOTS_X_FRAME * 2], uint32_t N_x_id)
{
  srslte_sequence_t seq;
  bzero(&seq, sizeof(srslte_sequence_t));

  if (srslte_sequence_LTE_pr(&seq, 160 * 2, N_x_id / 30)) {
    return SRSLTE_ERROR;
  }

  for (uint32_t ns = 0; ns < SRSLTE_NSLOTS_X_FRAME * 2; ns++) {
    f_gh[ns] = 0;
    for (int i = 0; i < 8; i++) {
      f_gh[ns] += (((uint32_t)seq.c[8 * ns + i]) << i);
    }
  }

  srslte_sequence_free(&seq);
  return SRSLTE_SUCCESS;
}

int srslte_sl_get_num_symbols(srslte_sl_tm_t tm, srslte_cp_t cp)
{
  if (tm == SRSLTE_SIDELINK_TM1 || tm == SRSLTE_SIDELINK_TM2) {
    if (cp == SRSLTE_CP_NORM) {
      return SRSLTE_CP_NORM_SF_NSYMB;
    } else {
      return SRSLTE_CP_EXT_SF_NSYMB;
    }
  } else if (tm == SRSLTE_SIDELINK_TM3 || tm == SRSLTE_SIDELINK_TM4) {
    if (cp == SRSLTE_CP_NORM) {
      return SRSLTE_CP_NORM_SF_NSYMB;
    } else {
      printf("Invalid CP\n");
      return SRSLTE_ERROR;
    }
  } else {
    printf("Invalid TM\n");
    return SRSLTE_ERROR;
  }
}

// Look-up tables for Sidelink channel symbols
static const srslte_sl_symbol_t srslte_psbch_symbol_map_tm12[SRSLTE_CP_NORM_SF_NSYMB] = {SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_SYNC_SYMBOL,
                                                                                         SRSLTE_SIDELINK_SYNC_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_SYNC_SYMBOL,
                                                                                         SRSLTE_SIDELINK_SYNC_SYMBOL,
                                                                                         SRSLTE_SIDELINK_GUARD_SYMBOL};

static const srslte_sl_symbol_t srslte_psbch_symbol_map_tm12_ext[SRSLTE_CP_EXT_SF_NSYMB] = {
    SRSLTE_SIDELINK_SYNC_SYMBOL,
    SRSLTE_SIDELINK_SYNC_SYMBOL,
    SRSLTE_SIDELINK_DMRS_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DMRS_SYMBOL,
    SRSLTE_SIDELINK_SYNC_SYMBOL,
    SRSLTE_SIDELINK_SYNC_SYMBOL,
    SRSLTE_SIDELINK_GUARD_SYMBOL};

static const srslte_sl_symbol_t srslte_psbch_symbol_map_tm34[SRSLTE_CP_NORM_SF_NSYMB] = {SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_SYNC_SYMBOL,
                                                                                         SRSLTE_SIDELINK_SYNC_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_SYNC_SYMBOL,
                                                                                         SRSLTE_SIDELINK_SYNC_SYMBOL,
                                                                                         SRSLTE_SIDELINK_GUARD_SYMBOL};

bool srslte_psbch_is_symbol(srslte_sl_symbol_t type, srslte_sl_tm_t tm, uint32_t i, srslte_cp_t cp)
{
  if (tm == SRSLTE_SIDELINK_TM1 || tm == SRSLTE_SIDELINK_TM2) {
    if (cp == SRSLTE_CP_EXT) {
      return srslte_psbch_symbol_map_tm12_ext[i] == type;
    }
    return srslte_psbch_symbol_map_tm12[i] == type;
  } else if (tm == SRSLTE_SIDELINK_TM3 || tm == SRSLTE_SIDELINK_TM4) {
    return srslte_psbch_symbol_map_tm34[i] == type;
  }
  return false;
}

static const srslte_sl_symbol_t srslte_pscch_symbol_map_tm12[SRSLTE_CP_NORM_SF_NSYMB] = {SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_GUARD_SYMBOL};

static const srslte_sl_symbol_t srslte_pscch_symbol_map_tm12_ext[SRSLTE_CP_EXT_SF_NSYMB] = {
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DMRS_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DMRS_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_GUARD_SYMBOL};

static const srslte_sl_symbol_t srslte_pscch_symbol_map_tm34[SRSLTE_CP_NORM_SF_NSYMB] = {SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_GUARD_SYMBOL};

bool srslte_pscch_is_symbol(srslte_sl_symbol_t type, srslte_sl_tm_t tm, uint32_t i, srslte_cp_t cp)
{
  if (tm == SRSLTE_SIDELINK_TM1 || tm == SRSLTE_SIDELINK_TM2) {
    if (cp == SRSLTE_CP_EXT) {
      return srslte_pscch_symbol_map_tm12_ext[i] == type;
    }
    return srslte_pscch_symbol_map_tm12[i] == type;
  } else if (tm == SRSLTE_SIDELINK_TM3 || tm == SRSLTE_SIDELINK_TM4) {
    return srslte_pscch_symbol_map_tm34[i] == type;
  }
  return false;
}

static const srslte_sl_symbol_t srslte_pssch_symbol_map_tm12[SRSLTE_CP_NORM_SF_NSYMB] = {SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_GUARD_SYMBOL};

static const srslte_sl_symbol_t srslte_pssch_symbol_map_tm12_ext[SRSLTE_CP_EXT_SF_NSYMB] = {
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DMRS_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DMRS_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_DATA_SYMBOL,
    SRSLTE_SIDELINK_GUARD_SYMBOL};

static const srslte_sl_symbol_t srslte_pssch_symbol_map_tm34[SRSLTE_CP_NORM_SF_NSYMB] = {SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DMRS_SYMBOL,
                                                                                         SRSLTE_SIDELINK_DATA_SYMBOL,
                                                                                         SRSLTE_SIDELINK_GUARD_SYMBOL};

bool srslte_pssch_is_symbol(srslte_sl_symbol_t type, srslte_sl_tm_t tm, uint32_t i, srslte_cp_t cp)
{
  if (tm == SRSLTE_SIDELINK_TM1 || tm == SRSLTE_SIDELINK_TM2) {
    if (cp == SRSLTE_CP_EXT) {
      return srslte_pssch_symbol_map_tm12_ext[i] == type;
    }
    return srslte_pssch_symbol_map_tm12[i] == type;
  } else if (tm == SRSLTE_SIDELINK_TM3 || tm == SRSLTE_SIDELINK_TM4) {
    return srslte_pssch_symbol_map_tm34[i] == type;
  }
  return false;
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

// Example pool configurations based on 3GPP TS 36.101 Version 15.6 Sections A.7 and A.9
int srslte_sl_comm_resource_pool_get_default_config(srslte_sl_comm_resource_pool_t* q, srslte_cell_sl_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    q->period_length = 40;
    if (cell.tm == SRSLTE_SIDELINK_TM3 || cell.tm == SRSLTE_SIDELINK_TM4) {
      q->period_length = 160;
    }

    // Use full Bandwidth
    q->prb_num   = (uint32_t)ceil(cell.nof_prb / 2.0);
    q->prb_start = 0;
    q->prb_end   = cell.nof_prb - 1;

    q->size_sub_channel      = 10;
    q->num_sub_channel       = 5;
    q->start_prb_sub_channel = 0;
    q->adjacency_pscch_pssch = true;

    if (cell.tm == SRSLTE_SIDELINK_TM4) {
      switch (cell.nof_prb) {
        case 6:
          q->size_sub_channel = 6;
          q->num_sub_channel  = 1;
          break;
        case 15:
        case 25:
        case 75:
          q->size_sub_channel = 5;
          q->num_sub_channel  = cell.nof_prb / q->size_sub_channel;
          break;
        case 50:
        case 100:
          q->size_sub_channel = 10;
          q->num_sub_channel  = cell.nof_prb / q->size_sub_channel;
          break;
        default:
          ERROR("Invalid nof_prb");
          return SRSLTE_ERROR;
      }
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}
