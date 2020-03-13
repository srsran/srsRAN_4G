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

#include "srslte/phy/common/phy_common_sl.h"
#include "srslte/phy/utils/debug.h"

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
      ERROR("Invalid CP");
      return SRSLTE_ERROR;
    }
  } else {
    ERROR("Invalid TM");
    return SRSLTE_ERROR;
  }
}

///< Look-up tables for Sidelink channel symbols
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

bool srslte_psbch_is_symbol(srslte_sl_symbol_t type, srslte_sl_tm_t tm, uint32_t i)
{
  if (tm <= SRSLTE_SIDELINK_TM2) {
    return srslte_psbch_symbol_map_tm12[i] == type;
  } else {
    return srslte_psbch_symbol_map_tm34[i] == type;
  }
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
