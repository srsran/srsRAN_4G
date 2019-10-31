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

meas_cell_cfg_t generate_cell1()
{
  meas_cell_cfg_t cell1{};
  cell1.earfcn   = 3400;
  cell1.pci      = 1;
  cell1.q_offset = 0;
  cell1.cell_id  = 0x19C01;
  return cell1;
}

bool is_cell_cfg_equal(const meas_cell_cfg_t& cfg, const cells_to_add_mod_s& cell)
{
  return cfg.pci == cell.pci and cell.cell_individual_offset.to_number() == (int8_t)round(cfg.q_offset) and
         cell.cell_idx == (cfg.cell_id & 0xFFu);
}

int test_correct_insertion()
{
  meas_cell_cfg_t cell1 = generate_cell1(), cell2{}, cell3{}, cell4{};
  cell2                 = cell1;
  cell2.pci             = 2;
  cell2.cell_id         = 0x19C02;
  cell3                 = cell1;
  cell3.earfcn          = 2850;
  cell4                 = cell1;
  cell4.q_offset        = 1;

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
    TESTASSERT(is_cell_cfg_equal(cell1, eutra.cells_to_add_mod_list[0]));
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

int test_correct_meascfg_calculation()
{
  var_meas_cfg_t src_var(&log_h), target_var(&log_h);

  meas_cell_cfg_t cell1{}, cell2{};
  cell1.earfcn   = 3400;
  cell1.pci      = 1;
  cell1.q_offset = 0;
  cell1.cell_id  = 0x19C01;
  cell2          = cell1;
  cell2.pci      = 2;
  cell2.cell_id  = 0x19C02;

  {
    meas_cfg_s result_meascfg;

    // TEST: Insertion of two cells in var_meas propagates to the resulting meas_cfg_s cellsToAddMod list
    target_var.add_cell_cfg(cell1);
    target_var.add_cell_cfg(cell2);
    src_var.compute_diff_meas_cfg(target_var, &result_meascfg);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list_present);
    TESTASSERT(not result_meascfg.meas_obj_to_rem_list_present);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list.size() == 1);
    auto* item = &result_meascfg.meas_obj_to_add_mod_list[0];
    TESTASSERT(item->meas_obj_id == 1 and
               item->meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra);
    auto& eutra = item->meas_obj.meas_obj_eutra();
    TESTASSERT(eutra.cells_to_add_mod_list_present and not eutra.cells_to_rem_list_present);
    TESTASSERT(eutra.cells_to_add_mod_list.size() == 2);
    auto* cell_item = &eutra.cells_to_add_mod_list[0];
    TESTASSERT(is_cell_cfg_equal(cell1, *cell_item));
    cell_item++;
    TESTASSERT(is_cell_cfg_equal(cell2, *cell_item));

    // TEST: if measCfg is empty if nothing was updated
    src_var = target_var;
    src_var.compute_diff_meas_cfg(target_var, &result_meascfg);
    TESTASSERT(not result_meascfg.meas_obj_to_add_mod_list_present);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list.size() == 0);

    // TEST: Cell is added to cellsToAddModList if just a field was updated
    cell1.pci = 3;
    src_var   = target_var;
    target_var.add_cell_cfg(cell1);
    src_var.compute_diff_meas_cfg(target_var, &result_meascfg);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list_present);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list.size() == 1);
    item = &result_meascfg.meas_obj_to_add_mod_list[0];
    TESTASSERT(item->meas_obj_id == 1 and
               item->meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra);
    eutra = item->meas_obj.meas_obj_eutra();
    TESTASSERT(eutra.cells_to_add_mod_list_present and not eutra.cells_to_rem_list_present);
    TESTASSERT(eutra.cells_to_add_mod_list.size() == 1);
    cell_item = &eutra.cells_to_add_mod_list[0];
    TESTASSERT(is_cell_cfg_equal(cell1, *cell_item));

    // TEST: Removal of cell from target propagates to the resulting meas_cfg_s cellsToRemoveList
    src_var    = target_var;
    target_var = var_meas_cfg_t{&log_h};
    target_var.add_cell_cfg(cell2);
    src_var.compute_diff_meas_cfg(target_var, &result_meascfg);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list_present);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list.size() == 1);
    item = &result_meascfg.meas_obj_to_add_mod_list[0];
    TESTASSERT(item->meas_obj_id == 1 and
               item->meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra);
    eutra = item->meas_obj.meas_obj_eutra();
    TESTASSERT(not eutra.cells_to_add_mod_list_present and eutra.cells_to_rem_list_present);
    TESTASSERT(eutra.cells_to_rem_list.size() == 1);
    TESTASSERT(eutra.cells_to_rem_list[0] == (cell1.cell_id & 0xFFu));
  }

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_correct_insertion() == 0);
  TESTASSERT(test_correct_meascfg_calculation() == 0);

  printf("Success\n");

  return 0;
}