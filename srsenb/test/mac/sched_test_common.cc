/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sched_test_common.h"
#include "srsenb/hdr/stack/mac/sched.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srslte/mac/pdu.h"

#include "sched_common_test_suite.h"
#include "sched_ue_ded_test_suite.h"
#include "srslte/common/test_common.h"

using namespace srsenb;

/***************************
 *     Random Utils
 **************************/

std::default_random_engine rand_gen;

float ::srsenb::randf()
{
  static std::uniform_real_distribution<float> unif_dist(0, 1.0);
  return unif_dist(rand_gen);
}

void ::srsenb::set_randseed(uint64_t seed)
{
  rand_gen = std::default_random_engine(seed);
}

std::default_random_engine& ::srsenb::get_rand_gen()
{
  return rand_gen;
}

/***********************
 *  User State Tester
 ***********************/

ue_ctxt_test::ue_ctxt_test(uint16_t                rnti_,
                           srslte::tti_point       prach_tti_,
                           const ue_ctxt_test_cfg& cfg_,
                           ue_sim&                 ue_ctxt_) :
  sim_cfg(cfg_), rnti(rnti_), current_tti_rx(prach_tti_), ue_ctxt(&ue_ctxt_)
{
  set_cfg(cfg_.ue_cfg);
}

int ue_ctxt_test::set_cfg(const sched::ue_cfg_t& ue_cfg_)
{
  for (uint32_t ue_cc_idx = 0; ue_cc_idx < ue_cfg_.supported_cc_list.size(); ++ue_cc_idx) {
    const auto& cc = ue_cfg_.supported_cc_list[ue_cc_idx];
    if (ue_cc_idx >= active_ccs.size()) {
      // new cell
      active_ccs.emplace_back();
      active_ccs.back().ue_cc_idx  = active_ccs.size() - 1;
      active_ccs.back().enb_cc_idx = cc.enb_cc_idx;
    } else {
      CONDERROR(cc.enb_cc_idx != active_ccs[ue_cc_idx].enb_cc_idx, "changing ccs not supported\n");
    }
  }

  user_cfg = ue_cfg_;
  return SRSLTE_SUCCESS;
}

ue_ctxt_test::cc_ue_ctxt_test* ue_ctxt_test::get_cc_state(uint32_t enb_cc_idx)
{
  auto it = std::find_if(active_ccs.begin(), active_ccs.end(), [enb_cc_idx](const cc_ue_ctxt_test& c) {
    return c.enb_cc_idx == enb_cc_idx;
  });
  return it == active_ccs.end() ? nullptr : &(*it);
}

int ue_ctxt_test::new_tti(sched* sched_ptr, srslte::tti_point tti_rx)
{
  current_tti_rx = tti_rx;

  TESTASSERT(fwd_pending_acks(sched_ptr) == SRSLTE_SUCCESS);
  if (sim_cfg.periodic_cqi and (tti_rx.to_uint() % sim_cfg.cqi_Npd) == sim_cfg.cqi_Noffset) {
    for (auto& cc : active_ccs) {
      sched_ptr->dl_cqi_info(
          tti_rx.to_uint(), rnti, cc.enb_cc_idx, std::uniform_int_distribution<uint32_t>{5, 24}(get_rand_gen()));
      sched_ptr->ul_cqi_info(
          tti_rx.to_uint(), rnti, cc.enb_cc_idx, std::uniform_int_distribution<uint32_t>{5, 24}(get_rand_gen()), 0);
    }
  }

  return SRSLTE_SUCCESS;
}

int ue_ctxt_test::fwd_pending_acks(sched* sched_ptr)
{
  /* Ack DL HARQs */
  // Checks:
  // - Pending DL ACK {cc_idx,rnti,tb} exist in scheduler harqs
  // - Pending DL ACK tti_ack correspond to the expected based on tti_tx_dl
  while (not pending_dl_acks.empty()) {
    auto& p = pending_dl_acks.top();
    if (p.tti_ack > current_tti_rx) {
      break;
    }
    auto& h = ue_ctxt->get_ctxt().cc_list[p.ue_cc_idx].dl_harqs[p.pid];
    CONDERROR(not h.active, "The ACKed DL Harq pid=%d is not active\n", h.pid);
    CONDERROR(to_tx_dl(h.last_tti_rx) + FDD_HARQ_DELAY_DL_MS != p.tti_ack, "dl ack hasn't arrived when expected\n");
    CONDERROR(sched_ptr->dl_ack_info(current_tti_rx.to_uint(), rnti, p.cc_idx, p.tb, p.ack) <= 0,
              "The ACKed DL Harq pid=%d does not exist.\n",
              p.pid);

    if (p.ack) {
      log_h->info("DL ACK tti=%u rnti=0x%x pid=%d\n", current_tti_rx.to_uint(), rnti, p.pid);
    }
    if (p.ack or ue_ctxt->get_ctxt().is_last_dl_retx(p.ue_cc_idx, p.pid)) {
      h.active = false;
    }
    pending_dl_acks.pop();
  }

  /* Ack UL HARQs */
  while (not pending_ul_acks.empty()) {
    auto& p = pending_ul_acks.top();
    if (p.tti_ack > current_tti_rx) {
      break;
    }
    auto& h = ue_ctxt->get_ctxt().cc_list[p.ue_cc_idx].ul_harqs[p.pid];
    CONDERROR(not h.active, "The ACKed UL Harq pid=%d is not active\n", h.pid);
    CONDERROR(to_tx_ul(h.last_tti_rx) != p.tti_ack, "UL CRC wasn't set when expected\n");
    CONDERROR(sched_ptr->ul_crc_info(current_tti_rx.to_uint(), rnti, p.cc_idx, p.ack) != SRSLTE_SUCCESS,
              "Failed UL ACK\n");

    if (p.ack) {
      log_h->info("UL ACK tti=%u rnti=0x%x pid=%d\n", current_tti_rx.to_uint(), rnti, p.pid);
    }
    pending_ul_acks.pop();
  }

  return SRSLTE_SUCCESS;
}

int ue_ctxt_test::test_sched_result(uint32_t                     enb_cc_idx,
                                    const sched::dl_sched_res_t& dl_result,
                                    const sched::ul_sched_res_t& ul_result)
{
  cc_result result{enb_cc_idx, &dl_result, &ul_result};
  TESTASSERT(test_scell_activation(result) == SRSLTE_SUCCESS);
  TESTASSERT(schedule_acks(result) == SRSLTE_SUCCESS);
  return SRSLTE_SUCCESS;
}

/**
 * Tests whether the SCells are correctly activated. Individual tests:
 * - no DL and UL allocations in inactive carriers
 */
int ue_ctxt_test::test_scell_activation(cc_result result)
{
  auto cc_it =
      std::find_if(user_cfg.supported_cc_list.begin(),
                   user_cfg.supported_cc_list.end(),
                   [&result](const sched::ue_cfg_t::cc_cfg_t& cc) { return cc.enb_cc_idx == result.enb_cc_idx; });

  if (cc_it == user_cfg.supported_cc_list.end() or not cc_it->active) {
    // cell not active. Ensure data allocations are not made
    for (uint32_t i = 0; i < result.dl_result->nof_data_elems; ++i) {
      CONDERROR(result.dl_result->data[i].dci.rnti == rnti, "Allocated user in inactive carrier\n");
    }
    for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
      CONDERROR(result.ul_result->pusch[i].dci.rnti == rnti, "Allocated user in inactive carrier\n");
    }
  } else {
    uint32_t ue_cc_idx = std::distance(user_cfg.supported_cc_list.begin(), cc_it);
    for (uint32_t i = 0; i < result.dl_result->nof_data_elems; ++i) {
      if (result.dl_result->data[i].dci.rnti == rnti) {
        CONDERROR(result.dl_result->data[i].dci.ue_cc_idx != ue_cc_idx, "User cell index was incorrectly set\n");
      }
    }
    for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
      if (result.ul_result->pusch[i].dci.rnti == rnti) {
        CONDERROR(result.ul_result->pusch[i].dci.ue_cc_idx != ue_cc_idx, "The user cell index was incorrectly set\n");
      }
    }
  }

  return SRSLTE_SUCCESS;
}

int ue_ctxt_test::schedule_acks(cc_result result)
{
  auto* cc = get_cc_state(result.enb_cc_idx);
  if (cc == nullptr) {
    return SRSLTE_SUCCESS;
  }
  /* Schedule DL ACKs */
  for (uint32_t i = 0; i < result.dl_result->nof_data_elems; ++i) {
    const auto& data = result.dl_result->data[i];
    if (data.dci.rnti != rnti) {
      continue;
    }
    pending_ack_t ack_data;
    ack_data.tti_ack   = to_tx_dl_ack(current_tti_rx);
    ack_data.cc_idx    = result.enb_cc_idx;
    ack_data.tb        = 0;
    ack_data.pid       = data.dci.pid;
    ack_data.ue_cc_idx = data.dci.ue_cc_idx;
    uint32_t nof_retx  = sched_utils::get_nof_retx(data.dci.tb[0].rv); // 0..3
    ack_data.ack       = randf() < sim_cfg.prob_dl_ack_mask[nof_retx % sim_cfg.prob_dl_ack_mask.size()];

    pending_dl_acks.push(ack_data);
  }

  /* Schedule UL ACKs */
  for (uint32_t i = 0; i < result.ul_result->nof_dci_elems; ++i) {
    const auto& pusch = result.ul_result->pusch[i];
    if (pusch.dci.rnti != rnti) {
      continue;
    }

    pending_ack_t ack_data;
    ack_data.tti_ack   = to_tx_ul(current_tti_rx);
    ack_data.cc_idx    = result.enb_cc_idx;
    ack_data.ue_cc_idx = pusch.dci.ue_cc_idx;
    ack_data.tb        = 0;
    ack_data.pid       = to_tx_ul(current_tti_rx).to_uint() % (FDD_HARQ_DELAY_DL_MS + FDD_HARQ_DELAY_UL_MS);
    uint32_t nof_retx  = sched_utils::get_nof_retx(pusch.dci.tb.rv); // 0..3
    ack_data.ack       = randf() < sim_cfg.prob_ul_ack_mask[nof_retx % sim_cfg.prob_ul_ack_mask.size()];

    pending_ul_acks.push(ack_data);
  }
  return SRSLTE_SUCCESS;
}

void user_state_sched_tester::new_tti(sched* sched_ptr, uint32_t tti_rx)
{
  tic++;
  for (auto& u : users) {
    u.second.new_tti(sched_ptr, srslte::tti_point{tti_rx});
  }
}

int user_state_sched_tester::add_user(uint16_t rnti, uint32_t preamble_idx, const ue_ctxt_test_cfg& cfg_)
{
  CONDERROR(!srslte_prach_tti_opportunity_config_fdd(
                cell_params[cfg_.ue_cfg.supported_cc_list[0].enb_cc_idx].prach_config, tic.to_uint(), -1),
            "New user added in a non-PRACH TTI\n");
  TESTASSERT(users.count(rnti) == 0);
  sim_users.add_user(rnti, generate_rach_ue_cfg(cfg_.ue_cfg), tic, preamble_idx);

  ue_ctxt_test ue{rnti, tic, cfg_, sim_users.at(rnti)};
  users.insert(std::make_pair(rnti, ue));

  return SRSLTE_SUCCESS;
}

int user_state_sched_tester::user_reconf(uint16_t rnti, const srsenb::sched_interface::ue_cfg_t& ue_cfg)
{
  TESTASSERT(users.count(rnti) > 0);
  users.at(rnti).set_cfg(ue_cfg);
  sim_users.ue_recfg(rnti, ue_cfg);
  return SRSLTE_SUCCESS;
}

int user_state_sched_tester::bearer_cfg(uint16_t                                        rnti,
                                        uint32_t                                        lcid,
                                        const srsenb::sched_interface::ue_bearer_cfg_t& bearer_cfg)
{
  auto it = users.find(rnti);
  TESTASSERT(it != users.end());
  it->second.user_cfg.ue_bearers[lcid] = bearer_cfg;
  users.at(rnti).drb_cfg_flag          = false;
  for (uint32_t i = 2; i < it->second.user_cfg.ue_bearers.size(); ++i) {
    if (it->second.user_cfg.ue_bearers[i].direction != sched_interface::ue_bearer_cfg_t::IDLE) {
      users.at(rnti).drb_cfg_flag = true;
    }
  }
  return SRSLTE_SUCCESS;
}

void user_state_sched_tester::rem_user(uint16_t rnti)
{
  users.erase(rnti);
  sim_users.rem_user(rnti);
}

int user_state_sched_tester::test_all(const sf_output_res_t& sf_out)
{
  // Perform UE-dedicated sched result tests
  sim_enb_ctxt_t enb_ctxt;
  enb_ctxt.cell_params = &cell_params;
  enb_ctxt.ue_db       = sim_users.get_ues_ctxt();
  TESTASSERT(test_all_ues(enb_ctxt, sf_out) == SRSLTE_SUCCESS);

  // Update Simulated UEs state
  sim_users.update(sf_out);

  for (uint32_t enb_cc_idx = 0; enb_cc_idx < enb_ctxt.cell_params->size(); ++enb_cc_idx) {
    for (auto& u : users) {
      TESTASSERT(u.second.test_sched_result(
                     enb_cc_idx, sf_out.dl_cc_result[enb_cc_idx], sf_out.ul_cc_result[enb_cc_idx]) == SRSLTE_SUCCESS);
    }
  }

  return SRSLTE_SUCCESS;
}

/***********************
 *  Sim Stats Storage
 **********************/

void sched_result_stats::process_results(const tti_params_t&                                 tti_params,
                                         const std::vector<sched_interface::dl_sched_res_t>& dl_result,
                                         const std::vector<sched_interface::ul_sched_res_t>& ul_result)
{
  for (uint32_t ccidx = 0; ccidx < dl_result.size(); ++ccidx) {
    for (uint32_t i = 0; i < dl_result[ccidx].nof_data_elems; ++i) {
      user_stats* user = get_user(dl_result[ccidx].data[i].dci.rnti);
      user->tot_dl_sched_data[ccidx] += dl_result[ccidx].data[i].tbs[0];
      user->tot_dl_sched_data[ccidx] += dl_result[ccidx].data[i].tbs[1];
    }
    for (uint32_t i = 0; i < ul_result[ccidx].nof_dci_elems; ++i) {
      user_stats* user = get_user(ul_result[ccidx].pusch[i].dci.rnti);
      user->tot_ul_sched_data[ccidx] += ul_result[ccidx].pusch[i].tbs;
    }
  }
}

sched_result_stats::user_stats* sched_result_stats::get_user(uint16_t rnti)
{
  if (users.count(rnti) != 0) {
    return &users[rnti];
  }
  users[rnti].rnti = rnti;
  users[rnti].tot_dl_sched_data.resize(cell_params.size(), 0);
  users[rnti].tot_ul_sched_data.resize(cell_params.size(), 0);
  return &users[rnti];
}

/***********************
 * Common Sched Tester
 **********************/

const sched::ue_cfg_t* common_sched_tester::get_current_ue_cfg(uint16_t rnti) const
{
  return ue_tester->get_user_cfg(rnti);
}

int common_sched_tester::sim_cfg(sim_sched_args args)
{
  sim_args0 = std::move(args);

  sched::cell_cfg(sim_args0.cell_cfg); // call parent cfg
  sched::set_sched_cfg(&sim_args0.sched_args);

  ue_tester.reset(new user_state_sched_tester{sim_args0.cell_cfg});
  sched_stats.reset(new sched_result_stats{sim_args0.cell_cfg});

  tester_log = sim_args0.sim_log;

  return SRSLTE_SUCCESS;
}

int common_sched_tester::add_user(uint16_t rnti, const ue_ctxt_test_cfg& ue_cfg_)
{
  CONDERROR(ue_cfg(rnti, generate_rach_ue_cfg(ue_cfg_.ue_cfg)) != SRSLTE_SUCCESS,
            "Configuring new user rnti=0x%x to sched\n",
            rnti);
  //        CONDERROR(!srslte_prach_tti_opportunity_config_fdd(
  //            sched_cell_params[CARRIER_IDX].cfg.prach_config, tti_info.tti_params.tti_rx, -1),
  //                  "New user added in a non-PRACH TTI\n");

  dl_sched_rar_info_t rar_info = {};
  rar_info.prach_tti           = tti_info.tti_params.tti_rx;
  rar_info.temp_crnti          = rnti;
  rar_info.msg3_size           = 7;
  rar_info.preamble_idx        = tti_info.nof_prachs++;
  uint32_t pcell_idx           = ue_cfg_.ue_cfg.supported_cc_list[0].enb_cc_idx;
  dl_rach_info(pcell_idx, rar_info);

  ue_tester->add_user(rnti, rar_info.preamble_idx, ue_cfg_);

  tester_log->info("Adding user rnti=0x%x\n", rnti);
  return SRSLTE_SUCCESS;
}

int common_sched_tester::reconf_user(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_)
{
  CONDERROR(not ue_tester->user_exists(rnti), "User must already exist to be configured\n");
  CONDERROR(ue_cfg(rnti, ue_cfg_) != SRSLTE_SUCCESS, "Configuring new user rnti=0x%x to sched\n", rnti);
  ue_tester->user_reconf(rnti, ue_cfg_);
  return SRSLTE_SUCCESS;
}

void common_sched_tester::rem_user(uint16_t rnti)
{
  tester_log->info("Removing user rnti=0x%x\n", rnti);
  sched::ue_rem(rnti);
  ue_tester->rem_user(rnti);
}

void common_sched_tester::new_test_tti()
{
  if (not tic.is_valid()) {
    tic = srslte::tti_point{sim_args0.start_tti};
  } else {
    tic++;
  }

  tti_info.tti_params = tti_params_t{tic.to_uint()};
  tti_info.nof_prachs = 0;
  tti_info.dl_sched_result.clear();
  tti_info.ul_sched_result.clear();
  tti_info.dl_sched_result.resize(sched_cell_params.size());
  tti_info.ul_sched_result.resize(sched_cell_params.size());

  tester_log->step(tti_info.tti_params.tti_rx);
}

int common_sched_tester::process_results()
{
  // Perform common eNB result tests
  sf_output_res_t sf_out{sched_cell_params,
                         srslte::tti_point{tti_info.tti_params.tti_rx},
                         tti_info.ul_sched_result,
                         tti_info.dl_sched_result};
  TESTASSERT(test_all_common(sf_out) == SRSLTE_SUCCESS);

  TESTASSERT(ue_tester->test_all(sf_out) == SRSLTE_SUCCESS);

  sched_stats->process_results(tti_info.tti_params, tti_info.dl_sched_result, tti_info.ul_sched_result);

  return SRSLTE_SUCCESS;
}

int common_sched_tester::process_tti_events(const tti_ev& tti_ev)
{
  for (const tti_ev::user_cfg_ev& ue_ev : tti_ev.user_updates) {
    // There is a new configuration
    if (ue_ev.ue_sim_cfg != nullptr) {
      if (not ue_tester->user_exists(ue_ev.rnti)) {
        // new user
        TESTASSERT(add_user(ue_ev.rnti, *ue_ev.ue_sim_cfg) == SRSLTE_SUCCESS);
      } else {
        // reconfiguration
        TESTASSERT(reconf_user(ue_ev.rnti, ue_ev.ue_sim_cfg->ue_cfg) == SRSLTE_SUCCESS);
      }
    }

    // There is a user to remove
    if (ue_ev.rem_user) {
      rem_user(ue_ev.rnti);
    }

    // configure bearers
    if (ue_ev.bearer_cfg != nullptr) {
      CONDERROR(not ue_tester->user_exists(ue_ev.rnti), "User rnti=0x%x does not exist\n", ue_ev.rnti);
      // TODO: Instantiate more bearers
      bearer_ue_cfg(ue_ev.rnti, 0, ue_ev.bearer_cfg.get());
    }

    const ue_ctxt_test* user = ue_tester->get_user_ctxt(ue_ev.rnti);

    if (user != nullptr) {
      const auto& ue_sim_ctxt = user->ue_ctxt->get_ctxt();
      if (not ue_sim_ctxt.msg4_tti_rx.is_valid() and ue_sim_ctxt.msg3_tti_rx.is_valid() and
          to_tx_ul(ue_sim_ctxt.msg3_tti_rx) <= tic) {
        // Msg3 has been received but Msg4 has not been yet transmitted
        // Setup default UE config
        reconf_user(user->rnti, generate_setup_ue_cfg(sim_args0.default_ue_sim_cfg.ue_cfg));

        // Schedule RRC Setup and ConRes CE
        uint32_t pending_dl_new_data = ue_db[ue_ev.rnti].get_pending_dl_rlc_data();
        if (pending_dl_new_data == 0) {
          uint32_t lcid = RB_ID_SRB0; // Use SRB0 to schedule Msg4
          dl_rlc_buffer_state(ue_ev.rnti, lcid, 50, 0);
          dl_mac_buffer_state(ue_ev.rnti, (uint32_t)srslte::dl_sch_lcid::CON_RES_ID);
        } else {
          // Let SRB0 Msg4 get fully transmitted
        }
      }
    }

    // push UL SRs and DL packets
    if (ue_ev.buffer_ev != nullptr) {
      CONDERROR(user == nullptr, "TESTER ERROR: Trying to schedule data for user that does not exist\n");
      const auto& ue_sim_ctxt = user->ue_ctxt->get_ctxt();
      if (ue_ev.buffer_ev->dl_data > 0 and ue_sim_ctxt.msg4_tti_rx.is_valid()) {
        // If Msg4 has already been tx and there DL data to transmit
        uint32_t lcid                = RB_ID_DRB1;
        uint32_t pending_dl_new_data = ue_db[ue_ev.rnti].get_pending_dl_rlc_data();
        if (user->drb_cfg_flag or pending_dl_new_data == 0) {
          // If RRCSetup finished
          if (not user->drb_cfg_flag) {
            reconf_user(user->rnti, sim_args0.default_ue_sim_cfg.ue_cfg);
            // setup lcid==drb1 bearer
            sched::ue_bearer_cfg_t cfg = {};
            cfg.direction              = ue_bearer_cfg_t::BOTH;
            cfg.group                  = 1;
            ue_tester->bearer_cfg(ue_ev.rnti, lcid, cfg);
            bearer_ue_cfg(ue_ev.rnti, lcid, &cfg);
          }
          // DRB is set. Update DL buffer
          uint32_t tot_dl_data = pending_dl_new_data + ue_ev.buffer_ev->dl_data; // TODO: derive pending based on rx
          dl_rlc_buffer_state(ue_ev.rnti, lcid, tot_dl_data, 0);                 // TODO: Check retx_queue
        } else {
          // Let SRB0 get emptied
        }
      }

      if (ue_ev.buffer_ev->sr_data > 0 and user->drb_cfg_flag) {
        uint32_t tot_ul_data =
            ue_db[ue_ev.rnti].get_pending_ul_new_data(tti_info.tti_params.tti_tx_ul, -1) + ue_ev.buffer_ev->sr_data;
        uint32_t lcg = 1;
        ul_bsr(ue_ev.rnti, lcg, tot_ul_data);
      }
    }
  }
  return SRSLTE_SUCCESS;
}

int common_sched_tester::run_tti(const tti_ev& tti_events)
{
  new_test_tti();
  tester_log->info("---- tti=%u | nof_ues=%zd ----\n", tic.to_uint(), ue_db.size());

  ue_tester->new_tti(this, tti_info.tti_params.tti_rx);
  process_tti_events(tti_events);
  before_sched();

  // Call scheduler for all carriers
  tti_info.dl_sched_result.resize(sched_cell_params.size());
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    dl_sched(tti_info.tti_params.tti_tx_dl, i, tti_info.dl_sched_result[i]);
  }
  tti_info.ul_sched_result.resize(sched_cell_params.size());
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    ul_sched(tti_info.tti_params.tti_tx_ul, i, tti_info.ul_sched_result[i]);
  }

  process_results();
  tti_count++;
  return SRSLTE_SUCCESS;
}

int common_sched_tester::test_next_ttis(const std::vector<tti_ev>& tti_events)
{
  while (tti_count < tti_events.size()) {
    TESTASSERT(run_tti(tti_events[tti_count]) == SRSLTE_SUCCESS);
  }
  return SRSLTE_SUCCESS;
}
