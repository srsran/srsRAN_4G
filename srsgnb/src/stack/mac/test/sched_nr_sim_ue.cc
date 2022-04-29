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

#include "sched_nr_sim_ue.h"
#include "sched_nr_common_test.h"
#include "sched_nr_ue_ded_test_suite.h"
#include "srsran/common/test_common.h"
#include "srsran/common/thread_pool.h"

namespace srsenb {

sched_nr_ue_sim::sched_nr_ue_sim(uint16_t rnti_, const sched_nr_ue_cfg_t& ue_cfg_) :
  logger(srslog::fetch_basic_logger("MAC"))
{
  ctxt.rnti = rnti_;
  ctxt.ue_cfg.apply_config_request(ue_cfg_);
  ctxt.preamble_idx = -1;

  ctxt.cc_list.resize(ue_cfg_.carriers.size());
  for (auto& cc : ctxt.cc_list) {
    for (size_t pid = 0; pid < SCHED_NR_MAX_HARQ; ++pid) {
      cc.ul_harqs[pid].pid = pid;
      cc.dl_harqs[pid].pid = pid;
    }
  }
}

sched_nr_ue_sim::sched_nr_ue_sim(uint16_t                 rnti_,
                                 const sched_nr_ue_cfg_t& ue_cfg_,
                                 slot_point               prach_slot_rx,
                                 uint32_t                 preamble_idx) :
  sched_nr_ue_sim(rnti_, ue_cfg_)
{
  ctxt.prach_slot_rx = prach_slot_rx;
  ctxt.preamble_idx  = preamble_idx;
}

int sched_nr_ue_sim::update(const sched_nr_cc_result_view& cc_out)
{
  update_dl_harqs(cc_out);

  for (uint32_t i = 0; i < cc_out.dl->phy.pdcch_dl.size(); ++i) {
    const auto& data = cc_out.dl->phy.pdcch_dl[i];
    if (data.dci.ctx.rnti != ctxt.rnti) {
      continue;
    }
    slot_point pdcch_slot = cc_out.slot;
    uint32_t   k1         = ctxt.ue_cfg.phy_cfg.harq_ack
                      .dl_data_to_ul_ack[pdcch_slot.slot_idx() % ctxt.ue_cfg.phy_cfg.harq_ack.nof_dl_data_to_ul_ack];
    slot_point uci_slot = pdcch_slot + k1;

    ctxt.cc_list[cc_out.cc].pending_acks[uci_slot.to_uint()]++;
  }

  // clear up old slots
  ctxt.cc_list[cc_out.cc].pending_acks[(cc_out.slot - 1).to_uint()] = 0;

  return SRSRAN_SUCCESS;
}

void sched_nr_ue_sim::update_dl_harqs(const sched_nr_cc_result_view& cc_out)
{
  uint32_t cc = cc_out.cc;

  // Update DL Harqs
  for (uint32_t i = 0; i < cc_out.dl->phy.pdcch_dl.size(); ++i) {
    const auto& data = cc_out.dl->phy.pdcch_dl[i];
    if (data.dci.ctx.rnti != ctxt.rnti) {
      continue;
    }
    auto& h = ctxt.cc_list[cc].dl_harqs[data.dci.pid];
    if (h.nof_txs == 0 or h.ndi != data.dci.ndi) {
      // It is newtx
      h.nof_retxs     = 0;
      h.ndi           = data.dci.ndi;
      h.first_slot_tx = cc_out.slot;
      h.dci_loc       = data.dci.ctx.location;
      for (const sched_nr_impl::pdsch_t& pdsch : cc_out.dl->phy.pdsch) {
        if (pdsch.sch.grant.rnti == data.dci.ctx.rnti) {
          h.tbs = pdsch.sch.grant.tb[0].tbs / 8u;
        }
      }
    } else {
      // it is retx
      h.nof_retxs++;
    }
    h.active       = true;
    h.last_slot_tx = cc_out.slot;
    h.last_slot_ack =
        h.last_slot_tx +
        ctxt.ue_cfg.phy_cfg.harq_ack
            .dl_data_to_ul_ack[h.last_slot_tx.slot_idx() % ctxt.ue_cfg.phy_cfg.harq_ack.nof_dl_data_to_ul_ack];
    h.nof_txs++;
  }

  // Update UL harqs
  for (uint32_t i = 0; i < cc_out.dl->phy.pdcch_ul.size(); ++i) {
    const auto& data = cc_out.dl->phy.pdcch_ul[i];
    if (data.dci.ctx.rnti != ctxt.rnti) {
      continue;
    }
    auto& h = ctxt.cc_list[cc].ul_harqs[data.dci.pid];
    if (h.nof_txs == 0 or h.ndi != data.dci.ndi) {
      // It is newtx
      h.is_msg3       = false;
      h.nof_retxs     = 0;
      h.ndi           = data.dci.ndi;
      h.first_slot_tx = cc_out.slot + 4; // TODO
      h.dci_loc       = data.dci.ctx.location;
      h.tbs           = 100; // TODO
    } else {
      // it is retx
      h.nof_retxs++;
    }
    h.active        = true;
    h.last_slot_tx  = cc_out.slot + 4; // TODO
    h.last_slot_ack = h.last_slot_tx;
    h.nof_txs++;
  }

  uint32_t rar_count = 0;
  for (uint32_t i = 0; i < cc_out.dl->phy.pdcch_dl.size(); ++i) {
    const auto& rar_pdcch = cc_out.dl->phy.pdcch_dl[i];
    if (rar_pdcch.dci.ctx.rnti_type != srsran_rnti_type_ra) {
      continue;
    }
    const auto& rar_data = cc_out.dl->rar[rar_count++];
    for (uint32_t j = 0; j < rar_data.grants.size(); ++j) {
      auto& msg3_grant = rar_data.grants[j];
      if (msg3_grant.msg3_dci.ctx.rnti != ctxt.rnti) {
        continue;
      }
      auto& h = ctxt.cc_list[cc].ul_harqs[msg3_grant.msg3_dci.pid];
      if (h.nof_txs == 0) {
        // It is newtx
        h.is_msg3       = true;
        h.nof_retxs     = 0;
        h.ndi           = msg3_grant.msg3_dci.ndi;
        h.first_slot_tx = cc_out.slot + 4 + MSG3_DELAY_MS; // TODO
        h.dci_loc       = msg3_grant.msg3_dci.ctx.location;
        h.tbs           = 100; // TODO
      } else {
        // it is retx
        h.nof_retxs++;
      }
      h.active        = true;
      h.last_slot_tx  = cc_out.slot + 4 + MSG3_DELAY_MS; // TODO
      h.last_slot_ack = h.last_slot_tx;
      h.nof_txs++;
    }
  }
}

sched_nr_base_test_bench::sched_nr_base_test_bench(const sched_nr_interface::sched_args_t& sched_args,
                                                   const std::vector<sched_nr_cell_cfg_t>& cell_cfg_list,
                                                   std::string                             test_name_,
                                                   uint32_t                                nof_workers) :
  logger(srslog::fetch_basic_logger("TEST")),
  mac_logger(srslog::fetch_basic_logger("MAC-NR")),
  sched_ptr(new sched_nr()),
  test_delimiter(new srsran::test_delimit_logger{test_name_.c_str()})
{
  sem_init(&slot_sem, 0, 1);

  cell_params.reserve(cell_cfg_list.size());
  for (uint32_t cc = 0; cc < cell_cfg_list.size(); ++cc) {
    cell_params.emplace_back(cc, cell_cfg_list[cc], sched_args);
  }
  sched_ptr->config(sched_args, cell_cfg_list); // call parent cfg

  cc_workers.resize(nof_workers - 1);
  for (uint32_t i = 0; i < cc_workers.size(); ++i) {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, "worker{}", i + 1);
    cc_workers[i].reset(new srsran::task_worker{to_string(fmtbuf), 10});
  }

  cc_results.resize(cell_params.size());

  TESTASSERT(cell_params.size() > 0);
}

sched_nr_base_test_bench::~sched_nr_base_test_bench()
{
  stop();
}

void sched_nr_base_test_bench::stop()
{
  bool stopping = not stopped.exchange(true);
  if (stopping) {
    sem_wait(&slot_sem);
    sem_post(&slot_sem);
    for (auto& worker : cc_workers) {
      worker->stop();
    }
    sem_destroy(&slot_sem);
    test_delimiter.reset();
  }
}

std::vector<sched_nr_base_test_bench::cc_result_t> sched_nr_base_test_bench::get_slot_results() const
{
  sem_wait(&slot_sem);
  auto ret = cc_results;
  sem_post(&slot_sem);
  return ret;
}

int sched_nr_base_test_bench::rach_ind(uint16_t rnti, uint32_t cc, slot_point tti_rx, uint32_t preamble_idx)
{
  sem_wait(&slot_sem);

  TESTASSERT(ue_db.count(rnti) == 0);

  sched_nr_interface::rar_info_t rach_info{};
  rach_info.cc           = cc;
  rach_info.temp_crnti   = rnti;
  rach_info.prach_slot   = tti_rx;
  rach_info.preamble_idx = preamble_idx;
  rach_info.msg3_size    = 7;
  sched_ptr->dl_rach_info(rach_info);

  sched_nr_ue_cfg_t uecfg;
  uecfg.carriers.resize(1);
  uecfg.carriers[0].active = true;
  uecfg.carriers[0].cc     = cc;
  uecfg.phy_cfg            = cell_params[cc].default_ue_phy_cfg;
  ue_db.insert(std::make_pair(rnti, sched_nr_ue_sim(rnti, uecfg, current_slot_tx, preamble_idx)));

  sem_post(&slot_sem);
  return SRSRAN_SUCCESS;
}

void sched_nr_base_test_bench::user_cfg(uint16_t rnti, const sched_nr_interface::ue_cfg_t& ue_cfg_)
{
  sem_wait(&slot_sem);

  if (ue_db.count(rnti) == 0) {
    ue_db.insert(std::make_pair(rnti, sched_nr_ue_sim(rnti, ue_cfg_)));
  } else {
    ue_db.at(rnti).get_ctxt().ue_cfg.apply_config_request(ue_cfg_);
  }
  sched_ptr->ue_cfg(rnti, ue_cfg_);

  sem_post(&slot_sem);
}

void sched_nr_base_test_bench::add_rlc_dl_bytes(uint16_t rnti, uint32_t lcid, uint32_t pdu_size_bytes)
{
  TESTASSERT(ue_db.count(rnti) > 0);
  dl_buffer_state_diff(rnti, lcid, pdu_size_bytes);
}

void sched_nr_base_test_bench::run_slot(slot_point slot_tx)
{
  srsran_assert(not stopped.load(std::memory_order_relaxed), "Running scheduler when it has already been stopped");
  // Block concurrent or out-of-order calls to the scheduler
  sem_wait(&slot_sem);
  current_slot_tx  = slot_tx;
  nof_cc_remaining = cell_params.size();

  logger.set_context(slot_tx.to_uint());
  mac_logger.set_context(slot_tx.to_uint());

  // Clear previous slot results
  for (uint32_t cc = 0; cc < cc_results.size(); ++cc) {
    cc_results[cc] = {};
  }
  logger.info("---------------- TTI=%d ---------------", slot_tx.to_uint());

  // Process pending feedback
  for (auto& ue : ue_db) {
    ue_nr_slot_events events;
    set_default_slot_events(ue.second.get_ctxt(), events);
    set_external_slot_events(ue.second.get_ctxt(), events);
    apply_slot_events(ue.second.get_ctxt(), events);
  }

  slot_ctxt     = get_enb_ctxt();
  slot_start_tp = std::chrono::steady_clock::now();

  sched_ptr->slot_indication(current_slot_tx);

  // Generate CC result (parallel or serialized)
  uint32_t worker_idx = 0;
  for (uint32_t cc = 0; cc < cell_params.size(); ++cc) {
    if (worker_idx == cc_workers.size()) {
      generate_cc_result(cc);
    } else {
      cc_workers[worker_idx]->push_task([this, cc]() { generate_cc_result(cc); });
    }
    worker_idx = (worker_idx + 1) % (cc_workers.size() + 1);
  }
}

void sched_nr_base_test_bench::generate_cc_result(uint32_t cc)
{
  // Run scheduler
  cc_results[cc].res.slot      = current_slot_tx;
  cc_results[cc].res.cc        = cc;
  cc_results[cc].res.dl        = sched_ptr->get_dl_sched(current_slot_tx, cc);
  cc_results[cc].res.ul        = sched_ptr->get_ul_sched(current_slot_tx, cc);
  auto tp2                     = std::chrono::steady_clock::now();
  cc_results[cc].cc_latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(tp2 - slot_start_tp);

  if (--nof_cc_remaining > 0) {
    // there are still missing CC results
    return;
  }

  // Run tests and update UE state
  process_results();

  // Notify awaiting new slot worker
  sem_post(&slot_sem);
}

void sched_nr_base_test_bench::process_results()
{
  // Derived class-defined tests
  process_slot_result(slot_ctxt, cc_results);

  for (uint32_t cc = 0; cc < cell_params.size(); ++cc) {
    sched_nr_cc_result_view cc_out = cc_results[cc].res;

    // Run common tests
    test_dl_pdcch_consistency(cell_params[cc], cc_out.dl->phy.pdcch_dl);
    test_pdsch_consistency(cc_out.dl->phy.pdsch);
    test_ssb_scheduled_grant(cc_out.slot, cell_params[cc_out.cc], cc_out.dl->phy.ssb);

    // Run UE-dedicated tests
    test_dl_sched_result(slot_ctxt, cc_out);

    // Update UE state
    for (auto& u : ue_db) {
      u.second.update(cc_out);
    }

    // Update scheduler buffers
    update_sched_buffer_state(cc_out);
  }
}

void sched_nr_base_test_bench::dl_buffer_state_diff(uint16_t rnti, uint32_t lcid, int newtx)
{
  auto& lch       = gnb_ue_db[rnti].logical_channels[lcid];
  lch.rlc_unacked = std::max(0, (int)lch.rlc_unacked + newtx);
  update_sched_buffer_state(rnti);
  logger.debug("STATUS: rnti=0x%x, lcid=%d DL buffer state is (unacked=%d, newtx=%d)",
               rnti,
               lcid,
               lch.rlc_unacked,
               lch.rlc_newtx);
}

void sched_nr_base_test_bench::dl_buffer_state_diff(uint16_t rnti, int diff_bs)
{
  if (diff_bs == 0) {
    return;
  }
  if (diff_bs > 0) {
    const auto& ue_bearers = ue_db.at(rnti).get_ctxt().ue_cfg.ue_bearers;
    for (int i = ue_bearers.size() - 1; i >= 0; --i) {
      if (ue_bearers[i].is_dl()) {
        dl_buffer_state_diff(rnti, i, diff_bs);
        return;
      }
    }
    srsran_terminate("Updating UE RLC buffer state but no bearers are active");
  }
  for (auto& lch : gnb_ue_db[rnti].logical_channels) {
    if (not ue_db.at(rnti).get_ctxt().ue_cfg.ue_bearers[lch.first].is_dl()) {
      continue;
    }
    int max_diff = -std::min((int)lch.second.rlc_unacked, -diff_bs);
    if (max_diff != 0) {
      dl_buffer_state_diff(rnti, lch.first, max_diff);
      diff_bs -= max_diff;
    }
  }
}

void sched_nr_base_test_bench::update_sched_buffer_state(uint16_t rnti)
{
  auto& u = ue_db.at(rnti);
  for (auto& lch : gnb_ue_db[rnti].logical_channels) {
    int newtx = lch.second.rlc_unacked;
    for (auto& cc : u.get_ctxt().cc_list) {
      if (newtx <= 0) {
        break;
      }
      for (auto& dl_h : cc.dl_harqs) {
        int tbs = dl_h.active ? dl_h.tbs : 0;
        newtx   = std::max(0, newtx - tbs);
      }
    }
    if (newtx != (int)lch.second.rlc_newtx) {
      sched_ptr->dl_buffer_state(rnti, lch.first, newtx, 0);
      lch.second.rlc_newtx = newtx;
      logger.debug("STATUS: rnti=0x%x, lcid=%d DL buffer state is (unacked=%d, newtx=%d)",
                   rnti,
                   lch.first,
                   lch.second.rlc_unacked,
                   lch.second.rlc_newtx);
    }
  }
}

void sched_nr_base_test_bench::update_sched_buffer_state(const sched_nr_cc_result_view& cc_out)
{
  for (auto& dl : cc_out.dl->phy.pdcch_dl) {
    if (dl.dci.ctx.rnti_type == srsran_rnti_type_c or dl.dci.ctx.rnti_type == srsran_rnti_type_tc) {
      update_sched_buffer_state(dl.dci.ctx.rnti);
    }
  }
}

int sched_nr_base_test_bench::set_default_slot_events(const sim_nr_ue_ctxt_t& ue_ctxt,
                                                      ue_nr_slot_events&      pending_events)
{
  pending_events.cc_list.clear();
  pending_events.cc_list.resize(cell_params.size());
  pending_events.slot_rx = current_slot_tx;

  for (uint32_t enb_cc_idx = 0; enb_cc_idx < pending_events.cc_list.size(); ++enb_cc_idx) {
    auto& cc_feedback = pending_events.cc_list[enb_cc_idx];

    cc_feedback.configured = true;
    for (uint32_t pid = 0; pid < SCHED_NR_MAX_HARQ; ++pid) {
      auto& dl_h = ue_ctxt.cc_list[enb_cc_idx].dl_harqs[pid];
      auto& ul_h = ue_ctxt.cc_list[enb_cc_idx].ul_harqs[pid];

      // Set default DL ACK
      if (dl_h.active and dl_h.last_slot_ack == current_slot_tx) {
        cc_feedback.dl_acks.push_back(ue_nr_slot_events::ack_t{pid, true});
      }

      // Set default UL ACK
      if (ul_h.active and ul_h.last_slot_ack == current_slot_tx) {
        cc_feedback.ul_acks.emplace_back(ue_nr_slot_events::ack_t{pid, true});
      }

      // Set default CQI
      if (ue_ctxt.ue_cfg.phy_cfg.csi.reports[0].type == SRSRAN_CSI_REPORT_TYPE_PERIODIC) {
        auto& p = ue_ctxt.ue_cfg.phy_cfg.csi.reports[0].periodic;
        if (p.offset == pending_events.slot_rx.to_uint() % p.period) {
          cc_feedback.cqi = 15;
        }
      }

      // TODO: other CSI
    }
  }

  return SRSRAN_SUCCESS;
}

int sched_nr_base_test_bench::apply_slot_events(sim_nr_ue_ctxt_t& ue_ctxt, const ue_nr_slot_events& events)
{
  for (uint32_t enb_cc_idx = 0; enb_cc_idx < events.cc_list.size(); ++enb_cc_idx) {
    const auto& cc_feedback = events.cc_list[enb_cc_idx];
    if (not cc_feedback.configured) {
      continue;
    }

    for (auto& ack : cc_feedback.dl_acks) {
      auto& h = ue_ctxt.cc_list[enb_cc_idx].dl_harqs[ack.pid];

      if (ack.ack) {
        logger.info("EVENT: DL ACK rnti=0x%x slot_dl_tx=%u cc=%d pid=%d, tbs=%d",
                    ue_ctxt.rnti,
                    h.last_slot_tx.to_uint(),
                    enb_cc_idx,
                    ack.pid,
                    h.tbs);
        dl_buffer_state_diff(ue_ctxt.rnti, -(int)h.tbs);
      }

      // update scheduler
      sched_ptr->dl_ack_info(ue_ctxt.rnti, enb_cc_idx, h.pid, 0, ack.ack);

      // update UE sim context
      if (ack.ack or ue_ctxt.is_last_dl_retx(enb_cc_idx, h.pid)) {
        h.active = false;
      }
    }

    for (auto& ack : cc_feedback.ul_acks) {
      auto& h = ue_ctxt.cc_list[enb_cc_idx].ul_harqs[ack.pid];

      if (ack.ack) {
        logger.info("EVENT: UL ACK rnti=0x%x, slot_ul_tx=%u, cc=%d pid=%d",
                    ue_ctxt.rnti,
                    h.last_slot_tx.to_uint(),
                    enb_cc_idx,
                    h.pid);
      }

      // update scheduler
      sched_ptr->ul_crc_info(ue_ctxt.rnti, enb_cc_idx, ack.pid, ack.ack);

      if (h.is_msg3) {
        logger.info("STATUS: rnti=0x%x received Msg3", ue_ctxt.rnti);
        dl_buffer_state_diff(ue_ctxt.rnti, 0, 150); // Schedule RRC setup
      }
    }

    if (cc_feedback.cqi >= 0) {
      logger.info("EVENT: DL CQI rnti=0x%x, cqi=%d", ue_ctxt.rnti, cc_feedback.cqi);
      sched_ptr->dl_cqi_info(ue_ctxt.rnti, enb_cc_idx, cc_feedback.cqi);
    }
  }

  return SRSRAN_SUCCESS;
}

sim_nr_enb_ctxt_t sched_nr_base_test_bench::get_enb_ctxt() const
{
  sim_nr_enb_ctxt_t ctxt;
  ctxt.cell_params = cell_params;

  for (auto& ue_pair : ue_db) {
    ctxt.ue_db.insert(std::make_pair(ue_pair.first, &ue_pair.second.get_ctxt()));
  }

  return ctxt;
}

} // namespace srsenb