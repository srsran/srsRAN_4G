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

/**
 *  \brief Common parameters and lookup functions for Sidelink PHY
 */

#include "srslte/phy/common/phy_common.h"

typedef enum SRSLTE_API {
  SRSLTE_SIDELINK_TM1 = 0,
  SRSLTE_SIDELINK_TM2,
  SRSLTE_SIDELINK_TM3,
  SRSLTE_SIDELINK_TM4
} srslte_sl_tm_t;

typedef enum SRSLTE_API {
  SRSLTE_SIDELINK_PSBCH = 0, // Physical Sidelink Broadcast Channel
  SRSLTE_SIDELINK_PSCCH,     // Physical Sidelink Control Channel
  SRSLTE_SIDELINK_PSSCH,     // Physical Sidelink Shared Channel
  SRSLTE_SIDELINK_PSDCH      // Physical Sidelink Discovery Channel
} srslte_sl_channels_t;

typedef struct SRSLTE_API {
  srslte_sl_tm_t tm;
  uint32_t       N_sl_id;
  uint32_t       nof_prb;
  srslte_cp_t    cp;
} srslte_cell_sl_t;

#define SRSLTE_SL_MAX_PERIOD_LENGTH 320 // SL-PeriodComm-r12 3GPP TS 36.331 Section 6.3.8
// SL-CommResourcePool: 3GPP TS 36.331 version 15.6.0 Release 15 Section 6.3.8
typedef struct SRSLTE_API {
  uint32_t period_length;

  uint32_t prb_num;
  uint32_t prb_start;
  uint32_t prb_end;

  uint8_t pscch_sf_bitmap[SRSLTE_SL_MAX_PERIOD_LENGTH];
  uint8_t pssch_sf_bitmap[SRSLTE_SL_MAX_PERIOD_LENGTH];

  uint32_t size_sub_channel;      // sizeSubchannel-r14
  uint32_t num_sub_channel;       // numSubchannel-r14
  uint32_t start_prb_sub_channel; // startRB-Subchannel-r14 offset
  bool     adjacency_pscch_pssch; // adjacencyPSCCH-PSSCH-r14

  uint32_t sf_bitmap_tm34_len;
  uint8_t  sf_bitmap_tm34[SRSLTE_SL_MAX_PERIOD_LENGTH]; // sl_Subframe_r14: 3GPP 36.331 Section 6.3.8
} srslte_sl_comm_resource_pool_t;

typedef enum SRSLTE_API {
  SRSLTE_SIDELINK_DATA_SYMBOL = 0,
  SRSLTE_SIDELINK_SYNC_SYMBOL,
  SRSLTE_SIDELINK_DMRS_SYMBOL,
  SRSLTE_SIDELINK_GUARD_SYMBOL
} srslte_sl_symbol_t;

#define SRSLTE_SL_DUPLEX_MODE_FDD (1)
#define SRSLTE_SL_DUPLEX_MODE_TDD (2)

#define SRSLTE_SLSS_SIDE_PEAK_OFFSET (0.005f)
#define SRSLTE_SLSS_SIDE_PEAK_THRESHOLD_HIGH (0.49f) // square(0.7), max 70% of main peak
#define SRSLTE_SLSS_SIDE_PEAK_THRESHOLD_LOW (0.09f)  // square(0.3), min 30% of main peak

#define SRSLTE_PSBCH_NOF_PRB (6)
#define SRSLTE_PSCCH_TM34_NOF_PRB (2)

#define SRSLTE_MIB_SL_LEN (40)     // TM1/2: 40 bits
#define SRSLTE_MIB_SL_V2X_LEN (48) // TM3/4: 48 bits
#define SRSLTE_MIB_SL_MAX_LEN (SRSLTE_MIB_SL_V2X_LEN)

#define SRSLTE_SL_TM12_DEFAULT_NUM_DMRS_SYMBOLS (2)
#define SRSLTE_SL_TM34_DEFAULT_NUM_DMRS_SYMBOLS (4) ///< In TM3/4, all channels have 4 DMRS by default

#define SRSLTE_PSBCH_TM12_NUM_DATA_SYMBOLS (8) // PSBCH is in 8 OFDM symbols (but only 7 are tx'ed)
#define SRSLTE_PSBCH_TM12_NUM_DATA_SYMBOLS_EXT                                                                         \
  (6) // PSBCH is in 7 OFDM symbols for extended cyclic prefix (but only 6 are tx'ed)
#define SRSLTE_PSBCH_TM12_NUM_DMRS_SYMBOLS (2) ///< PSBCH has 2 DMRS symbols
#define SRSLTE_PSBCH_TM12_NUM_SYNC_SYMBOLS (4) ///< Two symbols PSSS and two SSSS

#define SRSLTE_PSBCH_TM34_NUM_DATA_SYMBOLS (7) ///< SL-BCH is in 7 OFDM symbols (but only 6 are tx'ed)
#define SRSLTE_PSBCH_TM34_NUM_DMRS_SYMBOLS (3) ///< PSBCH has 3 DMRS symbols in TM3 and TM4
#define SRSLTE_PSBCH_TM34_NUM_SYNC_SYMBOLS (4) ///< Two symbols PSSS and two SSSS

#define SRSLTE_SCI_CRC_LEN (16)
#define SRSLTE_SCI_MAX_LEN (45)
#define SRSLTE_SCI_TM34_LEN (32)

#define SRSLTE_PSCCH_QM 2
#define SRSLTE_PSCCH_TM12_NOF_PRB (1)
#define SRSLTE_PSCCH_TM34_NOF_PRB (2)
#define SRSLTE_PSCCH_MAX_NOF_PRB (SRSLTE_PSCCH_TM34_NOF_PRB)
#define SRSLTE_PSCCH_SCRAMBLING_SEED (510) ///< Scrambling seed for PSCCH is 510

#define SRSLTE_PSCCH_TM12_NUM_DATA_SYMBOLS (12)
#define SRSLTE_PSCCH_TM12_NUM_DMRS_SYMBOLS (2)
#define SRSLTE_PSCCH_TM12_NUM_DATA_SYMBOLS_EXT (10)

#define SRSLTE_PSCCH_TM34_NUM_DATA_SYMBOLS (10)
#define SRSLTE_PSCCH_TM34_NUM_DMRS_SYMBOLS (4)

#define SRSLTE_PSCCH_TM12_NOF_CODED_BITS                                                                               \
  (SRSLTE_NRE * SRSLTE_PSCCH_TM12_NUM_DATA_SYMBOLS * SRSLTE_PSCCH_TM12_NOF_PRB * SRSLTE_PSCCH_QM)
#define SRSLTE_PSCCH_TM34_NOF_CODED_BITS                                                                               \
  (SRSLTE_NRE * SRSLTE_PSCCH_TM34_NUM_DATA_SYMBOLS * SRSLTE_PSCCH_TM34_NOF_PRB * SRSLTE_PSCCH_QM)
#define SRSLTE_PSCCH_MAX_CODED_BITS SRSLTE_MAX(SRSLTE_PSCCH_TM12_NOF_CODED_BITS, SRSLTE_PSCCH_TM34_NOF_CODED_BITS)

#define SRSLTE_PSSCH_MAX_QM 6
#define SRSLTE_PSSCH_CRC_LEN 24
#define SRSLTE_MAX_CODEWORD_LEN 168000   // 12 subcarriers * 100 PRB * 14 symbols * 10 bits, assuming 1024QAM
#define SRSLTE_SL_SCH_MAX_TB_LEN (48936) // 3GPP 36.306 v15.4.0 Table 4.1B-1
#define SRSLTE_PSSCH_MAX_CODED_BITS (3 * SRSLTE_TCOD_MAX_LEN_CB + SRSLTE_TCOD_TOTALTAIL)

#define SRSLTE_PSSCH_TM12_NUM_DATA_SYMBOLS (12) // PSSCH is in 12 OFDM symbols (but only 11 are tx'ed)
#define SRSLTE_PSSCH_TM12_NUM_DMRS_SYMBOLS (2)  // PSSCH has 2 DMRS symbols in TM1 and TM2

#define SRSLTE_PSSCH_TM12_NUM_DATA_SYMBOLS_CP_EXT                                                                      \
  (10) // PSSCH is in 10 OFDM symbols for extended cyclic prefix (but only 9 are tx'ed)
#define SRSLTE_PSSCH_TM12_NUM_DMRS_SYMBOLS_CP_EXT                                                                      \
  (2) // PSSCH has 2 DMRS symbols for extended cyclic prefix in TM1 and TM2

#define SRSLTE_PSSCH_TM34_NUM_DATA_SYMBOLS (10) // PSSCH is in 10 OFDM symbols (but only 9 are tx'ed)
#define SRSLTE_PSSCH_TM34_NUM_DMRS_SYMBOLS (4)  // PSSCH has 4 DMRS symbols in TM3 and TM4

SRSLTE_API int srslte_sl_group_hopping_f_gh(uint32_t f_gh[SRSLTE_NSLOTS_X_FRAME * 2], uint32_t N_x_id);
#define SRSLTE_PSCCH_MAX_NUM_DATA_SYMBOLS (SRSLTE_PSCCH_TM12_NUM_DATA_SYMBOLS)

SRSLTE_API bool srslte_slss_side_peak_pos_is_valid(uint32_t side_peak_pos,
                                                   uint32_t main_peak_pos,
                                                   uint32_t side_peak_delta_a,
                                                   uint32_t side_peak_delta_b);
SRSLTE_API bool srslte_slss_side_peak_value_is_valid(float side_peak_value, float threshold_low, float threshold_high);

SRSLTE_API int srslte_sl_tm_to_cell_sl_tm_t(srslte_cell_sl_t* q, uint32_t tm);

SRSLTE_API uint32_t srslte_sl_get_num_symbols(srslte_sl_tm_t tm, srslte_cp_t cp);
SRSLTE_API bool     srslte_psbch_is_symbol(srslte_sl_symbol_t type, srslte_sl_tm_t tm, uint32_t i, srslte_cp_t cp);
SRSLTE_API bool     srslte_pscch_is_symbol(srslte_sl_symbol_t type, srslte_sl_tm_t tm, uint32_t i, srslte_cp_t cp);
SRSLTE_API bool     srslte_pssch_is_symbol(srslte_sl_symbol_t type, srslte_sl_tm_t tm, uint32_t i, srslte_cp_t cp);

SRSLTE_API uint32_t srslte_sci_format0_sizeof(uint32_t nof_prb);

SRSLTE_API int srslte_sl_comm_resource_pool_get_default_config(srslte_sl_comm_resource_pool_t* q,
                                                               srslte_cell_sl_t                cell);

#endif // SRSLTE_PHY_COMMON_SL_H