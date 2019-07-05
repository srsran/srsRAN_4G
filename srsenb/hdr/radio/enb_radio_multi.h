
/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
 * File:        enb_radio_multi.h
 * Description: Header-only class for eNB to set DL/UL freq during init
 *****************************************************************************/

#ifndef SRSENB_RADIO_MULTI_H
#define SRSENB_RADIO_MULTI_H

#include "srslte/common/logger.h"
#include "srslte/interfaces/common_interfaces.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/radio/radio_base.h"
#include "srslte/radio/radio_metrics.h"
#include "srslte/radio/radio_multi.h"

namespace srsenb {

class enb_radio_multi : public srslte::radio_multi
{
public:
  enb_radio_multi(srslte::logger* logger_);
  ~enb_radio_multi() override;
  int init(const srslte::rf_args_t& args_, srslte::phy_interface_radio* phy_);
};
} // namespace srsenb

#endif // SRSENB_RADIO_MULTI_H
