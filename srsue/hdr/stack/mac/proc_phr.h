/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSUE_PROC_PHR_H
#define SRSUE_PROC_PHR_H

#include "srslte/common/task_scheduler.h"
#include "srslte/interfaces/ue_mac_interfaces.h"
#include "srslte/srslog/srslog.h"
#include <stdint.h>

/* Power headroom report procedure */

namespace srsue {

class phy_interface_mac_lte;

class phr_proc : public srslte::timer_callback
{
public:
  explicit phr_proc(srslog::basic_logger& logger);
  void init(phy_interface_mac_lte* phy_h, srslte::ext_task_sched_handle* task_sched_);
  void set_config(srslte::phr_cfg_t& cfg);
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
