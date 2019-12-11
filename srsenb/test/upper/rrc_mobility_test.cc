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

#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srsenb/src/enb_cfg_parser.h"
#include "srsenb/test/common/dummy_classes.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/test_common.h"
#include <iostream>
#include <srslte/common/log_filter.h>

using namespace srsenb;
using namespace asn1::rrc;

srslte::scoped_tester_log log_h("ALL");

namespace argparse {

std::string            repository_dir;
srslte::LOG_LEVEL_ENUM log_level;

void usage(char* prog)
{
  printf("Usage: %s [v] -i repository_dir\n", prog);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "i")) != -1) {
    switch (opt) {
      case 'i':
        repository_dir = argv[optind];
        break;
      case 'v':
        log_level = srslte::LOG_LEVEL_DEBUG;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (repository_dir.empty()) {
    usage(argv[0]);
    exit(-1);
  }
}

} // namespace argparse

namespace test_dummies {

class s1ap_mobility_dummy : public s1ap_dummy
{
public:
  struct ho_req_data {
    uint16_t                     rnti;
    uint32_t                     target_eci;
    srslte::plmn_id_t            target_plmn;
    srslte::unique_byte_buffer_t rrc_container;
  } last_ho_required;

  bool send_ho_required(uint16_t                     rnti,
                        uint32_t                     target_eci,
                        srslte::plmn_id_t            target_plmn,
                        srslte::unique_byte_buffer_t rrc_container) final
  {
    last_ho_required = ho_req_data{rnti, target_eci, target_plmn, std::move(rrc_container)};
    return true;
  }
};

class pdcp_mobility_dummy : public pdcp_dummy
{
public:
  struct last_sdu_t {
    uint16_t                     rnti;
    uint32_t                     lcid;
    srslte::unique_byte_buffer_t sdu;
  } last_sdu;

  void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) override
  {
    last_sdu.rnti = rnti;
    last_sdu.lcid = lcid;
    last_sdu.sdu  = std::move(sdu);
  }
};

} // namespace test_dummies

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
  var_meas_cfg_t            src_var(&log_h), target_var(&log_h);

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

namespace test_helpers {

int  parse_default_cfg(rrc_cfg_t* rrc_cfg, srsenb::all_args_t& args);
void copy_msg_to_buffer(srslte::unique_byte_buffer_t& pdu, uint8_t* msg, size_t nof_bytes);
int  bring_rrc_to_reconf_state(srsenb::rrc& rrc, srslte::timer_handler& timers, uint16_t rnti);

} // namespace test_helpers

struct mobility_test_params {
  enum class test_fail_at { success, wrong_measreport, concurrent_ho, ho_prep_failure } fail_at;
  const char* to_string()
  {
    switch (fail_at) {
      case test_fail_at::success:
        return "success";
      case test_fail_at::wrong_measreport:
        return "wrong measreport";
      case test_fail_at::concurrent_ho:
        return "measreport while in handover";
      case test_fail_at::ho_prep_failure:
        return "ho preparation failure";
      default:
        return "none";
    }
  }
};

int test_mobility_class(mobility_test_params test_params)
{
  printf("\n===== TEST: test_mobility_class() for event \"%s\" =====\n", test_params.to_string());
  srslte::scoped_tester_log    rrc_log("RRC ");
  srslte::timer_handler        timers;
  srslte::unique_byte_buffer_t pdu;

  srsenb::all_args_t args;
  rrc_cfg_t          cfg;
  TESTASSERT(test_helpers::parse_default_cfg(&cfg, args) == SRSLTE_SUCCESS);
  report_cfg_eutra_s rep = generate_rep1();
  cfg.meas_cfg.meas_reports.push_back(rep);
  meas_cell_cfg_t cell2 = generate_cell1();
  cell2.pci             = 2;
  cell2.eci             = 0x19C02;
  cfg.meas_cfg.meas_cells.push_back(cell2);

  srsenb::rrc                       rrc;
  mac_dummy                         mac;
  rlc_dummy                         rlc;
  test_dummies::pdcp_mobility_dummy pdcp;
  phy_dummy                         phy;
  test_dummies::s1ap_mobility_dummy s1ap;
  gtpu_dummy                        gtpu;
  rrc_log.set_level(srslte::LOG_LEVEL_INFO);
  rrc_log.set_hex_limit(1024);
  rrc.init(&cfg, &phy, &mac, &rlc, &pdcp, &s1ap, &gtpu, &timers, &rrc_log);

  auto tic = [&timers, &rrc] {
    timers.step_all();
    rrc.tti_clock();
  };

  uint16_t rnti = 0x46;
  rrc.add_user(rnti);

  rrc_log.set_level(srslte::LOG_LEVEL_NONE); // mute all the startup log
  // Do all the handshaking until the first RRC Connection Reconf
  test_helpers::bring_rrc_to_reconf_state(rrc, timers, rnti);

  rrc_log.set_level(srslte::LOG_LEVEL_INFO);

  /* Receive MeasReport from UE (correct if PCI=2) */
  if (test_params.fail_at == mobility_test_params::test_fail_at::wrong_measreport) {
    uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x0D, 0xBC, 0x80}; // PCI == 3
    test_helpers::copy_msg_to_buffer(pdu, meas_report, sizeof(meas_report));
  } else {
    uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x09, 0xBC, 0x80}; // PCI == 2
    test_helpers::copy_msg_to_buffer(pdu, meas_report, sizeof(meas_report));
  }
  rrc.write_pdu(rnti, 1, std::move(pdu));
  tic();

  /* Test Case: the MeasReport is not valid */
  if (test_params.fail_at == mobility_test_params::test_fail_at::wrong_measreport) {
    TESTASSERT(s1ap.last_ho_required.rrc_container == nullptr);
    TESTASSERT(rrc_log.error_counter == 1);
    return SRSLTE_SUCCESS;
  }

  /* Test Case: Multiple concurrent MeasReports arrived. Only one HO procedure should be running */
  if (test_params.fail_at == mobility_test_params::test_fail_at::concurrent_ho) {
    s1ap.last_ho_required = {};
    uint8_t meas_report[] = {0x08, 0x10, 0x38, 0x74, 0x00, 0x09, 0xBC, 0x80}; // PCI == 2
    test_helpers::copy_msg_to_buffer(pdu, meas_report, sizeof(meas_report));
    rrc.write_pdu(rnti, 1, std::move(pdu));
    tic();
    TESTASSERT(s1ap.last_ho_required.rrc_container == nullptr);
    TESTASSERT(rrc_log.error_counter == 1);
    return SRSLTE_SUCCESS;
  }

  /* Check HO Required was sent to S1AP */
  TESTASSERT(s1ap.last_ho_required.rnti == rnti);
  TESTASSERT(s1ap.last_ho_required.target_eci == cell2.eci);
  TESTASSERT(s1ap.last_ho_required.target_plmn.to_string() == "00101");
  {
    asn1::bit_ref bref(s1ap.last_ho_required.rrc_container->msg, s1ap.last_ho_required.rrc_container->N_bytes);
    asn1::rrc::ho_prep_info_s hoprep;
    TESTASSERT(hoprep.unpack(bref) == asn1::SRSASN_SUCCESS);
    ho_prep_info_r8_ies_s& hoprepr8 = hoprep.crit_exts.c1().ho_prep_info_r8();
    TESTASSERT(hoprepr8.as_cfg_present);
    // Check if RRC sends the current active bearers
    TESTASSERT(hoprepr8.as_cfg.source_rr_cfg.drb_to_add_mod_list_present);
    TESTASSERT(hoprepr8.as_cfg.source_rr_cfg.drb_to_add_mod_list[0].drb_id == 1);
  }

  /* Test Case: HandoverPreparation has failed */
  if (test_params.fail_at == mobility_test_params::test_fail_at::ho_prep_failure) {
    rrc.ho_preparation_complete(rnti, false, nullptr);
    TESTASSERT(rrc_log.error_counter == 1);
    return SRSLTE_SUCCESS;
  }

  /* MME returns back an HandoverCommand, S1AP unwraps the RRC container */
  uint8_t ho_cmd_rrc_container[] = {0x01, 0xa9, 0x00, 0xd9, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x22, 0x04, 0x00, 0x00,
                                    0x01, 0x48, 0x04, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0xa0, 0x07, 0xa0,
                                    0x10, 0x00, 0x01, 0x00, 0x05, 0x00, 0xa7, 0xd0, 0xc1, 0xf6, 0xaf, 0x3e, 0x12, 0xcc,
                                    0x86, 0x0d, 0x30, 0x00, 0x0b, 0x5a, 0x02, 0x17, 0x86, 0x00, 0x05, 0xa0, 0x20};
  test_helpers::copy_msg_to_buffer(pdu, ho_cmd_rrc_container, sizeof(ho_cmd_rrc_container));
  rrc.ho_preparation_complete(rnti, true, std::move(pdu));

  TESTASSERT(rrc_log.error_counter == 0);
  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  log_h.set_level(srslte::LOG_LEVEL_INFO);

  if (argc < 3) {
    argparse::usage(argv[0]);
    return -1;
  }
  argparse::parse_args(argc, argv);

  TESTASSERT(test_correct_insertion() == 0);
  TESTASSERT(test_correct_meascfg_calculation() == 0);
  TESTASSERT(test_mobility_class(mobility_test_params{mobility_test_params::test_fail_at::wrong_measreport}) == 0);
  TESTASSERT(test_mobility_class(mobility_test_params{mobility_test_params::test_fail_at::concurrent_ho}) == 0);
  TESTASSERT(test_mobility_class(mobility_test_params{mobility_test_params::test_fail_at::ho_prep_failure}) == 0);
  TESTASSERT(test_mobility_class(mobility_test_params{mobility_test_params::test_fail_at::success}) == 0);

  printf("\nSuccess\n");

  return 0;
}

namespace test_helpers {

int parse_default_cfg(rrc_cfg_t* rrc_cfg, srsenb::all_args_t& args)
{
  args                      = {};
  *rrc_cfg                  = {};
  args.enb_files.sib_config = argparse::repository_dir + "/sib.conf.example";
  args.enb_files.rr_config  = argparse::repository_dir + "/rr.conf.example";
  args.enb_files.drb_config = argparse::repository_dir + "/drb.conf.example";
  log_h.debug("sib file path=%s\n", args.enb_files.sib_config.c_str());

  args.enb.dl_earfcn = 3400;
  args.enb.n_prb     = 50;
  TESTASSERT(srslte::string_to_mcc("001", &args.stack.s1ap.mcc));
  TESTASSERT(srslte::string_to_mnc("01", &args.stack.s1ap.mnc));
  args.enb.transmission_mode = 1;
  args.enb.nof_ports         = 1;
  args.general.eia_pref_list = "EIA2, EIA1, EIA0";
  args.general.eea_pref_list = "EEA0, EEA2, EEA1";

  phy_cfg_t phy_cfg;

  return enb_conf_sections::parse_cfg_files(&args, rrc_cfg, &phy_cfg);
}

void copy_msg_to_buffer(srslte::unique_byte_buffer_t& pdu, uint8_t* msg, size_t nof_bytes)
{
  srslte::byte_buffer_pool* pool = srslte::byte_buffer_pool::get_instance();
  pdu                            = srslte::allocate_unique_buffer(*pool, true);
  memcpy(pdu->msg, msg, nof_bytes);
  pdu->N_bytes = nof_bytes;
};

int bring_rrc_to_reconf_state(srsenb::rrc& rrc, srslte::timer_handler& timers, uint16_t rnti)
{
  srslte::unique_byte_buffer_t pdu;

  // Send RRCConnectionRequest
  uint8_t rrc_conn_request[] = {0x40, 0x12, 0xf6, 0xfb, 0xe2, 0xc6};
  copy_msg_to_buffer(pdu, rrc_conn_request, sizeof(rrc_conn_request));
  rrc.write_pdu(rnti, 0, std::move(pdu));
  timers.step_all();
  rrc.tti_clock();

  // Send RRCConnectionSetupComplete
  uint8_t rrc_conn_setup_complete[] = {0x20, 0x00, 0x40, 0x2e, 0x90, 0x50, 0x49, 0xe8, 0x06, 0x0e, 0x82, 0xa2,
                                       0x17, 0xec, 0x13, 0xe2, 0x0f, 0x00, 0x02, 0x02, 0x5e, 0xdf, 0x7c, 0x58,
                                       0x05, 0xc0, 0xc0, 0x00, 0x08, 0x04, 0x03, 0xa0, 0x23, 0x23, 0xc0};
  copy_msg_to_buffer(pdu, rrc_conn_setup_complete, sizeof(rrc_conn_setup_complete));
  rrc.write_pdu(rnti, 1, std::move(pdu));
  timers.step_all();
  rrc.tti_clock();

  // S1AP receives InitialContextSetupRequest and forwards it to RRC
  uint8_t s1ap_init_ctxt_setup_req[] = {
      0x00, 0x09, 0x00, 0x80, 0xc6, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x64, 0x00, 0x08, 0x00, 0x02, 0x00,
      0x01, 0x00, 0x42, 0x00, 0x0a, 0x18, 0x3b, 0x9a, 0xca, 0x00, 0x60, 0x3b, 0x9a, 0xca, 0x00, 0x00, 0x18, 0x00, 0x78,
      0x00, 0x00, 0x34, 0x00, 0x73, 0x45, 0x00, 0x09, 0x3c, 0x0f, 0x80, 0x0a, 0x00, 0x21, 0xf0, 0xb7, 0x36, 0x1c, 0x56,
      0x64, 0x27, 0x3e, 0x5b, 0x04, 0xb7, 0x02, 0x07, 0x42, 0x02, 0x3e, 0x06, 0x00, 0x09, 0xf1, 0x07, 0x00, 0x07, 0x00,
      0x37, 0x52, 0x66, 0xc1, 0x01, 0x09, 0x1b, 0x07, 0x74, 0x65, 0x73, 0x74, 0x31, 0x32, 0x33, 0x06, 0x6d, 0x6e, 0x63,
      0x30, 0x37, 0x30, 0x06, 0x6d, 0x63, 0x63, 0x39, 0x30, 0x31, 0x04, 0x67, 0x70, 0x72, 0x73, 0x05, 0x01, 0xc0, 0xa8,
      0x03, 0x02, 0x27, 0x0e, 0x80, 0x80, 0x21, 0x0a, 0x03, 0x00, 0x00, 0x0a, 0x81, 0x06, 0x08, 0x08, 0x08, 0x08, 0x50,
      0x0b, 0xf6, 0x09, 0xf1, 0x07, 0x80, 0x01, 0x01, 0xf6, 0x7e, 0x72, 0x69, 0x13, 0x09, 0xf1, 0x07, 0x00, 0x01, 0x23,
      0x05, 0xf4, 0xf6, 0x7e, 0x72, 0x69, 0x00, 0x6b, 0x00, 0x05, 0x18, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x49, 0x00, 0x20,
      0x45, 0x25, 0xe4, 0x9a, 0x77, 0xc8, 0xd5, 0xcf, 0x26, 0x33, 0x63, 0xeb, 0x5b, 0xb9, 0xc3, 0x43, 0x9b, 0x9e, 0xb3,
      0x86, 0x1f, 0xa8, 0xa7, 0xcf, 0x43, 0x54, 0x07, 0xae, 0x42, 0x2b, 0x63, 0xb9};
  LIBLTE_S1AP_S1AP_PDU_STRUCT s1ap_pdu;
  LIBLTE_BYTE_MSG_STRUCT      byte_buf;
  byte_buf.N_bytes = sizeof(s1ap_init_ctxt_setup_req);
  memcpy(byte_buf.msg, s1ap_init_ctxt_setup_req, byte_buf.N_bytes);
  liblte_s1ap_unpack_s1ap_pdu(&byte_buf, &s1ap_pdu);
  rrc.setup_ue_ctxt(rnti, &s1ap_pdu.choice.initiatingMessage.choice.InitialContextSetupRequest);
  timers.step_all();
  rrc.tti_clock();

  // Send SecurityModeComplete
  uint8_t sec_mode_complete[] = {0x28, 0x00};
  copy_msg_to_buffer(pdu, sec_mode_complete, sizeof(sec_mode_complete));
  rrc.write_pdu(rnti, 1, std::move(pdu));
  timers.step_all();
  rrc.tti_clock();

  return SRSLTE_SUCCESS;
}

} // namespace test_helpers
