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
#include "sched_nr_common_test.h"
#include "srsgnb/hdr/stack/mac/sched_nr_bwp.h"
#include "srsran/common/test_common.h"
#include "srsran/support/srsran_test.h"
#include <random>

uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

namespace srsenb {

void test_single_prach()
{
  using namespace sched_nr_impl;
  const uint16_t               rnti       = 0x1234;
  static srslog::basic_logger& mac_logger = srslog::fetch_basic_logger("MAC");
  std::default_random_engine   rand_gen(seed);
  std::default_random_engine   rgen(rand_gen());

  // Set scheduler configuration
  sched_nr_interface::sched_args_t sched_cfg{};
  sched_cfg.auto_refill_buffer = std::uniform_int_distribution<uint32_t>{0, 1}(rgen) > 0;

  // Set cells configuration
  std::vector<sched_nr_cell_cfg_t> cells_cfg = get_default_cells_cfg(1);
  sched_params_t                   schedparams{sched_cfg};
  schedparams.cells.emplace_back(0, cells_cfg[0], sched_cfg);
  const bwp_params_t& bwpparams = schedparams.cells[0].bwps[0];
  slot_ue_map_t       slot_ues;

  ra_sched rasched(bwpparams);
  TESTASSERT(rasched.empty());

  std::unique_ptr<bwp_res_grid> res_grid(new bwp_res_grid{bwpparams});

  // Create UE
  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(1);
  ue                           u(rnti, uecfg, schedparams);

  slot_point pdcch_slot{0, TX_ENB_DELAY};
  slot_point prach_slot{0, std::uniform_int_distribution<uint32_t>{TX_ENB_DELAY, 20}(rgen)};

  const bwp_slot_grid* result   = nullptr;
  auto                 run_slot = [&res_grid, &rasched, &pdcch_slot, &slot_ues, &u]() -> const bwp_slot_grid* {
    mac_logger.set_context(pdcch_slot.to_uint());

    // delete old outputs
    (*res_grid)[pdcch_slot - TX_ENB_DELAY - 1].reset();

    // setup UE state for slot
    u.new_slot(pdcch_slot);

    // pre-calculate UE slot vars
    slot_ues.clear();
    slot_ue sfu = u.make_slot_ue(pdcch_slot, 0);
    if (not sfu.empty()) {
      slot_ues.insert(rnti, std::move(sfu));
    }
    bwp_slot_allocator alloc(*res_grid, pdcch_slot, slot_ues);

    rasched.run_slot(alloc);

    log_sched_bwp_result(mac_logger, alloc.get_pdcch_tti(), alloc.res_grid(), slot_ues);
    const bwp_slot_grid* result = &alloc.res_grid()[alloc.get_pdcch_tti()];
    test_dl_pdcch_consistency(res_grid->cfg->cell_cfg, result->dl.phy.pdcch_dl);
    ++pdcch_slot;
    return result;
  };

  // Start Run

  for (; pdcch_slot - TX_ENB_DELAY < prach_slot;) {
    result = run_slot();
    TESTASSERT(result->dl.phy.pdcch_dl.empty());
  }

  // A PRACH arrives...
  sched_nr_interface::rar_info_t rainfo{};
  rainfo.ofdm_symbol_idx = 0;
  rainfo.preamble_idx    = 10;
  rainfo.temp_crnti      = rnti;
  rainfo.prach_slot      = prach_slot;
  rainfo.msg3_size       = 7;
  TESTASSERT_SUCCESS(rasched.dl_rach_info(rainfo));
  uint16_t ra_rnti = 1 + rainfo.ofdm_symbol_idx + 14 * rainfo.prach_slot.slot_idx() + 14 * 80 * rainfo.freq_idx;

  // RAR is scheduled
  const uint32_t prach_duration = 1;
  slot_point     rar_slot;
  while (true) {
    slot_point current_slot = pdcch_slot;
    result                  = run_slot();
    if (bwpparams.slots[current_slot.slot_idx()].is_dl and
        bwpparams.slots[(current_slot + bwpparams.pusch_ra_list[0].msg3_delay).slot_idx()].is_ul) {
      TESTASSERT_EQ(1, result->dl.phy.pdcch_dl.size());
      const auto& pdcch = result->dl.phy.pdcch_dl[0];
      TESTASSERT_EQ(pdcch.dci.ctx.rnti, ra_rnti);
      TESTASSERT_EQ(pdcch.dci.ctx.rnti_type, srsran_rnti_type_ra);
      TESTASSERT(current_slot < prach_slot + prach_duration + bwpparams.cfg.rar_window_size);
      rar_slot = current_slot;
      break;
    } else {
      TESTASSERT(result->dl.phy.pdcch_dl.empty());
    }
  }

  slot_point msg3_slot = rar_slot + bwpparams.pusch_ra_list[0].msg3_delay;
  while (pdcch_slot <= msg3_slot) {
    result = run_slot();
  }
  TESTASSERT(result->ul.pusch.size() == 1);
}

} // namespace srsenb

int main(int argc, char** argv)
{
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::info);
  auto& mac_logger = srslog::fetch_basic_logger("MAC");
  mac_logger.set_level(srslog::basic_levels::info);

  srsran::test_init(argc, argv);

  printf("Test random seed=%u\n\n", seed);

  srsenb::test_single_prach();
}
