
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

#ifndef SRSRAN_COMMON_NR_H
#define SRSRAN_COMMON_NR_H

#include <array>
#include <cstdint>

namespace srsran {

// Radio Bearers
enum class nr_srb { srb0, srb1, srb2, srb3, count };
const uint32_t MAX_NR_SRB_ID = 3;
enum class nr_drb {
  drb1 = 1,
  drb2,
  drb3,
  drb4,
  drb5,
  drb6,
  drb7,
  drb8,
  drb9,
  drb10,
  drb11,
  drb12,
  drb13,
  drb14,
  drb15,
  drb16,
  drb17,
  drb18,
  drb19,
  drb20,
  drb21,
  drb22,
  drb23,
  drb24,
  drb25,
  drb26,
  drb27,
  drb28,
  drb29,
  invalid
};
const uint32_t MAX_NR_DRB_ID      = 29;
const uint32_t MAX_NR_NOF_BEARERS = MAX_NR_DRB_ID + MAX_NR_SRB_ID;

constexpr bool is_nr_lcid(uint32_t lcid)
{
  return lcid < MAX_NR_NOF_BEARERS;
}
constexpr bool is_nr_srb(uint32_t srib)
{
  return srib <= MAX_NR_SRB_ID;
}
inline const char* get_srb_name(nr_srb srb_id)
{
  static const char* names[] = {"SRB0", "SRB1", "SRB2", "SRB3", "invalid SRB id"};
  return names[(uint32_t)(srb_id < nr_srb::count ? srb_id : nr_srb::count)];
}
constexpr uint32_t srb_to_lcid(nr_srb srb_id)
{
  return static_cast<uint32_t>(srb_id);
}
constexpr nr_srb nr_lcid_to_srb(uint32_t lcid)
{
  return static_cast<nr_srb>(lcid);
}
constexpr nr_drb nr_drb_id_to_drb(uint32_t drb_id)
{
  return static_cast<nr_drb>(drb_id);
}
constexpr bool is_nr_drb(uint32_t drib)
{
  return drib > MAX_NR_SRB_ID and is_nr_lcid(drib);
}
inline const char* get_drb_name(nr_drb drb_id)
{
  static const char* names[] = {"DRB1",  "DRB2",  "DRB3",  "DRB4",  "DRB5",  "DRB6",          "DRB7",  "DRB8",
                                "DRB9",  "DRB10", "DRB11", "DRB12", "DRB13", "DRB14",         "DRB15", "DRB16",
                                "DRB17", "DRB18", "DRB19", "DRB20", "DRB21", "DRB22",         "DRB23", "DRB24",
                                "DRB25", "DRB26", "DRB27", "DRB28", "DRB29", "invalid DRB id"};
  return names[(uint32_t)(drb_id < nr_drb::invalid ? drb_id : nr_drb::invalid) - 1];
}
} // namespace srsran

#endif // SRSRAN_COMMON_NR_H