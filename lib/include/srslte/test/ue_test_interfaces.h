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

  srslte::tti_point get_current_tti() override
  {
    return srslte::tti_point{task_sched.get_timer_handler()->get_cur_time() % 10240};
  }

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
  bool     has_data_locked(const uint32_t lcid) override { return false; }
  uint32_t get_buffer_state(const uint32_t lcid) override { return 0; }
  int      read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) override { return 0; }
  void     write_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) override {}
  void     write_pdu_bcch_bch(srslte::unique_byte_buffer_t payload) override {}
  void     write_pdu_bcch_dlsch(uint8_t* payload, uint32_t nof_bytes) override {}
  void     write_pdu_pcch(srslte::unique_byte_buffer_t payload) override {}
  void     write_pdu_mch(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) override {}
};

class phy_dummy_interface : public phy_interface_rrc_lte
{
  bool set_config(srslte::phy_cfg_t config, uint32_t cc_idx) override { return true; }
  bool set_scell(srslte_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn) override { return true; }
  void set_config_tdd(srslte_tdd_config_t& tdd_config) override {}
  void set_config_mbsfn_sib2(srslte::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs) override {}
  void set_config_mbsfn_sib13(const srslte::sib13_t& sib13) override {}
  void set_config_mbsfn_mcch(const srslte::mcch_msg_t& mcch) override {}
  void set_activation_deactivation_scell(uint32_t cmd) override {}

  /* Measurements interface */
  void set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci) override {}
  void meas_stop() override {}

  /* Cell search and selection procedures */
  bool cell_search() override { return true; }
  bool cell_select(phy_cell_t cell) override { return true; }
  bool cell_is_camping() override { return false; }

  void enable_pregen_signals(bool enable) override {}
};

} // namespace srsue

#endif // SRSUE_DUMMY_CLASSES_H
