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

#ifndef SRSUE_PRACH_H
#define SRSUE_PRACH_H

#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio.h"
#include "srslte/srslte.h"
#include <bitset>

namespace srsue {

class prach
{
public:
  prach() = default;
  ~prach() { stop(); }

  void  init(uint32_t max_prb, srslte::log* log_h);
  void  stop();
  bool  set_cell(srslte_cell_t cell, srslte_prach_cfg_t prach_cfg);
  void  reset_cfg();
  bool  prepare_to_send(uint32_t preamble_idx, int allowed_subframe = -1, float target_power_dbm = -1);
  bool  is_ready_to_send(uint32_t current_tti, uint32_t current_pci);
  bool  is_pending() const;
  cf_t* generate(float cfo, uint32_t* nof_sf, float* target_power = NULL);

  phy_interface_mac_lte::prach_info_t get_info() const;

private:
  bool generate_buffer(uint32_t f_idx);

  bool is_buffer_generated(uint32_t f_idx, uint32_t preamble_index) const
  {
    return buffer_bitmask.test(f_idx * 64 + preamble_index);
  }

  void set_buffer_as_generated(uint32_t f_idx, uint32_t preamble_index)
  {
    buffer_bitmask.set(f_idx * 64 + preamble_index);
  }

private:
  static constexpr unsigned MAX_LEN_SF    = 3;
  static constexpr unsigned max_fs        = 12;
  static constexpr unsigned max_preambles = 64;

  srslte::log*                                         log_h            = nullptr;
  srslte_prach_t                                       prach_obj        = {};
  srslte_cell_t                                        cell             = {};
  srslte_cfo_t                                         cfo_h            = {};
  srslte_prach_cfg_t                                   cfg              = {};
  std::array<std::array<cf_t*, max_preambles>, max_fs> buffer           = {};
  cf_t*                                                signal_buffer    = nullptr;
  int                                                  preamble_idx     = -1;
  uint32_t                                             len              = 0;
  int                                                  allowed_subframe = 0;
  int                                                  transmitted_tti  = 0;
  float                                                target_power_dbm = 0;
  bool                                                 mem_initiated    = false;
  bool                                                 cell_initiated   = false;
  std::bitset<max_fs * max_preambles>                  buffer_bitmask;
};

} // namespace srsue

#endif // SRSUE_PRACH_H
