/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "sched_sim_ue.h"
#include "lib/include/srsran/mac/pdu.h"
#include "sched_test_utils.h"

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
               srsran::tti_point                prach_tti_rx_,
               uint32_t                         preamble_idx) :
  logger(srslog::fetch_basic_logger("MAC"))
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

void ue_sim::bearer_cfg(uint32_t lc_id, const mac_lc_ch_cfg_t& cfg)
{
  ctxt.ue_cfg.ue_bearers.at(lc_id) = cfg;
}

int ue_sim::update(const sf_output_res_t& sf_out)
{
  update_conn_state(sf_out);
  update_dl_harqs(sf_out);
  update_ul_harqs(sf_out);

  return SRSRAN_SUCCESS;
}

void ue_sim::update_dl_harqs(const sf_output_res_t& sf_out)
{
  for (uint32_t cc = 0; cc < sf_out.cc_params.size(); ++cc) {
    for (uint32_t i = 0; i < sf_out.dl_cc_result[cc].data.size(); ++i) {
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
        h.dci_loc      = data.dci.location;
        h.tbs          = data.tbs[0];
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
    const auto *cc_cfg = ctxt.get_cc_cfg(cc), *start = &ctxt.ue_cfg.supported_cc_list[0];
    if (cc_cfg == nullptr) {
      continue;
    }
    uint32_t ue_cc_idx  = std::distance(start, cc_cfg);
    auto&    ue_cc_ctxt = ctxt.cc_list[ue_cc_idx];
    auto&    h          = ue_cc_ctxt.ul_harqs[pid];

    // Update UL harqs with PHICH info
    bool found_phich = false;
    bool is_msg3 = h.nof_txs == h.nof_retxs + 1 and ctxt.msg3_tti_rx.is_valid() and h.first_tti_rx == ctxt.msg3_tti_rx;
    uint32_t max_retxs = is_msg3 ? sf_out.cc_params[0].cfg.maxharq_msg3tx : ctxt.ue_cfg.maxharq_tx;
    bool     last_retx = h.nof_retxs + 1 >= max_retxs;
    for (uint32_t i = 0; i < sf_out.ul_cc_result[cc].phich.size(); ++i) {
      const auto& phich = sf_out.ul_cc_result[cc].phich[i];
      if (phich.rnti != ctxt.rnti) {
        continue;
      }
      found_phich = true;

      bool is_ack = phich.phich == phich_t::ACK;
      if (is_ack or last_retx) {
        h.active = false;
      }
    }
    if (not found_phich and h.active) {
      // There can be missing PHICH due to measGap collisions. In such case, we deactivate the harq and assume hi=1
      h.active = false;
    }

    // Update UL harqs with PUSCH grants
    bool pusch_found = false;
    for (uint32_t i = 0; i < sf_out.ul_cc_result[cc].pusch.size(); ++i) {
      const auto& data = sf_out.ul_cc_result[cc].pusch[i];
      if (data.dci.rnti != ctxt.rnti) {
        continue;
      }
      pusch_found = true;

      if (h.nof_txs == 0 or h.ndi != data.dci.tb.ndi) {
        // newtx
        h.nof_retxs    = 0;
        h.ndi          = data.dci.tb.ndi;
        h.first_tti_rx = sf_out.tti_rx;
        h.tbs          = data.tbs;
      } else {
        h.nof_retxs++;
      }
      h.active      = true;
      h.last_tti_rx = sf_out.tti_rx;
      h.riv         = data.dci.type2_alloc.riv;
      h.nof_txs++;
    }
    if (not pusch_found and h.nof_retxs < max_retxs) {
      // PUSCH *may* be skipped due to measGap. nof_retxs keeps getting incremented
      h.nof_retxs++;
      h.nof_txs++;
    }
  }
}

void ue_sim::update_conn_state(const sf_output_res_t& sf_out)
{
  if (ctxt.conres_rx) {
    return;
  }

  // only check for RAR/Msg3 presence for a UE's PCell
  uint32_t          cc           = ctxt.ue_cfg.supported_cc_list[0].enb_cc_idx;
  const auto&       dl_cc_result = sf_out.dl_cc_result[cc];
  const auto&       ul_cc_result = sf_out.ul_cc_result[cc];
  srsran::tti_point tti_tx_dl    = to_tx_dl(sf_out.tti_rx);

  if (not ctxt.rar_tti_rx.is_valid()) {
    // RAR not yet found
    uint32_t             rar_win_size = sf_out.cc_params[cc].cfg.prach_rar_window;
    srsran::tti_interval rar_window{ctxt.prach_tti_rx + 3, ctxt.prach_tti_rx + 3 + rar_win_size};

    if (rar_window.contains(tti_tx_dl)) {
      for (uint32_t i = 0; i < dl_cc_result.rar.size(); ++i) {
        for (uint32_t j = 0; j < dl_cc_result.rar[i].msg3_grant.size(); ++j) {
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
    srsran::tti_point expected_msg3_tti_rx = ctxt.rar_tti_rx + MSG3_DELAY_MS;
    if (expected_msg3_tti_rx == sf_out.tti_rx) {
      // Msg3 should exist
      for (uint32_t i = 0; i < ul_cc_result.pusch.size(); ++i) {
        if (ul_cc_result.pusch[i].dci.rnti == ctxt.rnti) {
          ctxt.msg3_tti_rx = sf_out.tti_rx;
        }
      }
    }
  }

  if (ctxt.msg3_tti_rx.is_valid() and not ctxt.msg4_tti_rx.is_valid()) {
    // Msg3 scheduled, but Msg4 not yet scheduled
    for (uint32_t i = 0; i < dl_cc_result.data.size(); ++i) {
      if (dl_cc_result.data[i].dci.rnti == ctxt.rnti) {
        for (uint32_t j = 0; j < dl_cc_result.data[i].nof_pdu_elems[0]; ++j) {
          if (dl_cc_result.data[i].pdu[0][j].lcid == (uint32_t)srsran::dl_sch_lcid::CON_RES_ID) {
            // ConRes found
            ctxt.msg4_tti_rx = sf_out.tti_rx;
          }
        }
      }
    }
  }
}

sched_sim_base::sched_sim_base(sched_interface*                                sched_ptr_,
                               const sched_interface::sched_args_t&            sched_args,
                               const std::vector<sched_interface::cell_cfg_t>& cell_cfg_list) :
  logger(srslog::fetch_basic_logger("TEST")), sched_ptr(sched_ptr_), cell_params(cell_cfg_list.size())
{
  for (uint32_t cc = 0; cc < cell_params.size(); ++cc) {
    cell_params[cc].set_cfg(cc, cell_cfg_list[cc], sched_args);
  }
  sched_ptr->cell_cfg(cell_cfg_list); // call parent cfg
}

int sched_sim_base::add_user(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_, uint32_t preamble_idx)
{
  CONDERROR(!srsran_prach_tti_opportunity_config_fdd(
                cell_params[ue_cfg_.supported_cc_list[0].enb_cc_idx].cfg.prach_config, current_tti_rx.to_uint(), -1),
            "New user added in a non-PRACH TTI");
  TESTASSERT(ue_db.count(rnti) == 0);

  final_ue_cfg[rnti] = ue_cfg_;
  auto rach_cfg      = generate_rach_ue_cfg(ue_cfg_);
  ue_db.insert(std::make_pair(rnti, ue_sim(rnti, rach_cfg, current_tti_rx, preamble_idx)));

  CONDERROR(sched_ptr->ue_cfg(rnti, rach_cfg) != SRSRAN_SUCCESS, "Configuring new user rnti=0x%x to sched", rnti);

  sched_interface::dl_sched_rar_info_t rar_info = {};
  rar_info.prach_tti                            = current_tti_rx.to_uint();
  rar_info.temp_crnti                           = rnti;
  rar_info.msg3_size                            = 7;
  rar_info.preamble_idx                         = preamble_idx;
  uint32_t pcell_idx                            = ue_cfg_.supported_cc_list[0].enb_cc_idx;
  TESTASSERT(sched_ptr->dl_rach_info(pcell_idx, rar_info) == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}

int sched_sim_base::ue_recfg(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_)
{
  CONDERROR(ue_db.count(rnti) == 0, "User must already exist to be configured");
  ue_db.at(rnti).set_cfg(ue_cfg_);
  CONDERROR(sched_ptr->ue_cfg(rnti, ue_cfg_) != SRSRAN_SUCCESS, "Configuring new user rnti=0x%x to sched", rnti);

  return SRSRAN_SUCCESS;
}

int sched_sim_base::bearer_cfg(uint16_t rnti, uint32_t lc_id, const mac_lc_ch_cfg_t& cfg)
{
  ue_db.at(rnti).bearer_cfg(lc_id, cfg);
  return sched_ptr->bearer_ue_cfg(rnti, lc_id, cfg);
}

int sched_sim_base::rem_user(uint16_t rnti)
{
  ue_db.erase(rnti);
  return sched_ptr->ue_rem(rnti);
}

void sched_sim_base::update(const sf_output_res_t& sf_out)
{
  for (auto& ue_pair : ue_db) {
    ue_pair.second.update(sf_out);
  }
}

sim_enb_ctxt_t sched_sim_base::get_enb_ctxt() const
{
  sim_enb_ctxt_t ctxt;
  ctxt.cell_params = cell_params;

  for (auto& ue_pair : ue_db) {
    ctxt.ue_db.insert(std::make_pair(ue_pair.first, &ue_pair.second.get_ctxt()));
  }

  return ctxt;
}

int sched_sim_base::set_default_tti_events(const sim_ue_ctxt_t& ue_ctxt, ue_tti_events& pending_events)
{
  pending_events.cc_list.clear();
  pending_events.cc_list.resize(cell_params.size());
  pending_events.tti_rx = current_tti_rx;

  for (uint32_t enb_cc_idx = 0; enb_cc_idx < pending_events.cc_list.size(); ++enb_cc_idx) {
    auto& cc_feedback = pending_events.cc_list[enb_cc_idx];
    if (ue_ctxt.enb_to_ue_cc_idx(enb_cc_idx) < 0) {
      continue;
    }

    cc_feedback.configured = true;
    cc_feedback.ue_cc_idx  = ue_ctxt.enb_to_ue_cc_idx(enb_cc_idx);
    for (uint32_t pid = 0; pid < SRSRAN_FDD_NOF_HARQ; ++pid) {
      auto& dl_h = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].dl_harqs[pid];
      auto& ul_h = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].ul_harqs[pid];

      // Set default DL ACK
      if (dl_h.active and to_tx_dl_ack(dl_h.last_tti_rx) == current_tti_rx) {
        cc_feedback.dl_pid = pid;
        cc_feedback.dl_ack = true; // default is ACK
      }

      // Set default UL ACK
      if (ul_h.active and to_tx_ul(ul_h.last_tti_rx) == current_tti_rx) {
        cc_feedback.ul_pid = pid;
        cc_feedback.ul_ack = true;
      }

      // Set default DL CQI
      if (srsran_cqi_periodic_send(&ue_ctxt.ue_cfg.supported_cc_list[cc_feedback.ue_cc_idx].dl_cfg.cqi_report,
                                   current_tti_rx.to_uint(),
                                   SRSRAN_FDD)) {
        cc_feedback.dl_cqi = 28;
      }

      // TODO: UL CQI
    }
  }

  return SRSRAN_SUCCESS;
}

int sched_sim_base::apply_tti_events(sim_ue_ctxt_t& ue_ctxt, const ue_tti_events& events)
{
  for (uint32_t enb_cc_idx = 0; enb_cc_idx < events.cc_list.size(); ++enb_cc_idx) {
    const auto& cc_feedback = events.cc_list[enb_cc_idx];
    if (not cc_feedback.configured) {
      continue;
    }

    if (cc_feedback.dl_pid >= 0) {
      auto& h = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].dl_harqs[cc_feedback.dl_pid];

      if (cc_feedback.dl_ack) {
        logger.info("DL ACK rnti=0x%x tti_dl_tx=%u pid=%d",
                    ue_ctxt.rnti,
                    to_tx_dl(h.last_tti_rx).to_uint(),
                    cc_feedback.dl_pid);
      }

      // update scheduler
      if (sched_ptr->dl_ack_info(
              events.tti_rx.to_uint(), ue_ctxt.rnti, enb_cc_idx, cc_feedback.tb, cc_feedback.dl_ack) < 0) {
        logger.error("The ACKed DL Harq pid=%d does not exist.", cc_feedback.dl_pid);
        error_counter++;
      }

      // update UE sim context
      if (cc_feedback.dl_ack or ue_ctxt.is_last_dl_retx(cc_feedback.ue_cc_idx, cc_feedback.dl_pid)) {
        h.active = false;
      }
    }

    if (cc_feedback.ul_pid >= 0) {
      auto& h = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].ul_harqs[cc_feedback.ul_pid];

      if (cc_feedback.ul_ack) {
        logger.info("UL ACK rnti=0x%x tti_ul_tx=%u pid=%d",
                    ue_ctxt.rnti,
                    to_tx_ul(h.last_tti_rx).to_uint(),
                    cc_feedback.ul_pid);
      }

      // update scheduler
      if (sched_ptr->ul_crc_info(events.tti_rx.to_uint(), ue_ctxt.rnti, enb_cc_idx, cc_feedback.ul_ack) < 0) {
        logger.error("The ACKed UL Harq pid=%d does not exist.", cc_feedback.ul_pid);
        error_counter++;
      }
    }

    if (cc_feedback.dl_cqi >= 0) {
      sched_ptr->dl_cqi_info(events.tti_rx.to_uint(), ue_ctxt.rnti, enb_cc_idx, cc_feedback.dl_cqi);
    }

    if (cc_feedback.ul_snr >= 0) {
      sched_ptr->ul_snr_info(events.tti_rx.to_uint(), ue_ctxt.rnti, enb_cc_idx, cc_feedback.ul_snr, 0);
    }
  }

  if (not ue_ctxt.conres_rx and ue_ctxt.msg3_tti_rx.is_valid() and to_tx_ul(ue_ctxt.msg3_tti_rx) <= events.tti_rx) {
    uint32_t enb_cc_idx  = ue_ctxt.ue_cfg.supported_cc_list[0].enb_cc_idx;
    auto&    cc_feedback = events.cc_list[enb_cc_idx];

    // Schedule Msg4 when Msg3 is received
    if (cc_feedback.ul_pid >= 0 and cc_feedback.ul_ack) {
      sched_interface::ue_cfg_t ue_cfg = generate_setup_ue_cfg(final_ue_cfg[ue_ctxt.rnti]);
      TESTASSERT(ue_recfg(ue_ctxt.rnti, ue_cfg) == SRSRAN_SUCCESS);

      uint32_t lcid = srb_to_lcid(lte_srb::srb0); // Use SRB0 to schedule Msg4
      TESTASSERT(sched_ptr->dl_rlc_buffer_state(ue_ctxt.rnti, lcid, 50, 0) == SRSRAN_SUCCESS);
      TESTASSERT(sched_ptr->dl_mac_buffer_state(ue_ctxt.rnti, (uint32_t)srsran::dl_sch_lcid::CON_RES_ID, 1) ==
                 SRSRAN_SUCCESS);
    }

    // Perform DRB config when Msg4 is received
    if (cc_feedback.dl_pid >= 0 and cc_feedback.dl_ack) {
      ue_ctxt.conres_rx = true;

      TESTASSERT(ue_recfg(ue_ctxt.rnti, final_ue_cfg[ue_ctxt.rnti]) == SRSRAN_SUCCESS);
    }
  }

  return SRSRAN_SUCCESS;
}

void sched_sim_base::new_tti(srsran::tti_point tti_rx)
{
  current_tti_rx = tti_rx;
  for (auto& ue : ue_db) {
    ue_tti_events events;
    set_default_tti_events(ue.second.get_ctxt(), events);
    set_external_tti_events(ue.second.get_ctxt(), events);
    apply_tti_events(ue.second.get_ctxt(), events);
  }
}

} // namespace srsenb
