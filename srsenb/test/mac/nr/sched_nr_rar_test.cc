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
#include "srsenb/hdr/stack/mac/nr/sched_nr_cell.h"
#include "srsran/common/test_common.h"
#include "srsran/support/srsran_test.h"
#include <random>

namespace srsenb {

void test_single_prach()
{
  using namespace sched_nr_impl;
  const uint16_t               rnti       = 0x1234;
  static srslog::basic_logger& mac_logger = srslog::fetch_basic_logger("MAC");
  std::random_device           r;
  std::default_random_engine   rgen(r());

  sched_nr_interface::sched_cfg_t             sched_cfg{};
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(1);
  sched_params                                schedparams{sched_cfg};
  schedparams.cells.emplace_back(0, cells_cfg[0], sched_cfg);
  bwp_params    bwp_cfg(cells_cfg[0], sched_cfg, 0, 0);
  slot_ue_map_t slot_ues;

  ra_sched rasched(bwp_cfg);
  TESTASSERT(rasched.empty());

  std::unique_ptr<bwp_res_grid> res_grid(new bwp_res_grid{bwp_cfg});
  bwp_slot_allocator            alloc(*res_grid);

  // Create UE
  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(1);
  ue                           u(rnti, uecfg, schedparams);

  slot_point pdcch_slot{0, TX_ENB_DELAY};
  slot_point prach_slot{0, std::uniform_int_distribution<uint32_t>{TX_ENB_DELAY, 20}(rgen)};

  const bwp_slot_grid* result   = nullptr;
  auto                 run_slot = [&alloc, &rasched, &pdcch_slot, &slot_ues, &u]() -> const bwp_slot_grid* {
    mac_logger.set_context(pdcch_slot.to_uint());
    slot_ues.clear();
    slot_ues.insert(rnti, u.try_reserve(pdcch_slot, 0));
    alloc.new_slot(pdcch_slot);
    rasched.run_slot(alloc, slot_ues);
    ++pdcch_slot;
    return &alloc.res_grid()[alloc.get_pdcch_tti()];
  };

  // Start Run

  for (; pdcch_slot - TX_ENB_DELAY < prach_slot;) {
    result = run_slot();
    TESTASSERT(result->dl_pdcchs.empty());
  }

  sched_nr_interface::dl_sched_rar_info_t rainfo{};
  rainfo.preamble_idx = 10;
  rainfo.temp_crnti   = rnti;
  rainfo.prach_slot   = prach_slot;
  rainfo.msg3_size    = 7;
  TESTASSERT_SUCCESS(rasched.dl_rach_info(rainfo));
  uint16_t ra_rnti = 1 + rainfo.ofdm_symbol_idx + 14 * rainfo.prach_slot.slot_idx() + 14 * 80 * rainfo.freq_idx;

  while (true) {
    result = run_slot();
    if (result->is_dl()) {
      TESTASSERT(result->dl_pdcchs.size() == 1);
      const auto& pdcch = result->dl_pdcchs[0];
      TESTASSERT_EQ(pdcch.dci.ctx.rnti, ra_rnti);
      TESTASSERT_EQ(pdcch.dci.ctx.rnti_type, srsran_rnti_type_ra);
      break;
    } else {
      TESTASSERT(result->dl_pdcchs.empty());
    }
  }
}

} // namespace srsenb

int main(int argc, char** argv)
{
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::info);
  auto& mac_logger = srslog::fetch_basic_logger("MAC");
  mac_logger.set_level(srslog::basic_levels::info);

  srsran::test_init(argc, argv);

  srsenb::test_single_prach();
}
