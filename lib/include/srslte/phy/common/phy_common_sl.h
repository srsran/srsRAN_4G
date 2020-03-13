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

/**********************************************************************************************
 *  File:         phy_common_sl.h
 *
 *  Description:  Common parameters and lookup functions for Sidelink PHY
 *
 *  Reference:    3GPP TS 36.211 version 15.3.0 Release 15
 *********************************************************************************************/

#ifndef SRSLTE_PHY_COMMON_SL_H
#define SRSLTE_PHY_COMMON_SL_H

#include "phy_common.h"

typedef enum SRSLTE_API {
  SRSLTE_SIDELINK_TM1 = 0,
  SRSLTE_SIDELINK_TM2,
  SRSLTE_SIDELINK_TM3,
  SRSLTE_SIDELINK_TM4
} srslte_sl_tm_t;

typedef enum SRSLTE_API {
  SRSLTE_SIDELINK_PSBCH = 0,
  SRSLTE_SIDELINK_PSCCH,
  SRSLTE_SIDELINK_PSSCH,
  SRSLTE_SIDELINK_PSDCH
} srslte_sl_channels_t;

typedef struct SRSLTE_API {
  srslte_sl_tm_t tm;
  uint32_t       N_sl_id;
  uint32_t       nof_prb;
  srslte_cp_t    cp;
} srslte_cell_sl_t;

typedef enum SRSLTE_API {
  SRSLTE_SIDELINK_DATA_SYMBOL = 0,
  SRSLTE_SIDELINK_SYNC_SYMBOL,
  SRSLTE_SIDELINK_DMRS_SYMBOL,
  SRSLTE_SIDELINK_GUARD_SYMBOL
} srslte_sl_symbol_t;

#define SRSLTE_PSBCH_NOF_PRB (6)
#define SRSLTE_PSCCH_TM34_NOF_PRB (2)

#define SRSLTE_MIB_SL_LEN (40)     // TM1/2: 40 bits
#define SRSLTE_MIB_SL_V2X_LEN (48) // TM3/4: 48 bits
#define SRSLTE_MIB_SL_MAX_LEN (SRSLTE_MIB_SL_V2X_LEN)

#define SRSLTE_SL_TM12_DEFAULT_NUM_DMRS_SYMBOLS (2)
#define SRSLTE_SL_TM34_DEFAULT_NUM_DMRS_SYMBOLS (4) ///< In TM3/4, all channels have 4 DMRS by default

#define SRSLTE_PSBCH_TM12_NUM_DATA_SYMBOLS (8) ///< SL-BCH is in 8 OFDM symbols (but only 7 are tx'ed)
#define SRSLTE_PSBCH_TM12_NUM_DMRS_SYMBOLS (2) ///< PSBCH has 2 DMRS symbols
#define SRSLTE_PSBCH_TM12_NUM_SYNC_SYMBOLS (4) ///< Two symbols PSSS and two SSSS

#define SRSLTE_PSBCH_TM34_NUM_DATA_SYMBOLS (7) ///< SL-BCH is in 7 OFDM symbols (but only 6 are tx'ed)
#define SRSLTE_PSBCH_TM34_NUM_DMRS_SYMBOLS (3) ///< PSBCH has 3 DMRS symbols in TM3 and TM4
#define SRSLTE_PSBCH_TM34_NUM_SYNC_SYMBOLS (4) ///< Two symbols PSSS and two SSSS

#define SRSLTE_PSCCH_QM 2
#define SRSLTE_PSCCH_TM12_NOF_PRB (1)
#define SRSLTE_PSCCH_TM34_NOF_PRB (2)
#define SRSLTE_PSCCH_MAX_NOF_PRB (SRSLTE_PSCCH_TM34_NOF_PRB)
#define SRSLTE_PSCCH_SCRAMBLING_SEED (510) ///< Scrambling seed for PSCCH is 510

#define SRSLTE_PSCCH_MAX_CODED_BITS (3 * (SRSLTE_SCI_MAX_LEN + SRSLTE_SCI_CRC_LEN))

#define SRSLTE_PSCCH_TM12_NUM_DATA_SYMBOLS (12)
#define SRSLTE_PSCCH_TM12_NUM_DMRS_SYMBOLS (2)
#define SRSLTE_PSCCH_TM12_NUM_DATA_SYMBOLS_EXT (10)

#define SRSLTE_PSCCH_TM34_NUM_DATA_SYMBOLS (10)
#define SRSLTE_PSCCH_TM34_NUM_DMRS_SYMBOLS (4)

#define SRSLTE_PSCCH_MAX_NUM_DATA_SYMBOLS (SRSLTE_PSCCH_TM12_NUM_DATA_SYMBOLS)

SRSLTE_API int srslte_sl_get_num_symbols(srslte_sl_tm_t tm, srslte_cp_t cp);

SRSLTE_API bool srslte_psbch_is_symbol(srslte_sl_symbol_t type, srslte_sl_tm_t tm, uint32_t i);

SRSLTE_API bool srslte_pscch_is_symbol(srslte_sl_symbol_t type, srslte_sl_tm_t tm, uint32_t i, srslte_cp_t cp);

SRSLTE_API uint32_t srslte_sci_format0_sizeof(uint32_t nof_prb);

#endif // SRSLTE_PHY_COMMON_SL_H
