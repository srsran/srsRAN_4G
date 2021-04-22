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

/**
 * @file radio_base.h
 * @brief Base class for all eNB/UE radios.
 */

#ifndef SRSRAN_RADIO_BASE_H
#define SRSRAN_RADIO_BASE_H

#include "srsran/common/interfaces_common.h"
#include "srsran/radio/radio_metrics.h"

namespace srsran {

class phy_interface_radio;

class radio_base
{
public:
  virtual ~radio_base() = default;

  virtual std::string get_type() = 0;

  virtual int  init(const rf_args_t& args_, phy_interface_radio* phy_) = 0;
  virtual void stop()                                                  = 0;

  virtual bool get_metrics(rf_metrics_t* metrics) = 0;
};

} // namespace srsran

#endif // SRSRAN_RADIO_BASE_H
