/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
