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

#ifndef SRSUE_UE_STACK_NR_H
#define SRSUE_UE_STACK_NR_H

#include <functional>
#include <pthread.h>
#include <stdarg.h>
#include <string>

#include "mac/mac_nr.h"
#include "rrc/rrc_nr.h"
#include "srslte/radio/radio.h"
#include "srslte/upper/pdcp.h"
#include "srslte/upper/rlc.h"
#include "upper/nas.h"
#include "upper/usim.h"

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/mac_nr_pcap.h"
#include "srslte/common/multiqueue.h"
#include "srslte/common/thread_pool.h"
#include "srslte/interfaces/ue_nr_interfaces.h"

#include "srsue/hdr/ue_metrics_interface.h"
#include "ue_stack_base.h"

namespace srsue {

/** \brief L2/L3 stack class for 5G/NR UEs.
 *
 *  This class wraps all L2/L3 blocks and provides a single interface towards the PHY.
 */

class ue_stack_nr final : public ue_stack_base,
                          public stack_interface_phy_nr,
                          public stack_interface_gw,
                          public stack_interface_rrc,
                          public srslte::thread
{
public:
  ue_stack_nr(srslte::logger* logger_);
  ~ue_stack_nr();

  std::string get_type() final;

  int  init(const stack_args_t& args_);
  int  init(const stack_args_t& args_, phy_interface_stack_nr* phy_, gw_interface_stack* gw_);
  bool switch_on() final;
  bool switch_off() final;
  void stop();

  bool get_metrics(stack_metrics_t* metrics);
  bool is_rrc_connected();

  // RRC interface for PHY
  void in_sync() final;
  void out_of_sync() final;
  void run_tti(uint32_t tti) final;

  // MAC interface for PHY
  int sf_indication(const uint32_t tti)
  {
    run_tti(tti);
    return SRSLTE_SUCCESS;
  }
  void tb_decoded(const uint32_t cc_idx, mac_nr_grant_dl_t& grant) final { mac->tb_decoded(cc_idx, grant); }
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant) final { mac->new_grant_ul(cc_idx, grant); }

  // Interface for GW
  void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking) final;
  bool is_lcid_enabled(uint32_t lcid) final { return pdcp->is_lcid_enabled(lcid); }

  // Interface for RRC
  void              start_cell_search() final;
  void              start_cell_select(const phy_interface_rrc_lte::phy_cell_t* cell) final;
  srslte::tti_point get_current_tti() { return srslte::tti_point{0}; };

  // Task Handling interface
  srslte::timer_handler::unique_timer    get_unique_timer() final { return timers.get_unique_timer(); }
  srslte::task_multiqueue::queue_handler make_task_queue() final { return pending_tasks.get_queue_handler(); }
  srslte::task_multiqueue::queue_handler make_task_queue(uint32_t qsize) final
  {
    return pending_tasks.get_queue_handler(qsize);
  }
  void enqueue_background_task(std::function<void(uint32_t)> f) final;
  void notify_background_task_result(srslte::move_task_t task) final;
  void defer_callback(uint32_t duration_ms, std::function<void()> func) final;
  void defer_task(srslte::move_task_t task) final;

private:
  void run_thread() final;
  void run_tti_impl(uint32_t tti);
  void stop_impl();

  bool                running = false;
  srsue::stack_args_t args    = {};

  // timers
  srslte::timer_handler timers;

  // UE stack logging
  srslte::logger* logger = nullptr;
  srslte::log_ref rlc_log;
  srslte::log_ref pdcp_log;
  srslte::log_ref pool_log;

  // stack components
  std::unique_ptr<mac_nr>       mac;
  std::unique_ptr<rrc_nr>       rrc;
  std::unique_ptr<srslte::rlc>  rlc;
  std::unique_ptr<srslte::pdcp> pdcp;

  std::unique_ptr<srslte::mac_nr_pcap> mac_pcap;

  // RAT-specific interfaces
  phy_interface_stack_nr* phy = nullptr;
  gw_interface_stack*     gw  = nullptr;

  // Thread
  static const int STACK_MAIN_THREAD_PRIO = 4;

  srslte::task_multiqueue pending_tasks;
  int sync_queue_id = -1, ue_queue_id = -1, gw_queue_id = -1, mac_queue_id = -1, background_queue_id = -1;
  srslte::task_thread_pool         background_tasks;     ///< Thread pool used for long, low-priority tasks
  std::vector<srslte::move_task_t> deferred_stack_tasks; ///< enqueues stack tasks from within. Avoids locking
};

} // namespace srsue

#endif // SRSUE_UE_STACK_NR_H
