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
  cell_cfg.enable_phr_handling            = true;
  sched_interface::sched_args_t sched_cfg = {};
  sched_cell_params_t           cell_params;
  cell_params.set_cfg(0, cell_cfg, sched_cfg);
  sched_interface::ue_cfg_t ue_cfg = generate_default_ue_cfg();

  sched_ue_cell ue_cc(0x46, cell_params, tti_point(0));
  ue_cc.set_ue_cfg(ue_cfg);

  float snr = 0;
  ue_cc.set_ul_snr(tti_point(0), snr, 0);
  ue_cc.set_ul_snr(tti_point(0), snr, 1);
  ue_cc.tpc_fsm.set_phr(-5);
  ue_cc.new_tti(tti_point(0));

  uint32_t req_bytes    = 10000;
  uint32_t pending_prbs = get_required_prb_ul(ue_cc, req_bytes);
  TESTASSERT(pending_prbs < 10); // The PHR<0 is limiting the number of allocated PRBs

  uint32_t N_srs          = 0;
  uint32_t prb_grant_size = pending_prbs;
  uint32_t nof_symb       = 2 * (SRSRAN_CP_NSYMB(cell_cfg.cell.cp) - 1) - N_srs;
  uint32_t nof_re         = nof_symb * prb_grant_size * SRSRAN_NRE;
  tbs_info tbinfo         = cqi_to_tbs_ul(ue_cc, prb_grant_size, nof_re, req_bytes);
  TESTASSERT(tbinfo.tbs_bytes >= 10);
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

  srslog::flush();

  srsran::console("Success\n");
}
