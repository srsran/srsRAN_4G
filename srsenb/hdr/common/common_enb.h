/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSENB_COMMON_ENB_H
#define SRSENB_COMMON_ENB_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "srsran/adt/circular_map.h"
#include "srsran/common/common_lte.h"
#include <stdint.h>

namespace srsenb {

#define SRSENB_RRC_MAX_N_PLMN_IDENTITIES 6

#define SRSENB_N_SRB 3
#define SRSENB_MAX_UES 64
const uint32_t MAX_ERAB_ID   = 15;
const uint32_t MAX_NOF_ERABS = 16;

using srsran::lte_drb;
using srsran::lte_srb;
using srsran::srb_to_lcid;
inline const char* get_rb_name(uint32_t lcid)
{
  return (srsran::is_lte_srb(lcid)) ? srsran::get_srb_name(srsran::lte_lcid_to_srb(lcid))
                                    : srsran::get_drb_name(static_cast<srsran::lte_drb>(lcid - srsran::MAX_LTE_SRB_ID));
}
constexpr uint32_t drb_to_lcid(lte_drb drb_id)
{
  return srb_to_lcid(lte_srb::srb2) + static_cast<uint32_t>(drb_id);
}
constexpr lte_drb lte_lcid_to_drb(uint32_t lcid)
{
  return srsran::is_lte_drb(lcid) ? static_cast<lte_drb>(lcid - srb_to_lcid(lte_srb::srb2)) : lte_drb::invalid;
}

// Cat 3 UE - Max number of DL-SCH transport block bits received within a TTI
// 3GPP 36.306 Table 4.1.1
#define SRSENB_MAX_BUFFER_SIZE_BITS 102048
#define SRSENB_MAX_BUFFER_SIZE_BYTES 12756
#define SRSENB_BUFFER_HEADER_OFFSET 1024

/// Typedef of circular map container which key corresponding to rnti value and that can be used across layers
template <typename UEObject>
using rnti_map_t = srsran::static_circular_map<uint16_t, UEObject, SRSENB_MAX_UES>;

} // namespace srsenb

#endif // SRSENB_COMMON_ENB_H
