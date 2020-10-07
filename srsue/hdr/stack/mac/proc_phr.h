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

#ifndef SRSUE_PROC_PHR_H
#define SRSUE_PROC_PHR_H

#include "srslte/common/logmap.h"
#include "srslte/common/timers.h"
#include "srslte/interfaces/ue_interfaces.h"
#include <stdint.h>

/* Power headroom report procedure */

namespace srsue {

class phr_proc : public srslte::timer_callback
{
public:
  phr_proc();
  void init(phy_interface_mac_lte* phy_h, srslte::log_ref log_h_, srslte::ext_task_sched_handle* task_sched_);
  void set_config(srslte::phr_cfg_t& cfg);
  void step();
  void reset();

  bool generate_phr_on_ul_grant(float* phr);
  bool is_extended();
  void timer_expired(uint32_t timer_id);

  void start_periodic_timer();

private:
  bool pathloss_changed();

  srslte::log_ref                log_h;
  phy_interface_mac_lte*         phy_h;
  srslte::ext_task_sched_handle* task_sched;
  srslte::phr_cfg_t              phr_cfg;
  bool                           initiated;
  int                            last_pathloss_db;
  bool                           phr_is_triggered;

  srslte::timer_handler::unique_timer timer_periodic;
  srslte::timer_handler::unique_timer timer_prohibit;
};

} // namespace srsue

#endif // SRSUE_PROC_PHR_H
