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

/******************************************************************************
 * File:        enb_phy_base.h
 * Description: Base class for all eNB PHYs.
 *****************************************************************************/

#ifndef SRSENB_PHY_BASE_H
#define SRSENB_PHY_BASE_H

#include "srsue/hdr/phy/phy_metrics.h"

namespace srsenb {

class enb_phy_base
{
public:
  enb_phy_base(){};
  virtual ~enb_phy_base(){};

  virtual std::string get_type() = 0;

  virtual void stop() = 0;

  virtual void start_plot() = 0;

  virtual void get_metrics(phy_metrics_t* m) = 0;
};

} // namespace srsenb

#endif // SRSENB_PHY_BASE_H
