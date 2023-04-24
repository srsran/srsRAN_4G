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

#ifndef SRSRAN_SCHED_CONFIG_H
#define SRSRAN_SCHED_CONFIG_H

#include <cstdint>

namespace srsenb {

/**
 * Structure used in UE logical channel configuration
 */
struct mac_lc_ch_cfg_t {
  enum direction_t { IDLE = 0, UL, DL, BOTH } direction = IDLE;
  int      priority                                     = 1;    // channel priority (1 is highest)
  uint32_t bsd                                          = 1000; // msec
  uint32_t pbr                                          = -1;   // prioritised bit rate
  int      group                                        = 0;    // logical channel group

  bool is_active() const { return direction != IDLE; }
  bool is_dl() const { return direction == DL or direction == BOTH; }
  bool is_ul() const { return direction == UL or direction == BOTH; }
  bool operator==(const mac_lc_ch_cfg_t& other) const
  {
    return direction == other.direction and priority == other.priority and bsd == other.bsd and pbr == other.pbr and
           group == other.group;
  }
  bool operator!=(const mac_lc_ch_cfg_t& other) const { return not(*this == other); }
};

inline const char* to_string(mac_lc_ch_cfg_t::direction_t dir)
{
  switch (dir) {
    case mac_lc_ch_cfg_t::direction_t::IDLE:
      return "idle";
    case mac_lc_ch_cfg_t::direction_t::BOTH:
      return "bi-dir";
    case mac_lc_ch_cfg_t::direction_t::DL:
      return "DL";
    case mac_lc_ch_cfg_t::direction_t::UL:
      return "UL";
    default:
      return "unrecognized direction";
  }
}

} // namespace srsenb

#endif // SRSRAN_SCHED_CONFIG_H
