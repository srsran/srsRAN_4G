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

#ifndef SRSUE_SCELL_RECV_H
#define SRSUE_SCELL_RECV_H

#include "srsue/hdr/phy/phy_common.h"
#include <set>
#include <srsran/srsran.h>

namespace srsue {
namespace scell {

// Class to receive secondary cell
class scell_recv
{
public:
  explicit scell_recv(srslog::basic_logger& logger) : logger(logger) {}

  void init(uint32_t max_sf_window);
  void deinit();
  void reset();

  /**
   * @brief Find neighbour cells in a given buffer
   * @param input_buffer Provides the baseband samples
   * @param serving_cell Current serving cell
   * @param nof_sf Number of subframes contained in the baseband buffer
   * @param found_cell_ids Provides a set where to insert the found cell identifiers (PCIs)
   */
  void find_cells(const cf_t*          input_buffer,
                  const srsran_cell_t& serving_cell,
                  const uint32_t&      nof_sf,
                  std::set<uint32_t>&  found_cell_ids);

private:
  cf_t*                 sf_buffer[SRSRAN_MAX_PORTS] = {};
  srslog::basic_logger& logger;
  srsran_sync_t         sync_find = {};

  uint32_t current_fft_sz;
};

} // namespace scell
} // namespace srsue

#endif // SRSUE_SCELL_RECV_H
