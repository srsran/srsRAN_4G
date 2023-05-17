/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSUE_PROC_SR_NR_H
#define SRSUE_PROC_SR_NR_H

#include "srsran/interfaces/ue_mac_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/mac_nr/mac_nr_interfaces.h"
#include <mutex>
#include <stdint.h>

/// Scheduling Request procedure as defined in 5.4.4 of 38.321
/// Note: currently only a single SR config for all logical channels is supported

namespace srsue {

class proc_ra_nr;
class rrc_interface_mac;

class proc_sr_nr
{
public:
  explicit proc_sr_nr(srslog::basic_logger& logger);
  int32_t init(mac_interface_sr_nr* mac_, phy_interface_mac_nr* phy_, rrc_interface_mac* rrc_);
  void    step(uint32_t tti);
  int32_t set_config(const srsran::sr_cfg_nr_t& cfg);
  void    reset();
  void    start();
  bool    sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx);

private:
  void reset_nolock();
  int  sr_counter    = 0;
  bool is_pending_sr = false;

  srsran::sr_cfg_nr_t cfg = {};

  mac_interface_sr_nr*  mac = nullptr;
  rrc_interface_mac*    rrc = nullptr;
  phy_interface_mac_nr* phy = nullptr;
  srslog::basic_logger& logger;

  bool       initiated = false;
  std::mutex mutex;
};

} // namespace srsue

#endif // SRSUE_PROC_SR_H
