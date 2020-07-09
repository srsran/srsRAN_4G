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

#ifndef SRSUE_DUMMY_CLASSES_H
#define SRSUE_DUMMY_CLASSES_H

#include "srslte/common/task_scheduler.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {

class stack_test_dummy : public stack_interface_rrc
{
public:
  stack_test_dummy() {}

  srslte::timer_handler::unique_timer get_unique_timer() override { return task_sched.get_unique_timer(); }
  void                                start_cell_search() override {}
  void                                start_cell_select(const phy_interface_rrc_lte::phy_cell_t* cell) override {}
  srslte::tti_point                   get_current_tti() override
  {
    return srslte::tti_point{task_sched.get_timer_handler()->get_cur_time() % 10240};
  }
  srslte::task_multiqueue::queue_handle make_task_queue() final { return task_sched.make_task_queue(); }
  void                                  enqueue_background_task(std::function<void(uint32_t)> f) override
  {
    task_sched.enqueue_background_task(std::move(f));
  }
  void notify_background_task_result(srslte::move_task_t task) override { task(); }
  void defer_callback(uint32_t duration_ms, std::function<void()> func) final
  {
    task_sched.defer_callback(duration_ms, std::move(func));
  }
  void defer_task(srslte::move_task_t task) final { task_sched.defer_task(std::move(task)); }

  // Testing utility functions
  void run_tti()
  {
    // update clock and run internal tasks
    task_sched.tic();

    task_sched.run_pending_tasks();
  }

  // run pending tasks without updating timers
  void run_pending_tasks() { task_sched.run_pending_tasks(); }

  srslte::task_scheduler task_sched{512, 0, 100};
};

class rlc_dummy_interface : public rlc_interface_mac
{
public:
  bool     has_data(const uint32_t lcid) override { return false; }
  uint32_t get_buffer_state(const uint32_t lcid) override { return 0; }
  int      read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) override { return 0; }
  void     write_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) override {}
  void     write_pdu_bcch_bch(srslte::unique_byte_buffer_t payload) override {}
  void     write_pdu_bcch_dlsch(uint8_t* payload, uint32_t nof_bytes) override {}
  void     write_pdu_pcch(srslte::unique_byte_buffer_t payload) override {}
  void     write_pdu_mch(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) override {}
};

} // namespace srsue

#endif // SRSUE_DUMMY_CLASSES_H
