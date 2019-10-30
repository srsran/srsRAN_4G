/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include <iostream>
#include <srslte/common/log_filter.h>

#define TESTASSERT(cond)                                                                                               \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  } while (0)

using namespace srsenb;
using namespace asn1::rrc;

srslte::log_filter log_h("ALL");

int test_correct_insertion()
{
  meas_cell_cfg_t cell1{}, cell2{}, cell3{}, cell4{};
  cell1.earfcn   = 3400;
  cell1.pci      = 1;
  cell1.q_offset = 0;
  cell1.cell_id  = 0x19C01;
  cell2          = cell1;
  cell2.pci      = 2;
  cell2.cell_id  = 0x19C02;
  cell3          = cell1;
  cell3.earfcn   = 2850;
  cell4          = cell1;
  cell4.q_offset = 1;

  // TEST 1: cell insertion in empty varMeasCfg
  {
    var_meas_cfg_t var_cfg(&log_h);
    auto           ret = var_cfg.add_cell_cfg(cell1);
    TESTASSERT(std::get<0>(ret) and std::get<1>(ret) != nullptr);
    const auto& objs = var_cfg.meas_objs();
    TESTASSERT(objs.size() == 1 and objs[0].meas_obj_id == 1);
    TESTASSERT(objs[0].meas_obj.type().value ==
               asn1::rrc::meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra);
    auto& eutra = objs[0].meas_obj.meas_obj_eutra();
    TESTASSERT(eutra.carrier_freq == cell1.earfcn);
    TESTASSERT(eutra.cells_to_add_mod_list.size() == 1);
    TESTASSERT(eutra.cells_to_add_mod_list[0].pci == cell1.pci);
    TESTASSERT(eutra.cells_to_add_mod_list[0].cell_idx == (cell1.cell_id & 0xFFu));
    TESTASSERT(eutra.cells_to_add_mod_list[0].cell_individual_offset.to_number() == (int8_t)round(cell1.q_offset));
  }

  {
    var_meas_cfg_t var_cfg(&log_h);
    const auto&    objs = var_cfg.meas_objs();

    // TEST 2: insertion of out-of-order cell ids in same earfcn
    var_cfg.add_cell_cfg(cell2);
    var_cfg.add_cell_cfg(cell1);
    TESTASSERT(objs.size() == 1 and objs[0].meas_obj_id == 1);
    auto& eutra = objs[0].meas_obj.meas_obj_eutra();
    TESTASSERT(eutra.carrier_freq == cell1.earfcn);
    TESTASSERT(eutra.cells_to_add_mod_list.size() == 2);
    const cells_to_add_mod_s* cell_it = eutra.cells_to_add_mod_list.begin();
    TESTASSERT(cell_it[0].cell_idx == (cell1.cell_id & 0xFFu));
    TESTASSERT(cell_it[1].cell_idx == (cell2.cell_id & 0xFFu));
    TESTASSERT(cell_it[1].pci == cell2.pci);

    // TEST 3: insertion of cell in another frequency
    auto ret1 = var_cfg.add_cell_cfg(cell3);
    TESTASSERT(std::get<0>(ret1) and std::get<1>(ret1)->meas_obj_id == 2);
    TESTASSERT(objs.size() == 2 and objs[1].meas_obj_id == 2);
    const auto& eutra2 = objs[1].meas_obj.meas_obj_eutra();
    TESTASSERT(eutra2.carrier_freq == cell3.earfcn);
    TESTASSERT(eutra2.cells_to_add_mod_list.size() == 1);

    // TEST 4: update of existing cell
    auto ret2 = var_cfg.add_cell_cfg(cell4);
    TESTASSERT(std::get<0>(ret2) and std::get<1>(ret2)->meas_obj_id == 1);
    auto& eutra3 = objs[0].meas_obj.meas_obj_eutra();
    TESTASSERT(objs.size() == 2 and objs[0].meas_obj_id == 1);
    TESTASSERT(eutra3.carrier_freq == cell4.earfcn);
    TESTASSERT(eutra3.cells_to_add_mod_list.size() == 2);
    TESTASSERT(eutra3.cells_to_add_mod_list[0].cell_idx == (cell1.cell_id & 0xFFu));
    TESTASSERT(eutra3.cells_to_add_mod_list[0].cell_individual_offset.to_number() == 1);
  }

  return 0;
}

int main()
{
  TESTASSERT(test_correct_insertion() == 0);

  return 0;
}