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

#include "sched_nr_ue_ded_test_suite.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_grant_allocator.h"
#include "srsran/common/test_common.h"

namespace srsenb {

using namespace srsenb::sched_nr_impl;

void test_dl_sched_result(const sim_nr_enb_ctxt_t& enb_ctxt, const sched_nr_cc_result_view& cc_out)
{
  slot_point             pdcch_slot = cc_out.slot;
  const pdcch_dl_list_t& pdcchs     = cc_out.dl_cc_result.dl_sched.pdcch_dl;

  // Iterate over UE PDCCH allocations
  for (const pdcch_dl_t& pdcch : pdcchs) {
    if (pdcch.dci.ctx.rnti_type != srsran_rnti_type_c) {
      continue;
    }
    const sim_nr_ue_ctxt_t& ue = *enb_ctxt.ue_db.at(pdcch.dci.ctx.rnti);
    uint32_t                k1 = ue.ue_cfg.phy_cfg.harq_ack
                      .dl_data_to_ul_ack[pdcch_slot.slot_idx() % ue.ue_cfg.phy_cfg.harq_ack.nof_dl_data_to_ul_ack];

    // CHECK: Carrier activation
    TESTASSERT(ue.ue_cfg.carriers[cc_out.cc].active);

    // CHECK: Coreset chosen/DCI content
    TESTASSERT(ue.ue_cfg.phy_cfg.pdcch.coreset_present[pdcch.dci.ctx.coreset_id]);
    const auto& coreset = ue.ue_cfg.phy_cfg.pdcch.coreset[pdcch.dci.ctx.coreset_id];
    TESTASSERT(coreset.id == pdcch.dci.ctx.coreset_id);

    // CHECK: UCI
    if (pdcch.dci.ctx.format == srsran_dci_format_nr_1_0) {
      TESTASSERT(pdcch.dci.harq_feedback == k1 - 1);
    } else {
      TESTASSERT(pdcch.dci.harq_feedback == pdcch_slot.slot_idx());
    }
    TESTASSERT(ue.cc_list[cc_out.cc].pending_acks[(pdcch_slot + k1).to_uint()] % 4 == pdcch.dci.dai);
  }
}

} // namespace srsenb
