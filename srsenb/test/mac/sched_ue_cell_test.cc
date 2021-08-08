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

#include "sched_test_common.h"
#include "sched_test_utils.h"
#include "srsenb/hdr/stack/mac/sched_ue.h"
#include "srsran/common/test_common.h"

using namespace srsenb;
const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

/**
 * Test scenario where PHR < 0, and the UL grant size is limited.
 * - The UL grant size should be the smallest possible that guarantees fitting both a BSR, RLC header, and some RLC
 * payload.
 */
void test_neg_phr_scenario()
{
  sched_interface::cell_cfg_t cell_cfg    = generate_default_cell_cfg(50);
  cell_cfg.target_pucch_ul_sinr           = 20;
  cell_cfg.target_pusch_ul_sinr           = 20;
  cell_cfg.min_phr_thres                  = 0;
  cell_cfg.enable_phr_handling            = true;
  sched_interface::sched_args_t sched_cfg = {};
  sched_cell_params_t           cell_params;
  cell_params.set_cfg(0, cell_cfg, sched_cfg);
  sched_interface::ue_cfg_t ue_cfg = generate_default_ue_cfg();

  sched_ue_cell ue_cc(0x46, cell_params, tti_point(0));
  ue_cc.set_ue_cfg(ue_cfg);

  float     snr = 20;
  tti_point tti{0};
  for (; ue_cc.tpc_fsm.get_ul_snr_estim(0) < snr - 2; ++tti) {
    ue_cc.set_ul_snr(tti, snr, 0);
    ue_cc.set_ul_snr(tti, snr, 1);
    ue_cc.tpc_fsm.set_phr(-5, 1);
    ue_cc.new_tti(tti);
  }

  uint32_t req_bytes    = 10000;
  uint32_t pending_prbs = get_required_prb_ul(ue_cc, req_bytes);
  TESTASSERT(pending_prbs == 1); // The PHR<0 is limiting the number of allocated PRBs

  uint32_t N_srs          = 0;
  uint32_t prb_grant_size = pending_prbs;
  uint32_t nof_symb       = 2 * (SRSRAN_CP_NSYMB(cell_cfg.cell.cp) - 1) - N_srs;
  uint32_t nof_re         = nof_symb * prb_grant_size * SRSRAN_NRE;
  tbs_info tbinfo         = cqi_to_tbs_ul(ue_cc, prb_grant_size, nof_re, req_bytes);
  TESTASSERT(tbinfo.tbs_bytes >= 10);
}

void test_interferer_subband_cqi_scenario()
{
  uint32_t                      Nprb      = 50;
  sched_interface::cell_cfg_t   cell_cfg  = generate_default_cell_cfg(Nprb);
  sched_interface::sched_args_t sched_cfg = {};
  sched_cell_params_t           cell_params;
  cell_params.set_cfg(0, cell_cfg, sched_cfg);
  sched_interface::ue_cfg_t ue_cfg = generate_default_ue_cfg();

  sched_ue_cell ue_cc(0x46, cell_params, tti_point(0));
  ue_cfg.supported_cc_list[0].dl_cfg.cqi_report.subband_wideband_ratio = 4;
  ue_cfg.supported_cc_list[0].dl_cfg.cqi_report.periodic_configured    = true;
  ue_cc.set_ue_cfg(ue_cfg);

  TESTASSERT(ue_cc.dl_cqi().subband_cqi_enabled());
  TESTASSERT(ue_cc.dl_cqi().nof_bandwidth_parts() == 3);
  TESTASSERT(ue_cc.dl_cqi().nof_subbands() == 9);

  ue_cc.set_dl_wb_cqi(tti_point{0}, 10);
  ue_cc.set_dl_sb_cqi(tti_point{40}, 1, 15);
  ue_cc.set_dl_sb_cqi(tti_point{80}, 3, 15);
  ue_cc.set_dl_sb_cqi(tti_point{160}, 8, 0); // interferer in last BP

  rbgmask_t test_mask(cell_params.nof_rbgs);
  test_mask.fill(0, 12);

  rbgmask_t rbgs(cell_params.nof_rbgs);
  tbs_info  tb;
  rbgmask_t grant_mask(cell_params.nof_rbgs);
  TESTASSERT(find_optimal_rbgmask(ue_cc,
                                  tti_point{160 + TX_ENB_DELAY},
                                  rbgs,
                                  SRSRAN_DCI_FORMAT1,
                                  srsran::interval<uint32_t>{0, 10000},
                                  tb,
                                  grant_mask));
  TESTASSERT(grant_mask == test_mask);

  ue_cc.set_dl_wb_cqi(tti_point{0}, 15);
  ue_cc.set_dl_sb_cqi(tti_point{40}, 1, 15);
  ue_cc.set_dl_sb_cqi(tti_point{80}, 3, 15);
  ue_cc.set_dl_sb_cqi(tti_point{160}, 8, 10); // interferer in last BP
  TESTASSERT(find_optimal_rbgmask(ue_cc,
                                  tti_point{160 + TX_ENB_DELAY},
                                  rbgs,
                                  SRSRAN_DCI_FORMAT1,
                                  srsran::interval<uint32_t>{0, 10000},
                                  tb,
                                  grant_mask));
  TESTASSERT(grant_mask == test_mask);

  ue_cc.set_dl_wb_cqi(tti_point{0}, 15);
  ue_cc.set_dl_sb_cqi(tti_point{40}, 1, 15);
  ue_cc.set_dl_sb_cqi(tti_point{80}, 3, 15);
  ue_cc.set_dl_sb_cqi(tti_point{160}, 8, 14); // interferer in last BP
  TESTASSERT(find_optimal_rbgmask(ue_cc,
                                  tti_point{160 + TX_ENB_DELAY},
                                  rbgs,
                                  SRSRAN_DCI_FORMAT1,
                                  srsran::interval<uint32_t>{0, 10000},
                                  tb,
                                  grant_mask));
  test_mask.reset();
  test_mask.fill(0, cell_params.nof_rbgs);
  TESTASSERT(grant_mask == test_mask);
}

int main()
{
  srsenb::set_randseed(seed);
  srsran::console("This is the chosen seed: %u\n", seed);

  auto& test_log = srslog::fetch_basic_logger("TEST", false);
  test_log.set_level(srslog::basic_levels::info);

  // Start the log backend.
  srslog::init();

  test_neg_phr_scenario();
  test_interferer_subband_cqi_scenario();

  srslog::flush();

  srsran::console("Success\n");
}
