/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "scheduler_test_common.h"
#include "scheduler_test_utils.h"
#include "srsenb/hdr/stack/mac/scheduler.h"

using namespace srsenb;

template <class MapContainer, class Predicate>
void erase_if(MapContainer& c, Predicate should_remove)
{
  for (auto it = c.begin(); it != c.end();) {
    if (should_remove(*it)) {
      it = c.erase(it);
    } else {
      ++it;
    }
  }
}

/*******************
 *     Logging     *
 *******************/

class sched_test_log final : public srslte::test_log_filter
{
public:
  sched_test_log() : srslte::test_log_filter("TEST") { exit_on_error = true; }
  ~sched_test_log() override { log_diagnostics(); }

  void log_diagnostics() override
  {
    info("[TESTER] Number of assertion warnings: %u\n", warn_counter);
    info("[TESTER] Number of assertion errors: %u\n", error_counter);
    info("[TESTER] This was the seed: %u\n", seed);
  }
};
srslte::scoped_log<sched_test_log> log_global{};

/******************************
 * Setup Scheduler Tester Args
 *****************************/

sched_sim_events generate_default_sim_events(uint32_t nof_prb, uint32_t nof_ccs)
{
  sched_sim_events sim_events;
  sim_sched_args&  sim_args = sim_events.sim_args;

  sim_args.nof_ttis = 10240 + 10;
  sim_args.P_retx   = 0.1;

  sim_args.ue_cfg = generate_default_ue_cfg();

  // setup two cells
  std::vector<srsenb::sched_interface::cell_cfg_t> cell_cfg(nof_ccs, generate_default_cell_cfg(nof_prb));
  cell_cfg[0].scell_list.resize(1);
  cell_cfg[0].scell_list[0].enb_cc_idx               = 1;
  cell_cfg[0].scell_list[0].cross_carrier_scheduling = false;
  cell_cfg[0].scell_list[0].ul_allowed               = true;
  cell_cfg[1].cell.id                                = 2; // id=2
  cell_cfg[1].scell_list                             = cell_cfg[0].scell_list;
  cell_cfg[1].scell_list[0].enb_cc_idx               = 0;
  sim_args.cell_cfg                                  = std::move(cell_cfg);

  sim_args.bearer_cfg           = {};
  sim_args.bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;

  /* Setup Derived Params */
  sim_args.ue_cfg.supported_cc_list.resize(nof_ccs);
  for (uint32_t i = 0; i < sim_args.ue_cfg.supported_cc_list.size(); ++i) {
    sim_args.ue_cfg.supported_cc_list[i].active     = true;
    sim_args.ue_cfg.supported_cc_list[i].enb_cc_idx = i;
  }

  return sim_events;
}

sched_sim_events generate_sim1()
{
  /* Simulation Configuration */
  uint32_t nof_prb = 25;
  uint32_t nof_ccs = 2;

  sched_sim_events sim_events = generate_default_sim_events(nof_prb, nof_ccs);

  /* Internal configurations. Do not touch */
  float ul_sr_exps[]   = {1, 4}; // log rand
  float dl_data_exps[] = {1, 4}; // log rand
  float P_ul_sr = randf() * 0.5, P_dl = randf() * 0.5;

  sched_sim_event_generator generator;

  /* Setup Events */
  uint32_t prach_tti = 1, msg4_tot_delay = 10; // TODO: check correct value
  uint32_t msg4_size = 20;                     // TODO: Check
  uint32_t duration  = 1000;

  // Event PRACH: at prach_tti
  generator.step_until(prach_tti);
  tti_ev::user_cfg_ev* user = generator.add_new_default_user(duration);
  uint16_t             rnti = user->rnti;

  // Event (TTI=prach_tti+msg4_tot_delay): First Tx (Msg4). Goes in SRB0 and contains ConRes
  generator.step_tti(msg4_tot_delay);
  generator.add_dl_data(rnti, msg4_size);

  // Event (20 TTIs): Data back and forth
  auto generate_data = [&](uint32_t nof_ttis) {
    for (uint32_t i = 0; i < nof_ttis; ++i) {
      generator.step_tti();
      bool ul_flag = randf() < P_ul_sr, dl_flag = randf() < P_dl;
      if (dl_flag) {
        float exp = dl_data_exps[0] + randf() * (dl_data_exps[1] - dl_data_exps[0]);
        generator.add_dl_data(rnti, pow(10, exp));
      }
      if (ul_flag) {
        float exp = ul_sr_exps[0] + randf() * (ul_sr_exps[1] - ul_sr_exps[0]);
        generator.add_ul_data(rnti, pow(10, exp));
      }
    }
  };
  generate_data(20);

  // Event: Reconf Complete. Activate SCells
  user = generator.user_reconf(rnti);
  user->ue_cfg->supported_cc_list.resize(nof_ccs);
  for (uint32_t i = 0; i < user->ue_cfg->supported_cc_list.size(); ++i) {
    user->ue_cfg->supported_cc_list[i].active     = true;
    user->ue_cfg->supported_cc_list[i].enb_cc_idx = i;
  }
  // now we have two CCs

  // Generate a bit more data, now it should go through both cells
  generate_data(20);
  // We should have scheduled the SCell Activation by now

  sim_events.tti_events        = std::move(generator.tti_events);
  sim_events.sim_args.nof_ttis = sim_events.tti_events.size();

  return sim_events;
}

/******************************
 *   Scheduler Tester for CA
 *****************************/

class sched_ca_tester : public srsenb::sched
{
public:
  struct tti_info_t {
    tti_params_t                                 tti_params{10241};
    uint32_t                                     nof_prachs = 0;
    std::vector<sched_interface::dl_sched_res_t> dl_sched_result;
    std::vector<sched_interface::ul_sched_res_t> ul_sched_result;
  };

  int  cell_cfg(const std::vector<cell_cfg_t>& cell_params) final;
  int  add_user(uint16_t rnti, const ue_cfg_t& ue_cfg_);
  void new_test_tti(uint32_t tti_rx);
  int  process_tti_events(const tti_ev& tti_events);
  int  process_ack_txs();
  int  set_acks();

  void run_tti(uint32_t tti, const tti_ev& tti_events);

  int process_results();

  // args
  sim_sched_args sim_args; ///< arguments used to generate TTI events

  // tti specific params
  tti_info_t tti_info;

  // testers
  std::vector<output_sched_tester>         output_tester;
  std::unique_ptr<user_state_sched_tester> ue_tester;

private:
  struct ack_info_t {
    uint16_t             rnti;
    uint32_t             tti;
    uint32_t             ue_cc_idx;
    bool                 ack        = false;
    uint32_t             retx_delay = 0;
    srsenb::dl_harq_proc dl_harq;
  };
  struct ul_ack_info_t {
    uint16_t             rnti;
    uint32_t             tti_ack, tti_tx_ul;
    uint32_t             ue_cc_idx;
    bool                 ack = false;
    srsenb::ul_harq_proc ul_harq;
  };

  std::multimap<uint32_t, ack_info_t>    to_ack;
  std::multimap<uint32_t, ul_ack_info_t> to_ul_ack;
};

int sched_ca_tester::cell_cfg(const std::vector<cell_cfg_t>& cell_params)
{
  sched::cell_cfg(cell_params); // call parent
  ue_tester.reset(new user_state_sched_tester{cell_params});
  output_tester.clear();
  output_tester.reserve(cell_params.size());
  for (uint32_t i = 0; i < cell_params.size(); ++i) {
    output_tester.emplace_back(sched_cell_params[i]);
  }
  return SRSLTE_SUCCESS;
}

int sched_ca_tester::add_user(uint16_t rnti, const ue_cfg_t& ue_cfg_)
{
  CONDERROR(ue_cfg(rnti, ue_cfg_) != SRSLTE_SUCCESS, "[TESTER] Configuring new user rnti=0x%x to sched\n", rnti);

  dl_sched_rar_info_t rar_info = {};
  rar_info.prach_tti           = tti_info.tti_params.tti_rx;
  rar_info.temp_crnti          = rnti;
  rar_info.msg3_size           = 7;
  rar_info.preamble_idx        = tti_info.nof_prachs++;
  uint32_t pcell_idx           = ue_cfg_.supported_cc_list[0].enb_cc_idx;
  dl_rach_info(pcell_idx, rar_info);

  ue_tester->add_user(rnti, rar_info.preamble_idx, ue_cfg_);

  log_global->info("[TESTER] Adding user rnti=0x%x\n", rnti);
  return SRSLTE_SUCCESS;
}

void sched_ca_tester::new_test_tti(uint32_t tti_rx)
{
  tti_info.tti_params = tti_params_t{tti_rx};
  tti_info.nof_prachs = 0;
  tti_info.dl_sched_result.clear();
  tti_info.ul_sched_result.clear();
  ue_tester->new_tti(tti_rx);
}

int sched_ca_tester::process_tti_events(const tti_ev& tti_ev)
{
  for (const tti_ev::user_cfg_ev& ue_ev : tti_ev.user_updates) {
    // There is a new configuration
    if (ue_ev.ue_cfg != nullptr) {
      if (not ue_tester->user_exists(ue_ev.rnti)) {
        // new user
        TESTASSERT(add_user(ue_ev.rnti, *ue_ev.ue_cfg) == SRSLTE_SUCCESS);
      } else {
        // reconfiguration
        TESTASSERT(ue_cfg(ue_ev.rnti, *ue_ev.ue_cfg) == SRSLTE_SUCCESS);
        ue_tester->user_reconf(ue_ev.rnti, *ue_ev.ue_cfg);
      }
    }

    // There is a user to remove
    if (ue_ev.rem_user) {
      //        bearer_ue_rem(ue_ev.rnti, 0);
      ue_rem(ue_ev.rnti);
      ue_tester->rem_user(ue_ev.rnti);
      log_global->info("[TESTER] Removing user rnti=0x%x\n", ue_ev.rnti);
    }

    // configure carriers
    if (ue_ev.bearer_cfg != nullptr) {
      CONDERROR(not ue_tester->user_exists(ue_ev.rnti), "User rnti=0x%x does not exist\n", ue_ev.rnti);
      // TODO: Instantiate more bearers
      bearer_ue_cfg(ue_ev.rnti, 0, ue_ev.bearer_cfg.get());
    }

    // push UL SRs and DL packets
    if (ue_ev.buffer_ev != nullptr) {
      auto* user = ue_tester->get_user_state(ue_ev.rnti);
      CONDERROR(user == nullptr, "TESTER ERROR: Trying to schedule data for user that does not exist\n");

      if (ue_ev.buffer_ev->dl_data > 0) {
        // If Msg3 has already been received
        if (user->msg3_tti >= 0 and (uint32_t) user->msg3_tti <= tti_info.tti_params.tti_rx) {
          // If Msg4 not yet sent, allocate data in SRB0 buffer
          uint32_t lcid                = (user->msg4_tti >= 0) ? 2 : 0;
          uint32_t pending_dl_new_data = ue_db[ue_ev.rnti].get_pending_dl_new_data();
          if (lcid == 2 and not user->drb_cfg_flag) {
            // If RRCSetup finished
            if (pending_dl_new_data == 0) {
              // setup lcid==2 bearer
              sched::ue_bearer_cfg_t cfg = {};
              cfg.direction              = ue_bearer_cfg_t::BOTH;
              ue_tester->bearer_cfg(ue_ev.rnti, 2, cfg);
              bearer_ue_cfg(ue_ev.rnti, 2, &cfg);
            } else {
              // Let SRB0 get emptied
              continue;
            }
          }
          // Update DL buffer
          uint32_t tot_dl_data = pending_dl_new_data + ue_ev.buffer_ev->dl_data; // TODO: derive pending based on rx
          dl_rlc_buffer_state(ue_ev.rnti, lcid, tot_dl_data, 0);                 // TODO: Check retx_queue
        }
      }

      if (ue_ev.buffer_ev->sr_data > 0 and user->drb_cfg_flag) {
        uint32_t tot_ul_data =
            ue_db[ue_ev.rnti].get_pending_ul_new_data(tti_info.tti_params.tti_tx_ul) + ue_ev.buffer_ev->sr_data;
        uint32_t lcid = 2;
        ul_bsr(ue_ev.rnti, lcid, tot_ul_data, true);
      }
    }
  }
  return SRSLTE_SUCCESS;
}

int sched_ca_tester::process_ack_txs()
{
  /* check if user was removed. If so, clean respective acks */
  erase_if(to_ack,
           [this](std::pair<const uint32_t, ack_info_t>& elem) { return this->ue_db.count(elem.second.rnti) == 0; });
  erase_if(to_ul_ack,
           [this](std::pair<const uint32_t, ul_ack_info_t>& elem) { return this->ue_db.count(elem.second.rnti) == 0; });

  /* Ack DL HARQs */
  for (const auto& ack_it : to_ack) {
    if (ack_it.second.tti != tti_info.tti_params.tti_rx) {
      continue;
    }
    const ack_info_t& dl_ack = ack_it.second;

    srsenb::dl_harq_proc*       h    = ue_db[dl_ack.rnti].get_dl_harq(ack_it.second.dl_harq.get_id(), dl_ack.ue_cc_idx);
    const srsenb::dl_harq_proc& hack = dl_ack.dl_harq;
    CONDERROR(hack.is_empty(), "[TESTER] The acked DL harq was not active\n");

    bool ret = false;
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; ++tb) {
      if (dl_ack.dl_harq.is_empty(tb)) {
        continue;
      }
      ret |= dl_ack_info(tti_info.tti_params.tti_rx, dl_ack.rnti, dl_ack.ue_cc_idx, tb, dl_ack.ack) > 0;
    }
    CONDERROR(not ret, "[TESTER] The dl harq proc that was ACKed does not exist\n");

    if (dl_ack.ack) {
      CONDERROR(!h->is_empty(), "[TESTER] ACKed dl harq was not emptied\n");
      CONDERROR(h->has_pending_retx(0, tti_info.tti_params.tti_tx_dl),
                "[TESTER] ACKed dl harq still has pending retx\n");
      log_global->info("[TESTER] DL ACK tti=%u rnti=0x%x pid=%d\n",
                       tti_info.tti_params.tti_rx,
                       dl_ack.rnti,
                       dl_ack.dl_harq.get_id());
    } else {
      CONDERROR(h->is_empty() and hack.nof_retx(0) + 1 < hack.max_nof_retx(), "[TESTER] NACKed DL harq got emptied\n");
    }
  }

  /* Ack UL HARQs */
  for (const auto& ack_it : to_ul_ack) {
    if (ack_it.first != tti_info.tti_params.tti_rx) {
      continue;
    }
    const ul_ack_info_t& ul_ack = ack_it.second;

    srsenb::ul_harq_proc*       h    = ue_db[ul_ack.rnti].get_ul_harq(tti_info.tti_params.tti_rx, ul_ack.ue_cc_idx);
    const srsenb::ul_harq_proc& hack = ul_ack.ul_harq;
    CONDERROR(h == nullptr or h->get_tti() != hack.get_tti(), "[TESTER] UL Harq TTI does not match the ACK TTI\n");
    CONDERROR(h->is_empty(0), "[TESTER] The acked UL harq is not active\n");
    CONDERROR(hack.is_empty(0), "[TESTER] The acked UL harq was not active\n");

    ul_crc_info(tti_info.tti_params.tti_rx, ul_ack.rnti, ul_ack.ue_cc_idx, ul_ack.ack);

    CONDERROR(!h->get_pending_data(), "[TESTER] UL harq lost its pending data\n");
    CONDERROR(!h->has_pending_ack(), "[TESTER] ACK/NACKed UL harq should have a pending ACK\n");

    if (ul_ack.ack) {
      CONDERROR(!h->is_empty(), "[TESTER] ACKed UL harq did not get emptied\n");
      CONDERROR(h->has_pending_retx(), "[TESTER] ACKed UL harq still has pending retx\n");
      log_global->info(
          "[TESTER] UL ACK tti=%u rnti=0x%x pid=%d\n", tti_info.tti_params.tti_rx, ul_ack.rnti, hack.get_id());
    } else {
      // NACK
      CONDERROR(!h->is_empty() and !h->has_pending_retx(), "[TESTER] If NACKed, UL harq has to have pending retx\n");
      CONDERROR(h->is_empty() and hack.nof_retx(0) + 1 < hack.max_nof_retx(),
                "[TESTER] Nacked UL harq did get emptied\n");
    }
  }

  // erase processed acks
  to_ack.erase(tti_info.tti_params.tti_rx);
  to_ul_ack.erase(tti_info.tti_params.tti_rx);

  //  bool ack = true; //(tti_data.tti_rx % 3) == 0;
  //  if (tti_data.tti_rx >= FDD_HARQ_DELAY_MS) {
  //    for (auto it = ue_db.begin(); it != ue_db.end(); ++it) {
  //      uint16_t              rnti = it->first;
  //      srsenb::ul_harq_proc* h    = ue_db[rnti].get_ul_harq(tti_data.tti_rx);
  //      if (h != nullptr and not h->is_empty()) {
  //        ul_crc_info(tti_data.tti_rx, rnti, ack);
  //      }
  //    }
  //  }
  return SRSLTE_SUCCESS;
}

void sched_ca_tester::run_tti(uint32_t tti_rx, const tti_ev& tti_events)
{
  new_test_tti(tti_rx);
  log_global->info("[TESTER] ---- tti=%u | nof_ues=%zd ----\n", tti_rx, ue_db.size());

  process_tti_events(tti_events);
  process_ack_txs();
  //  before_sched();

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
  set_acks();
}

int sched_ca_tester::process_results()
{
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    TESTASSERT(ue_tester->test_all(i, tti_info.dl_sched_result[i], tti_info.ul_sched_result[i]) == SRSLTE_SUCCESS);
    TESTASSERT(output_tester[i].test_all(
                   tti_info.tti_params, tti_info.dl_sched_result[i], tti_info.ul_sched_result[i]) == SRSLTE_SUCCESS);
  }

  return SRSLTE_SUCCESS;
}

int sched_ca_tester::set_acks()
{
  for (uint32_t ccidx = 0; ccidx < sched_cell_params.size(); ++ccidx) {
    // schedule future acks
    for (uint32_t i = 0; i < tti_info.dl_sched_result[ccidx].nof_data_elems; ++i) {
      ack_info_t ack_data;
      ack_data.rnti      = tti_info.dl_sched_result[ccidx].data[i].dci.rnti;
      ack_data.tti       = FDD_HARQ_DELAY_MS + tti_info.tti_params.tti_tx_dl;
      ack_data.ue_cc_idx = ue_db[ack_data.rnti].get_cell_index(ccidx).second;
      const srsenb::dl_harq_proc* dl_h =
          ue_db[ack_data.rnti].get_dl_harq(tti_info.dl_sched_result[ccidx].data[i].dci.pid, ccidx);
      ack_data.dl_harq = *dl_h;
      if (ack_data.dl_harq.nof_retx(0) == 0) {
        ack_data.ack = randf() > sim_args.P_retx;
      } else { // always ack after three retxs
        ack_data.ack = ack_data.dl_harq.nof_retx(0) == 3;
      }

      // Remove harq from the ack list if there was a harq rewrite
      auto it = to_ack.begin();
      while (it != to_ack.end() and it->first < ack_data.tti) {
        if (it->second.rnti == ack_data.rnti and it->second.dl_harq.get_id() == ack_data.dl_harq.get_id() and
            it->second.ue_cc_idx == ack_data.ue_cc_idx) {
          CONDERROR(it->second.tti + 2 * FDD_HARQ_DELAY_MS > ack_data.tti,
                    "[TESTER] The retx dl harq id=%d was transmitted too soon\n",
                    ack_data.dl_harq.get_id());
          auto toerase_it = it++;
          to_ack.erase(toerase_it);
          continue;
        }
        ++it;
      }
      // add new ack to the list
      to_ack.insert(std::make_pair(ack_data.tti, ack_data));
    }

    /* Schedule UL ACKs */
    for (uint32_t i = 0; i < tti_info.ul_sched_result[ccidx].nof_dci_elems; ++i) {
      const auto&   pusch = tti_info.ul_sched_result[ccidx].pusch[i];
      ul_ack_info_t ack_data;
      ack_data.rnti      = pusch.dci.rnti;
      ack_data.ul_harq   = *ue_db[ack_data.rnti].get_ul_harq(tti_info.tti_params.tti_tx_ul, ccidx);
      ack_data.tti_tx_ul = tti_info.tti_params.tti_tx_ul;
      ack_data.tti_ack   = tti_info.tti_params.tti_tx_ul + FDD_HARQ_DELAY_MS;
      ack_data.ue_cc_idx = ue_db[ack_data.rnti].get_cell_index(ccidx).second;
      if (ack_data.ul_harq.nof_retx(0) == 0) {
        ack_data.ack = randf() > sim_args.P_retx;
      } else {
        ack_data.ack = ack_data.ul_harq.nof_retx(0) == 3;
      }
      to_ul_ack.insert(std::make_pair(ack_data.tti_tx_ul, ack_data));
    }
  }
  return SRSLTE_SUCCESS;
}

int test_scheduler_ca(const sched_sim_events& sim_events)
{
  sched_ca_tester tester;
  tester.sim_args = sim_events.sim_args;

  // Setup scheduler
  tester.init(nullptr);
  TESTASSERT(tester.cell_cfg(sim_events.sim_args.cell_cfg) == SRSLTE_SUCCESS);

  uint32_t tti_start = 0; // rand_int(0, 10240);
  for (uint32_t nof_ttis = 0; nof_ttis < sim_events.sim_args.nof_ttis; ++nof_ttis) {
    uint32_t tti = (tti_start + nof_ttis) % 10240;
    log_global->step(tti);
    tester.run_tti(tti, sim_events.tti_events[nof_ttis]);
  }

  return SRSLTE_SUCCESS;
}

int main()
{
  srslte::logmap::get_instance()->set_default_log_level(srslte::LOG_LEVEL_INFO);
  printf("[TESTER] This is the chosen seed: %u\n", seed);
  uint32_t N_runs = 1;
  for (uint32_t n = 0; n < N_runs; ++n) {
    printf("Sim run number: %u\n", n + 1);
    sched_sim_events sim_events = generate_sim1();

    TESTASSERT(test_scheduler_ca(sim_events) == SRSLTE_SUCCESS);
  }

  return 0;
}