/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_LTE_COMMON_H
#define SRSRAN_LTE_COMMON_H

#include <array>
#include <cstdint>

namespace srsran {

// Cell nof PRBs
const std::array<uint32_t, 6> lte_cell_nof_prbs = {6, 15, 25, 50, 75, 100};

// Radio Bearers
enum class lte_srb { srb0, srb1, srb2, count };
const uint32_t MAX_LTE_SRB_ID = 2;
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

enum class lte_drb { drb1 = 1, drb2, drb3, drb4, drb5, drb6, drb7, drb8, drb9, drb10, drb11, invalid };
const uint32_t MAX_LTE_DRB_ID = 11;
constexpr bool is_lte_drb(uint32_t lcid)
{
  return lcid > MAX_LTE_SRB_ID and lcid <= MAX_LTE_DRB_ID + MAX_LTE_SRB_ID;
}
inline const char* get_drb_name(lte_drb drb_id)
{
  static const char* names[] = {
      "DRB1", "DRB2", "DRB3", "DRB4", "DRB5", "DRB6", "DRB7", "DRB8", "DRB9", "DRB10", "DRB11", "invalid DRB id"};
  return names[(uint32_t)(drb_id < lte_drb::invalid ? drb_id : lte_drb::invalid) - 1];
}
constexpr bool is_lte_rb(uint32_t lcid)
{
  return lcid <= MAX_LTE_DRB_ID + MAX_LTE_SRB_ID;
}

} // namespace srsran

#endif // SRSRAN_LTE_COMMON_H
