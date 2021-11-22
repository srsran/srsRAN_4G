/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_nr_test_helpers.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr_config_utils.h"
#include "srsgnb/src/stack/mac/test/sched_nr_cfg_generators.h"
#include "srsran/common/test_common.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"
#include <iostream>

using namespace asn1::rrc_nr;

namespace srsenb {

int test_cell_cfg(const srsenb::sched_interface::cell_cfg_t& cellcfg)
{
  // SIB1 must exist and have period 16rf
  TESTASSERT(cellcfg.sibs[0].len > 0);
  TESTASSERT(cellcfg.sibs[0].period_rf == 16);

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
  set_derived_nr_cell_params(rrc_cfg_nr.is_standalone, rrc_cfg_nr.cell_list[0]);
  srsran_assert(check_rrc_nr_cfg_valid(rrc_cfg_nr) == SRSRAN_SUCCESS, "Invalid RRC NR configuration");

  TESTASSERT(rrc_obj.init(rrc_cfg_nr, &phy_obj, &mac_obj, &rlc_obj, &pdcp_obj, nullptr, nullptr, nullptr) ==
             SRSRAN_SUCCESS);

  const sched_nr_interface::cell_cfg_t& nrcell = mac_obj.nr_cells.at(0);

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
  rrc_nr                 rrc_obj(&task_sched);

  // set cfg
  rrc_nr_cfg_t rrc_cfg_nr = rrc_nr_cfg_t{};
  rrc_cfg_nr.cell_list.emplace_back();
  generate_default_nr_cell(rrc_cfg_nr.cell_list[0]);
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.pci     = 500;
  rrc_cfg_nr.cell_list[0].dl_arfcn                 = 634240;
  rrc_cfg_nr.cell_list[0].band                     = 78;
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.nof_prb = 52;
  rrc_cfg_nr.is_standalone                         = false;
  set_derived_nr_cell_params(rrc_cfg_nr.is_standalone, rrc_cfg_nr.cell_list[0]);
  srsran_assert(check_rrc_nr_cfg_valid(rrc_cfg_nr) == SRSRAN_SUCCESS, "Invalid RRC NR configuration");
  TESTASSERT(rrc_obj.init(rrc_cfg_nr, &phy_obj, &mac_obj, &rlc_obj, &pdcp_obj, nullptr, nullptr, nullptr) ==
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
  ngap_dummy             ngap_obj;

  rrc_nr rrc_obj(&task_sched);

  // set cfg
  rrc_nr_cfg_t rrc_cfg_nr = rrc_nr_cfg_t{};
  rrc_cfg_nr.cell_list.emplace_back();
  generate_default_nr_cell(rrc_cfg_nr.cell_list[0]);
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.pci     = 500;
  rrc_cfg_nr.cell_list[0].dl_arfcn                 = 368500;
  rrc_cfg_nr.cell_list[0].band                     = 3;
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.nof_prb = 52;
  rrc_cfg_nr.cell_list[0].duplex_mode              = SRSRAN_DUPLEX_MODE_FDD;
  rrc_cfg_nr.is_standalone                         = true;
  set_derived_nr_cell_params(rrc_cfg_nr.is_standalone, rrc_cfg_nr.cell_list[0]);
  srsran_assert(check_rrc_nr_cfg_valid(rrc_cfg_nr) == SRSRAN_SUCCESS, "Invalid RRC NR configuration");

  TESTASSERT(rrc_obj.init(rrc_cfg_nr, &phy_obj, &mac_obj, &rlc_obj, &pdcp_obj, &ngap_obj, nullptr, nullptr) ==
             SRSRAN_SUCCESS);

  sched_nr_ue_cfg_t uecfg                     = get_default_ue_cfg(1);
  uecfg.phy_cfg.pdcch                         = rrc_cfg_nr.cell_list[0].phy_cell.pdcch;
  uecfg.phy_cfg.pdcch.search_space_present[2] = false;
  TESTASSERT_SUCCESS(rrc_obj.add_user(0x4601, uecfg));

  test_rrc_nr_connection_establishment(task_sched, rrc_obj, rlc_obj, mac_obj, 0x4601);
  test_rrc_nr_security_mode_cmd(task_sched, rrc_obj, pdcp_obj, 0x4601);
}

} // namespace srsenb

int main(int argc, char** argv)
{
  auto& logger = srslog::fetch_basic_logger("ASN1");
  logger.set_level(srslog::basic_levels::info);
  auto& rrc_logger = srslog::fetch_basic_logger("RRC-NR");
  rrc_logger.set_level(srslog::basic_levels::debug);

  srslog::init();

  srsenb::test_sib_generation();
  TESTASSERT(srsenb::test_rrc_setup() == SRSRAN_SUCCESS);
  srsenb::test_rrc_sa_connection();

  return SRSRAN_SUCCESS;
}
