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

#include "sched_nr_sim_ue.h"
#include "sched_nr_common_test.h"
#include "sched_nr_ue_ded_test_suite.h"
#include "srsran/common/test_common.h"

namespace srsenb {

sched_nr_ue_sim::sched_nr_ue_sim(uint16_t                            rnti_,
                                 const sched_nr_interface::ue_cfg_t& ue_cfg_,
                                 slot_point                          prach_slot_rx,
                                 uint32_t                            preamble_idx) :
  logger(srslog::fetch_basic_logger("MAC"))
{
  ctxt.rnti          = rnti_;
  ctxt.prach_slot_rx = prach_slot_rx;
  ctxt.preamble_idx  = preamble_idx;
  ctxt.ue_cfg        = ue_cfg_;

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

  for (uint32_t i = 0; i < cc_out.dl_cc_result->dl_sched.pdcch_dl.size(); ++i) {
    const auto& data = cc_out.dl_cc_result->dl_sched.pdcch_dl[i];
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

void sched_nr_ue_sim::update_dl_harqs(const sched_nr_cc_output_res_t& cc_out)
{
  uint32_t cc = cc_out.cc;
  for (uint32_t i = 0; i < cc_out.dl_cc_result->dl_sched.pdcch_dl.size(); ++i) {
    const auto& data = cc_out.dl_cc_result->dl_sched.pdcch_dl[i];
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
      h.tbs           = 100; // TODO
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
}

sched_nr_sim_base::sched_nr_sim_base(const sched_nr_interface::sched_cfg_t&             sched_args,
                                     const std::vector<sched_nr_interface::cell_cfg_t>& cell_cfg_list,
                                     std::string                                        test_name_) :
  logger(srslog::fetch_basic_logger("TEST")),
  mac_logger(srslog::fetch_basic_logger("MAC")),
  sched_ptr(new sched_nr()),
  test_name(std::move(test_name_))
{
  logger.info("\n=========== Start %s ===========", test_name.c_str());
  cell_params.reserve(cell_cfg_list.size());
  for (uint32_t cc = 0; cc < cell_cfg_list.size(); ++cc) {
    cell_params.emplace_back(cc, cell_cfg_list[cc], sched_args);
  }
  sched_ptr->config(sched_args, cell_cfg_list); // call parent cfg

  TESTASSERT(cell_params.size() > 0);
}

sched_nr_sim_base::~sched_nr_sim_base()
{
  logger.info("=========== End %s ==========\n", test_name.c_str());
}

int sched_nr_sim_base::add_user(uint16_t                            rnti,
                                const sched_nr_interface::ue_cfg_t& ue_cfg_,
                                slot_point                          tti_rx,
                                uint32_t                            preamble_idx)
{
  TESTASSERT(ue_db.count(rnti) == 0);

  sched_ptr->ue_cfg(rnti, ue_cfg_);
  ue_db.insert(std::make_pair(rnti, sched_nr_ue_sim(rnti, ue_cfg_, current_slot_tx, preamble_idx)));

  sched_nr_interface::dl_sched_rar_info_t rach_info{};
  rach_info.temp_crnti   = rnti;
  rach_info.prach_slot   = tti_rx;
  rach_info.preamble_idx = preamble_idx;
  rach_info.msg3_size    = 7;
  sched_ptr->dl_rach_info(ue_cfg_.carriers[0].cc, rach_info);

  return SRSRAN_SUCCESS;
}

void sched_nr_sim_base::new_slot(slot_point slot_tx)
{
  std::unique_lock<std::mutex> lock(mutex);
  while (cc_finished > 0) {
    cvar.wait(lock);
  }
  logger.set_context(slot_tx.to_uint());
  mac_logger.set_context(slot_tx.to_uint());
  logger.info("---------------- TTI=%d ---------------", slot_tx.to_uint());
  current_slot_tx = slot_tx;
  cc_finished     = cell_params.size();
  for (auto& ue : ue_db) {
    ue_nr_slot_events events;
    set_default_slot_events(ue.second.get_ctxt(), events);
    set_external_slot_events(ue.second.get_ctxt(), events);
    apply_slot_events(ue.second.get_ctxt(), events);
  }
}

void sched_nr_sim_base::update(sched_nr_cc_output_res_t& cc_out)
{
  std::unique_lock<std::mutex> lock(mutex);

  sim_nr_enb_ctxt_t ctxt;
  ctxt = get_enb_ctxt();

  // Run common tests
  test_dl_pdcch_consistency(cc_out.dl_cc_result->dl_sched.pdcch_dl);
  test_pdsch_consistency(cc_out.dl_cc_result->dl_sched.pdsch);
  test_ssb_scheduled_grant(cc_out.slot, ctxt.cell_params[cc_out.cc].cell_cfg, cc_out.dl_cc_result->dl_sched.ssb);

  // Run UE-dedicated tests
  test_dl_sched_result(ctxt, cc_out);

  for (auto& u : ue_db) {
    u.second.update(cc_out);
  }

  if (--cc_finished <= 0) {
    cvar.notify_one();
  }
}

int sched_nr_sim_base::set_default_slot_events(const sim_nr_ue_ctxt_t& ue_ctxt, ue_nr_slot_events& pending_events)
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
      if (dl_h.active and (dl_h.last_slot_ack) == current_slot_tx) {
        cc_feedback.dl_acks.push_back(ue_nr_slot_events::ack_t{pid, true});
      }

      // Set default UL ACK
      if (ul_h.active and (ul_h.last_slot_tx + 8) == current_slot_tx) {
        cc_feedback.ul_acks.emplace_back(ue_nr_slot_events::ack_t{pid, true});
      }

      // TODO: other CSI
    }
  }

  return SRSRAN_SUCCESS;
}

int sched_nr_sim_base::apply_slot_events(sim_nr_ue_ctxt_t& ue_ctxt, const ue_nr_slot_events& events)
{
  for (uint32_t enb_cc_idx = 0; enb_cc_idx < events.cc_list.size(); ++enb_cc_idx) {
    const auto& cc_feedback = events.cc_list[enb_cc_idx];
    if (not cc_feedback.configured) {
      continue;
    }

    for (auto& ack : cc_feedback.dl_acks) {
      auto& h = ue_ctxt.cc_list[enb_cc_idx].dl_harqs[ack.pid];

      if (ack.ack) {
        logger.info(
            "DL ACK rnti=0x%x slot_dl_tx=%u cc=%d pid=%d", ue_ctxt.rnti, h.last_slot_tx.to_uint(), enb_cc_idx, ack.pid);
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
        logger.info(
            "UL ACK rnti=0x%x, slot_ul_tx=%u, cc=%d pid=%d", ue_ctxt.rnti, h.last_slot_tx.to_uint(), enb_cc_idx, h.pid);
      }

      //      // update scheduler
      //      if (sched_ptr->ul_crc_info(events.slot_rx.to_uint(), ue_ctxt.rnti, enb_cc_idx, cc_feedback.ul_ack) < 0) {
      //        logger.error("The ACKed UL Harq pid=%d does not exist.", cc_feedback.ul_pid);
      //        error_counter++;
      //      }
    }
  }

  return SRSRAN_SUCCESS;
}

sim_nr_enb_ctxt_t sched_nr_sim_base::get_enb_ctxt() const
{
  sim_nr_enb_ctxt_t ctxt;
  ctxt.cell_params = cell_params;

  for (auto& ue_pair : ue_db) {
    ctxt.ue_db.insert(std::make_pair(ue_pair.first, &ue_pair.second.get_ctxt()));
  }

  return ctxt;
}

} // namespace srsenb