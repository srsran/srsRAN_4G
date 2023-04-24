/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_UE_SL_MIB_H
#define SRSRAN_UE_SL_MIB_H

#include <stdbool.h>
#include <stdint.h>

#define SRSRAN_MIB_SL_MAX_LEN 40

// 3GPP TS 36.331 Sec. 6.5.2
typedef struct SRSRAN_API {

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

  // reserved-r12 (19 bits)

} srsran_ue_mib_sl_t;

int srsran_ue_mib_sl_set(srsran_ue_mib_sl_t* q,
                         uint32_t            nof_prb,
                         uint32_t            tdd_config,
                         uint32_t            direct_frame_number,
                         uint32_t            direct_subframe_number,
                         bool                in_coverage);

void srsran_ue_mib_sl_pack(srsran_ue_mib_sl_t* q, uint8_t* msg);
void srlste_ue_mib_sl_unpack(srsran_ue_mib_sl_t* q, uint8_t* msg);

void srsran_ue_mib_sl_free(srsran_ue_mib_sl_t* q);

#endif // SRSRAN_UE_SL_MIB_H