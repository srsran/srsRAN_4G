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

#include "sched_nr_ue_ded_test_suite.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_rb_grid.h"
#include "srsran/common/test_common.h"

namespace srsenb {

using namespace srsenb::sched_nr_impl;

void test_dl_sched_result(const sim_nr_enb_ctxt_t& enb_ctxt, const sched_nr_cc_output_res_t& cc_out)
{
  tti_point              pdcch_tti = cc_out.tti;
  const pdcch_dl_list_t& pdcchs    = cc_out.dl_cc_result->pdcch_dl;
  const pdsch_list_t&    pdschs    = cc_out.dl_cc_result->pdsch;

  // Iterate over UE PDCCH allocations
  for (const pdcch_dl_t& pdcch : pdcchs) {
    if (pdcch.dci.ctx.rnti_type != srsran_rnti_type_c) {
      continue;
    }
    const sim_nr_ue_ctxt_t& ue = *enb_ctxt.ue_db.at(pdcch.dci.ctx.rnti);
    uint32_t                k1 = ue.ue_cfg.phy_cfg.harq_ack
                      .dl_data_to_ul_ack[pdcch_tti.sf_idx() % ue.ue_cfg.phy_cfg.harq_ack.nof_dl_data_to_ul_ack];

    // CHECK: Carrier activation
    TESTASSERT(ue.ue_cfg.carriers[cc_out.cc].active);

    // CHECK: Coreset chosen/DCI content
    TESTASSERT(ue.ue_cfg.phy_cfg.pdcch.coreset_present[pdcch.dci.ctx.coreset_id]);
    const auto& coreset = ue.ue_cfg.phy_cfg.pdcch.coreset[pdcch.dci.ctx.coreset_id];
    TESTASSERT(coreset.id == pdcch.dci.ctx.coreset_id);
    TESTASSERT(pdcch.dci.ctx.format == srsran_dci_format_nr_1_0 or pdcch.dci.ctx.format == srsran_dci_format_nr_1_1);

    // CHECK: UCI
    if (pdcch.dci.ctx.format == srsran_dci_format_nr_1_0) {
      TESTASSERT(pdcch.dci.harq_feedback == k1 - 1);
    } else {
      TESTASSERT(pdcch.dci.harq_feedback == pdcch_tti.sf_idx());
    }
    TESTASSERT(ue.cc_list[cc_out.cc].pending_acks[(pdcch_tti + k1).to_uint()] % 4 == pdcch.dci.dai);
  }

  for (const pdsch_t& pdsch : pdschs) {
    TESTASSERT(pdsch.tx_softbuffer[0].buffer_b != nullptr);
    TESTASSERT(pdsch.tx_softbuffer[0].max_cb > 0);
  }
}

} // namespace srsenb
