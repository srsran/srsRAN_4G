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

#ifndef SRSRAN_COMMON_LTE_H
#define SRSRAN_COMMON_LTE_H

#include <array>
#include <cstdint>

namespace srsran {

// Cell nof PRBs
const std::array<uint32_t, 6> lte_cell_nof_prbs = {6, 15, 25, 50, 75, 100};
inline uint32_t               lte_cell_nof_prb_to_index(uint32_t nof_prb)
{
  switch (nof_prb) {
    case 6:
      return 0;
    case 15:
      return 1;
    case 25:
      return 2;
    case 50:
      return 3;
    case 75:
      return 4;
    case 100:
      return 5;
    default:
      break;
  }
  return -1;
}
inline bool is_lte_cell_nof_prb(uint32_t nof_prb)
{
  return lte_cell_nof_prb_to_index(nof_prb) < lte_cell_nof_prbs.size();
}

// Radio Bearers
enum class lte_srb { srb0, srb1, srb2, count };
const uint32_t MAX_LTE_SRB_ID = 2;
enum class lte_drb { drb1 = 1, drb2, drb3, drb4, drb5, drb6, drb7, drb8, drb9, drb10, drb11, invalid };
const uint32_t MAX_LTE_DRB_ID        = 11;
const uint32_t MAX_LTE_LCID          = 10; // logicalChannelIdentity 3..10 in TS 36.331 v15.3
const uint32_t MAX_EPS_BEARER_ID     = 15; // EPS Bearer ID range [5, 15] in 36 413
const uint32_t MIN_EPS_BEARER_ID     = 5;
const uint32_t INVALID_LCID          = 99; // random invalid LCID
const uint32_t INVALID_EPS_BEARER_ID = 99; // random invalid eps bearer id

constexpr bool is_eps_bearer_id(uint32_t eps_bearer_id)
{
  return  eps_bearer_id >= MIN_EPS_BEARER_ID and eps_bearer_id <= MAX_EPS_BEARER_ID;
}

constexpr bool is_lte_rb(uint32_t lcid)
{
  return lcid <= MAX_LTE_LCID;
}

constexpr bool is_lte_srb(uint32_t lcid)
{
  return lcid <= MAX_LTE_SRB_ID;
}
inline const char* get_srb_name(lte_srb srb_id)
{
  static const char* names[] = {"SRB0", "SRB1", "SRB2", "invalid SRB id"};
  return names[(uint32_t)(srb_id < lte_srb::count ? srb_id : lte_srb::count)];
}
constexpr uint32_t srb_to_lcid(lte_srb srb_id)
{
  return static_cast<uint32_t>(srb_id);
}
constexpr lte_srb lte_lcid_to_srb(uint32_t lcid)
{
  return static_cast<lte_srb>(lcid);
}

constexpr bool is_lte_drb(uint32_t lcid)
{
  return lcid > MAX_LTE_SRB_ID and is_lte_rb(lcid);
}
inline const char* get_drb_name(lte_drb drb_id)
{
  static const char* names[] = {
      "DRB1", "DRB2", "DRB3", "DRB4", "DRB5", "DRB6", "DRB7", "DRB8", "DRB9", "DRB10", "DRB11", "invalid DRB id"};
  return names[(uint32_t)(drb_id < lte_drb::invalid ? drb_id : lte_drb::invalid) - 1];
}

inline const char* get_rb_name(uint32_t lcid)
{
  if (is_lte_srb(lcid)) {
    return get_srb_name(static_cast<lte_srb>(lcid));
  }
  return get_drb_name(static_cast<lte_drb>(lcid - MAX_LTE_SRB_ID));
}

} // namespace srsran

#endif // SRSRAN_COMMON_LTE_H
