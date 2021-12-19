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

#include "sched_nr_cfg_generators.h"
#include "sched_nr_sim_ue.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/test_common.h"

namespace srsenb {

class sched_tester : public sched_nr_base_tester
{
public:
  using sched_nr_base_tester::sched_nr_base_tester;

  void process_slot_result(const sim_nr_enb_ctxt_t& enb_ctxt, srsran::const_span<cc_result_t> cc_out) override
  {
    for (auto& cc : cc_out) {
      for (auto& pdsch : cc.res.dl->phy.pdsch) {
        if (pdsch.sch.grant.rnti_type == srsran_rnti_type_c or pdsch.sch.grant.rnti_type == srsran_rnti_type_tc) {
          ue_metrics[pdsch.sch.grant.rnti].nof_dl_txs++;
          ue_metrics[pdsch.sch.grant.rnti].nof_dl_bytes += pdsch.sch.grant.tb[0].tbs / 8u;
        }
      }
      for (auto& pusch : cc.res.ul->pusch) {
        if (pusch.sch.grant.rnti_type == srsran_rnti_type_c or pusch.sch.grant.rnti_type == srsran_rnti_type_tc) {
          ue_metrics[pusch.sch.grant.rnti].nof_ul_txs++;
          ue_metrics[pusch.sch.grant.rnti].nof_ul_bytes += pusch.sch.grant.tb[0].tbs / 8u;
        }
      }
    }
  }

  void print_results()
  {
    srslog::flush();
    fmt::print("SCHED UE metrics:\n");
    for (auto& u : ue_metrics) {
      fmt::print("  0x{:x}: nof_txs=({}, {}), nof_bytes=({}, {})\n",
                 u.first,
                 u.second.nof_dl_txs,
                 u.second.nof_ul_txs,
                 u.second.nof_dl_bytes,
                 u.second.nof_ul_bytes);
    }
  }

  struct sched_ue_metrics {
    uint32_t nof_dl_txs = 0, nof_ul_txs = 0;
    uint64_t nof_dl_bytes = 0, nof_ul_bytes = 0;
  };
  std::map<uint16_t, sched_ue_metrics> ue_metrics;
};

struct sched_event_t {
  uint32_t                                   slot_count;
  std::function<void(sched_nr_base_tester&)> run;
};

sched_event_t add_user(uint32_t slot_count, uint16_t rnti, uint32_t preamble_idx)
{
  auto task = [rnti, preamble_idx](sched_nr_base_tester& tester) {
    tester.add_user(rnti, get_rach_ue_cfg(0), tester.get_slot_tx() - TX_ENB_DELAY, preamble_idx);
  };
  return sched_event_t{slot_count, task};
}

sched_event_t ue_cfg(uint32_t slot_count, uint16_t rnti, const sched_nr_ue_cfg_t& ue_cfg)
{
  auto task = [rnti, ue_cfg](sched_nr_base_tester& tester) { tester.user_cfg(rnti, ue_cfg); };
  return sched_event_t{slot_count, task};
}

sched_event_t add_rlc_dl_bytes(uint32_t slot_count, uint16_t rnti, uint32_t lcid, uint32_t pdu_size)
{
  auto task = [rnti, pdu_size, lcid](sched_nr_base_tester& tester) { tester.add_rlc_dl_bytes(rnti, lcid, pdu_size); };
  return sched_event_t{slot_count, task};
}

void test_sched_nr_no_data()
{
  uint32_t max_nof_ttis = 1000, nof_sectors = 1;
  uint16_t rnti = 0x4601;

  sched_nr_interface::sched_args_t cfg;
  cfg.auto_refill_buffer                                = false;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  std::string  test_name = "Test with no data";
  sched_tester tester(cfg, cells_cfg, test_name);

  /* Set events */
  std::deque<sched_event_t> events;
  events.push_back(add_user(9, rnti, 0));
  events.push_back(ue_cfg(20, rnti, get_default_ue_cfg(1)));

  /* Run Test */
  for (uint32_t nof_slots = 0; nof_slots < max_nof_ttis; ++nof_slots) {
    slot_point slot_rx(0, nof_slots % 10240);
    slot_point slot_tx = slot_rx + TX_ENB_DELAY;

    // run events
    while (not events.empty() and events.front().slot_count <= nof_slots) {
      events.front().run(tester);
      events.pop_front();
    }

    // call sched
    tester.run_slot(slot_tx);
  }

  tester.print_results();

  // Since DL buffers were not externally updated, we should only see Msg4 as DL tx
  TESTASSERT_EQ(1, tester.ue_metrics[rnti].nof_dl_txs);
  // Since UL buffers were not externally updated, we should only see Msg3 as UL tx
  TESTASSERT_EQ(1, tester.ue_metrics[rnti].nof_ul_txs);
}

void test_sched_nr_data()
{
  uint32_t max_nof_ttis = 1000, nof_sectors = 1;
  uint16_t rnti               = 0x4601;
  uint32_t nof_dl_bytes_to_tx = 2e6;

  sched_nr_interface::sched_args_t cfg;
  cfg.auto_refill_buffer                                = false;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  std::string  test_name = "Test with data";
  sched_tester tester(cfg, cells_cfg, test_name);

  /* Set events */
  std::deque<sched_event_t> events;
  events.push_back(add_user(9, rnti, 0));
  events.push_back(ue_cfg(20, rnti, get_default_ue_cfg(1)));
  events.push_back(add_rlc_dl_bytes(50, rnti, 0, nof_dl_bytes_to_tx));

  /* Run Test */
  for (uint32_t nof_slots = 0; nof_slots < max_nof_ttis; ++nof_slots) {
    slot_point slot_rx(0, nof_slots % 10240);
    slot_point slot_tx = slot_rx + TX_ENB_DELAY;

    // run events
    while (not events.empty() and events.front().slot_count <= nof_slots) {
      events.front().run(tester);
      events.pop_front();
    }

    // call sched
    tester.run_slot(slot_tx);
  }

  tester.print_results();

  TESTASSERT(tester.ue_metrics[rnti].nof_dl_txs > 1);
  TESTASSERT(tester.ue_metrics[rnti].nof_dl_bytes >= nof_dl_bytes_to_tx);
  // Since UL buffers were not externally updated, we should only see Msg3 as UL tx
  TESTASSERT_EQ(1, tester.ue_metrics[rnti].nof_ul_txs);
}

} // namespace srsenb

int main()
{
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::info);
  auto& mac_nr_logger = srslog::fetch_basic_logger("MAC-NR");
  mac_nr_logger.set_level(srslog::basic_levels::debug);
  auto& pool_logger = srslog::fetch_basic_logger("POOL");
  pool_logger.set_level(srslog::basic_levels::debug);

  // Start the log backend.
  srslog::init();

  srsenb::test_sched_nr_no_data();
  srsenb::test_sched_nr_data();
}
