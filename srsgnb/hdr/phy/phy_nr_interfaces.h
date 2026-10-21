// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
