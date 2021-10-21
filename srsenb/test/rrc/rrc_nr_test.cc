/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
#include "srsenb/hdr/stack/rrc/rrc_nr.h"
#include "srsenb/test/common/dummy_classes_common.h"
#include "srsenb/test/common/dummy_classes_nr.h"
#include "srsenb/test/rrc/test_helpers.h"
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
int test_sib_generation()
{
  srsran::task_scheduler task_sched;

  mac_nr_dummy mac_obj;
  rlc_dummy    rlc_obj;
  pdcp_dummy   pdcp_obj;
  rrc_nr       rrc_obj(&task_sched);

  // set cfg
  rrc_nr_cfg_t default_cfg = {};
  rrc_nr_cfg_t rrc_cfg     = rrc_obj.update_default_cfg(default_cfg);
  auto&        sched_elem  = rrc_cfg.sib1.si_sched_info.sched_info_list[0];

  TESTASSERT(rrc_obj.init(rrc_cfg, nullptr, &mac_obj, &rlc_obj, &pdcp_obj, nullptr, nullptr, nullptr) ==
             SRSRAN_SUCCESS);

  TESTASSERT(test_cell_cfg(mac_obj.cellcfgobj) == SRSRAN_SUCCESS);
  // TEMP tests
  TESTASSERT(mac_obj.cellcfgobj.sibs[1].len > 0);
  TESTASSERT(mac_obj.cellcfgobj.sibs[1].period_rf == sched_elem.si_periodicity.to_number());
  for (int i = 2; i < 16; ++i) {
    TESTASSERT(mac_obj.cellcfgobj.sibs[i].len == 0);
  }
  TESTASSERT(mac_obj.cellcfgobj.cell.nof_prb == 25);

  return SRSRAN_SUCCESS;
}

int test_rrc_setup()
{
  srsran::task_scheduler task_sched;

  phy_nr_dummy phy_obj;
  mac_nr_dummy mac_obj;
  rlc_dummy    rlc_obj;
  pdcp_dummy   pdcp_obj;
  rrc_nr       rrc_obj(&task_sched);

  // set cfg
  all_args_t   args{};
  phy_cfg_t    phy_cfg{};
  rrc_nr_cfg_t rrc_cfg_nr = rrc_obj.update_default_cfg(rrc_nr_cfg_t{});
  rrc_cfg_nr.cell_list.emplace_back();
  rrc_cfg_nr.cell_list[0].phy_cell.carrier.pci = 500;
  rrc_cfg_nr.cell_list[0].dl_arfcn             = 634240;
  rrc_cfg_nr.cell_list[0].band                 = 78;
  args.enb.n_prb                               = 50;
  enb_conf_sections::set_derived_args_nr(&args, &rrc_cfg_nr, &phy_cfg);
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

} // namespace srsenb

int main(int argc, char** argv)
{
  auto& logger = srslog::fetch_basic_logger("ASN1");
  logger.set_level(srslog::basic_levels::info);

  srslog::init();

  if (argc < 3) {
    argparse::usage(argv[0]);
    return -1;
  }
  argparse::parse_args(argc, argv);

  // FIXME: disabled temporarily until SIB generation is fixed
  // TESTASSERT(srsenb::test_sib_generation() == SRSRAN_SUCCESS);
  TESTASSERT(srsenb::test_rrc_setup() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
