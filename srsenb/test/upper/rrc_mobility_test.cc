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

#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srsenb/src/enb_cfg_parser.h"
#include "srsenb/test/common/dummy_classes.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/test_common.h"
#include "test_helpers.h"
#include <iostream>
#include <srslte/common/log_filter.h>

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
  return cfg.pci == cell.pci and cell.cell_individual_offset.to_number() == (int8_t)round(cfg.q_offset);
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
    var_meas_cfg_t var_cfg{};
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
    var_meas_cfg_t var_cfg{};
    const auto&    objs = var_cfg.meas_objs();

    // TEST 2: insertion of out-of-order cell ids in same earfcn
    var_cfg.add_cell_cfg(cell2);
    var_cfg.add_cell_cfg(cell1);
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
    auto ret1 = var_cfg.add_cell_cfg(cell3);
    TESTASSERT(std::get<0>(ret1) and std::get<1>(ret1)->meas_obj_id == 2);
    TESTASSERT(objs.size() == 2 and objs[1].meas_obj_id == 2);
    const auto& eutra2 = objs[1].meas_obj.meas_obj_eutra();
    TESTASSERT(eutra2.carrier_freq == cell3.earfcn);
    TESTASSERT(eutra2.cells_to_add_mod_list_present and eutra2.cells_to_add_mod_list.size() == 1);
    TESTASSERT(eutra2.cells_to_add_mod_list[0].cell_idx == 1);
    TESTASSERT(eutra2.cells_to_add_mod_list[0].pci == cell3.pci);

    // TEST 4: update of existing cell
    auto ret2 = var_cfg.add_cell_cfg(cell4);
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
  var_meas_cfg_t src_var{}, target_var{};

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

    // TEST 1: Insertion of two cells in var_meas propagates to the resulting meas_cfg_s cellsToAddMod list
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

    // TEST 2: measConfig is empty if nothing was updated
    src_var = target_var;
    src_var.compute_diff_meas_cfg(target_var, &result_meascfg);
    TESTASSERT(not result_meascfg.meas_obj_to_add_mod_list_present and not result_meascfg.meas_obj_to_rem_list_present);
    TESTASSERT(result_meascfg.meas_obj_to_add_mod_list.size() == 0);
    TESTASSERT(not result_meascfg.report_cfg_to_add_mod_list_present and
               not result_meascfg.report_cfg_to_rem_list_present);
    TESTASSERT(result_meascfg.report_cfg_to_add_mod_list.size() == 0);

    // TEST 3: Cell is added to cellsToAddModList if just a field was updated
    cell1.q_offset = 5;
    src_var        = target_var;
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

    // TEST 4: Removal of cell/rep from target propagates to the resulting meas_cfg_s
    src_var = target_var;
    TESTASSERT(src_var.meas_objs().size() == 1);
    TESTASSERT(src_var.meas_objs()[0].meas_obj.meas_obj_eutra().cells_to_add_mod_list.size() == 2);
    target_var = {};
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
    // TEST: creation of a var_meas_cfg using the srsenb::rrc_cfg_t
    rrc_cfg_t cfg;
    cfg.enb_id           = 0x19B;
    cfg.cell.nof_prb     = 6;
    cfg.meas_cfg_present = true;
    cfg.cell_list.resize(2);
    cfg.cell_list[0].dl_earfcn = 2850;
    cfg.cell_list[0].cell_id   = 0x01;
    cfg.cell_list[0].scell_list.resize(1);
    cfg.cell_list[0].scell_list[0].cell_id = 0x02;
    cfg.cell_list[1].dl_earfcn             = 3400;
    cfg.cell_list[1].cell_id               = 0x02;
    cfg.sibs[1].set_sib2();

    // TEST: correct construction of list of cells
    cell_info_common_list cell_list{cfg};
    TESTASSERT(cell_list.nof_cells() == 2);
    TESTASSERT(cell_list.get_cc_idx(0)->scells.size() == 1);
    TESTASSERT(cell_list.get_cc_idx(0)->scells[0] == cell_list.get_cc_idx(1));
    TESTASSERT(cell_list.get_cc_idx(1)->scells.empty());

    // measConfig only includes earfcns of active carriers for a given pcell
    var_meas_cfg_t var_meas = var_meas_cfg_t::make(cfg, *cell_list.get_cc_idx(0));
    TESTASSERT(var_meas.meas_objs().size() == 2);
    TESTASSERT(var_meas.meas_objs()[0].meas_obj.meas_obj_eutra().carrier_freq == 2850);
    TESTASSERT(var_meas.meas_objs()[1].meas_obj.meas_obj_eutra().carrier_freq == 3400);

    var_meas_cfg_t var_meas2 = var_meas_cfg_t::make(cfg, *cell_list.get_cc_idx(1));
    TESTASSERT(var_meas2.meas_objs().size() == 1);
    TESTASSERT(var_meas2.meas_objs()[0].meas_obj.meas_obj_eutra().carrier_freq == 3400);
  }

  return SRSLTE_SUCCESS;
}

struct mobility_test_params {
  enum class test_event {
    success,
    wrong_measreport,
    concurrent_ho,
    ho_prep_failure,
    duplicate_crnti_ce,
    recover
  } fail_at;
  const char* to_string()
  {
    switch (fail_at) {
      case test_event::success:
        return "success";
      case test_event::wrong_measreport:
        return "wrong measreport";
      case test_event::concurrent_ho:
        return "measreport while in handover";
      case test_event::ho_prep_failure:
        return "ho preparation failure";
      case test_event::recover:
        return "fail and success";
      case test_event::duplicate_crnti_ce:
        return "duplicate CRNTI CE";
      default:
        return "none";
    }
  }
};

struct mobility_tester {
  explicit mobility_tester(const mobility_test_params& args_) : args(args_), rrc(&task_sched)
  {
    rrc_log->set_level(srslte::LOG_LEVEL_INFO);
    rrc_log->set_hex_limit(1024);
  }
  virtual int generate_rrc_cfg() = 0;
  virtual int setup_rrc() { return setup_rrc_common(); }
  int         run_preamble()
  {
    rrc_log->set_level(srslte::LOG_LEVEL_NONE); // mute all the startup log
    // add user
    sched_interface::ue_cfg_t ue_cfg{};
    ue_cfg.supported_cc_list.resize(1);
    ue_cfg.supported_cc_list[0].enb_cc_idx = 0;
    ue_cfg.supported_cc_list[0].active     = true;
    rrc.add_user(rnti, ue_cfg);

    // Do all the handshaking until the first RRC Connection Reconf
    test_helpers::bring_rrc_to_reconf_state(rrc, *task_sched.get_timer_handler(), rnti);
    rrc_log->set_level(srslte::LOG_LEVEL_INFO);
    return SRSLTE_SUCCESS;
  }

  mobility_test_params                        args;
  srslte::scoped_log<srslte::test_log_filter> rrc_log{"RRC"};
  srslte::task_scheduler                      task_sched;
  rrc_cfg_t                                   cfg;

  srsenb::rrc                       rrc;
  test_dummies::mac_mobility_dummy  mac;
  test_dummies::rlc_mobility_dummy  rlc;
  test_dummies::pdcp_mobility_dummy pdcp;
  test_dummies::phy_mobility_dummy  phy;
  test_dummies::s1ap_mobility_dummy s1ap;
  gtpu_dummy                        gtpu;

  void tic()
  {
    task_sched.tic();
    rrc.tti_clock();
    task_sched.run_pending_tasks();
  };

  const uint16_t rnti = 0x46;

protected:
  int generate_rrc_cfg_common()
  {
    srsenb::all_args_t all_args;
    TESTASSERT(test_helpers::parse_default_cfg(&cfg, all_args) == SRSLTE_SUCCESS);
    cfg.meas_cfg_present   = true;
    report_cfg_eutra_s rep = generate_rep1();
    cfg.cell_list[0].meas_cfg.meas_reports.push_back(rep);
    cfg.cell_list[0].meas_cfg.meas_cells.resize(1);
    cfg.cell_list[0].meas_cfg.meas_cells[0]     = generate_cell1();
    cfg.cell_list[0].meas_cfg.meas_cells[0].pci = 2;
    return SRSLTE_SUCCESS;
  }
  int setup_rrc_common()
  {
    rrc.init(cfg, &phy, &mac, &rlc, &pdcp, &s1ap, &gtpu);
    return SRSLTE_SUCCESS;
  }
};

struct s1ap_mobility_tester : public mobility_tester {
  explicit s1ap_mobility_tester(const mobility_test_params& args_) : mobility_tester(args_) {}
  int generate_rrc_cfg() final
  {
    TESTASSERT(generate_rrc_cfg_common() == SRSLTE_SUCCESS);
    cfg.cell_list[0].meas_cfg.meas_cells[0].eci = 0x19C02;
    return SRSLTE_SUCCESS;
  }
};

struct intraenb_mobility_tester : public mobility_tester {
  explicit intraenb_mobility_tester(const mobility_test_params& args_) : mobility_tester(args_) {}
  int generate_rrc_cfg() final
  {
    TESTASSERT(generate_rrc_cfg_common() == SRSLTE_SUCCESS);
    cfg.cell_list[0].meas_cfg.meas_cells[0].eci = 0x19B02;

    cell_cfg_t cell2                 = cfg.cell_list[0];
    cell2.pci                        = 2;
    cell2.cell_id                    = 2;
    cell2.meas_cfg.meas_cells[0].pci = 1;
    cell2.meas_cfg.meas_cells[0].eci = 0x19B01;
    cfg.cell_list.push_back(cell2);

    return SRSLTE_SUCCESS;
  }
};

int test_s1ap_mobility(mobility_test_params test_params)
{
  printf("\n===== TEST: test_s1ap_mobility() for event %s =====\n", test_params.to_string());
  s1ap_mobility_tester         tester{test_params};
  srslte::unique_byte_buffer_t pdu;

  TESTASSERT(tester.generate_rrc_cfg() == SRSLTE_SUCCESS);
  TESTASSERT(tester.setup_rrc() == SRSLTE_SUCCESS);
  TESTASSERT(tester.run_preamble() == SRSLTE_SUCCESS);
  test_dummies::s1ap_mobility_dummy& s1ap = tester.s1ap;

  /* Receive MeasReport from UE (correct if PCI=2) */
  if (test_params.fail_at == mobility_test_params::test_event::wrong_measreport) {
    uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x0D, 0xBC, 0x80}; // PCI == 3
    test_helpers::copy_msg_to_buffer(pdu, meas_report);
  } else {
    uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x09, 0xBC, 0x80}; // PCI == 2
    test_helpers::copy_msg_to_buffer(pdu, meas_report);
  }
  tester.rrc.write_pdu(tester.rnti, 1, std::move(pdu));
  tester.tic();

  /* Test Case: the MeasReport is not valid */
  if (test_params.fail_at == mobility_test_params::test_event::wrong_measreport) {
    TESTASSERT(s1ap.last_ho_required.rrc_container == nullptr);
    TESTASSERT(tester.rrc_log->warn_counter == 1);
    return SRSLTE_SUCCESS;
  }
  TESTASSERT(s1ap.last_ho_required.rrc_container != nullptr);

  /* Test Case: Multiple concurrent MeasReports arrived. Only one HO procedure should be running */
  if (test_params.fail_at == mobility_test_params::test_event::concurrent_ho) {
    s1ap.last_ho_required = {};
    uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x09, 0xBC, 0x80}; // PCI == 2
    test_helpers::copy_msg_to_buffer(pdu, meas_report);
    tester.rrc.write_pdu(tester.rnti, 1, std::move(pdu));
    tester.tic();
    TESTASSERT(s1ap.last_ho_required.rrc_container == nullptr);
    return SRSLTE_SUCCESS;
  }

  /* Test Case: Check HO Required was sent to S1AP */
  TESTASSERT(s1ap.last_ho_required.rnti == tester.rnti);
  TESTASSERT(s1ap.last_ho_required.target_eci == tester.cfg.cell_list[0].meas_cfg.meas_cells[0].eci);
  TESTASSERT(s1ap.last_ho_required.target_plmn.to_string() == "00101");
  {
    asn1::cbit_ref bref(s1ap.last_ho_required.rrc_container->msg, s1ap.last_ho_required.rrc_container->N_bytes);
    asn1::rrc::ho_prep_info_s hoprep;
    TESTASSERT(hoprep.unpack(bref) == asn1::SRSASN_SUCCESS);
    ho_prep_info_r8_ies_s& hoprepr8 = hoprep.crit_exts.c1().ho_prep_info_r8();
    TESTASSERT(hoprepr8.as_cfg_present);
    // Check if RRC sends the current active bearers
    TESTASSERT(hoprepr8.as_cfg.source_rr_cfg.srb_to_add_mod_list_present);
    TESTASSERT(hoprepr8.as_cfg.source_rr_cfg.srb_to_add_mod_list[0].srb_id == 1);
    TESTASSERT(hoprepr8.as_cfg.source_rr_cfg.srb_to_add_mod_list[1].srb_id == 2);
    TESTASSERT(hoprepr8.as_cfg.source_rr_cfg.drb_to_add_mod_list_present);
    TESTASSERT(hoprepr8.as_cfg.source_rr_cfg.drb_to_add_mod_list[0].drb_id == 1);
  }

  /* Test Case: HandoverPreparation has failed */
  if (test_params.fail_at == mobility_test_params::test_event::ho_prep_failure) {
    tester.rrc.ho_preparation_complete(tester.rnti, false, nullptr);
    //    TESTASSERT(rrc_log->error_counter == 1);
    TESTASSERT(not s1ap.last_enb_status.status_present);
    return SRSLTE_SUCCESS;
  }

  /* MME returns back an HandoverCommand, S1AP unwraps the RRC container */
  uint8_t ho_cmd_rrc_container[] = {0x01, 0xa9, 0x00, 0xd9, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x22, 0x04, 0x00, 0x00,
                                    0x01, 0x48, 0x04, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0xa0, 0x07, 0xa0,
                                    0x10, 0x00, 0x01, 0x00, 0x05, 0x00, 0xa7, 0xd0, 0xc1, 0xf6, 0xaf, 0x3e, 0x12, 0xcc,
                                    0x86, 0x0d, 0x30, 0x00, 0x0b, 0x5a, 0x02, 0x17, 0x86, 0x00, 0x05, 0xa0, 0x20};
  test_helpers::copy_msg_to_buffer(pdu, ho_cmd_rrc_container);
  TESTASSERT(s1ap.last_enb_status.rnti != tester.rnti);
  tester.rrc.ho_preparation_complete(tester.rnti, true, std::move(pdu));
  TESTASSERT(s1ap.last_enb_status.status_present);
  TESTASSERT(tester.rrc_log->error_counter == 0);
  asn1::rrc::dl_dcch_msg_s ho_cmd;
  TESTASSERT(test_helpers::unpack_asn1(ho_cmd, srslte::make_span(tester.pdcp.last_sdu.sdu)));
  auto& recfg_r8 = ho_cmd.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();
  TESTASSERT(recfg_r8.mob_ctrl_info_present);

  return SRSLTE_SUCCESS;
}

int test_s1ap_tenb_mobility(mobility_test_params test_params)
{
  printf("\n===== TEST: test_s1ap_tenb_mobility() for event %s =====\n", test_params.to_string());
  s1ap_mobility_tester         tester{test_params};
  srslte::unique_byte_buffer_t pdu;

  TESTASSERT(tester.generate_rrc_cfg() == SRSLTE_SUCCESS);
  tester.cfg.cell_list[0].meas_cfg.meas_cells[0].eci = 0x19B01;
  tester.cfg.enb_id                                  = 0x19C;
  tester.cfg.cell.id                                 = 0x02;
  tester.cfg.cell_list[0].cell_id                    = 0x02;
  tester.cfg.cell_list[0].pci                        = 2;
  TESTASSERT(tester.setup_rrc() == SRSLTE_SUCCESS);
  security_cfg_handler sec_cfg{tester.cfg};

  /* TeNB receives S1AP Handover Request */
  asn1::s1ap::ho_request_s ho_req;
  ho_req.protocol_ies.erab_to_be_setup_list_ho_req.value.resize(1);
  auto& erab   = ho_req.protocol_ies.erab_to_be_setup_list_ho_req.value[0].value.erab_to_be_setup_item_ho_req();
  erab.erab_id = 5;
  erab.erab_level_qos_params.qci = 9;
  asn1::s1ap::sourceenb_to_targetenb_transparent_container_s container;
  container.target_cell_id.cell_id.from_number(0x19C02);
  uint8_t ho_prep_container[] = {
      0x0a, 0x10, 0x0b, 0x81, 0x80, 0x00, 0x01, 0x80, 0x00, 0xf3, 0x02, 0x08, 0x00, 0x00, 0x15, 0x80, 0x00, 0x14,
      0x06, 0xa4, 0x02, 0xf0, 0x04, 0x04, 0xf0, 0x00, 0x14, 0x80, 0x4a, 0x00, 0x00, 0x00, 0x02, 0x12, 0x31, 0xb6,
      0xf8, 0x3e, 0xa0, 0x6f, 0x05, 0xe4, 0x65, 0x14, 0x1d, 0x39, 0xd0, 0x54, 0x4c, 0x00, 0x02, 0x54, 0x00, 0x20,
      0x04, 0x60, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x05, 0x00, 0x04, 0x14,
      0x00, 0x67, 0x0d, 0xfb, 0xc4, 0x66, 0x06, 0x50, 0x0f, 0x00, 0x08, 0x00, 0x20, 0x80, 0x0c, 0x14, 0xca, 0x2d,
      0x5c, 0xe1, 0x86, 0x35, 0x39, 0x80, 0x0e, 0x06, 0xa4, 0x40, 0x0f, 0x22, 0x78};
  // 0a100b818000018000f3020800001580001406a402f00404f00014804a000000021231b6f83ea06f05e465141d39d0544c00025400200460000000100100c000000000020500041400670dfbc46606500f00080020800c14ca2d5ce1863539800e06a4400f2278
  container.rrc_container.resize(sizeof(ho_prep_container));
  memcpy(container.rrc_container.data(), ho_prep_container, sizeof(ho_prep_container));
  tester.rrc.start_ho_ue_resource_alloc(ho_req, container);
  tester.tic();
  TESTASSERT(tester.rrc.get_nof_users() == 1);
  TESTASSERT(tester.mac.ue_db.count(0x46));
  auto& mac_ue = tester.mac.ue_db[0x46];
  TESTASSERT(mac_ue.supported_cc_list[0].active);
  TESTASSERT(mac_ue.supported_cc_list[0].enb_cc_idx == 0);
  TESTASSERT(mac_ue.ue_bearers[rb_id_t::RB_ID_SRB0].direction == sched_interface::ue_bearer_cfg_t::BOTH);
  // Check Security Configuration
  TESTASSERT(tester.pdcp.bearers.count(0x46));
  TESTASSERT(tester.pdcp.bearers[0x46].count(rb_id_t::RB_ID_SRB1) and
             tester.pdcp.bearers[0x46].count(rb_id_t::RB_ID_SRB2));
  TESTASSERT(tester.pdcp.bearers[0x46][rb_id_t::RB_ID_SRB1].enable_encryption);
  TESTASSERT(tester.pdcp.bearers[0x46][rb_id_t::RB_ID_SRB1].enable_integrity);
  sec_cfg.set_security_capabilities(ho_req.protocol_ies.ue_security_cap.value);
  sec_cfg.set_security_key(ho_req.protocol_ies.security_context.value.next_hop_param);
  sec_cfg.regenerate_keys_handover(tester.cfg.cell_list[0].pci, tester.cfg.cell_list[0].dl_earfcn);
  srslte::as_security_config_t as_sec_cfg = sec_cfg.get_as_sec_cfg();
  TESTASSERT(tester.pdcp.bearers[0x46][rb_id_t::RB_ID_SRB1].sec_cfg.k_rrc_int == as_sec_cfg.k_rrc_int);
  TESTASSERT(tester.pdcp.bearers[0x46][rb_id_t::RB_ID_SRB1].sec_cfg.k_rrc_enc == as_sec_cfg.k_rrc_enc);
  TESTASSERT(tester.pdcp.bearers[0x46][rb_id_t::RB_ID_SRB1].sec_cfg.k_up_int == as_sec_cfg.k_up_int);
  TESTASSERT(tester.pdcp.bearers[0x46][rb_id_t::RB_ID_SRB1].sec_cfg.cipher_algo == as_sec_cfg.cipher_algo);
  TESTASSERT(tester.pdcp.bearers[0x46][rb_id_t::RB_ID_SRB1].sec_cfg.integ_algo == as_sec_cfg.integ_algo);

  // Check if S1AP Handover Request ACK send is called
  TESTASSERT(tester.s1ap.last_ho_req_ack.rnti == 0x46);
  TESTASSERT(tester.s1ap.last_ho_req_ack.ho_cmd_pdu != nullptr);
  TESTASSERT(tester.s1ap.last_ho_req_ack.admitted_bearers.size() ==
             ho_req.protocol_ies.erab_to_be_setup_list_ho_req.value.size());
  ho_cmd_s       ho_cmd;
  asn1::cbit_ref bref{tester.s1ap.last_ho_req_ack.ho_cmd_pdu->msg, tester.s1ap.last_ho_req_ack.ho_cmd_pdu->N_bytes};
  TESTASSERT(ho_cmd.unpack(bref) == asn1::SRSASN_SUCCESS);
  bref = asn1::cbit_ref{ho_cmd.crit_exts.c1().ho_cmd_r8().ho_cmd_msg.data(),
                        ho_cmd.crit_exts.c1().ho_cmd_r8().ho_cmd_msg.size()};
  dl_dcch_msg_s dl_dcch_msg;
  TESTASSERT(dl_dcch_msg.unpack(bref) == asn1::SRSASN_SUCCESS);
  TESTASSERT(dl_dcch_msg.msg.type().value == dl_dcch_msg_type_c::types_opts::c1);
  TESTASSERT(dl_dcch_msg.msg.c1().type().value == dl_dcch_msg_type_c::c1_c_::types_opts::rrc_conn_recfg);
  auto& recfg_r8 = dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  // Receives MMEStatusTransfer
  asn1::s1ap::bearers_subject_to_status_transfer_list_l bearers;
  bearers.resize(1);
  bearers[0].value.bearers_subject_to_status_transfer_item().erab_id                = 5;
  bearers[0].value.bearers_subject_to_status_transfer_item().dl_coun_tvalue.pdcp_sn = 100;
  bearers[0].value.bearers_subject_to_status_transfer_item().dl_coun_tvalue.hfn     = 3;
  bearers[0].value.bearers_subject_to_status_transfer_item().ul_coun_tvalue.pdcp_sn = 120;
  bearers[0].value.bearers_subject_to_status_transfer_item().ul_coun_tvalue.hfn     = 4;
  tester.rrc.set_erab_status(0x46, bearers);
  TESTASSERT(tester.pdcp.bearers.count(0x46));
  TESTASSERT(tester.pdcp.bearers[0x46].count(3));
  TESTASSERT(tester.pdcp.bearers[0x46][3].state.next_pdcp_tx_sn == 100);
  TESTASSERT(tester.pdcp.bearers[0x46][3].state.tx_hfn == 3);
  TESTASSERT(tester.pdcp.bearers[0x46][3].state.next_pdcp_rx_sn == 120);
  TESTASSERT(tester.pdcp.bearers[0x46][3].state.rx_hfn == 4);

  // user PRACHs and sends C-RNTI CE
  sched_interface::ue_cfg_t ue_cfg{};
  ue_cfg.supported_cc_list.resize(1);
  ue_cfg.supported_cc_list[0].enb_cc_idx = 0;
  ue_cfg.supported_cc_list[0].active     = true;
  tester.rrc.add_user(0x47, ue_cfg);
  tester.rrc.upd_user(0x47, 0x46);

  uint8_t recfg_complete[] = {0x10, 0x00};
  test_helpers::copy_msg_to_buffer(pdu, recfg_complete);
  tester.rrc.write_pdu(0x46, rb_id_t::RB_ID_SRB1, std::move(pdu));
  tester.tic();
  TESTASSERT(mac_ue.ue_bearers[rb_id_t::RB_ID_SRB1].direction == sched_interface::ue_bearer_cfg_t::BOTH);
  TESTASSERT(mac_ue.ue_bearers[rb_id_t::RB_ID_SRB2].direction == sched_interface::ue_bearer_cfg_t::BOTH);
  TESTASSERT(mac_ue.ue_bearers[rb_id_t::RB_ID_DRB1].direction == sched_interface::ue_bearer_cfg_t::BOTH);
  TESTASSERT(mac_ue.pucch_cfg.I_sr == recfg_r8.rr_cfg_ded.phys_cfg_ded.sched_request_cfg.setup().sr_cfg_idx);
  TESTASSERT(mac_ue.pucch_cfg.n_pucch_sr ==
             recfg_r8.rr_cfg_ded.phys_cfg_ded.sched_request_cfg.setup().sr_pucch_res_idx);

  return SRSLTE_SUCCESS;
}

int test_intraenb_mobility(mobility_test_params test_params)
{
  printf("\n===== TEST: test_intraenb_mobility() for event %s =====\n", test_params.to_string());
  intraenb_mobility_tester     tester{test_params};
  srslte::unique_byte_buffer_t pdu;

  TESTASSERT(tester.generate_rrc_cfg() == SRSLTE_SUCCESS);
  TESTASSERT(tester.setup_rrc() == SRSLTE_SUCCESS);
  TESTASSERT(tester.run_preamble() == SRSLTE_SUCCESS);
  tester.pdcp.last_sdu.sdu = nullptr;
  tester.rlc.test_reset_all();
  tester.phy.phy_cfg_set = false;

  /* Receive MeasReport from UE (correct if PCI=2) */
  if (test_params.fail_at == mobility_test_params::test_event::wrong_measreport) {
    uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x0D, 0xBC, 0x80}; // PCI == 3
    test_helpers::copy_msg_to_buffer(pdu, meas_report);
  } else {
    uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x09, 0xBC, 0x80}; // PCI == 2
    test_helpers::copy_msg_to_buffer(pdu, meas_report);
  }
  tester.rrc.write_pdu(tester.rnti, 1, std::move(pdu));
  tester.tic();
  TESTASSERT(tester.s1ap.last_ho_required.rrc_container == nullptr);

  /* Test Case: the MeasReport is not valid */
  if (test_params.fail_at == mobility_test_params::test_event::wrong_measreport) {
    TESTASSERT(tester.rrc_log->warn_counter == 1);
    TESTASSERT(tester.pdcp.last_sdu.sdu == nullptr);
    return SRSLTE_SUCCESS;
  }
  TESTASSERT(tester.pdcp.last_sdu.sdu != nullptr);
  TESTASSERT(tester.s1ap.last_ho_required.rrc_container == nullptr);
  TESTASSERT(not tester.s1ap.last_enb_status.status_present);

  /* Test Case: Multiple concurrent MeasReports arrived. Only one HO procedure should be running */
  if (test_params.fail_at == mobility_test_params::test_event::concurrent_ho) {
    tester.pdcp.last_sdu  = {};
    uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x09, 0xBC, 0x80}; // PCI == 2
    test_helpers::copy_msg_to_buffer(pdu, meas_report);
    tester.rrc.write_pdu(tester.rnti, 1, std::move(pdu));
    tester.tic();
    TESTASSERT(tester.pdcp.last_sdu.sdu == nullptr);
    return SRSLTE_SUCCESS;
  }

  /* Test Case: the HandoverCommand was sent to the lower layers */
  TESTASSERT(tester.rrc_log->error_counter == 0);
  TESTASSERT(tester.pdcp.last_sdu.rnti == tester.rnti);
  TESTASSERT(tester.pdcp.last_sdu.lcid == 1); // SRB1
  asn1::rrc::dl_dcch_msg_s ho_cmd;
  TESTASSERT(test_helpers::unpack_asn1(ho_cmd, srslte::make_span(tester.pdcp.last_sdu.sdu)));
  auto& recfg_r8 = ho_cmd.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();
  TESTASSERT(recfg_r8.mob_ctrl_info_present);
  TESTASSERT(recfg_r8.mob_ctrl_info.new_ue_id.to_number() == tester.rnti);
  TESTASSERT(recfg_r8.mob_ctrl_info.target_pci == 2);
  TESTASSERT(recfg_r8.rr_cfg_ded_present);
  TESTASSERT(recfg_r8.rr_cfg_ded.phys_cfg_ded_present);
  const asn1::rrc::phys_cfg_ded_s& phy_cfg_ded = recfg_r8.rr_cfg_ded.phys_cfg_ded;
  TESTASSERT(phy_cfg_ded.sched_request_cfg_present);
  TESTASSERT(phy_cfg_ded.cqi_report_cfg_present);
  // PHY should not be updated until the UE handovers to the new cell
  TESTASSERT(not tester.phy.phy_cfg_set);

  /* Test Case: The UE sends a C-RNTI CE. Bearers are reestablished, PHY is configured */
  tester.pdcp.last_sdu.sdu = nullptr;
  tester.rrc.upd_user(tester.rnti + 1, tester.rnti);
  TESTASSERT(tester.rlc.ue_db[tester.rnti].reest_sdu_counter == 0);
  TESTASSERT(tester.pdcp.last_sdu.sdu == nullptr);
  TESTASSERT(tester.phy.phy_cfg_set);
  TESTASSERT(tester.phy.last_cfg.size() == 1 and tester.mac.ue_db[tester.rnti].supported_cc_list.size() == 1);
  TESTASSERT(tester.phy.last_cfg[0].enb_cc_idx == tester.mac.ue_db[tester.rnti].supported_cc_list[0].enb_cc_idx);

  /* Test Case: The UE receives a duplicate C-RNTI CE. Nothing should happen */
  if (test_params.fail_at == mobility_test_params::test_event::duplicate_crnti_ce) {
    TESTASSERT(tester.rlc.ue_db[tester.rnti].reest_sdu_counter == 0);
    tester.rrc.upd_user(tester.rnti + 2, tester.rnti);
    TESTASSERT(tester.rlc.ue_db[tester.rnti].reest_sdu_counter == 0);
    TESTASSERT(tester.pdcp.last_sdu.sdu == nullptr);
    TESTASSERT(tester.rlc.ue_db[tester.rnti].last_sdu == nullptr); // No Reject sent
  }

  /* Test Case: Terminate first Handover. No extra messages should be sent DL. SR/CQI resources match recfg message */
  uint8_t recfg_complete[] = {0x10, 0x00};
  test_helpers::copy_msg_to_buffer(pdu, recfg_complete);
  tester.rrc.write_pdu(tester.rnti, rb_id_t::RB_ID_SRB2, std::move(pdu));
  TESTASSERT(tester.pdcp.last_sdu.sdu == nullptr);
  sched_interface::ue_cfg_t& ue_cfg = tester.mac.ue_db[tester.rnti];
  TESTASSERT(ue_cfg.pucch_cfg.sr_configured);
  TESTASSERT(ue_cfg.pucch_cfg.n_pucch_sr == phy_cfg_ded.sched_request_cfg.setup().sr_pucch_res_idx);
  TESTASSERT(ue_cfg.pucch_cfg.I_sr == phy_cfg_ded.sched_request_cfg.setup().sr_cfg_idx);
  TESTASSERT(ue_cfg.supported_cc_list[0].dl_cfg.cqi_report.pmi_idx ==
             phy_cfg_ded.cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx);
  TESTASSERT(ue_cfg.pucch_cfg.n_pucch == phy_cfg_ded.cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx);

  /* Test Case: The RRC should be able to start a new handover */
  uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x05, 0xBC, 0x80}; // PCI == 1
  test_helpers::copy_msg_to_buffer(pdu, meas_report);
  tester.rrc.write_pdu(tester.rnti, 1, std::move(pdu));
  tester.tic();
  TESTASSERT(tester.s1ap.last_ho_required.rrc_container == nullptr);
  TESTASSERT(tester.pdcp.last_sdu.sdu != nullptr);
  TESTASSERT(tester.s1ap.last_ho_required.rrc_container == nullptr);
  TESTASSERT(not tester.s1ap.last_enb_status.status_present);
  TESTASSERT(test_helpers::unpack_asn1(ho_cmd, srslte::make_span(tester.pdcp.last_sdu.sdu)));
  recfg_r8 = ho_cmd.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();
  TESTASSERT(recfg_r8.mob_ctrl_info_present);
  TESTASSERT(recfg_r8.mob_ctrl_info.new_ue_id.to_number() == tester.rnti);
  TESTASSERT(recfg_r8.mob_ctrl_info.target_pci == 1);

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_INFO);
  using event = mobility_test_params::test_event;

  if (argc < 3) {
    argparse::usage(argv[0]);
    return -1;
  }
  argparse::parse_args(argc, argv);
  TESTASSERT(test_correct_insertion() == 0);
  TESTASSERT(test_correct_meascfg_calculation() == 0);

  // S1AP Handover
  TESTASSERT(test_s1ap_mobility(mobility_test_params{event::wrong_measreport}) == 0);
  TESTASSERT(test_s1ap_mobility(mobility_test_params{event::concurrent_ho}) == 0);
  TESTASSERT(test_s1ap_mobility(mobility_test_params{event::ho_prep_failure}) == 0);
  TESTASSERT(test_s1ap_mobility(mobility_test_params{event::success}) == 0);

  TESTASSERT(test_s1ap_tenb_mobility(mobility_test_params{event::success}) == 0);

  // intraeNB Handover
  TESTASSERT(test_intraenb_mobility(mobility_test_params{event::wrong_measreport}) == 0);
  TESTASSERT(test_intraenb_mobility(mobility_test_params{event::concurrent_ho}) == 0);
  TESTASSERT(test_intraenb_mobility(mobility_test_params{event::duplicate_crnti_ce}) == 0);
  TESTASSERT(test_intraenb_mobility(mobility_test_params{event::success}) == 0);

  printf("\nSuccess\n");

  return SRSLTE_SUCCESS;
}
