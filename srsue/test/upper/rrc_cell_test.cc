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
#include "srsue/hdr/stack/rrc/rrc_cell.h"

using namespace srsue;

int test_meas_cell()
{
  srslte::task_scheduler task_sched;
  meas_cell invalid_cell{task_sched.get_unique_timer()}, cell{phy_cell_t{1, 3400}, task_sched.get_unique_timer()};

  TESTASSERT(not invalid_cell.is_valid());
  TESTASSERT(cell.is_valid());
  TESTASSERT(std::isnan(cell.get_rsrp()));
  TESTASSERT(std::isnan(cell.get_rsrq()));
  TESTASSERT(not cell.has_sib(0) and not cell.has_sib(1));

  // NAN does not invalidate measurements
  cell.set_rsrp(-50);
  TESTASSERT(cell.get_rsrp() == -50);
  cell.set_rsrp(NAN);
  TESTASSERT(cell.get_rsrp() == -50);

  // Test meas timer expiry
  for (size_t i = 0; i < meas_cell::neighbour_timeout_ms; ++i) {
    TESTASSERT(not cell.timer.is_expired());
    task_sched.tic();
  }
  TESTASSERT(cell.timer.is_expired());
  cell.set_rsrp(-20);
  for (size_t i = 0; i < meas_cell::neighbour_timeout_ms; ++i) {
    TESTASSERT(not cell.timer.is_expired());
    task_sched.tic();
  }
  TESTASSERT(cell.timer.is_expired());

  return SRSLTE_SUCCESS;
}

int test_add_neighbours()
{
  srslte::task_scheduler task_sched;
  meas_cell_list         list{&task_sched};
  TESTASSERT(list.nof_neighbours() == 0);
  TESTASSERT(not list.serving_cell().is_valid());
  TESTASSERT(list.get_neighbour_cell_handle(0, 0) == nullptr);

  rrc_interface_phy_lte::phy_meas_t pmeas;
  pmeas.cfo_hz = 4;
  pmeas.rsrp   = -20;
  pmeas.pci    = 1;
  pmeas.earfcn = 3400;
  pmeas.rsrq   = -10;
  TESTASSERT(list.add_meas_cell(pmeas));
  TESTASSERT(not list.serving_cell().is_valid());
  TESTASSERT(list.nof_neighbours() == 1);
  meas_cell* c = list.get_neighbour_cell_handle(3400, 1);
  TESTASSERT(c != nullptr and c->is_valid() and c->equals(3400, 1));
  TESTASSERT(c->get_rsrp() == pmeas.rsrp and c->get_rsrq() == pmeas.rsrq);

  auto pmeas2 = pmeas;
  pmeas2.pci  = 2;
  list.add_meas_cell(pmeas2);
  TESTASSERT(list.nof_neighbours() == 2);
  TESTASSERT(list.set_serving_cell(phy_cell_t{2, 3400}, false) == SRSLTE_SUCCESS);
  TESTASSERT(list.nof_neighbours() == 1);
  TESTASSERT(list.serving_cell().equals(3400, 2));
  TESTASSERT(list.serving_cell().is_valid());

  TESTASSERT(list.remove_neighbour_cell(3400, 3) == nullptr); // fail - does not exit
  TESTASSERT(list.remove_neighbour_cell(3400, 2) == nullptr); // fail - it is serving cell
  TESTASSERT(list.nof_neighbours() == 1);
  TESTASSERT(list.serving_cell().is_valid());
  auto c2 = list.remove_neighbour_cell(3400, 1);
  TESTASSERT(c2 != nullptr and c2->is_valid() and c2->equals(3400, 1));
  TESTASSERT(list.nof_neighbours() == 0);

  TESTASSERT(list.add_meas_cell(pmeas));
  TESTASSERT(list.nof_neighbours() == 1);
  task_sched.tic();
  task_sched.tic();
  list.get_neighbour_cell_handle(3400, 1)->set_rsrp(-20);
  for (size_t i = 0; i < meas_cell::neighbour_timeout_ms; ++i) {
    TESTASSERT(list.nof_neighbours() == 1);
    list.clean_neighbours();
    task_sched.tic();
  }
  list.clean_neighbours();
  TESTASSERT(list.nof_neighbours() == 0);

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_meas_cell() == SRSLTE_SUCCESS);
  TESTASSERT(test_add_neighbours() == SRSLTE_SUCCESS);
  printf("Success\n");

  return SRSLTE_SUCCESS;
}
