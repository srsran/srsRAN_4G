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

int test_phy_ctrl_fsm()
{
  srslte::log_ref     test_log{"TEST"};
  stack_test_dummy    stack;
  phy_dummy_interface phy;
  phy_controller      phy_ctrl{&phy, &stack};

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
  bool                          csearch_res_present = false;
  phy_controller::cell_srch_res csearch_res         = {};
  auto cell_sel_callback = [&csearch_res_present, &csearch_res](const phy_controller::cell_srch_res& result) {
    csearch_res_present = true;
    csearch_res         = result;
  };
  TESTASSERT(phy_ctrl.start_cell_search(cell_sel_callback));
  TESTASSERT(not phy_ctrl.is_in_sync());

  // TEST: Cell Search only listens to a cell search result event and calls provided callback on completion
  phy_ctrl.in_sync();
  TESTASSERT(not phy_ctrl.is_in_sync());
  phy_ctrl.out_sync();
  TESTASSERT(not phy_ctrl.is_in_sync());
  TESTASSERT(phy_ctrl.current_state_name() == "searching_cell");
  phy_interface_rrc_lte::cell_search_ret_t cs_ret;
  cs_ret.found = phy_interface_rrc_lte::cell_search_ret_t::CELL_FOUND;
  phy_interface_rrc_lte::phy_cell_t found_cell;
  found_cell.pci    = 1;
  found_cell.earfcn = 2;
  phy_ctrl.cell_search_completed(cs_ret, found_cell);
  TESTASSERT(phy_ctrl.current_state_name() != "searching_cell");
  TESTASSERT(csearch_res_present);
  TESTASSERT(csearch_res.cs_ret.found == cs_ret.found);
  TESTASSERT(csearch_res.found_cell.pci == found_cell.pci);
  TESTASSERT(csearch_res.found_cell.earfcn == found_cell.earfcn);
  phy_ctrl.in_sync();
  TESTASSERT(phy_ctrl.is_in_sync());
  phy_ctrl.out_sync();

  // TEST: Correct initiation of Cell Select state
  int  cell_select_success = -1;
  auto csel_callback       = [&cell_select_success](const bool& res) { cell_select_success = res ? 1 : 0; };
  phy_ctrl.start_cell_select(found_cell, csel_callback);
  TESTASSERT(not phy_ctrl.is_in_sync());
  TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");

  // TEST: Cell Selection state ignores events other than the cell selection result, and callback is called
  phy_ctrl.in_sync();
  TESTASSERT(not phy_ctrl.is_in_sync());
  TESTASSERT(phy_ctrl.cell_selection_completed(true));
  // Note: Still in cell selection, but now waiting for the first in_sync
  TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");
  TESTASSERT(not phy_ctrl.is_in_sync());
  TESTASSERT(cell_select_success < 0);
  phy_ctrl.in_sync();
  TESTASSERT(phy_ctrl.is_in_sync());
  TESTASSERT(phy_ctrl.current_state_name() != "selecting_cell");
  TESTASSERT(cell_select_success == 1);

  // TEST: Cell Selection with timeout being reached
  cell_select_success = -1;
  TESTASSERT(phy_ctrl.start_cell_select(found_cell, csel_callback));
  TESTASSERT(not phy_ctrl.is_in_sync());
  phy_ctrl.cell_selection_completed(true);
  TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");
  TESTASSERT(cell_select_success < 0);

  for (uint32_t i = 0; i < phy_controller::wait_sync_timeout_ms; ++i) {
    TESTASSERT(phy_ctrl.current_state_name() == "selecting_cell");
    stack.run_tti();
  }
  TESTASSERT(phy_ctrl.current_state_name() != "selecting_cell");
  TESTASSERT(cell_select_success == 0);

  test_log->info("Finished RRC PHY controller test successfully\n");
  return SRSLTE_SUCCESS;
}

} // namespace srsue

int main()
{
  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_INFO);

  TESTASSERT(srsue::test_phy_ctrl_fsm() == SRSLTE_SUCCESS);
}
