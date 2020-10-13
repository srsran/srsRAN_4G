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

#ifndef SRSLTE_SCHEDULER_TEST_UTILS_H
#define SRSLTE_SCHEDULER_TEST_UTILS_H

#include "srsenb/hdr/stack/mac/scheduler.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srslte/common/test_common.h"
#include "srslte/interfaces/sched_interface.h"
#include <algorithm>
#include <chrono>
#include <unordered_map>

/*****************************
 * Setup Sched Configuration
 ****************************/

inline srsenb::sched_interface::cell_cfg_t generate_default_cell_cfg(uint32_t nof_prb)
{
  srsenb::sched_interface::cell_cfg_t cell_cfg     = {};
  srslte_cell_t&                      cell_cfg_phy = cell_cfg.cell;

  /* Set PHY cell configuration */
  cell_cfg_phy.id              = 1;
  cell_cfg_phy.cp              = SRSLTE_CP_NORM;
  cell_cfg_phy.nof_ports       = 1;
  cell_cfg_phy.nof_prb         = nof_prb;
  cell_cfg_phy.phich_length    = SRSLTE_PHICH_NORM;
  cell_cfg_phy.phich_resources = SRSLTE_PHICH_R_1;

  cell_cfg.sibs[0].len       = 18;
  cell_cfg.sibs[0].period_rf = 8;
  cell_cfg.sibs[1].len       = 41;
  cell_cfg.sibs[1].period_rf = 16;
  cell_cfg.si_window_ms      = 40;
  cell_cfg.nrb_pucch         = (cell_cfg_phy.nof_prb == 6) ? 1 : 2;
  cell_cfg.prach_freq_offset = (cell_cfg_phy.nof_prb == 6) ? 0 : 2;
  cell_cfg.prach_rar_window  = 3;
  cell_cfg.maxharq_msg3tx    = 3;
  cell_cfg.initial_dl_cqi    = 5;

  return cell_cfg;
}

inline srsenb::sched_interface::ue_cfg_t generate_default_ue_cfg()
{
  srsenb::sched_interface::ue_cfg_t ue_cfg = {};

  ue_cfg.maxharq_tx = 5;
  ue_cfg.supported_cc_list.resize(1);
  ue_cfg.supported_cc_list[0].aperiodic_cqi_period = 40;
  ue_cfg.supported_cc_list[0].enb_cc_idx           = 0;
  ue_cfg.supported_cc_list[0].active               = true;
  ue_cfg.supported_cc_list[0].dl_cfg.tm            = SRSLTE_TM1;
  ue_cfg.ue_bearers[0].direction                   = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;

  return ue_cfg;
}

inline srsenb::sched_interface::ue_cfg_t generate_default_ue_cfg2()
{
  srsenb::sched_interface::ue_cfg_t ue_cfg = generate_default_ue_cfg();

  ue_cfg.ue_bearers[srsenb::RB_ID_SRB1].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.ue_bearers[srsenb::RB_ID_SRB2].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].group     = 1;

  return ue_cfg;
}

inline srsenb::sched_interface::ue_cfg_t generate_rach_ue_cfg(const srsenb::sched_interface::ue_cfg_t& final_cfg)
{
  srsenb::sched_interface::ue_cfg_t cfg        = {};
  cfg.ue_bearers[srsenb::RB_ID_SRB0].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  cfg.supported_cc_list.resize(1);
  cfg.supported_cc_list[0].enb_cc_idx = final_cfg.supported_cc_list[0].enb_cc_idx;
  cfg.supported_cc_list[0].active     = true;
  return cfg;
}

inline srsenb::sched_interface::ue_cfg_t generate_setup_ue_cfg(const srsenb::sched_interface::ue_cfg_t& final_cfg)
{
  srsenb::sched_interface::ue_cfg_t cfg = generate_rach_ue_cfg(final_cfg);

  cfg.maxharq_tx                               = final_cfg.maxharq_tx;
  cfg.ue_bearers[srsenb::RB_ID_SRB1].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  cfg.supported_cc_list[0].dl_cfg.tm           = SRSLTE_TM1;
  cfg.continuous_pusch                         = final_cfg.continuous_pusch;

  cfg.supported_cc_list[0].dl_cfg.cqi_report    = final_cfg.supported_cc_list[0].dl_cfg.cqi_report;
  cfg.pucch_cfg                                 = final_cfg.pucch_cfg;
  cfg.supported_cc_list[0].aperiodic_cqi_period = final_cfg.supported_cc_list[0].aperiodic_cqi_period;

  return cfg;
}

inline srsenb::sched_interface::ue_cfg_t generate_reconf_ue_cfg(const srsenb::sched_interface::ue_cfg_t& final_cfg)
{
  srsenb::sched_interface::ue_cfg_t cfg = generate_setup_ue_cfg(final_cfg);

  cfg.supported_cc_list.resize(1);
  cfg.ue_bearers                     = {};
  cfg.ue_bearers[srsenb::RB_ID_SRB0] = final_cfg.ue_bearers[srsenb::RB_ID_SRB0];
  cfg.ue_bearers[srsenb::RB_ID_SRB1] = final_cfg.ue_bearers[srsenb::RB_ID_SRB1];

  return cfg;
}

/*****************************
 *       Event Types
 ****************************/

//! Struct with ue_cfg_t params used by the scheduler, and params used in its behavior simulation
struct ue_ctxt_test_cfg {
  bool                              periodic_cqi = false;
  uint32_t                          cqi_Npd = 10, cqi_Noffset = 5; // CQI reporting
  std::vector<float>                prob_dl_ack_mask{0.5, 0.5, 1}, prob_ul_ack_mask{0.5, 0.5, 1};
  srsenb::sched_interface::ue_cfg_t ue_cfg = generate_default_ue_cfg();
};

// Struct that represents all the events that take place in a TTI
struct tti_ev {
  struct user_buffer_ev {
    uint32_t sr_data      = 0; ///< update BSR
    uint32_t dl_data      = 0; ///< update DL buffer newtx
    uint32_t dl_nof_retxs = 0; ///< update DL buffer retx
  };
  struct user_cfg_ev {
    uint16_t                                                  rnti;
    std::unique_ptr<ue_ctxt_test_cfg>                         ue_sim_cfg;       ///< optional ue_cfg call
    std::unique_ptr<srsenb::sched_interface::ue_bearer_cfg_t> bearer_cfg;       ///< optional bearer_cfg call
    std::unique_ptr<user_buffer_ev>                           buffer_ev;        ///< update of a user dl/ul buffer
    bool                                                      rem_user = false; ///< whether to remove a ue
  };
  std::vector<user_cfg_ev> user_updates;
};

struct sim_sched_args {
  uint32_t                                         start_tti = 0;
  std::vector<srsenb::sched_interface::cell_cfg_t> cell_cfg;
  srslte::log*                                     sim_log = nullptr;
  ue_ctxt_test_cfg                                 default_ue_sim_cfg{};
  srsenb::sched_interface::sched_args_t            sched_args = {};
};

// generate all events up front
struct sched_sim_events {
  sim_sched_args      sim_args; ///< arguments used to generate TTI events
  std::vector<tti_ev> tti_events;
};

struct sched_sim_event_generator {
  uint16_t next_rnti   = 70;
  uint32_t tti_counter = 0;

  struct user_data {
    uint16_t rnti;
    uint32_t tti_start    = 0;
    uint32_t tti_duration = 0;
  };
  std::unordered_map<uint16_t, user_data> current_users;

  // generated events
  std::vector<tti_ev> tti_events;

  sched_sim_event_generator() { tti_events.push_back(tti_ev{}); }

  void step_tti(uint32_t nof_ttis = 1)
  {
    tti_counter += nof_ttis;
    if (tti_counter >= tti_events.size()) {
      tti_events.resize(tti_counter + 1);
    }
    rem_old_users();
  }

  int step_until(uint32_t tti)
  {
    if (tti_counter >= tti) {
      // error
      return -1;
    }
    int jump    = tti - tti_counter;
    tti_counter = tti;
    if (tti_counter >= tti_events.size()) {
      tti_events.resize(tti_counter + 1);
    }
    rem_old_users();
    return jump;
  }

  tti_ev::user_cfg_ev* add_new_default_user(uint32_t duration, const srsenb::sched_interface::ue_cfg_t& ue_cfg)
  {
    std::vector<tti_ev::user_cfg_ev>& user_updates = tti_events[tti_counter].user_updates;
    user_updates.emplace_back();
    auto& user = user_updates.back();
    user.rnti  = next_rnti++;
    // creates a user with one supported CC (PRACH stage)
    user.ue_sim_cfg.reset(new ue_ctxt_test_cfg{});
    auto& u                 = current_users[user.rnti];
    u.rnti                  = user.rnti;
    u.tti_start             = tti_counter;
    u.tti_duration          = duration;
    user.ue_sim_cfg->ue_cfg = ue_cfg;
    return &user;
  }

  int add_dl_data(uint16_t rnti, uint32_t new_data)
  {
    TESTASSERT(user_exists(rnti));
    tti_ev::user_cfg_ev* user = get_user_cfg(rnti);
    if (user->buffer_ev == nullptr) {
      user->buffer_ev.reset(new tti_ev::user_buffer_ev{});
    }
    user->buffer_ev->dl_data = new_data;
    return SRSLTE_SUCCESS;
  }

  int add_ul_data(uint16_t rnti, uint32_t new_data)
  {
    TESTASSERT(user_exists(rnti));
    tti_ev::user_cfg_ev* user = get_user_cfg(rnti);
    if (user->buffer_ev == nullptr) {
      user->buffer_ev.reset(new tti_ev::user_buffer_ev{});
    }
    user->buffer_ev->sr_data = new_data;
    return SRSLTE_SUCCESS;
  }

  tti_ev::user_cfg_ev* user_reconf(uint16_t rnti)
  {
    if (not user_exists(rnti)) {
      return nullptr;
    }
    tti_ev::user_cfg_ev* user = get_user_cfg(rnti);
    ue_ctxt_test_cfg     ue_sim_cfg{};
    ue_sim_cfg.ue_cfg = generate_default_ue_cfg();
    user->ue_sim_cfg.reset(new ue_ctxt_test_cfg{ue_sim_cfg});
    // it should by now have a DRB1. Add other DRBs manually
    user->ue_sim_cfg->ue_cfg.ue_bearers[srsenb::RB_ID_SRB2].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
    user->ue_sim_cfg->ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
    user->ue_sim_cfg->ue_cfg.ue_bearers[srsenb::RB_ID_DRB1].group     = 1;
    return user;
  }

private:
  tti_ev::user_cfg_ev* get_user_cfg(uint16_t rnti)
  {
    std::vector<tti_ev::user_cfg_ev>& user_updates = tti_events[tti_counter].user_updates;
    auto                              it           = std::find_if(
        user_updates.begin(), user_updates.end(), [&rnti](tti_ev::user_cfg_ev& user) { return user.rnti == rnti; });
    if (it == user_updates.end()) {
      user_updates.emplace_back();
      user_updates.back().rnti = rnti;
      return &user_updates.back();
    }
    return &(*it);
  }

  bool user_exists(uint16_t rnti) { return current_users.find(rnti) != current_users.end(); }

  void rem_old_users()
  {
    // remove users that pass their connection duration
    for (auto it = current_users.begin(); it != current_users.end();) {
      user_data& user    = it->second;
      uint32_t   rem_tti = user.tti_start + user.tti_duration;
      if (rem_tti > tti_counter) {
        ++it;
        continue;
      }
      // set the call rem_user(...) at the right tti
      auto& l       = tti_events[rem_tti].user_updates;
      auto  user_it = std::find_if(l.begin(), l.end(), [&it](tti_ev::user_cfg_ev& u) { return it->first == u.rnti; });
      if (user_it == l.end()) {
        l.emplace_back();
        l.back().rem_user = true;
        l.back().rnti     = it->first;
      } else {
        user_it->rem_user = true;
      }
      it = current_users.erase(it);
    }
  }
};

#endif // SRSLTE_SCHEDULER_TEST_UTILS_H
