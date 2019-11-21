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
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/test_common.h"
#include <iostream>
#include <srslte/common/log_filter.h>

using namespace srsenb;
using namespace asn1::rrc;

srslte::log_filter log_h("ALL");

class mac_dummy : public mac_interface_rrc
{
public:
  int  cell_cfg(sched_interface::cell_cfg_t* cell_cfg) override { return 0; }
  void reset() override {}
  int  ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* cfg) override { return 0; }
  int  ue_rem(uint16_t rnti) override { return 0; }
  int  bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg) override { return 0; }
  int  bearer_ue_rem(uint16_t rnti, uint32_t lc_id) override { return 0; }
  int  set_dl_ant_info(uint16_t rnti, asn1::rrc::phys_cfg_ded_s::ant_info_c_* dl_ant_info) override { return 0; }
  void phy_config_enabled(uint16_t rnti, bool enabled) override {}
  void write_mcch(asn1::rrc::sib_type2_s* sib2, asn1::rrc::sib_type13_r9_s* sib13, asn1::rrc::mcch_msg_s* mcch) override
  {
  }
};

meas_cell_cfg_t generate_cell1()
{
  meas_cell_cfg_t cell1{};
  cell1.earfcn   = 3400;
  cell1.pci      = 1;
  cell1.q_offset = 0;
  cell1.eci      = 0x19C01;
  return cell1;
}

report_cfg_eutra_s generate_rep1()
{
  report_cfg_eutra_s rep{};
  rep.report_amount.value = report_cfg_eutra_s::report_amount_opts::r16;
  rep.report_interv.value = report_interv_opts::ms240;
  rep.max_report_cells    = 2;
  rep.report_quant.value  = report_cfg_eutra_s::report_quant_opts::both;
  rep.trigger_quant.value = report_cfg_eutra_s::trigger_quant_opts::rsrp;
  rep.trigger_type.set_event().event_id.set_event_a3();
  rep.trigger_type.event().time_to_trigger.value               = time_to_trigger_opts::ms100;
  rep.trigger_type.event().hysteresis                          = 0;
  rep.trigger_type.event().event_id.event_a3().a3_offset       = 5;
  rep.trigger_type.event().event_id.event_a3().report_on_leave = true;
  return rep;
}

bool is_cell_cfg_equal(const meas_cell_cfg_t& cfg, const cells_to_add_mod_s& cell)
{
  return cfg.pci == cell.pci and cell.cell_individual_offset.to_number() == (int8_t)round(cfg.q_offset) and
         cell.cell_idx == (cfg.eci & 0xFFu);
}

int test_correct_insertion()
{
  meas_cell_cfg_t cell1 = generate_cell1(), cell2{}, cell3{}, cell4{};
  cell2                 = cell1;
  cell2.pci             = 2;
  cell2.eci             = 0x19C02;
  cell3                 = cell1;
  cell3.earfcn          = 2850;
  cell4                 = cell1;
  cell4.q_offset        = 1;

  report_cfg_eutra_s rep1 = generate_rep1();

  // TEST 1: cell/rep insertion in empty varMeasCfg
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

    auto ret2 = var_cfg.add_report_cfg(rep1);
    TESTASSERT(ret2->report_cfg_id == 1);
    TESTASSERT(ret2->report_cfg.report_cfg_eutra() == rep1);
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
    TESTASSERT(cell_it[0].cell_idx == (cell1.eci & 0xFFu));
    TESTASSERT(cell_it[1].cell_idx == (cell2.eci & 0xFFu));
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
    TESTASSERT(eutra3.cells_to_add_mod_list[0].cell_idx == (cell1.eci & 0xFFu));
    TESTASSERT(eutra3.cells_to_add_mod_list[0].cell_individual_offset.to_number() == 1);
  }

  return 0;
}

int test_correct_meascfg_calculation()
{
  srslte::scoped_tester_log log_test("MEASCFG_CALC");
  var_meas_cfg_t    src_var(&log_h), target_var(&log_h);

  meas_cell_cfg_t cell1{}, cell2{};
  cell1.earfcn   = 3400;
  cell1.pci      = 1;
  cell1.q_offset = 0;
  cell1.eci      = 0x19C01;
  cell2          = cell1;
  cell2.pci      = 2;
  cell2.eci      = 0x19C02;

  report_cfg_eutra_s rep1  = generate_rep1(), rep2{}, rep3{};
  rep2                     = rep1;
  rep2.trigger_quant.value = report_cfg_eutra_s::trigger_quant_opts::rsrq;
  rep3                     = rep2;
  rep3.report_quant.value  = report_cfg_eutra_s::report_quant_opts::same_as_trigger_quant;

  {
    meas_cfg_s result_meascfg;

    // TEST: Insertion of two cells in var_meas propagates to the resulting meas_cfg_s cellsToAddMod list
    target_var.add_cell_cfg(cell1);
    target_var.add_cell_cfg(cell2);
    target_var.add_report_cfg(rep1);
    target_var.add_report_cfg(rep2);
    target_var.add_measid_cfg(1, 1);
    target_var.add_measid_cfg(1, 2);
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

    // TEST: if measCfg is empty if nothing was updated
    src_var = target_var;
    src_var.compute_diff_meas_cfg(target_var, &result_meascfg);
    TESTASSERT(not result_meascfg.meas_obj_to_add_mod_list_present);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list.size() == 0);
    TESTASSERT(not result_meascfg.report_cfg_to_rem_list_present);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list.size() == 0);

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

    // TEST: Removal of cell/rep from target propagates to the resulting meas_cfg_s
    src_var    = target_var;
    target_var = var_meas_cfg_t{&log_h};
    target_var.add_cell_cfg(cell2);
    target_var.add_report_cfg(rep1);
    target_var.add_report_cfg(rep3);
    src_var.compute_diff_meas_cfg(target_var, &result_meascfg);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list_present);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list.size() == 1);
    item = &result_meascfg.meas_obj_to_add_mod_list[0];
    TESTASSERT(item->meas_obj_id == 1 and
               item->meas_obj.type().value == meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra);
    eutra = item->meas_obj.meas_obj_eutra();
    TESTASSERT(not eutra.cells_to_add_mod_list_present and eutra.cells_to_rem_list_present);
    TESTASSERT(eutra.cells_to_rem_list.size() == 1);
    TESTASSERT(eutra.cells_to_rem_list[0] == (cell1.eci & 0xFFu));
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list_present and not result_meascfg.report_cfg_to_rem_list_present);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list.size() == 1);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list[0].report_cfg_id == 2);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list[0].report_cfg.report_cfg_eutra() == rep3);
  }

  return SRSLTE_SUCCESS;
}

int test_mobility_class()
{
  srslte::scoped_tester_log log_test("MOBILITY_TEST");

  rrc_cfg_t cfg;
  cfg.sib1.cell_access_related_info.plmn_id_list.push_back({});
  cfg.sib1.cell_access_related_info.plmn_id_list[0].plmn_id.mnc.resize(2);
  cfg.sib1.cell_access_related_info.plmn_id_list[0].cell_reserved_for_oper.value =
      plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;
  cfg.sib1.cell_access_related_info.cell_barred.value =
      sib_type1_s::cell_access_related_info_s_::cell_barred_opts::not_barred;
  cfg.sib1.cell_access_related_info.intra_freq_resel.value =
      sib_type1_s::cell_access_related_info_s_::intra_freq_resel_opts::allowed;
  cfg.sib1.si_win_len.value = sib_type1_s::si_win_len_opts::ms5;
  cfg.sib1.sched_info_list.push_back({});
  cfg.sib1.sched_info_list[0].si_periodicity.value = sched_info_s::si_periodicity_opts::rf8;
  auto& sib2                                       = cfg.sibs[1].set_sib2();
  sib2.rr_cfg_common.rach_cfg_common.preamb_info.nof_ra_preambs.value =
      rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_opts::n4;
  sib2.rr_cfg_common.rach_cfg_common.pwr_ramp_params.pwr_ramp_step.value = pwr_ramp_params_s::pwr_ramp_step_opts::db0;
  sib2.rr_cfg_common.rach_cfg_common.pwr_ramp_params.preamb_init_rx_target_pwr.value =
      pwr_ramp_params_s::preamb_init_rx_target_pwr_opts::dbm_minus90;
  sib2.rr_cfg_common.rach_cfg_common.ra_supervision_info.preamb_trans_max.value = preamb_trans_max_opts::n4;
  sib2.rr_cfg_common.rach_cfg_common.ra_supervision_info.ra_resp_win_size.value =
      rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_opts::sf2;
  sib2.rr_cfg_common.rach_cfg_common.ra_supervision_info.mac_contention_resolution_timer.value =
      rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_opts::sf8;
  sib2.rr_cfg_common.bcch_cfg.mod_period_coeff.value     = bcch_cfg_s::mod_period_coeff_opts::n4;
  sib2.rr_cfg_common.pcch_cfg.default_paging_cycle.value = pcch_cfg_s::default_paging_cycle_opts::rf32;
  sib2.rr_cfg_common.pcch_cfg.nb.value                   = pcch_cfg_s::nb_opts::four_t;
  sib2.rr_cfg_common.pusch_cfg_common.pusch_cfg_basic.hop_mode.value =
      pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_opts::inter_sub_frame;
  sib2.rr_cfg_common.pucch_cfg_common.delta_pucch_shift.value = pucch_cfg_common_s::delta_pucch_shift_opts::ds1;
  sib2.rr_cfg_common.srs_ul_cfg_common.set(setup_opts::release);
  sib2.rr_cfg_common.ul_pwr_ctrl_common.alpha.value = alpha_r12_opts::al0;
  bzero(&sib2.rr_cfg_common.ul_pwr_ctrl_common.delta_flist_pucch,
        sizeof(sib2.rr_cfg_common.ul_pwr_ctrl_common.delta_flist_pucch));
  sib2.rr_cfg_common.ul_cp_len.value = ul_cp_len_opts::len1;
  bzero(&sib2.ue_timers_and_consts, sizeof(sib2.ue_timers_and_consts));
  sib2.time_align_timer_common.value = time_align_timer_opts::sf500;
  report_cfg_eutra_s rep             = generate_rep1();
  cfg.meas_cfg.meas_reports.push_back(rep);

  srsenb::rrc rrc;
  mac_dummy   mac;
  //  rrc.init(&cfg, nullptr, &mac, nullptr, nullptr, nullptr, nullptr, &log_h);

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_correct_insertion() == 0);
  TESTASSERT(test_correct_meascfg_calculation() == 0);
  TESTASSERT(test_mobility_class() == 0);

  printf("Success\n");

  return 0;
}