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

#include "sched_nr_sim_ue.h"
#include "srsran/common/test_common.h"

namespace srsenb {

sched_nr_ue_sim::sched_nr_ue_sim(uint16_t                            rnti_,
                                 const sched_nr_interface::ue_cfg_t& ue_cfg_,
                                 tti_point                           prach_tti_rx,
                                 uint32_t                            preamble_idx) :
  logger(srslog::fetch_basic_logger("MAC"))
{
  ctxt.rnti         = rnti_;
  ctxt.prach_tti_rx = prach_tti_rx;
  ctxt.preamble_idx = preamble_idx;
  ctxt.ue_cfg       = ue_cfg_;

  ctxt.cc_list.resize(ue_cfg_.carriers.size());
  for (auto& cc : ctxt.cc_list) {
    for (size_t pid = 0; pid < SCHED_NR_MAX_HARQ; ++pid) {
      cc.ul_harqs[pid].pid = pid;
      cc.dl_harqs[pid].pid = pid;
    }
  }
}

int sched_nr_ue_sim::update(const sched_nr_cc_output_res_t& cc_out)
{
  update_dl_harqs(cc_out);
  return SRSRAN_SUCCESS;
}

void sched_nr_ue_sim::update_dl_harqs(const sched_nr_cc_output_res_t& cc_out)
{
  uint32_t cc = cc_out.cc;
  for (uint32_t i = 0; i < cc_out.dl_cc_result->pdschs.size(); ++i) {
    const auto& data = cc_out.dl_cc_result->pdcchs[i];
    if (data.dci.ctx.rnti != ctxt.rnti) {
      continue;
    }
    auto& h = ctxt.cc_list[cc].dl_harqs[data.dci.pid];
    if (h.nof_txs == 0 or h.ndi != data.dci.ndi) {
      // It is newtx
      h.nof_retxs    = 0;
      h.ndi          = data.dci.ndi;
      h.first_tti_rx = cc_out.tti_rx;
      h.dci_loc      = data.dci.ctx.location;
      h.tbs          = 100; // TODO
    } else {
      // it is retx
      h.nof_retxs++;
    }
    h.active      = true;
    h.last_tti_rx = cc_out.tti_rx;
    h.nof_txs++;
  }
}

sched_nr_sim_base::sched_nr_sim_base(const sched_nr_interface::sched_cfg_t&             sched_args,
                                     const std::vector<sched_nr_interface::cell_cfg_t>& cell_cfg_list,
                                     std::string                                        test_name_) :
  logger(srslog::fetch_basic_logger("TEST")),
  mac_logger(srslog::fetch_basic_logger("MAC")),
  sched_ptr(new sched_nr(sched_args)),
  test_name(std::move(test_name_))
{
  logger.info("\n=========== Start %s ===========", test_name.c_str());
  cell_params.reserve(cell_cfg_list.size());
  for (uint32_t cc = 0; cc < cell_cfg_list.size(); ++cc) {
    cell_params.emplace_back(cc, cell_cfg_list[cc], sched_args);
  }
  sched_ptr->cell_cfg(cell_cfg_list); // call parent cfg
  TESTASSERT(cell_params.size() > 0);
}

sched_nr_sim_base::~sched_nr_sim_base()
{
  logger.info("=========== End %s ==========\n", test_name.c_str());
}

int sched_nr_sim_base::add_user(uint16_t rnti, const sched_nr_interface::ue_cfg_t& ue_cfg_, uint32_t preamble_idx)
{
  TESTASSERT(ue_db.count(rnti) == 0);

  sched_ptr->ue_cfg(rnti, ue_cfg_);
  ue_db.insert(std::make_pair(rnti, sched_nr_ue_sim(rnti, ue_cfg_, current_tti_rx, preamble_idx)));
  return SRSRAN_SUCCESS;
}

void sched_nr_sim_base::slot_indication(srsran::tti_point tti_rx)
{
  {
    std::unique_lock<std::mutex> lock(mutex);
    logger.set_context(tti_rx.to_uint());
    mac_logger.set_context(tti_rx.to_uint());
    current_tti_rx = tti_rx;
    logger.info("---------------- TTI=%d ---------------", tti_rx.to_uint());
    for (auto& ue : ue_db) {
      ue_tti_events events;
      set_default_tti_events(ue.second.get_ctxt(), events);
      set_external_tti_events(ue.second.get_ctxt(), events);
      apply_tti_events(ue.second.get_ctxt(), events);
    }
  }
  sched_ptr->slot_indication(tti_rx);
}

void sched_nr_sim_base::update(sched_nr_cc_output_res_t& cc_out)
{
  std::unique_lock<std::mutex> lock(mutex);
  for (auto& ue_pair : ue_db) {
    ue_pair.second.update(cc_out);
  }
}

int sched_nr_sim_base::set_default_tti_events(const sim_nr_ue_ctxt_t& ue_ctxt, ue_tti_events& pending_events)
{
  pending_events.cc_list.clear();
  pending_events.cc_list.resize(cell_params.size());
  pending_events.tti_rx = current_tti_rx;

  for (uint32_t enb_cc_idx = 0; enb_cc_idx < pending_events.cc_list.size(); ++enb_cc_idx) {
    auto& cc_feedback = pending_events.cc_list[enb_cc_idx];

    cc_feedback.configured = true;
    cc_feedback.ue_cc_idx  = enb_cc_idx;
    for (uint32_t pid = 0; pid < SCHED_NR_MAX_HARQ; ++pid) {
      auto& dl_h = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].dl_harqs[pid];
      auto& ul_h = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].ul_harqs[pid];

      // Set default DL ACK
      if (dl_h.active and (dl_h.last_tti_rx + 8) == current_tti_rx) {
        cc_feedback.dl_pid = pid;
        cc_feedback.dl_ack = true; // default is ACK
      }

      // Set default UL ACK
      if (ul_h.active and (ul_h.last_tti_rx + 8) == current_tti_rx) {
        cc_feedback.ul_pid = pid;
        cc_feedback.ul_ack = true;
      }

      // TODO: other CSI
    }
  }

  return SRSRAN_SUCCESS;
}

int sched_nr_sim_base::apply_tti_events(sim_nr_ue_ctxt_t& ue_ctxt, const ue_tti_events& events)
{
  for (uint32_t enb_cc_idx = 0; enb_cc_idx < events.cc_list.size(); ++enb_cc_idx) {
    const auto& cc_feedback = events.cc_list[enb_cc_idx];
    if (not cc_feedback.configured) {
      continue;
    }

    if (cc_feedback.dl_pid >= 0) {
      auto& h = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].dl_harqs[cc_feedback.dl_pid];

      if (cc_feedback.dl_ack) {
        logger.info("DL ACK rnti=0x%x tti_dl_tx=%u cc=%d pid=%d",
                    ue_ctxt.rnti,
                    to_tx_dl(h.last_tti_rx).to_uint(),
                    enb_cc_idx,
                    cc_feedback.dl_pid);
      }

      // update scheduler
      sched_ptr->dl_ack_info(ue_ctxt.rnti, enb_cc_idx, cc_feedback.dl_pid, cc_feedback.tb, cc_feedback.dl_ack);

      // update UE sim context
      if (cc_feedback.dl_ack or ue_ctxt.is_last_dl_retx(cc_feedback.ue_cc_idx, cc_feedback.dl_pid)) {
        h.active = false;
      }
    }

    if (cc_feedback.ul_pid >= 0) {
      auto& h = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].ul_harqs[cc_feedback.ul_pid];

      if (cc_feedback.ul_ack) {
        logger.info("UL ACK rnti=0x%x, tti_ul_tx=%u, cc=%d pid=%d",
                    ue_ctxt.rnti,
                    to_tx_ul(h.last_tti_rx).to_uint(),
                    enb_cc_idx,
                    cc_feedback.ul_pid);
      }

      //      // update scheduler
      //      if (sched_ptr->ul_crc_info(events.tti_rx.to_uint(), ue_ctxt.rnti, enb_cc_idx, cc_feedback.ul_ack) < 0) {
      //        logger.error("The ACKed UL Harq pid=%d does not exist.", cc_feedback.ul_pid);
      //        error_counter++;
      //      }
    }
  }

  return SRSRAN_SUCCESS;
}

} // namespace srsenb