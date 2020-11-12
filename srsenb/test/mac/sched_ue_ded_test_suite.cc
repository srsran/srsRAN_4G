/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "sched_ue_ded_test_suite.h"
#include "srslte/common/test_common.h"

namespace srsenb {

using phich_t = sched_interface::ul_sched_phich_t;
using pusch_t = sched_interface::ul_sched_data_t;

const sched_interface::ue_cfg_t::cc_cfg_t* sim_ue_ctxt_t::get_cc_cfg(uint32_t enb_cc_idx) const
{
  // TODO: verify SCELL Act was received
  auto it =
      std::find_if(ue_cfg.supported_cc_list.begin(),
                   ue_cfg.supported_cc_list.end(),
                   [enb_cc_idx](const sched_interface::ue_cfg_t::cc_cfg_t& cc) { return cc.enb_cc_idx == enb_cc_idx; });
  return (it == ue_cfg.supported_cc_list.end()) ? nullptr : &(*it);
}

int sim_ue_ctxt_t::enb_to_ue_cc_idx(uint32_t enb_cc_idx) const
{
  auto it =
      std::find_if(ue_cfg.supported_cc_list.begin(),
                   ue_cfg.supported_cc_list.end(),
                   [enb_cc_idx](const sched_interface::ue_cfg_t::cc_cfg_t& cc) { return cc.enb_cc_idx == enb_cc_idx; });
  return it == ue_cfg.supported_cc_list.end() ? -1 : std::distance(ue_cfg.supported_cc_list.begin(), it);
}

int test_pdsch_grant(const sim_ue_ctxt_t&                    ue_ctxt,
                     srslte::tti_point                       tti_rx,
                     uint32_t                                enb_cc_idx,
                     const sched_interface::dl_sched_data_t& pdsch)
{
  auto* cc_cfg = ue_ctxt.get_cc_cfg(enb_cc_idx);

  // TEST: Check if CC is configured and active
  CONDERROR(cc_cfg == nullptr or not cc_cfg->active, "PDSCH allocation for disabled or unavailable cc\n");
  CONDERROR(pdsch.dci.ue_cc_idx != std::distance(&ue_ctxt.ue_cfg.supported_cc_list.front(), cc_cfg),
            "Inconsistent enb_cc_idx -> ue_cc_idx mapping\n");

  // TEST: DCI is consistent with current UE DL harq state
  auto&    h        = ue_ctxt.cc_list[pdsch.dci.ue_cc_idx].dl_harqs[pdsch.dci.pid];
  uint32_t nof_retx = sched_utils::get_nof_retx(pdsch.dci.tb[0].rv); // 0..3
  if (h.nof_txs == 0 or h.ndi != pdsch.dci.tb[0].ndi) {
    // It is newtx
    CONDERROR(nof_retx != 0, "Invalid rv index for new tx\n");
  } else {
    // it is retx
    CONDERROR(sched_utils::get_rvidx(h.nof_retxs + 1) != (uint32_t)pdsch.dci.tb[0].rv, "Invalid rv index for retx\n");
    CONDERROR(not h.active, "retx for inactive dl harq pid=%d\n", h.pid);
    CONDERROR(to_tx_dl_ack(h.last_tti_rx) > tti_rx, "harq pid=%d reused too soon\n", h.pid);
    CONDERROR(h.nof_retxs + 1 > ue_ctxt.ue_cfg.maxharq_tx,
              "The number of retx=%d exceeded its max=%d\n",
              h.nof_retxs + 1,
              ue_ctxt.ue_cfg.maxharq_tx);
  }

  return SRSLTE_SUCCESS;
}

int test_ul_sched_result(const sim_enb_ctxt_t& enb_ctxt, const sf_output_res_t& sf_out)
{
  uint32_t pid = to_tx_ul(sf_out.tti_rx).to_uint() % (FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS);

  for (size_t cc = 0; cc < enb_ctxt.cell_params->size(); ++cc) {
    const auto* phich_begin = &sf_out.ul_cc_result[cc].phich[0];
    const auto* phich_end   = &sf_out.ul_cc_result[cc].phich[sf_out.ul_cc_result[cc].nof_phich_elems];
    const auto* pusch_begin = &sf_out.ul_cc_result[cc].pusch[0];
    const auto* pusch_end   = &sf_out.ul_cc_result[cc].pusch[sf_out.ul_cc_result[cc].nof_dci_elems];

    // TEST: rnti must exist for all PHICH
    CONDERROR(std::any_of(phich_begin,
                          phich_end,
                          [&enb_ctxt](const phich_t& phich) { return enb_ctxt.ue_db.count(phich.rnti) == 0; }),
              "Scheduled PHICH does not have associated rnti\n");

    // TEST: rnti must exist for all PUSCH
    CONDERROR(std::any_of(pusch_begin,
                          pusch_end,
                          [&enb_ctxt](const pusch_t& pusch) { return enb_ctxt.ue_db.count(pusch.dci.rnti) == 0; }),
              "Scheduled PUSCH does not have associated rnti.");

    for (const auto& ue_pair : enb_ctxt.ue_db) {
      const auto& ue        = *ue_pair.second;
      uint16_t    rnti      = ue.rnti;
      int         ue_cc_idx = ue.enb_to_ue_cc_idx(cc);

      // TEST: Check if CC is configured and active
      CONDERROR(ue_cc_idx < 0 or not ue.ue_cfg.supported_cc_list[ue_cc_idx].active,
                "PUSCH allocation for disabled or unavailable cc\n");

      const phich_t* phich_ptr =
          std::find_if(phich_begin, phich_end, [rnti](const phich_t& phich) { return phich.rnti == rnti; });
      phich_ptr = phich_ptr == phich_end ? nullptr : phich_ptr;
      const pusch_t* pusch_ptr =
          std::find_if(pusch_begin, pusch_end, [rnti](const pusch_t& pusch) { return pusch.dci.rnti == rnti; });
      pusch_ptr = pusch_ptr == pusch_end ? nullptr : pusch_ptr;

      // TEST: Already active UL HARQs have to receive PHICH
      const auto& h = ue.cc_list[ue_cc_idx].ul_harqs[pid];
      CONDERROR(
          h.active and phich_ptr == nullptr, "PHICH not received for rnti=0x%x active UL HARQ pid=%d\n", rnti, pid);
      CONDERROR(not h.active and phich_ptr != nullptr,
                "PHICH for rnti=0x%x corresponds to inactive UL HARQ pid=%d\n",
                rnti,
                pid);

      // TEST: absent PUSCH grants for active DL HARQs must be either ACKs, last retx, or interrupted HARQs
      if (phich_ptr != nullptr and pusch_ptr == nullptr) {
        bool is_ack    = phich_ptr->phich == phich_t::ACK;
        bool is_msg3   = h.first_tti_rx == ue.msg3_tti_rx and h.nof_txs == h.nof_retxs + 1;
        bool last_retx = h.nof_retxs + 1 >= (is_msg3 ? sf_out.cc_params[0].cfg.maxharq_msg3tx : ue.ue_cfg.maxharq_tx);
        CONDERROR(
            not is_ack and not last_retx, "PHICH NACK received for rnti=0x%x but no PUSCH retx reallocated\n", rnti);
      }

      if (pusch_ptr != nullptr) {
        CONDERROR(pusch_ptr->dci.ue_cc_idx != (uint32_t)ue_cc_idx, "Inconsistent enb_cc_idx -> ue_cc_idx mapping\n");

        // TEST: DCI is consistent with current UE UL harq state
        uint32_t nof_retx = sched_utils::get_nof_retx(pusch_ptr->dci.tb.rv); // 0..3

        if (h.nof_txs == 0 or h.ndi != pusch_ptr->dci.tb.ndi) {
          // newtx
          CONDERROR(nof_retx != 0, "Invalid rv index for new tx\n");
          CONDERROR(pusch_ptr->current_tx_nb != 0, "UL HARQ retxs need to have been previously transmitted\n");
        } else {
          CONDERROR(pusch_ptr->current_tx_nb == 0, "UL retx has to have nof tx > 0\n");
          if (not h.active) {
            // the HARQ is being resumed
            CONDERROR(not pusch_ptr->needs_pdcch, "Resumed UL HARQs need to be signalled in PDCCH\n");
          }
          if (pusch_ptr->needs_pdcch) {
            // adaptive retx
          } else {
            // non-adaptive retx
            CONDERROR(pusch_ptr->dci.type2_alloc.riv != h.riv, "Non-adaptive retx must keep the same riv\n");
          }
          if (pusch_ptr->tbs > 0) {
            CONDERROR(sched_utils::get_rvidx(h.nof_retxs + 1) != (uint32_t)pusch_ptr->dci.tb.rv,
                      "Invalid rv index for retx\n");
          }
          CONDERROR(to_tx_ul(h.last_tti_rx) > sf_out.tti_rx, "UL harq pid=%d was reused too soon\n", h.pid);
        }
      }
    }
  }

  return SRSLTE_SUCCESS;
}

int test_all_ues(const sim_enb_ctxt_t& enb_ctxt, const sf_output_res_t& sf_out)
{
  //  for (uint32_t cc = 0; cc < enb_ctxt.cell_params->size(); ++cc) {
  //    for (uint32_t i = 0; i < sf_out.dl_cc_result[cc].nof_data_elems; ++i) {
  //      const sched_interface::dl_sched_data_t& data = sf_out.dl_cc_result[cc].data[i];
  //      CONDERROR(
  //          enb_ctxt.ue_db.count(data.dci.rnti) == 0, "Allocated DL grant for non-existent rnti=0x%x\n",
  //          data.dci.rnti);
  //      TESTASSERT(test_pdsch_grant(*enb_ctxt.ue_db.at(data.dci.rnti), sf_out.tti_rx, cc, data) == SRSLTE_SUCCESS);
  //    }
  //  }

  TESTASSERT(test_ul_sched_result(enb_ctxt, sf_out) == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}

} // namespace srsenb
