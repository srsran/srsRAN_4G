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

#include "srsenb/hdr/radio/enb_radio_multi.h"
#include <mutex>

namespace srsenb {

enb_radio_multi::enb_radio_multi(srslte::logger* logger_) : radio_multi(logger_) {}

enb_radio_multi::~enb_radio_multi() {}

int enb_radio_multi::init(const srslte::rf_args_t& args_, srslte::phy_interface_radio* phy_)
{
  int ret = radio_multi::init(args_, phy_);

  if (ret == SRSLTE_SUCCESS) {
    ret = SRSLTE_ERROR;
    if (radios.size() > 0) {
      log.console("Setting frequency: DL=%.1f Mhz, UL=%.1f MHz\n", args_.dl_freq / 1e6, args_.ul_freq / 1e6);
      radios.at(0)->set_tx_freq(args.nof_tx_ports, args.dl_freq);
      radios.at(0)->set_rx_freq(args.nof_tx_ports, args.ul_freq);
      ret = SRSLTE_SUCCESS;
    }
  }

  return ret;
}

} // namespace srsenb
