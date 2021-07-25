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

#ifndef SRSRAN_DUMMY_NR_CLASSES_H
#define SRSRAN_DUMMY_NR_CLASSES_H

#include "srsran/interfaces/gnb_interfaces.h"

namespace srsenb {

class mac_dummy : public mac_interface_rrc_nr
{
public:
  int cell_cfg(srsenb::sched_interface::cell_cfg_t* cell_cfg_)
  {
    cellcfgobj = *cell_cfg_;
    return SRSRAN_SUCCESS;
  }
  uint16_t reserve_rnti() { return 0x4601; }

  srsenb::sched_interface::cell_cfg_t cellcfgobj;
};

} // namespace srsenb

#endif // SRSRAN_DUMMY_NR_CLASSES_H
