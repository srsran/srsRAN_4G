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

#include "sched_nr_cfg_generators.h"
#include "sched_nr_sim_ue.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/test_common.h"
#include <chrono>

namespace srsenb {

using dl_sched_t = sched_nr_interface::dl_sched_t;

static const srsran::phy_cfg_nr_t default_phy_cfg =
    srsran::phy_cfg_nr_default_t{srsran::phy_cfg_nr_default_t::reference_cfg_t{}};

class sched_nr_tester : public sched_nr_base_test_bench
{
public:
  using sched_nr_base_test_bench::sched_nr_base_test_bench;

  void process_slot_result(const sim_nr_enb_ctxt_t& slot_ctxt, srsran::const_span<cc_result_t> cc_list) override
  {
    tot_latency_sched_ns +=
        std::max_element(cc_list.begin(), cc_list.end(), [](const cc_result_t& lhs, const cc_result_t& rhs) {
          return lhs.cc_latency_ns < rhs.cc_latency_ns;
        })->cc_latency_ns.count();

    for (auto& cc_out : cc_list) {
      pdsch_count += cc_out.res.dl->phy.pdcch_dl.size();
      cc_res_count++;

      bool is_dl_slot = srsran_duplex_nr_is_dl(&cell_params[cc_out.res.cc].duplex, 0, current_slot_tx.slot_idx());

      if (is_dl_slot) {
        if (cc_out.res.dl->phy.ssb.empty() and not slot_ctxt.ue_db.empty()) {
          TESTASSERT(slot_ctxt.ue_db.empty() or cc_out.res.dl->phy.pdcch_dl.size() >= 1);
        } else {
          TESTASSERT(cc_out.res.dl->phy.pdcch_dl.size() == 0);
        }
      }
    }
  }

  void print_results() const
  {
    test_logger.info("TESTER: %f PDSCH/{slot,cc} were allocated", pdsch_count / (double)cc_res_count);
    srslog::flush();
  }

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  uint64_t tot_latency_sched_ns = 0;
  uint32_t cc_res_count         = 0;
  uint32_t pdsch_count          = 0;
};

void run_sched_nr_test(uint32_t nof_workers)
{
  srsran_assert(nof_workers > 0, "There must be at least one worker");
  uint32_t max_nof_ttis = 1000, nof_sectors = 4;
  uint16_t rnti = 0x4601;

  sched_nr_interface::sched_args_t cfg;
  cfg.auto_refill_buffer = true;

  std::vector<sched_nr_cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  std::string test_name = "Serialized Test";
  if (nof_workers > 1) {
    test_name = fmt::format("Parallel Test with {} workers", nof_workers);
  }
  sched_nr_tester tester(cfg, cells_cfg, test_name, nof_workers);

  for (uint32_t nof_slots = 0; nof_slots < max_nof_ttis; ++nof_slots) {
    slot_point slot_rx(0, nof_slots % 10240);
    slot_point slot_tx = slot_rx + TX_ENB_DELAY;
    if (nof_slots == 9) {
      sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(nof_sectors);
      uecfg.lc_ch_to_add.emplace_back();
      uecfg.lc_ch_to_add.back().lcid          = 1;
      uecfg.lc_ch_to_add.back().cfg.direction = mac_lc_ch_cfg_t::BOTH;
      tester.user_cfg(rnti, uecfg);
    }
    tester.run_slot(slot_tx);
  }

  tester.stop();
  tester.print_results();
  TESTASSERT(tester.pdsch_count > 0);
  //  TESTASSERT(tester.pdsch_count == (int)(max_nof_ttis * nof_sectors * 0.6));

  double final_avg_usec = tester.tot_latency_sched_ns;
  final_avg_usec        = final_avg_usec / 1000.0 / max_nof_ttis;
  printf("Total time taken per slot: %f usec\n", final_avg_usec);
}

} // namespace srsenb

int main()
{
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::warning);
  auto& mac_nr_logger = srslog::fetch_basic_logger("MAC-NR");
  mac_nr_logger.set_level(srslog::basic_levels::warning);
  auto& pool_logger = srslog::fetch_basic_logger("POOL");
  pool_logger.set_level(srslog::basic_levels::debug);

  // Start the log backend.
  srslog::init();

  srsenb::run_sched_nr_test(1);
  srsenb::run_sched_nr_test(2);
  srsenb::run_sched_nr_test(4);
}
