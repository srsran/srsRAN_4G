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

#include <string.h>

#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio.h"
#include "srslte/srslte.h"

namespace srsue {

class prach
{
public:
  prach()
  {
    bzero(&prach_obj, sizeof(srslte_prach_t));
    bzero(&cell, sizeof(srslte_cell_t));
    bzero(&cfo_h, sizeof(srslte_cfo_t));

    transmitted_tti  = 0;
    target_power_dbm = 0;
    mem_initiated    = false;
    cell_initiated   = false;
    signal_buffer    = NULL;
  }
  ~prach();
  void  init(uint32_t max_prb, srslte::log* log_h);
  void  stop();
  bool  set_cell(srslte_cell_t cell, srslte_prach_cfg_t prach_cfg);
  bool  prepare_to_send(uint32_t preamble_idx, int allowed_subframe = -1, float target_power_dbm = -1);
  bool  is_ready_to_send(uint32_t current_tti);
  bool  is_pending();
  cf_t* generate(float cfo, uint32_t* nof_sf, float* target_power = NULL);

  phy_interface_mac_lte::prach_info_t get_info();

private:
  const static int MAX_LEN_SF = 3;

  srslte::log*   log_h;
  int            preamble_idx;
  int            allowed_subframe;
  bool           mem_initiated;
  bool           cell_initiated;
  uint32_t       len;
  cf_t*          buffer[12][64];
  srslte_prach_t prach_obj;
  int            transmitted_tti;
  srslte_cell_t  cell;
  cf_t*          signal_buffer;
  srslte_cfo_t   cfo_h;
  float          target_power_dbm;
};

} // namespace srsue

#endif // SRSUE_PRACH_H
