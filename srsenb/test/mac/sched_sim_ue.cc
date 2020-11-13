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

#include "sched_sim_ue.h"
#include "lib/include/srslte/mac/pdu.h"

namespace srsenb {

using phich_t = sched_interface::ul_sched_phich_t;

bool sim_ue_ctxt_t::is_msg3_harq(uint32_t ue_cc_idx, uint32_t pid) const
{
  auto& h = cc_list.at(ue_cc_idx).ul_harqs[pid];
  return h.first_tti_rx == msg3_tti_rx and h.nof_txs == h.nof_retxs + 1;
}

bool sim_ue_ctxt_t::is_last_ul_retx(uint32_t ue_cc_idx, uint32_t pid, uint32_t maxharq_msg3tx) const
{
  bool  is_msg3 = is_msg3_harq(ue_cc_idx, pid);
  auto& h       = cc_list.at(ue_cc_idx).ul_harqs[pid];
  return h.nof_retxs + 1 >= (is_msg3 ? maxharq_msg3tx : ue_cfg.maxharq_tx);
}

bool sim_ue_ctxt_t::is_last_dl_retx(uint32_t ue_cc_idx, uint32_t pid) const
{
  auto& h = cc_list.at(ue_cc_idx).dl_harqs[pid];
  return h.nof_retxs + 1 >= ue_cfg.maxharq_tx;
}

ue_sim::ue_sim(uint16_t                         rnti_,
               const sched_interface::ue_cfg_t& ue_cfg_,
               srslte::tti_point                prach_tti_rx_,
               uint32_t                         preamble_idx)
{
  ctxt.rnti         = rnti_;
  ctxt.prach_tti_rx = prach_tti_rx_;
  ctxt.preamble_idx = preamble_idx;
  set_cfg(ue_cfg_);
}

void ue_sim::set_cfg(const sched_interface::ue_cfg_t& ue_cfg_)
{
  ctxt.ue_cfg = ue_cfg_;
  ctxt.cc_list.resize(ue_cfg_.supported_cc_list.size());
  for (auto& cc : ctxt.cc_list) {
    for (size_t pid = 0; pid < (FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS); ++pid) {
      cc.ul_harqs[pid].pid = pid;
      cc.dl_harqs[pid].pid = pid;
    }
  }
}

bool ue_sim::enqueue_pending_acks(srslte::tti_point                tti_rx,
                                  pucch_feedback&                  feedback_list,
                                  std::bitset<SRSLTE_MAX_CARRIERS> ack_val)
{
  bool ack_added = false;
  for (uint32_t ue_cc_idx = 0; ue_cc_idx < ctxt.cc_list.size(); ++ue_cc_idx) {
    uint32_t enb_cc_idx = ctxt.ue_cfg.supported_cc_list[ue_cc_idx].enb_cc_idx;
    auto&    ue_cc      = ctxt.cc_list[ue_cc_idx];

    for (uint32_t pid = 0; pid < SRSLTE_FDD_NOF_HARQ; ++pid) {
      auto& h = ue_cc.dl_harqs[pid];

      if (h.active and to_tx_dl_ack(h.last_tti_rx) == tti_rx) {
        if (feedback_list.cc_list.size() <= ue_cc_idx) {
          feedback_list.cc_list.resize(ue_cc_idx + 1);
        }
        auto& result      = feedback_list.cc_list[ue_cc_idx];
        result.enb_cc_idx = enb_cc_idx;
        result.ack        = ack_val[ue_cc_idx];
        result.pid        = pid;

        if (result.pid >= 0 and (result.ack or h.nof_retxs + 1 >= ctxt.ue_cfg.maxharq_tx)) {
          h.active = false;
        }

        ack_added = true;
      }
    }
  }

  return ack_added;
}

int ue_sim::update(const sf_output_res_t& sf_out)
{
  update_conn_state(sf_out);
  update_dl_harqs(sf_out);
  update_ul_harqs(sf_out);

  return SRSLTE_SUCCESS;
}

void ue_sim::update_dl_harqs(const sf_output_res_t& sf_out)
{
  for (uint32_t cc = 0; cc < sf_out.cc_params.size(); ++cc) {
    for (uint32_t i = 0; i < sf_out.dl_cc_result[cc].nof_data_elems; ++i) {
      const auto& data = sf_out.dl_cc_result[cc].data[i];
      if (data.dci.rnti != ctxt.rnti) {
        continue;
      }
      auto& h = ctxt.cc_list[data.dci.ue_cc_idx].dl_harqs[data.dci.pid];
      if (h.nof_txs == 0 or h.ndi != data.dci.tb[0].ndi) {
        // It is newtx
        h.nof_retxs    = 0;
        h.ndi          = data.dci.tb[0].ndi;
        h.first_tti_rx = sf_out.tti_rx;
      } else {
        // it is retx
        h.nof_retxs++;
      }
      h.active      = true;
      h.last_tti_rx = sf_out.tti_rx;
      h.nof_txs++;
    }
  }
}

void ue_sim::update_ul_harqs(const sf_output_res_t& sf_out)
{
  uint32_t pid = to_tx_ul(sf_out.tti_rx).to_uint() % (FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS);
  for (uint32_t cc = 0; cc < sf_out.cc_params.size(); ++cc) {
    // Update UL harqs with PHICH info
    for (uint32_t i = 0; i < sf_out.ul_cc_result[cc].nof_phich_elems; ++i) {
      const auto& phich = sf_out.ul_cc_result[cc].phich[i];
      if (phich.rnti != ctxt.rnti) {
        continue;
      }

      const auto *cc_cfg = ctxt.get_cc_cfg(cc), *start = &ctxt.ue_cfg.supported_cc_list[0];
      uint32_t    ue_cc_idx  = std::distance(start, cc_cfg);
      auto&       ue_cc_ctxt = ctxt.cc_list[ue_cc_idx];
      auto&       h          = ue_cc_ctxt.ul_harqs[pid];

      bool is_ack = phich.phich == phich_t::ACK;
      bool is_msg3 =
          h.nof_txs == h.nof_retxs + 1 and ctxt.msg3_tti_rx.is_valid() and h.first_tti_rx == ctxt.msg3_tti_rx;
      bool last_retx = h.nof_retxs + 1 >= (is_msg3 ? sf_out.cc_params[0].cfg.maxharq_msg3tx : ctxt.ue_cfg.maxharq_tx);
      if (is_ack or last_retx) {
        h.active = false;
      }
    }

    // Update UL harqs with PUSCH grants
    for (uint32_t i = 0; i < sf_out.ul_cc_result[cc].nof_dci_elems; ++i) {
      const auto& data = sf_out.ul_cc_result[cc].pusch[i];
      if (data.dci.rnti != ctxt.rnti) {
        continue;
      }
      auto& ue_cc_ctxt = ctxt.cc_list[data.dci.ue_cc_idx];
      auto& h          = ue_cc_ctxt.ul_harqs[to_tx_ul(sf_out.tti_rx).to_uint() % ue_cc_ctxt.ul_harqs.size()];

      if (h.nof_txs == 0 or h.ndi != data.dci.tb.ndi) {
        // newtx
        h.nof_retxs    = 0;
        h.ndi          = data.dci.tb.ndi;
        h.first_tti_rx = sf_out.tti_rx;
      } else {
        h.nof_retxs++;
      }
      h.active      = true;
      h.last_tti_rx = sf_out.tti_rx;
      h.riv         = data.dci.type2_alloc.riv;
      h.nof_txs++;
    }
  }
}

void ue_sim::update_conn_state(const sf_output_res_t& sf_out)
{
  if (ctxt.msg4_tti_rx.is_valid()) {
    return;
  }

  // only check for RAR/Msg3 presence for a UE's PCell
  uint32_t          cc           = ctxt.ue_cfg.supported_cc_list[0].enb_cc_idx;
  const auto&       dl_cc_result = sf_out.dl_cc_result[cc];
  const auto&       ul_cc_result = sf_out.ul_cc_result[cc];
  srslte::tti_point tti_tx_dl    = to_tx_dl(sf_out.tti_rx);

  if (not ctxt.rar_tti_rx.is_valid()) {
    // RAR not yet found
    uint32_t             rar_win_size = sf_out.cc_params[cc].cfg.prach_rar_window;
    srslte::tti_interval rar_window{ctxt.prach_tti_rx + 3, ctxt.prach_tti_rx + 3 + rar_win_size};

    if (rar_window.contains(tti_tx_dl)) {
      for (uint32_t i = 0; i < dl_cc_result.nof_rar_elems; ++i) {
        for (uint32_t j = 0; j < dl_cc_result.rar[i].nof_grants; ++j) {
          const auto& data = dl_cc_result.rar[i].msg3_grant[j].data;
          if (data.prach_tti == (uint32_t)ctxt.prach_tti_rx.to_uint() and data.preamble_idx == ctxt.preamble_idx) {
            ctxt.rar_tti_rx = sf_out.tti_rx;
            ctxt.msg3_riv   = dl_cc_result.rar[i].msg3_grant[j].grant.rba;
          }
        }
      }
    }
  }

  if (ctxt.rar_tti_rx.is_valid() and not ctxt.msg3_tti_rx.is_valid()) {
    // RAR scheduled, Msg3 not yet scheduled
    srslte::tti_point expected_msg3_tti_rx = ctxt.rar_tti_rx + MSG3_DELAY_MS;
    if (expected_msg3_tti_rx == sf_out.tti_rx) {
      // Msg3 should exist
      for (uint32_t i = 0; i < ul_cc_result.nof_dci_elems; ++i) {
        if (ul_cc_result.pusch[i].dci.rnti == ctxt.rnti) {
          ctxt.msg3_tti_rx = sf_out.tti_rx;
        }
      }
    }
  }

  if (ctxt.msg3_tti_rx.is_valid() and not ctxt.msg4_tti_rx.is_valid()) {
    // Msg3 scheduled, but Msg4 not yet scheduled
    for (uint32_t i = 0; i < dl_cc_result.nof_data_elems; ++i) {
      if (dl_cc_result.data[i].dci.rnti == ctxt.rnti) {
        for (uint32_t j = 0; j < dl_cc_result.data[i].nof_pdu_elems[0]; ++j) {
          if (dl_cc_result.data[i].pdu[0][j].lcid == (uint32_t)srslte::dl_sch_lcid::CON_RES_ID) {
            // ConRes found
            ctxt.msg4_tti_rx = sf_out.tti_rx;
          }
        }
      }
    }
  }
}

void ue_db_sim::add_user(uint16_t                         rnti,
                         const sched_interface::ue_cfg_t& ue_cfg_,
                         srslte::tti_point                prach_tti_rx_,
                         uint32_t                         preamble_idx)
{
  ue_db.insert(std::make_pair(rnti, ue_sim(rnti, ue_cfg_, prach_tti_rx_, preamble_idx)));
}

void ue_db_sim::ue_recfg(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_)
{
  ue_db.at(rnti).set_cfg(ue_cfg_);
}

void ue_db_sim::rem_user(uint16_t rnti)
{
  ue_db.erase(rnti);
}

void ue_db_sim::update(const sf_output_res_t& sf_out)
{
  for (auto& ue_pair : ue_db) {
    ue_pair.second.update(sf_out);
  }
}

std::map<uint16_t, const sim_ue_ctxt_t*> ue_db_sim::get_ues_ctxt() const
{
  std::map<uint16_t, const sim_ue_ctxt_t*> ret;

  for (auto& ue_pair : ue_db) {
    ret.insert(std::make_pair(ue_pair.first, &ue_pair.second.get_ctxt()));
  }

  return ret;
}

} // namespace srsenb
