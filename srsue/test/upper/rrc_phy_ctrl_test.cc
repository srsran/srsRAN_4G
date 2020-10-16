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

#include "srslte/common/test_common.h"
#include "srslte/test/ue_test_interfaces.h"
#include "srsue/hdr/stack/rrc/phy_controller.h"

namespace srsue {

srslte::log_ref test_log{"TEST"};

struct cell_search_result_test {
  cell_search_result_test(phy_controller* phy_ctrl_) : phy_ctrl(phy_ctrl_) {}

  void trigger(const phy_controller::cell_srch_res& result_)
  {
    trigger_called = true;
    result         = result_;

    if (phy_ctrl->current_state_name() == "searching_cell" or phy_ctrl->is_trigger_locked()) {
      phy_ctrl->get_log()->error(
          "When caller is signalled with cell search result, cell search state cannot be active\n");
      exit(1);
    }
  }

  bool                          trigger_called = false;
  phy_controller::cell_srch_res result         = {};
  phy_controller*               phy_ctrl       = nullptr;
};

struct cell_select_result_test {
  cell_select_result_test(phy_controller* phy_ctrl_) : phy_ctrl(phy_ctrl_) {}

  void trigger(const bool& result_)
  {
    result = result_ ? 1 : 0;
    if (phy_ctrl->current_state_name() == "selecting_cell" or phy_ctrl->is_trigger_locked()) {
      phy_ctrl->get_log()->error(
          "When caller is signalled with cell select result, cell select state cannot be active\n");
      exit(1);
    }

    // start a new cell selection right away
    if (counter++ < 1) {
      phy_cell_t new_cell = {};
      new_cell.pci    = 3;
      new_cell.earfcn = 3400;
      phy_ctrl->start_cell_select(new_cell, *this);
    }
  }

  int             result = -1, counter = 0;
  phy_controller* phy_ctrl = nullptr;
};

int test_phy_ctrl_fsm()
{
  srslte::task_scheduler  task_sched;
  phy_dummy_interface     phy;
  phy_controller          phy_ctrl{&phy, &task_sched};
  cell_search_result_test csearch_tester{&phy_ctrl};
  cell_select_result_test csel_tester{&phy_ctrl};

  TESTASSERT(not phy_ctrl.is_in_sync());
  TESTASSERT(not phy_ctrl.cell_is_camping());

  // TEST: Sync event changes phy controller state
  phy_ctrl.in_sync();
  TESTASSERT(phy_ctrl.is_in_sync());
  phy_ctrl.out_sync();
  TESTASSERT(not phy_ctrl.is_in_sync());
  phy_ctrl.in_sync();
  TESTASSERT(phy_ctrl.is_in_sync());

  // TEST: Correct initiation of Cell Search state
  TESTASSERT(phy_ctrl.start_cell_search(csearch_tester));
  TESTASSERT(not phy_ctrl.is_in_sync());

  // TEST: Cell Search only listens to a cell search result event
  phy_ctrl.in_sync();
  TESTASSERT(not phy_ctrl.is_in_sync());
  phy_ctrl.out_sync();
  TESTASSERT(not phy_ctrl.is_in_sync());
  TESTASSERT(phy_ctrl.current_state_name() == "searching_cell");
  rrc_interface_phy_lte::cell_search_ret_t cs_ret;
  cs_ret.found = rrc_interface_phy_lte::cell_search_ret_t::CELL_FOUND;
  phy_cell_t found_cell;
  found_cell.pci    = 1;
  found_cell.earfcn = 2;
  phy_ctrl.cell_search_completed(cs_ret, found_cell);
  TESTASSERT(phy_ctrl.current_state_name() != "searching_cell");

  // TEST: Check propagation of cell search result to caller
  task_sched.run_pending_tasks();
  TESTASSERT(csearch_tester.trigger_called);
  TESTASSERT(csearch_tester.result.cs_ret.found == cs_ret.found);
  TESTASSERT(csearch_tester.result.found_cell.pci == found_cell.pci);
  TESTASSERT(csearch_tester.result.found_cell.earfcn == found_cell.earfcn);
  phy_ctrl.in_sync();
  TESTASSERT(phy_ctrl.is_in_sync());
  phy_ctrl.out_sync();

  // TEST: Correct initiation of Cell Select state
  phy_ctrl.start_cell_select(found_cell, csel_tester);
  TESTASSERT(not phy_ctrl.is_in_sync());
  TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");

  // TEST: Cell Selection state ignores events other than the cell selection result
  //       Cell selection observer is called
  //       New cell selection is started right away without affecting normal operation (e.g. observer should be called)
  phy_ctrl.in_sync();
  TESTASSERT(not phy_ctrl.is_in_sync());
  phy_ctrl.cell_selection_completed(true);
  // Note: Still in cell selection, but now waiting for the first in_sync
  TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");
  TESTASSERT(not phy_ctrl.is_in_sync());
  TESTASSERT(csel_tester.result < 0);
  phy_ctrl.in_sync();
  task_sched.run_pending_tasks();
  // observer is called that starts new cell selection
  TESTASSERT(not phy_ctrl.is_in_sync());
  TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");
  TESTASSERT(csel_tester.result == 1);
  csel_tester.result = 0;
  phy_ctrl.cell_selection_completed(true);
  TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");
  phy_ctrl.in_sync();
  task_sched.run_pending_tasks();
  TESTASSERT(phy_ctrl.is_in_sync());
  TESTASSERT(phy_ctrl.current_state_name() != "selecting_cell");
  TESTASSERT(csel_tester.result == 1);

  // TEST: Cell Selection with timeout being reached
  csel_tester.result = -1;
  TESTASSERT(phy_ctrl.start_cell_select(found_cell, csel_tester));
  TESTASSERT(not phy_ctrl.is_in_sync());
  phy_ctrl.cell_selection_completed(true);
  TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");
  TESTASSERT(csel_tester.result < 0);

  for (uint32_t i = 0; i < phy_controller::wait_sync_timeout_ms; ++i) {
    TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");
    task_sched.tic();
    task_sched.run_pending_tasks();
  }
  TESTASSERT(phy_ctrl.current_state_name() != "selecting_cell");

  // TEST: Propagation of cell selection result to caller
  task_sched.run_pending_tasks();
  TESTASSERT(csel_tester.result == 0);

  phy_ctrl.start_cell_select(found_cell, csel_tester);

  return SRSLTE_SUCCESS;
}

class phy_test_dummy : public phy_dummy_interface
{
public:
  bool success_on_cell_select_init = true;

  bool cell_select(phy_cell_t cell) override { return success_on_cell_select_init; }
};

/// TEST: Check if controller handles the case when PHY fails to init cell selection
int test_phy_cell_select_init_error_handling()
{
  srslte::task_scheduler task_sched;
  phy_test_dummy         phy;
  phy_controller         phy_ctrl{&phy, &task_sched};
  phy_cell_t             found_cell{};
  found_cell.pci                  = 1;
  found_cell.earfcn               = 2;
  int test_result                 = -1;
  phy.success_on_cell_select_init = false;

  phy_ctrl.start_cell_select(found_cell, [&test_result](bool csel_result) { test_result = csel_result ? 1 : 0; });

  return test_result == 0;
}

} // namespace srsue

int main()
{
  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_INFO);

  TESTASSERT(srsue::test_phy_ctrl_fsm() == SRSLTE_SUCCESS);
  TESTASSERT(srsue::test_phy_cell_select_init_error_handling() == SRSLTE_SUCCESS);
  srsue::test_log->info("Finished RRC PHY controller test successfully\n");
}
