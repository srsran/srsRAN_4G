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

#ifndef SRSUE_UL_SPS_H
#define SRSUE_UL_SPS_H

#include "srslte/common/log.h"
#include "srslte/common/timers.h"

/* Uplink Semi-Persistent schedulign (Section 5.10.2) */

namespace srsue {

typedef _Complex float cf_t;

class ul_sps
{
public:
  void clear() {}
  void reset(uint32_t tti) {}
  bool get_pending_grant(uint32_t tti, mac_interface_phy_lte::mac_grant_ul_t* grant) { return false; }

private:
};

} // namespace srsue

#endif // SRSUE_UL_SPS_H
