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

#ifndef SRSLTE_MIB_SL_H
#define SRSLTE_MIB_SL_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common_sl.h"

/**
 * Master information block - Sidelink (MIB-SL and MIB-SL-V2X).
 *
 * \brief MIB-SL packing/unpacking functions to convert between bit streams
 *
 * Reference:  3GPP TS 36.331 version 15.6.0 Release 15 Sec. 6.5
 */
typedef struct SRSLTE_API {

  uint32_t mib_sl_len;

  // sl-Bandwidth-r12 enum {6, 15, 25, 50, 75, 100} (3 bits)
  uint32_t sl_bandwidth_r12;

  // tdd-ConfigSL-r12 (3 bits)
  uint32_t tdd_config_sl_r12;

  // directFrameNumber-r12 (10 bits)
  uint32_t direct_frame_number_r12;

  // directSubframeNumber-r12 (4 bits)
  uint32_t direct_subframe_number_r12;

  // inCoverage-r12 (1 bit)
  bool in_coverage_r12;

  // reserved-r12
  // TM2: (19 bits)
  // TM4: (27 bits)

} srslte_mib_sl_t;

static const int srslte_mib_sl_bandwith_to_prb[6] = {6, 15, 25, 50, 75, 100};

SRSLTE_API int srslte_mib_sl_init(srslte_mib_sl_t* q, srslte_sl_tm_t tm);

SRSLTE_API int srslte_mib_sl_set(srslte_mib_sl_t* q,
                                 uint32_t         nof_prb,
                                 uint32_t         tdd_config,
                                 uint32_t         direct_frame_number,
                                 uint32_t         direct_subframe_number,
                                 bool             in_coverage);

SRSLTE_API void srslte_mib_sl_pack(srslte_mib_sl_t* q, uint8_t* msg);

SRSLTE_API void srslte_mib_sl_unpack(srslte_mib_sl_t* q, uint8_t* msg);

SRSLTE_API void srslte_mib_sl_free(srslte_mib_sl_t* q);

SRSLTE_API void srslte_mib_sl_printf(FILE* f, srslte_mib_sl_t* q);

#endif // SRSLTE_MIB_SL_H
