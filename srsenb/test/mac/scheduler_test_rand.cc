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

#include "srsenb/hdr/stack/mac/scheduler.h"
#include "srsenb/hdr/stack/mac/scheduler_ue.h"
#include <algorithm>
#include <cstdlib>
#include <random>
#include <set>
#include <srslte/srslte.h>
#include <unistd.h>

#include "srsenb/hdr/phy/phy.h"
#include "srsenb/hdr/stack/mac/mac.h"

#include "srslte/common/log_filter.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/sched_interface.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/radio/radio.h"

// uint32_t const seed = std::random_device()();
uint32_t const                        seed = 2452071795; // time(NULL);
std::default_random_engine            rand_gen(seed);
std::uniform_real_distribution<float> unif_dist(0, 1.0);
float                                 randf()
{
  return unif_dist(rand_gen);
}
uint32_t err_counter  = 0;
uint32_t warn_counter = 0;
struct ue_stats_t {
  uint64_t nof_dl_rbs = 0;
  uint64_t nof_ul_rbs = 0;
};
std::map<uint16_t, ue_stats_t> ue_stats;

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

class log_tester : public srslte::log_filter
{
public:
  explicit log_tester(std::string layer) : srslte::log_filter(layer) {}
  void error(const char* message, ...) __attribute__((format(printf, 2, 3)));
};
void log_tester::error(const char* message, ...)
{
  if (level >= srslte::LOG_LEVEL_ERROR) {
    char*   args_msg = NULL;
    va_list args;
    va_start(args, message);
    if (vasprintf(&args_msg, message, args) > 0)
      all_log(srslte::LOG_LEVEL_ERROR, tti, args_msg);
    va_end(args);
    free(args_msg);
  }
}
log_tester log_out("ALL");

void log_on_exit()
{
  log_out.info("[TESTER] UE stats:\n");
  for (auto& e : ue_stats) {
    log_out.info("0x%x: {DL RBs: %lu, UL RBs: %lu}\n", e.first, e.second.nof_dl_rbs, e.second.nof_ul_rbs);
  }
  log_out.info("[TESTER] This was the seed: %u\n", seed);
}

#define Warning(fmt, ...)                                                                                              \
  log_out.warning(fmt, ##__VA_ARGS__);                                                                                 \
  warn_counter++;
#define TestError(fmt, ...)                                                                                            \
  log_out.error(fmt, ##__VA_ARGS__);                                                                                   \
  log_on_exit();                                                                                                       \
  exit(-1);
#define CondError(cond, fmt, ...)                                                                                      \
  if (cond) {                                                                                                          \
    log_out.error(fmt, ##__VA_ARGS__);                                                                                 \
    log_on_exit();                                                                                                     \
    exit(-1);                                                                                                          \
  }

/*******************
 *     Dummies     *
 *******************/

struct sched_sim_args {
  struct tti_event_t {
    struct user_event_t {
      uint32_t sr_data;
      uint32_t dl_data;
      uint32_t dl_nof_retxs;
      user_event_t() : sr_data(0), dl_data(0), dl_nof_retxs(0) {}
    };
    std::map<uint16_t, user_event_t> users;
    bool                             new_user;
    bool                             rem_user;
    uint32_t                         new_rnti;
    uint32_t                         rem_rnti;
    tti_event_t() : new_user(false), rem_user(false) {}
  };

  std::vector<tti_event_t>                 tti_events;
  uint32_t                                 nof_ttis;
  float                                    P_retx;
  srsenb::sched_interface::ue_cfg_t        ue_cfg;
  srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg;
};

// Designed for testing purposes
struct sched_tester : public srsenb::sched {
  struct tester_user_results {
    uint32_t                                  dl_pending_data     = 0;
    uint32_t                                  ul_pending_data     = 0; ///< data pending for UL
    bool                                      has_dl_retx         = false;
    bool                                      has_dl_tx           = false;
    bool                                      has_ul_tx           = false; ///< has either tx or retx
    bool                                      has_ul_retx         = false;
    bool                                      has_ul_newtx        = false; ///< *no* retx, but has tx
    bool                                      ul_retx_got_delayed = false;
    srsenb::sched_interface::ul_sched_data_t* ul_sched            = NULL; // fast lookup
    srsenb::sched_interface::dl_sched_data_t* dl_sched            = NULL; // fast lookup
    srsenb::dl_harq_proc                      dl_harqs[2 * FDD_HARQ_DELAY_MS];
    srsenb::ul_harq_proc                      ul_harq;
  };
  struct sched_tti_data {
    bool                              is_prach_tti_tx_ul = false;
    uint32_t                          ul_sf_idx;
    uint32_t                          tti_rx;
    uint32_t                          tti_tx_dl;
    uint32_t                          tti_tx_ul;
    uint32_t                          current_cfi;
    pending_msg3_t                    ul_pending_msg3;
    srslte::bounded_bitset<128, true> used_cce;
    //    std::vector<bool>                                         used_cce;
    std::map<uint16_t, tester_user_results>                   ue_data;   ///< stores buffer state of each user
    tester_user_results                                       total_ues; ///< stores combined UL/DL buffer state
    srsenb::sched_interface::ul_sched_res_t                   sched_result_ul;
    srsenb::sched_interface::dl_sched_res_t                   sched_result_dl;
    typedef std::map<uint16_t, tester_user_results>::iterator ue_it_t;
  };
  struct ue_info {
    int                                      prach_tti, rar_tti, msg3_tti;
    uint32_t                                 ra_id;
    srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg;
    srsenb::sched_interface::ue_cfg_t        user_cfg;
    uint32_t                                 dl_data;
    uint32_t                                 ul_data;
    ue_info() : prach_tti(-1), rar_tti(-1), msg3_tti(-1), dl_data(0), ul_data(0) {}
  };
  struct ack_info_t {
    uint16_t         rnti;
    uint32_t         tti;
    bool             dl_ack;
    uint32_t         retx_delay;
    srsenb::dl_harq_proc dl_harq;
    ack_info_t() : dl_ack(false), retx_delay(0) {}
  };
  struct ul_ack_info_t {
    uint16_t             rnti;
    uint32_t             tti_ack, tti_tx_ul;
    bool                 ack = false;
    srsenb::ul_harq_proc ul_harq;
  };

  sched_sim_args sim_args;

  // tester control data
  std::map<uint16_t, ue_info>                           tester_ues;
  std::multimap<uint32_t, ack_info_t>                   to_ack;
  std::multimap<uint32_t, ul_ack_info_t>                to_ul_ack;
  typedef std::multimap<uint32_t, ack_info_t>::iterator ack_it_t;

  // sched results
  sched_tti_data tti_data;

  void add_user(uint16_t                                 rnti,
                srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg,
                srsenb::sched_interface::ue_cfg_t        ue_cfg_);
  void rem_user(uint16_t rnti);
  void test_ra();
  void test_tti_result();
  void assert_no_empty_allocs();
  void test_collisions();
  void test_harqs();
  void run_tti(uint32_t tti_rx);

private:
  void new_test_tti(uint32_t tti_);
  void process_tti_args();
  void before_sched();
  void process_results();
  void ack_txs();
};

void sched_tester::add_user(uint16_t                                 rnti,
                            srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg,
                            srsenb::sched_interface::ue_cfg_t        ue_cfg_)
{
  uint32_t ra_id = rand() % 5;
  ue_info  info;
  info.prach_tti  = tti_data.tti_rx;
  info.ra_id      = ra_id;
  info.bearer_cfg = bearer_cfg;
  info.user_cfg   = ue_cfg_;
  tester_ues.insert(std::make_pair(rnti, info));

  if (ue_cfg(rnti, &ue_cfg_)) {
    TestError("[TESTER] Registering new user rnti=0x%x to SCHED\n", rnti);
  }
  dl_rach_info(tti_data.tti_rx, ra_id, rnti, 7);

  // setup bearers
  bearer_ue_cfg(rnti, 0, &bearer_cfg);

  log_out.info("[TESTER] Adding user rnti=0x%x\n", rnti);
}

void sched_tester::rem_user(uint16_t rnti)
{
  tester_ues.erase(rnti);
  tti_data.ue_data.erase(rnti);
}

void sched_tester::new_test_tti(uint32_t tti_)
{
  // NOTE: make a local copy, since some of these variables may be cleared during scheduling
  tti_data.tti_rx             = tti_;
  tti_data.tti_tx_dl          = TTI_TX(tti_);
  tti_data.tti_tx_ul          = TTI_RX_ACK(tti_);
  tti_data.is_prach_tti_tx_ul = srslte_prach_tti_opportunity_config_fdd(cfg.prach_config, tti_data.tti_tx_ul, -1);
  if (tti_data.tti_tx_ul > FDD_HARQ_DELAY_MS) {
    tti_data.ul_sf_idx = (tti_data.tti_tx_ul - FDD_HARQ_DELAY_MS) % 10;
  } else {
    tti_data.ul_sf_idx = (tti_data.tti_tx_ul + 10240 - FDD_HARQ_DELAY_MS) % 10;
  }
  tti_data.ul_pending_msg3 = pending_msg3[tti_data.tti_tx_ul % 10];
  tti_data.current_cfi     = sched_cfg.nof_ctrl_symbols;
  tti_data.used_cce.resize(srslte_regs_pdcch_ncce(&regs, tti_data.current_cfi));
  tti_data.used_cce.reset();
  tti_data.ue_data.clear();
  tti_data.total_ues = tester_user_results();
}

void sched_tester::process_tti_args()
{
  // may add a new user
  if (sim_args.tti_events[tti_data.tti_rx].new_user) {
    CondError(!srslte_prach_tti_opportunity_config_fdd(cfg.prach_config, tti_data.tti_rx, -1),
              "[TESTER] New user added in a non-PRACH TTI\n");
    uint16_t rnti = sim_args.tti_events[tti_data.tti_rx].new_rnti;
    add_user(rnti, sim_args.bearer_cfg, sim_args.ue_cfg);
  }

  // may remove an existing user
  if (sim_args.tti_events[tti_data.tti_rx].rem_user) {
    uint16_t rnti = sim_args.tti_events[tti_data.tti_rx].rem_rnti;
    bearer_ue_rem(rnti, 0);
    ue_rem(rnti);
    rem_user(rnti);
    log_out.info("[TESTER] Removing user rnti=0x%x\n", rnti);
  }

  // push UL SRs and DL packets
  for (auto& e : sim_args.tti_events[tti_data.tti_rx].users) {
    if (e.second.sr_data > 0) {
      uint32_t tot_ul_data = ue_db[e.first].get_pending_ul_new_data(tti_data.tti_tx_ul) + e.second.sr_data;
      uint32_t lcid        = 0;
      ul_bsr(e.first, lcid, tot_ul_data);
    }
    if (e.second.dl_data > 0) {
      uint32_t lcid = 0;
      // FIXME: Does it need TTI for checking pending data?
      uint32_t tot_dl_data = ue_db[e.first].get_pending_dl_new_data(tti_data.tti_tx_dl) + e.second.dl_data;
      dl_rlc_buffer_state(e.first, lcid, tot_dl_data, 0);
    }
  }
}

void sched_tester::before_sched()
{
  typedef std::map<uint16_t, srsenb::sched_ue>::iterator it_t;

  // check pending data buffers
  for (it_t it = ue_db.begin(); it != ue_db.end(); ++it) {
    uint16_t              rnti = it->first;
    srsenb::sched_ue*     user = &it->second;
    tester_user_results   d;
    srsenb::ul_harq_proc* hul = user->get_ul_harq(tti_data.tti_tx_ul);
    d.ul_pending_data         = get_ul_buffer(rnti);
    //        user->get_pending_ul_new_data(tti_data.tti_tx_ul) or hul->has_pending_retx(); // get_ul_buffer(rnti);
    d.dl_pending_data         = get_dl_buffer(rnti);
    d.has_ul_retx             = hul->has_pending_retx();
    d.has_ul_tx               = d.has_ul_retx or d.ul_pending_data > 0;
    srsenb::dl_harq_proc* hdl = user->get_pending_dl_harq(tti_data.tti_tx_dl);
    d.has_dl_retx             = (hdl != NULL) and hdl->has_pending_retx(0, tti_data.tti_tx_dl);
    d.has_dl_tx               = (hdl != NULL) or (it->second.get_empty_dl_harq() != NULL and d.dl_pending_data > 0);
    d.has_ul_newtx            = not d.has_ul_retx and d.ul_pending_data > 0;
    tti_data.ue_data.insert(std::make_pair(rnti, d));
    tti_data.total_ues.dl_pending_data += d.dl_pending_data;
    tti_data.total_ues.ul_pending_data += d.ul_pending_data;
    tti_data.total_ues.has_ul_tx |= d.has_ul_tx;
    tti_data.total_ues.has_dl_tx |= d.has_dl_tx;
    tti_data.total_ues.has_ul_newtx |= d.has_ul_newtx;

    for (uint32_t i = 0; i < 2 * FDD_HARQ_DELAY_MS; ++i) {
      const srsenb::dl_harq_proc* h      = user->get_dl_harq(i);
      tti_data.ue_data[rnti].dl_harqs[i] = *h;
    }
    // NOTE: ACK might have just cleared the harq for tti_data.tti_tx_ul
    tti_data.ue_data[rnti].ul_harq = *user->get_ul_harq(tti_data.tti_tx_ul);
  }

  // TODO: Check whether pending pending_rar.rar_tti correspond to a prach_tti
}

void sched_tester::process_results()
{
  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    uint16_t rnti                   = tti_data.sched_result_ul.pusch[i].dci.rnti;
    tti_data.ue_data[rnti].ul_sched = &tti_data.sched_result_ul.pusch[i];
    CondError(tester_ues.count(rnti) == 0,
              "[TESTER] [%d] The user rnti=0x%x that no longer exists got allocated.\n",
              tti_data.tti_rx,
              rnti);
  }
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_data_elems; ++i) {
    uint16_t rnti                   = tti_data.sched_result_dl.data[i].dci.rnti;
    tti_data.ue_data[rnti].dl_sched = &tti_data.sched_result_dl.data[i];
    CondError(tester_ues.count(rnti) == 0,
              "[TESTER] [%d] The user rnti=0x%x that no longer exists got allocated.\n",
              tti_data.tti_rx,
              rnti);
  }

  test_tti_result();
  test_ra();
  test_collisions();
  assert_no_empty_allocs();
  test_harqs();
}

void sched_tester::run_tti(uint32_t tti_rx)
{
  new_test_tti(tti_rx);
  log_out.info("[TESTER] ---- tti=%u | nof_ues=%lu ----\n", tti_rx, ue_db.size());

  process_tti_args();

  ack_txs();
  before_sched();

  dl_sched(tti_data.tti_tx_dl, &tti_data.sched_result_dl);
  ul_sched(tti_data.tti_tx_ul, &tti_data.sched_result_ul);

  process_results();
}

/**
 * Tests whether the RAR and Msg3 were scheduled within the expected windows
 */
void sched_tester::test_ra()
{
  uint32_t msg3_count = 0;

  // Test if allocations only take place for users with pending data or in RAR
  for (auto& iter : tti_data.ue_data) {
    uint16_t               rnti     = iter.first;
    sched_tester::ue_info& userinfo = tester_ues[rnti];

    // Check whether RA has completed correctly
    int prach_tti = userinfo.prach_tti;
    if (userinfo.msg3_tti > prach_tti) { // Msg3 already scheduled
      continue;
    }

    bool     rar_not_sent = prach_tti >= userinfo.rar_tti;
    uint32_t window[2]    = {(uint32_t)prach_tti + 3, prach_tti + 3 + cfg.prach_rar_window};
    if (rar_not_sent) {
      CondError(tti_data.tti_tx_dl > window[1], "[TESTER] There was no RAR scheduled within the RAR Window\n");
      if (tti_data.tti_tx_dl >= window[0]) {
        for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_rar_elems; ++i) {
          for (uint32_t j = 0; j < tti_data.sched_result_dl.rar[i].nof_grants; ++j) {
            if (tti_data.sched_result_dl.rar[i].msg3_grant[j].ra_id == userinfo.ra_id) {
              userinfo.rar_tti = tti_data.tti_tx_dl;
            }
          }
        }
      }
    } else { // RAR completed, check for Msg3
      uint32_t msg3_tti = (uint32_t)(userinfo.rar_tti + FDD_HARQ_DELAY_MS + MSG3_DELAY_MS) % 10240;
      if (msg3_tti == tti_data.tti_tx_ul) {
        for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
          if (tti_data.sched_result_ul.pusch[i].dci.rnti == rnti) {
            CondError(tti_data.sched_result_ul.pusch[i].needs_pdcch,
                      "[TESTER] Msg3 allocations do not require PDCCH\n");
            CondError(tti_data.ul_pending_msg3.rnti != rnti, "[TESTER] The UL pending msg3 RNTI did not match\n");
            CondError(not tti_data.ul_pending_msg3.enabled, "[TESTER] The UL pending msg3 RNTI did not match\n");
            userinfo.msg3_tti = tti_data.tti_tx_ul;
            msg3_count++;
          }
        }
        CondError(msg3_count == 0, "[TESTER] No UL msg3 allocation was made\n");
      } else if (msg3_tti < tti_data.tti_tx_ul) {
        TestError("[TESTER] No UL msg3 allocation was made\n");
      }
    }
  }
  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    msg3_count -= tti_data.sched_result_ul.pusch[i].needs_pdcch ? 0 : 1;
  }
  CondError(msg3_count > 0, "[TESTER] There are pending msg3 that do not belong to any active UE\n");
}

void sched_tester::assert_no_empty_allocs()
{
  // Test if allocations only take place for users with pending data or in RAR
  for (auto& iter : tti_data.ue_data) {
    uint16_t          rnti = iter.first;
    srsenb::sched_ue* user = &ue_db[rnti];

    if (!iter.second.has_ul_tx and tti_data.ue_data[rnti].ul_sched != NULL and
        tti_data.ue_data[rnti].ul_sched->needs_pdcch) {
      // FIXME: This test does not work for adaptive re-tx
      TestError("[TESTER] There was a user without data that got allocated in UL\n");
    }
    srsenb::ul_harq_proc* hul       = user->get_ul_harq(tti_data.tti_tx_ul);
    iter.second.ul_retx_got_delayed = iter.second.has_ul_retx and iter.second.ul_harq.is_empty(0);
    tti_data.total_ues.ul_retx_got_delayed |= iter.second.ul_retx_got_delayed;
    // Retxs cannot give space to newtx allocations
    CondError(
        tti_data.total_ues.ul_retx_got_delayed, "[TESTER] There was a retx that was erased for user rnti=0x%x\n", rnti);
  }

  // There must be allocations if there is pending data/retxs.
  bool no_dl_allocs = true;
  for (std::map<uint16_t, tester_user_results>::iterator it = tti_data.ue_data.begin(); it != tti_data.ue_data.end();
       ++it) {
    if (it->second.dl_sched != NULL) {
      no_dl_allocs = false;
    }
  }
  //  CondError(tti_data.total_ues.has_dl_tx and no_dl_allocs, "There was pending DL data but no user got allocated\n");
  // FIXME: You have to verify if there is space for the retx since it is non-adaptive
}

/**
 * Tests whether there were collisions in the DCI allocations
 */
void sched_tester::test_tti_result()
{
  tti_sched_t* tti_sched = get_tti_sched(tti_data.tti_rx);

  // Helper Function: checks if there is any collision. If not, fills the mask
  auto try_cce_fill = [&](const srslte_dci_location_t& dci_loc, const char* ch) {
    uint32_t cce_start = dci_loc.ncce, cce_stop = dci_loc.ncce + (1u << dci_loc.L);
    if (tti_data.used_cce.any(cce_start, cce_stop)) {
      TestError("[TESTER] %s DCI collision between CCE positions (%u, %u)\n", ch, cce_start, cce_stop);
    }
    tti_data.used_cce.fill(cce_start, cce_stop);
  };

  /* verify there are no dci collisions for UL, DL data, BC, RAR */
  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    const auto& pusch = tti_data.sched_result_ul.pusch[i];
    CondError(pusch.tbs == 0, "Allocated RAR process with invalid TBS=%d\n", pusch.tbs);
    CondError(ue_db.count(pusch.dci.rnti) == 0, "The allocated rnti=0x%x does not exist\n", pusch.dci.rnti);
    if (not pusch.needs_pdcch) {
      // In case of non-adaptive retx or Msg3
      continue;
    }
    CondError(pusch.dci.location.L == 0,
              "[TESTER] Invalid aggregation level %d\n",
              pusch.dci.location.L); // TODO: Extend this test
    try_cce_fill(pusch.dci.location, "UL");
  }
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_data_elems; ++i) {
    auto& data = tti_data.sched_result_dl.data[i];
    try_cce_fill(data.dci.location, "DL data");
    CondError(ue_db.count(data.dci.rnti) == 0, "Allocated rnti=0x%x that does not exist\n", data.dci.rnti);
  }
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_bc_elems; ++i) {
    auto& bc = tti_data.sched_result_dl.bc[i];
    try_cce_fill(bc.dci.location, "DL BC");
    if (bc.type == sched_interface::dl_sched_bc_t::BCCH) {
      CondError(bc.index >= MAX_SIBS, "Invalid SIB idx=%d\n", bc.index + 1);
      CondError(bc.tbs < cfg.sibs[bc.index].len,
                "Allocated BC process with TBS=%d < sib_len=%d\n",
                bc.tbs,
                cfg.sibs[bc.index].len);
    } else if (bc.type == sched_interface::dl_sched_bc_t::PCCH) {
      CondError(bc.tbs == 0, "Allocated paging process with invalid TBS=%d\n", bc.tbs);
    } else {
      TestError("Invalid broadcast process id=%d\n", (int)bc.type);
    }
  }
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_rar_elems; ++i) {
    const auto& rar = tti_data.sched_result_dl.rar[i];
    try_cce_fill(rar.dci.location, "DL RAR");
    CondError(rar.tbs == 0, "Allocated RAR process with invalid TBS=%d\n", rar.tbs);
    for (uint32_t j = 0; j < rar.nof_grants; ++j) {
      const auto& msg3_grant  = rar.msg3_grant[j];
      uint32_t    pending_tti = (tti_sched->get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY) % 10;
      CondError(not pending_msg3[pending_tti].enabled, "Pending Msg3 should have been set\n");
      uint32_t rba =
          srslte_ra_type2_to_riv(pending_msg3[pending_tti].L, pending_msg3[pending_tti].n_prb, cfg.cell.nof_prb);
      CondError(msg3_grant.grant.rba != rba, "Pending Msg3 RBA is not valid\n");
    }
  }

  /* verify if sched_result "used_cce" coincide with sched "used_cce" */
  auto* tti_alloc = get_tti_sched(tti_data.tti_rx);
  if (tti_data.used_cce != tti_alloc->get_pdcch_mask()) {
    std::string mask_str = tti_alloc->get_pdcch_mask().to_string();
    TestError("[TESTER] The used_cce do not match: (%s!=%s)\n", mask_str.c_str(), tti_data.used_cce.to_hex().c_str());
  }

  // FIXME: Check postponed retxs

  //  typedef std::map<uint16_t, srsenb::sched_ue>::iterator it_t;
  //  // There must be allocations if there is pending data/retxs.
  //  if(total_ues.has_ul_tx and ul_sched_result.empty()) {
  //    for (it_t it = ue_db.begin(); it != ue_db.end(); ++it) {
  //      uint32_t aggr_level = it->second.get_aggr_level(srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT0, cfg.cell.nof_prb,
  //      cfg.cell.nof_ports)); if (find_empty_dci(it->second.get_locations(current_cfi, sf_idx), aggr_level) > 0) {
  //        TestError("[%d] There was pending UL data and free CCEs, but no user got allocated\n", tti_data.tti_rx);
  //      }
  //    }
  //  }
}

void sched_tester::test_harqs()
{
  /* check consistency of DL harq procedures and allocations */
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_data_elems; ++i) {
    const auto&                 data = tti_data.sched_result_dl.data[i];
    uint32_t                    h_id = data.dci.pid;
    uint16_t                    rnti = data.dci.rnti;
    const srsenb::dl_harq_proc* h    = ue_db[rnti].get_dl_harq(h_id);
    CondError(h == nullptr, "[TESTER] scheduled DL harq pid=%d does not exist\n", h_id);
    CondError(h->is_empty(), "[TESTER] Cannot schedule an empty harq proc\n");
    CondError(h->get_tti() != tti_data.tti_tx_dl,
              "[TESTER] The scheduled DL harq pid=%d does not a valid tti=%u\n",
              h_id,
              tti_data.tti_tx_dl);
    CondError(h->get_n_cce() != data.dci.location.ncce, "[TESTER] Harq DCI location does not match with result\n");
    if (tti_data.ue_data[rnti].dl_harqs[h_id].has_pending_retx(0, tti_data.tti_tx_dl)) { // retx
      CondError(tti_data.ue_data[rnti].dl_harqs[h_id].nof_retx(0) + 1 != h->nof_retx(0),
                "[TESTER] A dl harq of user rnti=0x%x was likely overwritten.\n",
                rnti);
      CondError(h->nof_retx(0) >= sim_args.ue_cfg.maxharq_tx,
                "[TESTER] The number of retx=%d exceeded its max=%d\n",
                h->nof_retx(0),
                sim_args.ue_cfg.maxharq_tx);
    } else { // newtx
      CondError(h->nof_retx(0) != 0, "[TESTER] A new harq was scheduled but with invalid number of retxs\n");
    }
  }

  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    const auto&                 pusch = tti_data.sched_result_ul.pusch[i];
    uint16_t                    rnti  = pusch.dci.rnti;
    const auto&                 ue_data = tti_data.ue_data[rnti];
    const srsenb::ul_harq_proc* h       = ue_db[rnti].get_ul_harq(tti_data.tti_tx_ul);
    CondError(h == nullptr or h->is_empty(), "[TESTER] scheduled UL harq does not exist or is empty\n");
    CondError(h->get_tti() != tti_data.tti_tx_ul,
              "[TESTER] The scheduled UL harq does not a valid tti=%u\n",
              tti_data.tti_tx_ul);
    CondError(h->has_pending_ack(), "[TESTER] At the end of the TTI, there shouldnt be any pending ACKs\n");

    if (h->has_pending_retx()) {
      // retx
      CondError(ue_data.ul_harq.is_empty(0), "[TESTER] reTx in an UL harq that was empty\n");
      CondError(h->nof_retx(0) != ue_data.ul_harq.nof_retx(0) + 1,
                "[TESTER] A retx UL harq was scheduled but with invalid number of retxs\n");
      CondError(h->is_adaptive_retx() and not pusch.needs_pdcch, "[TESTER] Adaptive retxs need PDCCH alloc\n");
    } else {
      CondError(h->nof_retx(0) != 0, "[TESTER] A new harq was scheduled but with invalid number of retxs\n");
      CondError(not ue_data.ul_harq.is_empty(0), "[TESTER] UL new tx in a UL harq that was not empty\n");
    }
  }

  /* Check PHICH allocations */
  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_phich_elems; ++i) {
    const auto& phich = tti_data.sched_result_ul.phich[i];
    CondError(tti_data.ue_data.count(phich.rnti) == 0, "[TESTER] Allocated PHICH rnti no longer exists\n");
    const auto& hprev = tti_data.ue_data[phich.rnti].ul_harq;
    const auto* h     = ue_db[phich.rnti].get_ul_harq(tti_data.tti_tx_ul);
    CondError(not hprev.has_pending_ack(), "[TESTER] Alloc PHICH did not have any pending ack\n");
    bool maxretx_flag = hprev.nof_retx(0) + 1 >= hprev.max_nof_retx();
    if (phich.phich == sched_interface::ul_sched_phich_t::ACK) {
      CondError(!hprev.is_empty(), "[TESTER] ack phich for UL harq that is not empty\n");
    } else {
      CondError(h->get_pending_data() == 0 and !maxretx_flag, "[TESTER] NACKed harq has no pending data\n");
    }
  }
  for (const auto& ue : ue_db) {
    const auto& hprev = tti_data.ue_data[ue.first].ul_harq;
    if (not hprev.has_pending_ack())
      continue;
    uint32_t i = 0;
    for (; i < tti_data.sched_result_ul.nof_phich_elems; ++i) {
      const auto& phich = tti_data.sched_result_ul.phich[i];
      if (phich.rnti == ue.first)
        break;
    }
    CondError(i == tti_data.sched_result_ul.nof_phich_elems,
              "[TESTER] harq had pending ack but no phich was allocked\n");
  }

  // schedule future acks
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_data_elems; ++i) {
    ack_info_t ack_data;
    ack_data.rnti                    = tti_data.sched_result_dl.data[i].dci.rnti;
    ack_data.tti                     = FDD_HARQ_DELAY_MS + tti_data.tti_tx_dl;
    const srsenb::dl_harq_proc* dl_h = ue_db[ack_data.rnti].get_dl_harq(tti_data.sched_result_dl.data[i].dci.pid);
    ack_data.dl_harq                 = *dl_h;
    if (ack_data.dl_harq.nof_retx(0) == 0) {
      ack_data.dl_ack = randf() > sim_args.P_retx;
    } else { // always ack after three retxs
      ack_data.dl_ack = ack_data.dl_harq.nof_retx(0) == 3;
    }

    // Remove harq from the ack list if there was a harq rewrite
    ack_it_t it = to_ack.begin();
    while (it != to_ack.end() and it->first < ack_data.tti) {
      if (it->second.rnti == ack_data.rnti and it->second.dl_harq.get_id() == ack_data.dl_harq.get_id()) {
        CondError(it->second.tti + 2 * FDD_HARQ_DELAY_MS > ack_data.tti,
                  "[TESTER] The retx dl harq id=%d was transmitted too soon\n",
                  ack_data.dl_harq.get_id());
        ack_it_t toerase_it = it++;
        to_ack.erase(toerase_it);
        continue;
      }
      ++it;
    }

    // add new ack to the list
    to_ack.insert(std::make_pair(ack_data.tti, ack_data));
  }

  /* Schedule UL ACKs */
  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    const auto&   pusch = tti_data.sched_result_ul.pusch[i];
    ul_ack_info_t ack_data;
    ack_data.rnti      = pusch.dci.rnti;
    ack_data.ul_harq   = *ue_db[ack_data.rnti].get_ul_harq(tti_data.tti_tx_ul);
    ack_data.tti_tx_ul = tti_data.tti_tx_ul;
    ack_data.tti_ack   = tti_data.tti_tx_ul + FDD_HARQ_DELAY_MS;
    if (ack_data.ul_harq.nof_retx(0) == 0) {
      ack_data.ack = randf() > sim_args.P_retx;
    } else {
      ack_data.ack = ack_data.ul_harq.nof_retx(0) == 3;
    }
    to_ul_ack.insert(std::make_pair(ack_data.tti_tx_ul, ack_data));
  }

  // Check whether some pids got old
  for (auto& user : ue_db) {
    for (int i = 0; i < 2 * FDD_HARQ_DELAY_MS; i++) {
      if (not(user.second.get_dl_harq(i)->is_empty(0) and user.second.get_dl_harq(1))) {
        if (srslte_tti_interval(tti_data.tti_tx_dl, user.second.get_dl_harq(i)->get_tti()) > 49) {
          TestError("[TESTER] The pid=%d for rnti=0x%x got old.\n", user.second.get_dl_harq(i)->get_id(), user.first);
        }
      }
    }
  }
}

void sched_tester::test_collisions()
{
  tti_sched_t* tti_sched = get_tti_sched(tti_data.tti_rx);

  srsenb::prbmask_t ul_allocs(cfg.cell.nof_prb);

  // Helper function to fill RBG mask
  auto try_ul_fill = [&](srsenb::ul_harq_proc::ul_alloc_t alloc, const char* ch_str, bool strict = true) {
    CondError((alloc.RB_start + alloc.L) > cfg.cell.nof_prb,
              "[TESTER] Allocated RBs (%d,%d) out of bounds\n",
              alloc.RB_start,
              alloc.RB_start + alloc.L);
    CondError(alloc.L == 0, "[TESTER] Allocations must have at least one PRB\n");
    if (strict and ul_allocs.any(alloc.RB_start, alloc.RB_start + alloc.L)) {
      TestError("[TESTER] There is a collision of %s alloc=(%d,%d) and cumulative_mask=%s\n",
                ch_str,
                alloc.RB_start,
                alloc.RB_start + alloc.L,
                ul_allocs.to_hex().c_str());
    }
    ul_allocs.fill(alloc.RB_start, alloc.RB_start + alloc.L, true);
  };

  /* TEST: Check if there is space for PRACH */
  if (tti_data.is_prach_tti_tx_ul) {
    try_ul_fill({cfg.prach_freq_offset, 6}, "PRACH");
  }

  /* TEST: check collisions in the UL PUSCH and PUCCH */
  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    uint32_t L, RBstart;
    srslte_ra_type2_from_riv(
        tti_data.sched_result_ul.pusch[i].dci.type2_alloc.riv, &L, &RBstart, cfg.cell.nof_prb, cfg.cell.nof_prb);
    try_ul_fill({RBstart, L}, "PUSCH");
    ue_stats[tti_data.sched_result_ul.pusch[i].dci.rnti].nof_ul_rbs += L;
  }

  /* TEST: check collisions with PUCCH */
  bool strict = cfg.cell.nof_prb != 6 or (not tti_data.is_prach_tti_tx_ul and not tti_data.ul_pending_msg3.enabled);
  try_ul_fill({0, (uint32_t)cfg.nrb_pucch}, "PUCCH", strict);
  try_ul_fill({cfg.cell.nof_prb - cfg.nrb_pucch, (uint32_t)cfg.nrb_pucch}, "PUCCH", strict);

  /* TEST: Check if there is a collision with Msg3 or Msg3 alloc data is not consistent */
  if (tti_data.ul_pending_msg3.enabled) {
    bool passed = false;
    for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
      if (tti_data.ul_pending_msg3.rnti == tti_data.sched_result_ul.pusch[i].dci.rnti) {
        CondError(passed, "[TESTER] There can only be one msg3 allocation per UE\n");
        CondError(tti_data.sched_result_ul.pusch[i].needs_pdcch, "[TESTER] Msg3 allocations do not need PDCCH DCI\n");
        uint32_t L, RBstart;
        srslte_ra_type2_from_riv(
            tti_data.sched_result_ul.pusch[i].dci.type2_alloc.riv, &L, &RBstart, cfg.cell.nof_prb, cfg.cell.nof_prb);
        if (RBstart != tti_data.ul_pending_msg3.n_prb or L != tti_data.ul_pending_msg3.L) {
          TestError("[TESTER] The Msg3 allocation does not coincide with the expected.\n");
        }
        passed = true;
      }
    }
    CondError(not passed, "[TESTER] No Msg3 allocation was found in the sched_result\n");
  }

  /* TEST: check whether cumulative UL PRB masks coincide */
  if (ul_allocs != tti_sched->get_ul_mask()) {
    TestError("[TESTER] The UL PRB mask and the scheduler result UL mask are not consistent\n");
  }

  srslte::bounded_bitset<100, true> dl_allocs(cfg.cell.nof_prb), alloc_mask(cfg.cell.nof_prb);
  srslte_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);

  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_data_elems; ++i) {
    alloc_mask.reset();
    srslte_pdsch_grant_t grant;
    CondError(srslte_ra_dl_dci_to_grant(
                  &cfg.cell, &dl_sf, SRSLTE_TM1, false, &tti_data.sched_result_dl.data[i].dci, &grant) == SRSLTE_ERROR,
              "Failed to decode PDSCH grant\n");
    for (uint32_t i = 0; i < alloc_mask.size(); ++i) {
      if (grant.prb_idx[0][i]) {
        alloc_mask.set(i);
      } else {
        alloc_mask.reset(i);
      }
    }
    if ((dl_allocs & alloc_mask).any()) {
      TestError("[TESTER] Detected collision in the DL data allocation (%s intersects %s)\n",
                dl_allocs.to_string().c_str(),
                alloc_mask.to_string().c_str());
    }
    dl_allocs |= alloc_mask;
    ue_stats[tti_data.sched_result_dl.data[i].dci.rnti].nof_dl_rbs += alloc_mask.count();
  }
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_bc_elems; ++i) {
    srslte_pdsch_grant_t grant;
    CondError(srslte_ra_dl_dci_to_grant(
                  &cfg.cell, &dl_sf, SRSLTE_TM1, false, &tti_data.sched_result_dl.bc[i].dci, &grant) == SRSLTE_ERROR,
              "Failed to decode PDSCH grant\n");
    alloc_mask.reset();
    for (uint32_t i = 0; i < alloc_mask.size(); ++i) {
      if (grant.prb_idx[0][i]) {
        alloc_mask.set(i);
      }
    }
    if ((dl_allocs & alloc_mask).any()) {
      TestError("[TESTER] Detected collision in the DL bc allocation (%s intersects %s)\n",
                dl_allocs.to_string().c_str(),
                alloc_mask.to_string().c_str());
    }
    dl_allocs |= alloc_mask;
  }
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_rar_elems; ++i) {
    alloc_mask.reset();
    srslte_pdsch_grant_t grant;
    CondError(srslte_ra_dl_dci_to_grant(
                  &cfg.cell, &dl_sf, SRSLTE_TM1, false, &tti_data.sched_result_dl.rar[i].dci, &grant) == SRSLTE_ERROR,
              "Failed to decode PDSCH grant\n");
    for (uint32_t i = 0; i < alloc_mask.size(); ++i) {
      if (grant.prb_idx[0][i]) {
        alloc_mask.set(i);
      } else {
        alloc_mask.reset(i);
      }
    }
    if ((dl_allocs & alloc_mask).any()) {
      TestError("[TESTER] Detected collision in the DL RAR allocation (%s intersects %s)\n",
                dl_allocs.to_string().c_str(),
                alloc_mask.to_string().c_str());
    }
    dl_allocs |= alloc_mask;
  }

  // TEST: check if resulting DL mask is equal to scheduler internal DL mask
  srsenb::rbgmask_t                 rbgmask(nof_rbg);
  srslte::bounded_bitset<100, true> rev_alloc = ~dl_allocs;
  for (uint32_t i = 0; i < nof_rbg; ++i) {
    uint32_t lim = SRSLTE_MIN((i + 1) * P, dl_allocs.size());
    bool     val = dl_allocs.any(i * P, lim);
    CondError(rev_alloc.any(i * P, lim) and val, "[TESTER] No holes can be left in an RBG\n");
    if (val) {
      rbgmask.set(i);
    } else {
      rbgmask.reset(i);
    }
  }
  if (rbgmask != get_tti_sched(tti_data.tti_rx)->get_dl_mask()) {
    TestError("[TESTER] The UL PRB mask and the scheduler result UL mask are not consistent\n");
  }
}

void sched_tester::ack_txs()
{
  /* check if user was removed. If so, clean respective acks */
  erase_if(to_ack,
           [this](std::pair<const uint32_t, ack_info_t>& elem) { return this->ue_db.count(elem.second.rnti) == 0; });
  erase_if(to_ul_ack,
           [this](std::pair<const uint32_t, ul_ack_info_t>& elem) { return this->ue_db.count(elem.second.rnti) == 0; });

  /* Ack DL HARQs */
  for (const auto& ack_it : to_ack) {
    if (ack_it.second.tti != tti_data.tti_rx) {
      continue;
    }
    srsenb::dl_harq_proc*       h    = ue_db[ack_it.second.rnti].get_dl_harq(ack_it.second.dl_harq.get_id());
    const srsenb::dl_harq_proc& hack = ack_it.second.dl_harq;
    CondError(hack.is_empty(), "[TESTER] The acked DL harq was not active\n");

    bool ret = false;
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; ++tb) {
      if (ack_it.second.dl_harq.is_empty(tb)) {
        continue;
      }
      ret |= dl_ack_info(tti_data.tti_rx, ack_it.second.rnti, tb, ack_it.second.dl_ack) > 0;
    }
    CondError(not ret, "[TESTER] The dl harq proc that was acked does not exist\n");

    if (ack_it.second.dl_ack) {
      CondError(!h->is_empty(), "[TESTER] ACKed dl harq was not emptied\n");
      CondError(h->has_pending_retx(0, tti_data.tti_tx_dl), "[TESTER] ACKed dl harq still has pending retx\n");
      log_out.info("[TESTER] DL ACK tti=%u rnti=0x%x pid=%d\n",
                   tti_data.tti_rx,
                   ack_it.second.rnti,
                   ack_it.second.dl_harq.get_id());
    } else {
      CondError(h->is_empty() and hack.nof_retx(0) + 1 < hack.max_nof_retx(), "[TESTER] NACKed DL harq got emptied\n");
    }
  }

  /* Ack UL HARQs */
  for (const auto& ack_it : to_ul_ack) {
    if (ack_it.first != tti_data.tti_rx) {
      continue;
    }
    srsenb::ul_harq_proc*       h    = ue_db[ack_it.second.rnti].get_ul_harq(tti_data.tti_rx);
    const srsenb::ul_harq_proc& hack = ack_it.second.ul_harq;
    CondError(h == nullptr or h->get_tti() != hack.get_tti(), "[TESTER] UL Harq TTI does not match the ACK TTI\n");
    CondError(h->is_empty(0), "[TESTER] The acked UL harq is not active\n");
    CondError(hack.is_empty(0), "[TESTER] The acked UL harq was not active\n");

    ul_crc_info(tti_data.tti_rx, ack_it.second.rnti, ack_it.second.ack);

    CondError(!h->get_pending_data(), "[TESTER] UL harq lost its pending data\n");
    CondError(!h->has_pending_ack(), "[TESTER] ACK/NACKed UL harq should have a pending ACK\n");

    if (ack_it.second.ack) {
      CondError(!h->is_empty(), "[TESTER] ACKed UL harq did not get emptied\n");
      CondError(h->has_pending_retx(), "[TESTER] ACKed UL harq still has pending retx\n");
      log_out.info("[TESTER] UL ACK tti=%u rnti=0x%x pid=%d\n", tti_data.tti_rx, ack_it.second.rnti, hack.get_id());
    } else {
      // NACK
      CondError(!h->is_empty() and !h->has_pending_retx(), "[TESTER] If NACKed, UL harq has to have pending retx\n");
      CondError(h->is_empty() and hack.nof_retx(0) + 1 < hack.max_nof_retx(),
                "[TESTER] Nacked UL harq did get emptied\n");
    }
  }

  // erase processed acks
  to_ack.erase(tti_data.tti_rx);
  to_ul_ack.erase(tti_data.tti_rx);

  //  bool ack = true; //(tti_data.tti_rx % 3) == 0;
  //  if (tti_data.tti_rx >= FDD_HARQ_DELAY_MS) {
  //    for (auto it = ue_db.begin(); it != ue_db.end(); ++it) {
  //      uint16_t              rnti = it->first;
  //      srsenb::ul_harq_proc* h    = ue_db[rnti].get_ul_harq(tti_data.tti_rx);
  //      if (h != NULL and not h->is_empty()) {
  //        ul_crc_info(tti_data.tti_rx, rnti, ack);
  //      }
  //    }
  //  }
}

srsenb::sched_interface::cell_cfg_t generate_cell_cfg()
{
  srsenb::sched_interface::cell_cfg_t cell_cfg;
  srslte_cell_t&                      cell_cfg_phy = cell_cfg.cell;

  bzero(&cell_cfg, sizeof(srsenb::sched_interface::cell_cfg_t));

  /* Set PHY cell configuration */
  cell_cfg_phy.id              = 1;
  cell_cfg_phy.cp              = SRSLTE_CP_NORM;
  cell_cfg_phy.nof_ports       = 1;
  cell_cfg_phy.nof_prb         = 100;
  cell_cfg_phy.phich_length    = SRSLTE_PHICH_NORM;
  cell_cfg_phy.phich_resources = SRSLTE_PHICH_R_1;

  cell_cfg.sibs[0].len       = 18;
  cell_cfg.sibs[0].period_rf = 8;
  cell_cfg.sibs[1].len       = 41;
  cell_cfg.sibs[1].period_rf = 16;
  cell_cfg.si_window_ms      = 40;
  cell_cfg.nrb_pucch         = 2;
  cell_cfg.prach_freq_offset = (cell_cfg_phy.nof_prb == 6) ? 0 : 2;
  cell_cfg.prach_rar_window  = 3;
  cell_cfg.maxharq_msg3tx    = 3;

  return cell_cfg;
}

void test_scheduler_rand(srsenb::sched_interface::cell_cfg_t cell_cfg, const sched_sim_args& args)
{
  // Create classes
  sched_tester         tester;
  srsenb::sched        my_sched;
  srsenb::dl_metric_rr dl_metric;
  srsenb::ul_metric_rr ul_metric;

  log_out.set_level(srslte::LOG_LEVEL_INFO);

  tester.sim_args                                          = args;
  srslte_cell_t&                           cell_cfg_phy    = cell_cfg.cell;
  srsenb::sched_interface::dl_sched_res_t& sched_result_dl = tester.tti_data.sched_result_dl;
  srsenb::sched_interface::ul_sched_res_t& sched_result_ul = tester.tti_data.sched_result_ul;

  tester.init(NULL, &log_out);
  tester.set_metric(&dl_metric, &ul_metric);
  tester.cell_cfg(&cell_cfg);

  bool     running  = true;
  uint32_t tti      = 0;
  uint32_t nof_ttis = 0;
  while (running) {
    if (nof_ttis > args.nof_ttis) {
      running = false;
    }
    log_out.step(tti);

    tester.run_tti(tti);

    nof_ttis++;
    tti = (tti + 1) % 10240;
  }
}

sched_sim_args rand_sim_params(const srsenb::sched_interface::cell_cfg_t& cell_cfg, uint32_t nof_ttis)
{
  sched_sim_args                      sim_args;
  std::vector<std::vector<uint32_t> > current_rntis;
  uint16_t                            rnti_start   = 70;
  uint32_t                            max_conn_dur = 10000, min_conn_dur = 5000;
  float                               P_ul_sr = randf() * 0.5, P_dl = randf() * 0.5;
  float                               P_prach        = 0.99f;  // 0.1f + randf()*0.3f;
  float                               ul_sr_exps[]   = {1, 4}; // log rand
  float                               dl_data_exps[] = {1, 4}; // log rand
  uint32_t                            max_nof_users  = 500;

  bzero(&sim_args.ue_cfg, sizeof(srsenb::sched_interface::ue_cfg_t));
  sim_args.ue_cfg.aperiodic_cqi_period = 40;
  sim_args.ue_cfg.maxharq_tx           = 5;

  bzero(&sim_args.bearer_cfg, sizeof(srsenb::sched_interface::ue_bearer_cfg_t));
  sim_args.bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;

  sim_args.nof_ttis = nof_ttis;
  sim_args.P_retx   = 0.1;
  sim_args.tti_events.resize(sim_args.nof_ttis);

  for (uint32_t tti = 0; tti < sim_args.tti_events.size(); ++tti) {
    if (not current_rntis.empty()) {
      // may rem user
      for (uint32_t i = 0; i < current_rntis.size(); ++i) {
        if (current_rntis[i][2] + current_rntis[i][1] <= tti) {
          std::vector<std::vector<uint32_t> >::iterator it_to_rem = current_rntis.begin() + i;
          sim_args.tti_events[tti].rem_user                       = true;
          sim_args.tti_events[tti].rem_rnti                       = (*it_to_rem)[0];
          current_rntis.erase(it_to_rem);
        }
      }

      for (uint32_t i = 0; i < current_rntis.size(); ++i) {
        uint32_t rnti = current_rntis[i][0];
        if (randf() < P_ul_sr) {
          float exp                                    = ul_sr_exps[0] + randf() * (ul_sr_exps[1] - ul_sr_exps[0]);
          sim_args.tti_events[tti].users[rnti].sr_data = (uint32_t)pow(10, exp);
        }
        if (randf() < P_dl) {
          float exp = dl_data_exps[0] + randf() * (dl_data_exps[1] - dl_data_exps[0]);
          sim_args.tti_events[tti].users[rnti].dl_data = (uint32_t)pow(10, exp);
        }
      }
    }

    // may add new user (For now, we only support one UE per PRACH)
    bool is_prach_tti = srslte_prach_tti_opportunity_config_fdd(cell_cfg.prach_config, tti, -1);
    if (is_prach_tti and current_rntis.size() < max_nof_users and randf() < P_prach) {
      std::vector<uint32_t> elem(3);
      elem[0] = rnti_start;
      elem[1] = tti;
      elem[2] = min_conn_dur + rand() % (max_conn_dur - min_conn_dur);
      current_rntis.push_back(elem);
      sim_args.tti_events[tti].new_user = true;
      sim_args.tti_events[tti].new_rnti = rnti_start++;
    }
  }

  return sim_args;
}

int main(int argc, char* argv[])
{
  printf("[TESTER] This is the chosen seed: %u\n", seed);
  /* initialize random seed: */
  srand(seed);
  uint32_t N_runs = 1, nof_ttis = 10240 + 10;

  for (uint32_t n = 0; n < N_runs; ++n) {
    printf("Sim run number: %u\n", n + 1);
    srsenb::sched_interface::cell_cfg_t cell_cfg = generate_cell_cfg();
    sched_sim_args                      sim_args = rand_sim_params(cell_cfg, nof_ttis);

    test_scheduler_rand(cell_cfg, sim_args);
  }

  //  // low UL-Txs
  //  printf("\n\n********* New Test ***********\n");
  //  sim_args.P_sr = 0.05;
  //  test_scheduler_rand(sim_args);

  printf("[TESTER] Number of assertion warnings: %u\n", warn_counter);
  printf("[TESTER] Number of assertion errors: %u\n", err_counter);
  printf("[TESTER] This was the chosen seed: %u\n", seed);
}
