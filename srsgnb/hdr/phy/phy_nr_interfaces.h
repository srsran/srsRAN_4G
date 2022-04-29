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

#ifndef SRSRAN_PHY_NR_INTERFACES_H
#define SRSRAN_PHY_NR_INTERFACES_H

#include "srsran/srsran.h"
#include <vector>

namespace srsenb {

struct phy_cell_cfg_nr_t {
  srsran_carrier_nr_t carrier;
  uint32_t            rf_port;
  uint32_t            cell_id;
  float               gain_db;
  bool                  dl_measure;
};

using phy_cell_cfg_list_nr_t = std::vector<phy_cell_cfg_nr_t>;

} // namespace srsenb

#endif // SRSRAN_PHY_NR_INTERFACES_H
