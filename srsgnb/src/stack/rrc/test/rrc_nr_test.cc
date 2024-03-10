/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "rrc_nr_test_helpers.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr_config_utils.h"
#include "srsgnb/src/stack/mac/test/sched_nr_cfg_generators.h"
#include "srsran/common/bearer_manager.h"
#include "srsran/common/test_common.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"
#include <iostream>

using namespace asn1::rrc_nr;

namespace srsenb {

int test_cell_cfg(const srsenb::sched_interface::cell_cfg_t& cellcfg)
{
  // SIB1 must exist and have period 16rf
  TESTASSERT(!cellcfg.sibs[0].is_segmented());
  TESTASSERT(cellcfg.sibs[0].get_length() > 0);
  TESTASSERT(cellcfg.sibs[0].get_period_rf() == 16);

  TESTASSERT(cellcfg.si_window_ms > 0);
  return SRSRAN_SUCCESS;
}

/*
 * Test 1 - Test default SIB generation
 * Description: Check whether the SIBs were set correctly
 */
void test_sib_generation()
{
  srsran::test_delimit_logger test_logger{"SIB generation"};

  srsran::task_scheduler task_sched;
  phy_nr_dummy           phy_obj;
  mac_nr_dummy           mac_obj;
  rlc_dummy              rlc_obj;
  pdcp_dummy             pdcp_obj;
  rrc_nr                 rrc_obj(&task_sched);
  enb_bearer_manager     bearer_mapper;

  // set cfg
  rrc_nr_cfg_t rrc_cfg_nr = {};
  rrc_cfg_nr.cell_list.emplace_back();
  generate_default_nr_cell(rrc_cfg_nr.cell_list[0]);
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.pci     = 500;
  rrc_cfg_nr.cell_list[0].dl_arfcn                 = 368500;
  rrc_cfg_nr.cell_list[0].band                     = 3;
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.nof_prb = 52;
  rrc_cfg_nr.cell_list[0].duplex_mode              = SRSRAN_DUPLEX_MODE_FDD;
  rrc_cfg_nr.is_standalone                         = true;
  rrc_cfg_nr.enb_id                                = 0x19B;
  srsran::string_to_mcc("001", &rrc_cfg_nr.mcc);
  srsran::string_to_mnc("01", &rrc_cfg_nr.mnc);
  set_derived_nr_cell_params(rrc_cfg_nr.is_standalone, rrc_cfg_nr.cell_list[0]);

  TESTASSERT(
      rrc_obj.init(rrc_cfg_nr, &phy_obj, &mac_obj, &rlc_obj, &pdcp_obj, nullptr, nullptr, bearer_mapper, nullptr) ==
      SRSRAN_SUCCESS);

  const sched_nr_cell_cfg_t& nrcell = mac_obj.nr_cells.at(0);

  TESTASSERT(nrcell.sibs.size() > 0);

  // TEST SIB1
  TESTASSERT(nrcell.sibs[0].len > 0);
  TESTASSERT_EQ(16, nrcell.sibs[0].period_rf);

  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  TESTASSERT_EQ(SRSRAN_SUCCESS, rrc_obj.read_pdu_bcch_dlsch(0, *pdu));
  TESTASSERT(pdu->size() > 0);
  asn1::rrc_nr::bcch_dl_sch_msg_s msg;
  {
    asn1::cbit_ref bref{pdu->data(), pdu->size()};
    TESTASSERT_EQ(SRSRAN_SUCCESS, msg.unpack(bref));
  }
  TESTASSERT_EQ(bcch_dl_sch_msg_type_c::types_opts::c1, msg.msg.type().value);
  TESTASSERT_EQ(bcch_dl_sch_msg_type_c::c1_c_::types_opts::sib_type1, msg.msg.c1().type().value);
  asn1::rrc_nr::sib1_s& sib1 = msg.msg.c1().sib_type1();
  TESTASSERT(sib1.serving_cell_cfg_common_present);

  pdcch_cfg_common_s& pdcch = sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup();
  TESTASSERT(not pdcch.ctrl_res_set_zero_present); // CORESET#0 id is passed in MIB
  TESTASSERT(not pdcch.search_space_zero_present); // SS#0 id is passed in MIB
}

int test_rrc_setup()
{
  srsran::test_delimit_logger test_logger{"NSA RRC"};

  srsran::task_scheduler task_sched;
  phy_nr_dummy           phy_obj;
  mac_nr_dummy           mac_obj;
  rlc_dummy              rlc_obj;
  pdcp_dummy             pdcp_obj;
  enb_bearer_manager     bearer_mapper;
  rrc_nr                 rrc_obj(&task_sched);

  // set cfg
  rrc_nr_cfg_t rrc_cfg_nr = {};
  rrc_cfg_nr.cell_list.emplace_back();
  generate_default_nr_cell(rrc_cfg_nr.cell_list[0]);
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.pci     = 500;
  rrc_cfg_nr.cell_list[0].dl_arfcn                 = 634240;
  rrc_cfg_nr.cell_list[0].coreset0_idx             = 3;
  rrc_cfg_nr.cell_list[0].band                     = 78;
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.nof_prb = 52;
  rrc_cfg_nr.is_standalone                         = false;
  rrc_cfg_nr.enb_id                                = 0x19B;
  srsran::string_to_mcc("001", &rrc_cfg_nr.mcc);
  srsran::string_to_mnc("01", &rrc_cfg_nr.mnc);
  set_derived_nr_cell_params(rrc_cfg_nr.is_standalone, rrc_cfg_nr.cell_list[0]);
  TESTASSERT(
      rrc_obj.init(rrc_cfg_nr, &phy_obj, &mac_obj, &rlc_obj, &pdcp_obj, nullptr, nullptr, bearer_mapper, nullptr) ==
      SRSRAN_SUCCESS);

  for (uint32_t n = 0; n < 2; ++n) {
    uint32_t timeout = 5500;
    for (uint32_t i = 0; i < timeout and rlc_obj.last_sdu == nullptr; ++i) {
      task_sched.tic();
    }
    // TODO: trigger proper RRC Setup procedure (not timer based)
    // TESTASSERT(rlc_obj.last_sdu != nullptr);
  }
  return SRSRAN_SUCCESS;
}

void test_rrc_sa_connection()
{
  srsran::test_delimit_logger test_logger{"SA RRCConnectionEstablishment"};

  srsran::task_scheduler task_sched;
  phy_nr_dummy           phy_obj;
  mac_nr_dummy           mac_obj;
  rlc_nr_rrc_tester      rlc_obj;
  pdcp_nr_rrc_tester     pdcp_obj;
  ngap_rrc_tester        ngap_obj;
  enb_bearer_manager     bearer_mapper;

  rrc_nr rrc_obj(&task_sched);

  // Dummy RLC/PDCP configs
  asn1::rrc_nr::rlc_cfg_c rlc_cfg;
  rlc_cfg.set_um_bi_dir();
  rlc_cfg.um_bi_dir().dl_um_rlc.t_reassembly = t_reassembly_e::ms50;

  // set cfg
  rrc_nr_cfg_t rrc_cfg_nr = {};
  rrc_cfg_nr.cell_list.emplace_back();
  generate_default_nr_cell(rrc_cfg_nr.cell_list[0]);
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.pci     = 500;
  rrc_cfg_nr.cell_list[0].dl_arfcn                 = 368500;
  rrc_cfg_nr.cell_list[0].band                     = 3;
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.nof_prb = 52;
  rrc_cfg_nr.cell_list[0].duplex_mode              = SRSRAN_DUPLEX_MODE_FDD;
  rrc_cfg_nr.is_standalone                         = true;
  rrc_cfg_nr.enb_id                                = 0x19B;
  rrc_cfg_nr.five_qi_cfg[9].configured             = true;
  rrc_cfg_nr.five_qi_cfg[9].rlc_cfg                = rlc_cfg;
  rrc_cfg_nr.five_qi_cfg[9].pdcp_cfg               = {};
  srsran::string_to_mcc("001", &rrc_cfg_nr.mcc);
  srsran::string_to_mnc("01", &rrc_cfg_nr.mnc);
  set_derived_nr_cell_params(rrc_cfg_nr.is_standalone, rrc_cfg_nr.cell_list[0]);

  TESTASSERT(
      rrc_obj.init(rrc_cfg_nr, &phy_obj, &mac_obj, &rlc_obj, &pdcp_obj, &ngap_obj, nullptr, bearer_mapper, nullptr) ==
      SRSRAN_SUCCESS);

  TESTASSERT_SUCCESS(rrc_obj.add_user(0x4601, 0));
  TESTASSERT_SUCCESS(rrc_obj.ue_set_security_cfg_key(0x4601, {}));

  test_rrc_nr_connection_establishment(task_sched, rrc_obj, rlc_obj, mac_obj, ngap_obj, 0x4601);
  test_rrc_nr_info_transfer(task_sched, rrc_obj, pdcp_obj, ngap_obj, 0x4601);
  test_rrc_nr_security_mode_cmd(task_sched, rrc_obj, pdcp_obj, 0x4601);
  test_rrc_nr_ue_capability_enquiry(task_sched, rrc_obj, pdcp_obj, 0x4601);
  test_rrc_nr_reconfiguration(task_sched, rrc_obj, pdcp_obj, ngap_obj, 0x4601);
  test_rrc_nr_2nd_reconfiguration(task_sched, rrc_obj, pdcp_obj, ngap_obj, 0x4601);
}

} // namespace srsenb

int main(int argc, char** argv)
{
  // Setup the log spy to intercept error and warning log entries.
  if (!srslog::install_custom_sink(
          srsran::log_sink_spy::name(),
          std::unique_ptr<srsran::log_sink_spy>(new srsran::log_sink_spy(srslog::get_default_log_formatter())))) {
    return SRSRAN_ERROR;
  }

  auto* spy = static_cast<srsran::log_sink_spy*>(srslog::find_sink(srsran::log_sink_spy::name()));
  if (!spy) {
    return SRSRAN_ERROR;
  }

  auto& logger = srslog::fetch_basic_logger("ASN1", *spy, true);
  logger.set_level(srslog::basic_levels::info);
  auto& test_log = srslog::fetch_basic_logger("RRC-NR", *spy, true);
  test_log.set_level(srslog::basic_levels::debug);

  srslog::init();

  srsenb::test_sib_generation();
  TESTASSERT(srsenb::test_rrc_setup() == SRSRAN_SUCCESS);
  srsenb::test_rrc_sa_connection();
  TESTASSERT_EQ(0, spy->get_warning_counter());
  TESTASSERT_EQ(0, spy->get_error_counter());

  return SRSRAN_SUCCESS;
}
