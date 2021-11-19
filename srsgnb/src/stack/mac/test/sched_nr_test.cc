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

#include "sched_nr_cfg_generators.h"
#include "sched_nr_sim_ue.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/test_common.h"

namespace srsenb {

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

void run_sched_nr_test()
{
  uint32_t max_nof_ttis = 1000, nof_sectors = 1;
  uint16_t rnti = 0x4601;

  sched_nr_interface::sched_args_t cfg;
  cfg.auto_refill_buffer                                = false;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  std::string          test_name = "Serialized Test";
  sched_nr_base_tester tester(cfg, cells_cfg, test_name);

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

  srsenb::run_sched_nr_test();
}
