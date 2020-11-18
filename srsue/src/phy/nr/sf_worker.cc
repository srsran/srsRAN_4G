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

#include "srsue/hdr/phy/nr/sf_worker.h"

namespace srsue {
namespace nr {
sf_worker::sf_worker(phy_common* phy_, phy_nr_state* phy_state_, srslte::log* log) :
  phy(phy_), phy_state(phy_state_), log_h(log)
{
  for (uint32_t i = 0; i < phy_state->args.nof_carriers; i++) {
    cc_worker* w = new cc_worker(i, log, phy_state);
    cc_workers.push_back(std::unique_ptr<cc_worker>(w));
  }
}

bool sf_worker::set_carrier_unlocked(uint32_t cc_idx, const srslte_carrier_nr_t* carrier_)
{
  if (cc_idx >= cc_workers.size()) {
    return false;
  }

  return cc_workers.at(cc_idx)->set_carrier(carrier_);
}

cf_t* sf_worker::get_buffer(uint32_t cc_idx, uint32_t antenna_idx)
{
  if (cc_idx >= cc_workers.size()) {
    return nullptr;
  }

  return cc_workers.at(cc_idx)->get_rx_buffer(antenna_idx);
}

uint32_t sf_worker::get_buffer_len()
{
  return cc_workers.at(0)->get_buffer_len();
}

void sf_worker::set_tti(uint32_t tti)
{
  for (auto& w : cc_workers) {
    w->set_tti(tti);
  }
}

void sf_worker::work_imp()
{
  for (auto& w : cc_workers) {
    w->work_dl();
  }
}

}; // namespace nr
}; // namespace srsue