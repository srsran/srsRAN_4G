/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/stack/rrc/ue_meas_cfg.h"
#include "srsran/asn1/obj_id_cmp_utils.h"
#include "srsran/asn1/rrc_utils.h"
#include "srsran/common/test_common.h"
#include "srsran/interfaces/enb_rrc_interface_types.h"
#include "test_helpers.h"

using namespace asn1::rrc;

namespace srsenb {

/**
 * Checks if measObjs and reportConfigs are correctly added to the ASN1 RRC measConfig. Current checks:
 * - avoids repeated earfcns in the measObjsToAddModList
 * - avoids repeated PCIs in the CellsToAddModList of same earfcn
 * - measObjId, cellIdx, reportConfigId, measId are kept in order
 * @return error code
 */
int test_correct_meascfg_insertion()
{
  meas_cell_cfg_t cell1        = generate_cell1(), cell2{}, cell3{}, cell4{};
  cell2                        = cell1;
  cell2.pci                    = 2;
  cell2.eci                    = 0x19C02;
  cell3                        = cell1;
  cell3.earfcn                 = 2850;
  cell4                        = cell1;
  cell4.cell_individual_offset = asn1::rrc::q_offset_range_opts::db1;

  report_cfg_eutra_s rep1 = generate_rep1();

  // TEST 1: cell/rep insertion in empty MeasCfg
  {
    meas_cfg_s  meas_cfg{};
    const auto& objs = meas_cfg.meas_obj_to_add_mod_list;
    auto        ret  = add_cell_enb_cfg(meas_cfg.meas_obj_to_add_mod_list, cell1);
    TESTASSERT(std::get<0>(ret) and std::get<1>(ret) != nullptr);
    TESTASSERT(objs.size() == 1 and objs[0].meas_obj_id == 1);
    TESTASSERT(objs[0].meas_obj.type().value ==
               asn1::rrc::meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra);
    auto& eutra = objs[0].meas_obj.meas_obj_eutra();
    TESTASSERT(eutra.carrier_freq == cell1.earfcn);
    TESTASSERT(eutra.cells_to_add_mod_list.size() == 1);
    TESTASSERT(is_cell_cfg_equal(cell1, eutra.cells_to_add_mod_list[0]));

    auto* ret2 = add_report_cfg(meas_cfg.report_cfg_to_add_mod_list, rep1);
    TESTASSERT(ret2->report_cfg_id == 1);
    TESTASSERT(ret2->report_cfg.report_cfg_eutra() == rep1);
  }

  {
    meas_cfg_s  meas_cfg{};
    const auto& objs = meas_cfg.meas_obj_to_add_mod_list;

    // TEST 2: insertion of out-of-order cell ids in same earfcn
    add_cell_enb_cfg(meas_cfg.meas_obj_to_add_mod_list, cell2);
    add_cell_enb_cfg(meas_cfg.meas_obj_to_add_mod_list, cell1);
    TESTASSERT(objs.size() == 1 and objs[0].meas_obj_id == 1);
    auto& eutra = objs[0].meas_obj.meas_obj_eutra();
    TESTASSERT(eutra.carrier_freq == cell1.earfcn);
    TESTASSERT(eutra.cells_to_add_mod_list.size() == 2);
    const cells_to_add_mod_s* cell_it = eutra.cells_to_add_mod_list.begin();
    TESTASSERT(cell_it[0].cell_idx == 1);
    TESTASSERT(cell_it[1].cell_idx == 2);
    TESTASSERT(cell_it[0].pci == cell2.pci);
    TESTASSERT(cell_it[1].pci == cell1.pci);

    // TEST 3: insertion of cell in another frequency
    auto ret1 = add_cell_enb_cfg(meas_cfg.meas_obj_to_add_mod_list, cell3);
    TESTASSERT(std::get<0>(ret1) and std::get<1>(ret1)->meas_obj_id == 2);
    TESTASSERT(objs.size() == 2 and objs[1].meas_obj_id == 2);
    const auto& eutra2 = objs[1].meas_obj.meas_obj_eutra();
    TESTASSERT(eutra2.carrier_freq == cell3.earfcn);
    TESTASSERT(eutra2.cells_to_add_mod_list_present and eutra2.cells_to_add_mod_list.size() == 1);
    TESTASSERT(eutra2.cells_to_add_mod_list[0].cell_idx == 1);
    TESTASSERT(eutra2.cells_to_add_mod_list[0].pci == cell3.pci);

    // TEST 4 : update of existing cell
    auto ret2 = add_cell_enb_cfg(meas_cfg.meas_obj_to_add_mod_list, cell4);
    TESTASSERT(std::get<0>(ret2) and std::get<1>(ret2)->meas_obj_id == 1);
    auto& eutra3 = objs[0].meas_obj.meas_obj_eutra();
    TESTASSERT(objs.size() == 2 and objs[0].meas_obj_id == 1);
    TESTASSERT(eutra3.carrier_freq == cell4.earfcn);
    TESTASSERT(eutra3.cells_to_add_mod_list.size() == 2);
    TESTASSERT(eutra3.cells_to_add_mod_list[1].cell_idx == 2);
    TESTASSERT(eutra3.cells_to_add_mod_list[1].pci == cell4.pci);
    TESTASSERT(eutra3.cells_to_add_mod_list[1].cell_individual_offset.to_number() == 1);
  }

  return 0;
}

int test_correct_meascfg_calculation()
{
  meas_cfg_s src_var{}, target_var{};

  meas_cell_cfg_t cell1{}, cell2{};
  cell1.earfcn                 = 3400;
  cell1.pci                    = 1;
  cell1.cell_individual_offset = asn1::rrc::q_offset_range_opts::db0;
  cell1.eci                    = 0x19C01;
  cell2                        = cell1;
  cell2.pci                    = 2;
  cell2.eci                    = 0x19C02;

  report_cfg_eutra_s rep1  = generate_rep1(), rep2{}, rep3{};
  rep2                     = rep1;
  rep2.trigger_quant.value = report_cfg_eutra_s::trigger_quant_opts::rsrq;
  rep3                     = rep2;
  rep3.report_quant.value  = report_cfg_eutra_s::report_quant_opts::same_as_trigger_quant;

  {
    meas_cfg_s result_meascfg;

    // TEST 1: Insertion of two cells in target meas_cfg_s propagates to the diff meas_cfg_s
    add_cell_enb_cfg(target_var.meas_obj_to_add_mod_list, cell1);
    add_cell_enb_cfg(target_var.meas_obj_to_add_mod_list, cell2);
    add_report_cfg(target_var.report_cfg_to_add_mod_list, rep1);
    add_report_cfg(target_var.report_cfg_to_add_mod_list, rep2);
    add_measid_cfg(target_var.meas_id_to_add_mod_list, 1, 1);
    add_measid_cfg(target_var.meas_id_to_add_mod_list, 1, 2);
    TESTASSERT(compute_diff_meascfg(src_var, target_var, result_meascfg));
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
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list_present and not result_meascfg.report_cfg_to_rem_list_present);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list.size() == 2);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list[0].report_cfg_id == 1);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list[0].report_cfg.report_cfg_eutra() == rep1);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list[1].report_cfg_id == 2);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list[1].report_cfg.report_cfg_eutra() == rep2);
    TESTASSERT(result_meascfg.meas_id_to_add_mod_list_present and not result_meascfg.meas_id_to_rem_list_present);
    TESTASSERT(result_meascfg.meas_id_to_add_mod_list.size() == 2);
    auto* measid_item = &result_meascfg.meas_id_to_add_mod_list[0];
    TESTASSERT(measid_item->meas_id == 1 and measid_item->meas_obj_id == 1 and measid_item->report_cfg_id == 1);
    measid_item++;
    TESTASSERT(measid_item->meas_id == 2 and measid_item->meas_obj_id == 1 and measid_item->report_cfg_id == 2);

    // TEST 2: measConfig is empty if nothing was updated
    src_var = target_var;
    TESTASSERT(not compute_diff_meascfg(src_var, target_var, result_meascfg));
    TESTASSERT(not result_meascfg.meas_obj_to_add_mod_list_present and not result_meascfg.meas_obj_to_rem_list_present);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list.size() == 0);
    TESTASSERT(not result_meascfg.report_cfg_to_add_mod_list_present and
               not result_meascfg.report_cfg_to_rem_list_present);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list.size() == 0);

    // TEST 3: Cell is added to cellsToAddModList if just a field was updated
    cell1.cell_individual_offset = asn1::rrc::q_offset_range_opts::db5;
    src_var                      = target_var;
    add_cell_enb_cfg(target_var.meas_obj_to_add_mod_list, cell1);
    TESTASSERT(compute_diff_meascfg(src_var, target_var, result_meascfg));
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

    // TEST 4: Removal of cell/rep from target propagates to the resulting meas_cfg_s
    src_var                  = target_var;
    const auto& src_measobjs = src_var.meas_obj_to_add_mod_list;
    TESTASSERT(src_measobjs.size() == 1);
    TESTASSERT(src_measobjs[0].meas_obj.meas_obj_eutra().cells_to_add_mod_list.size() == 2);
    target_var = {};
    add_cell_enb_cfg(target_var.meas_obj_to_add_mod_list, cell2);
    add_report_cfg(target_var.report_cfg_to_add_mod_list, rep1);
    add_report_cfg(target_var.report_cfg_to_add_mod_list, rep3);
    TESTASSERT(compute_diff_meascfg(src_var, target_var, result_meascfg));
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list_present);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list.size() == 1);
    item = &result_meascfg.meas_obj_to_add_mod_list[0];
    TESTASSERT(item->meas_obj_id == 1 and
               item->meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra);
    eutra = item->meas_obj.meas_obj_eutra();
    TESTASSERT(eutra.cells_to_add_mod_list_present and eutra.cells_to_add_mod_list.size() == 1);
    TESTASSERT(eutra.cells_to_add_mod_list[0].pci == cell2.pci);
    TESTASSERT(eutra.cells_to_rem_list_present and eutra.cells_to_rem_list.size() == 1);
    TESTASSERT(eutra.cells_to_rem_list[0] == 2);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list_present and not result_meascfg.report_cfg_to_rem_list_present);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list.size() == 1);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list[0].report_cfg_id == 2);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list[0].report_cfg.report_cfg_eutra() == rep3);
  }

  {
    // TEST: creation of a meas_cfg using the srsenb::rrc_cfg_t
    rrc_cfg_t          cfg;
    srsenb::all_args_t all_args;
    TESTASSERT(test_helpers::parse_default_cfg(&cfg, all_args) == SRSRAN_SUCCESS);
    cfg.enb_id           = 0x19B;
    cfg.cell.nof_prb     = 6;
    cfg.meas_cfg_present = true;
    cfg.cell_list.resize(2);
    cfg.cell_list[0].dl_earfcn = 2850;
    cfg.cell_list[0].cell_id   = 0x01;
    cfg.cell_list[0].scell_list.resize(1);
    cfg.cell_list[0].scell_list[0].cell_id = 0x02;
    cfg.cell_list[0].meas_cfg.meas_cells.resize(1);
    cfg.cell_list[0].meas_cfg.meas_cells[0]     = generate_cell1();
    cfg.cell_list[0].meas_cfg.meas_cells[0].pci = 3;
    cfg.cell_list[1].dl_earfcn                  = 3400;
    cfg.cell_list[1].cell_id                    = 0x02;
    cfg.cell_list[0].meas_cfg.meas_reports.resize(1);
    cfg.cell_list[0].meas_cfg.meas_reports[0] = generate_rep1();

    // TEST: correct construction of list of cells
    enb_cell_common_list cell_list{cfg};
    TESTASSERT(cell_list.nof_cells() == 2);
    TESTASSERT(cell_list.get_cc_idx(0)->scells.size() == 1);
    TESTASSERT(cell_list.get_cc_idx(0)->scells[0] == cell_list.get_cc_idx(1));
    TESTASSERT(cell_list.get_cc_idx(1)->scells.empty());
    freq_res_common_list freq_res{cfg};

    // measConfig only includes earfcns of active carriers for a given pcell
    meas_cfg_s       cell_meas_cfg;
    ue_cell_ded_list ue_cell_list{cfg, freq_res, cell_list};
    ue_cell_list.set_cells({0});
    TESTASSERT(fill_meascfg_enb_cfg(cell_meas_cfg, ue_cell_list));
    const auto& measobjs = cell_meas_cfg.meas_obj_to_add_mod_list;
    TESTASSERT(measobjs.size() == 2);
    TESTASSERT(measobjs[0].meas_obj.meas_obj_eutra().carrier_freq == 2850);
    TESTASSERT(not measobjs[0].meas_obj.meas_obj_eutra().cells_to_add_mod_list_present);
    TESTASSERT(measobjs[1].meas_obj.meas_obj_eutra().carrier_freq == 3400);
    TESTASSERT(measobjs[1].meas_obj.meas_obj_eutra().cells_to_add_mod_list_present);
    TESTASSERT(measobjs[1].meas_obj.meas_obj_eutra().cells_to_add_mod_list.size() == 1);
    TESTASSERT(measobjs[1].meas_obj.meas_obj_eutra().cells_to_add_mod_list[0].pci == 3);
    TESTASSERT(cell_meas_cfg.report_cfg_to_add_mod_list_present);
    TESTASSERT(cell_meas_cfg.report_cfg_to_add_mod_list.size() == 1);
    TESTASSERT(cell_meas_cfg.report_cfg_to_add_mod_list[0].report_cfg.report_cfg_eutra() ==
               cfg.cell_list[0].meas_cfg.meas_reports[0]);
    TESTASSERT(cell_meas_cfg.meas_id_to_add_mod_list_present);
    const auto& measid = cell_meas_cfg.meas_id_to_add_mod_list[0];
    TESTASSERT(measid.meas_id == 1 and measid.meas_obj_id == 1 and measid.report_cfg_id == 1);

    meas_cfg_s cell_meas_cfg2;
    ue_cell_list.set_cells({1});
    TESTASSERT(fill_meascfg_enb_cfg(cell_meas_cfg2, ue_cell_list));
    const auto& measobjs2 = cell_meas_cfg2.meas_obj_to_add_mod_list;
    TESTASSERT(measobjs2.size() == 1);
    TESTASSERT(measobjs2[0].meas_obj.meas_obj_eutra().carrier_freq == 3400);
    TESTASSERT(not measobjs2[0].meas_obj.meas_obj_eutra().cells_to_add_mod_list_present);
  }

  return SRSRAN_SUCCESS;
}

int test_minimize_meascfg_reordering()
{
  rrc_cfg_t          default_cfg, cfg1;
  srsenb::all_args_t all_args;
  TESTASSERT(test_helpers::parse_default_cfg(&default_cfg, all_args) == SRSRAN_SUCCESS);
  cfg1                  = default_cfg;
  cfg1.enb_id           = 0x19B;
  cfg1.cell.nof_prb     = 6;
  cfg1.meas_cfg_present = true;
  cfg1.cell_list.resize(2);
  cfg1.cell_list[0].dl_earfcn = 2850;
  cfg1.cell_list[0].cell_id   = 0x01;
  cfg1.cell_list[0].scell_list.resize(1);
  cfg1.cell_list[0].scell_list[0].cell_id = 0x02;
  cfg1.cell_list[0].meas_cfg.meas_cells.resize(1);
  cfg1.cell_list[0].meas_cfg.meas_cells[0]     = generate_cell1();
  cfg1.cell_list[0].meas_cfg.meas_cells[0].pci = 3;
  cfg1.cell_list[1].dl_earfcn                  = 3400;
  cfg1.cell_list[1].cell_id                    = 0x02;

  enb_cell_common_list cell_list{cfg1};
  TESTASSERT(cell_list.nof_cells() == 2);
  TESTASSERT(cell_list.get_cc_idx(0)->scells.size() == 1);
  TESTASSERT(cell_list.get_cc_idx(0)->scells[0] == cell_list.get_cc_idx(1));
  TESTASSERT(cell_list.get_cc_idx(1)->scells.empty());
  freq_res_common_list freq_res{cfg1};
  ue_cell_ded_list     ue_cell_list1{cfg1, freq_res, cell_list};
  ue_cell_ded_list     ue_cell_list2{cfg1, freq_res, cell_list};

  meas_cfg_s mcfg1{}, mcfg2{};
  ue_cell_list1.set_cells({0, 1});
  ue_cell_list2.set_cells({1, 0});
  TESTASSERT(fill_meascfg_enb_cfg(mcfg1, ue_cell_list1));
  TESTASSERT(fill_meascfg_enb_cfg(mcfg2, ue_cell_list2));

  // TEST1: Ensure consistent order of measObjects based on DL-EARFCN
  TESTASSERT(mcfg1.meas_obj_to_add_mod_list_present and mcfg2.meas_obj_to_add_mod_list_present);
  TESTASSERT(mcfg1.meas_obj_to_add_mod_list.size() == mcfg2.meas_obj_to_add_mod_list.size());
  TESTASSERT(srsran::get_carrier_freq(mcfg1.meas_obj_to_add_mod_list[0]) ==
             srsran::get_carrier_freq(mcfg2.meas_obj_to_add_mod_list[0]));
  TESTASSERT(srsran::get_carrier_freq(mcfg1.meas_obj_to_add_mod_list[1]) ==
             srsran::get_carrier_freq(mcfg2.meas_obj_to_add_mod_list[1]));
  TESTASSERT(std::is_sorted(
      mcfg1.meas_obj_to_add_mod_list.begin(), mcfg1.meas_obj_to_add_mod_list.end(), srsran::rrc_obj_id_cmp{}));
  TESTASSERT(std::is_sorted(
      mcfg2.meas_obj_to_add_mod_list.begin(), mcfg2.meas_obj_to_add_mod_list.end(), srsran::rrc_obj_id_cmp{}));

  return SRSRAN_SUCCESS;
}

} // namespace srsenb

int main(int argc, char** argv)
{
  auto& logger = srslog::fetch_basic_logger("RRC", false);
  logger.set_level(srslog::basic_levels::info);
  srslog::init();

  if (argc < 3) {
    argparse::usage(argv[0]);
    return -1;
  }
  argparse::parse_args(argc, argv);

  TESTASSERT(test_correct_meascfg_insertion() == 0);
  TESTASSERT(test_correct_meascfg_calculation() == 0);
  TESTASSERT(test_minimize_meascfg_reordering() == 0);

  srslog::flush();

  srsran::console("Success\n");

  return 0;
}
