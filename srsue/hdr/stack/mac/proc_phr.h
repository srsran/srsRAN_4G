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

#ifndef SRSUE_PROC_PHR_H
#define SRSUE_PROC_PHR_H

#include "srsran/common/task_scheduler.h"
#include "srsran/interfaces/ue_mac_interfaces.h"
#include "srsran/srslog/srslog.h"
#include <stdint.h>

/* Power headroom report procedure */

namespace srsue {

class phy_interface_mac_lte;

class phr_proc : public srsran::timer_callback
{
public:
  explicit phr_proc(srslog::basic_logger& logger);
  void init(phy_interface_mac_lte* phy_h, srsran::ext_task_sched_handle* task_sched_);
  void set_config(srsran::phr_cfg_t& cfg);
  void step();
  void reset();

  bool generate_phr_on_ul_grant(float* phr);
  bool is_extended();
  void timer_expired(uint32_t timer_id);

  void start_periodic_timer();

private:
  bool pathloss_changed();

  srslog::basic_logger&          logger;
  phy_interface_mac_lte*         phy_h;
  srsran::ext_task_sched_handle* task_sched;
  srsran::phr_cfg_t              phr_cfg;
  bool                           initiated;
  int                            last_pathloss_db;
  bool                           phr_is_triggered;

  srsran::timer_handler::unique_timer timer_periodic;
  srsran::timer_handler::unique_timer timer_prohibit;

  std::mutex mutex;
};

} // namespace srsue

#endif // SRSUE_PROC_PHR_H
