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
 * File:        ue_nr_phy_base.h
 * Description: Base class for UE NR PHYs.
 *****************************************************************************/

#ifndef SRSUE_UE_NR_PHY_BASE_H
#define SRSUE_UE_NR_PHY_BASE_H

#include "srslte/common/log_filter.h"
#include "srslte/interfaces/radio_interfaces.h"
#include "srslte/interfaces/ue_nr_interfaces.h"
#include "srsue/hdr/phy/ue_phy_base.h"

namespace srsue {

class ue_phy_nr_base : public ue_phy_base, public phy_interface_stack_nr, public srslte::phy_interface_radio
{
public:
  ue_phy_nr_base(){};
  virtual ~ue_phy_nr_base() {}

  virtual std::string get_type() = 0;

  virtual int  init(const phy_args_t& args_)                                                                       = 0;
  virtual int  init(const phy_args_t& args_, stack_interface_phy_lte* stack_, srslte::radio_interface_phy* radio_) = 0;
  virtual void stop()                                                                                              = 0;

  virtual void set_earfcn(std::vector<uint32_t> earfcns) = 0;

  virtual void wait_initialize() = 0;
  virtual void start_plot()      = 0;

  virtual void get_metrics(phy_metrics_t* m) = 0;
};

} // namespace srsue

#endif // SRSUE_UE_NR_PHY_BASE_H