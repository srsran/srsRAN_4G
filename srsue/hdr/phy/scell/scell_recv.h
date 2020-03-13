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

#ifndef SRSUE_SCELL_RECV_H
#define SRSUE_SCELL_RECV_H

#include "srsue/hdr/phy/phy_common.h"
#include <set>
#include <srslte/srslte.h>

namespace srsue {
namespace scell {

// Class to receive secondary cell
class scell_recv
{
public:
  void               init(srslte::log* log_h, uint32_t max_sf_window);
  void deinit();
  void reset();
  std::set<uint32_t> find_cells(const cf_t* input_buffer, const srslte_cell_t serving_cell, const uint32_t nof_sf);

private:
  // 36.133 9.1.2.1 for band 7
  constexpr static float ABSOLUTE_RSRP_THRESHOLD_DBM = -125;

  cf_t*         sf_buffer[SRSLTE_MAX_PORTS];
  srslte::log*  log_h;
  srslte_sync_t sync_find;

  uint32_t current_fft_sz;
};

} // namespace scell
} // namespace srsue

#endif // SRSUE_SCELL_RECV_H
