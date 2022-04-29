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

#ifndef SRSRAN_GNB_MAC_INTERFACES_H
#define SRSRAN_GNB_MAC_INTERFACES_H

#include "srsgnb/hdr/stack/mac/sched_nr_interface.h"
#include "srsran/interfaces/enb_mac_interfaces.h"

namespace srsenb {

class mac_interface_rrc_nr
{
public:
  // Provides cell configuration including SIB periodicity, etc.
  virtual int cell_cfg(const std::vector<sched_nr_cell_cfg_t>& nr_cells) = 0;

  /// Allocates a new user/RNTI at MAC. Returns RNTI on success or SRSRAN_INVALID_RNTI otherwise.
  virtual uint16_t reserve_rnti(uint32_t enb_cc_idx, const sched_nr_interface::ue_cfg_t& uecfg) = 0;

  virtual int ue_cfg(uint16_t rnti, const sched_nr_interface::ue_cfg_t& ue_cfg) = 0;

  virtual int remove_ue(uint16_t rnti) = 0;
};

// NR interface is identical to EUTRA interface
class mac_interface_rlc_nr : public mac_interface_rlc
{};

} // namespace srsenb

#endif // SRSRAN_GNB_MAC_INTERFACES_H
