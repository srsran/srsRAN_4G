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
#include "srsenb/hdr/stack/mac/scheduler_carrier.h"
#include "srsenb/hdr/stack/mac/scheduler_ue.h"
#include <algorithm>
#include <chrono>
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

#include "scheduler_test_common.h"
#include "scheduler_test_utils.h"
#include "srslte/common/test_common.h"

/********************************************************
 * Random Tester for Scheduler.
 * Current Checks:
 * - Check if users are only added during a PRACH TTI
 * - Allocation (DCI+RBs) of users that no longer exist
 * - RAR is scheduled within the RAR window
 * - Msg3 checks:
 *   - scheduled/received at expected TTI
 *   - with the correct RNTI and without PDCCH alloc
 *   - unexpected msg3 arrival
 * - Users without data to Tx cannot be allocated in UL
 * - Retxs always take precedence
 * - DCI:
 *   - collisions detected
 *   - mismatch between the union of all dcis and
 *     scheduler class aggregate dci value
 * - Invalid BC SIB index or TBS
 * - Harqs:
 *   - invalid pids scheduled
 *   - empty harqs scheduled
 *   - invalid harq TTI
 *   - consistent NCCE loc
 *   - invalid retx number
 *   - DL adaptive retx/new tx <=> PDCCH alloc
 *   ...
 *******************************************************/

// uint32_t const seed = std::chrono::system_clock::now().time_since_epoch().count();
uint32_t const seed = 2452071795;
// uint32_t const seed = 1581009287; // prb==25
bool check_old_pids = false;

struct ue_stats_t {
  uint64_t nof_dl_rbs = 0;
  uint64_t nof_ul_rbs = 0;
};
std::map<uint16_t, ue_stats_t> ue_stats;

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
    info("[TESTER] UE stats:\n");
    for (auto& e : ue_stats) {
      info("0x%x: {DL RBs: %" PRIu64 ", UL RBs: %" PRIu64 "}\n", e.first, e.second.nof_dl_rbs, e.second.nof_ul_rbs);
    }
    info("[TESTER] Number of assertion warnings: %u\n", warn_counter);
    info("[TESTER] Number of assertion errors: %u\n", error_counter);
    info("[TESTER] This was the seed: %u\n", seed);
  }
};
srslte::scoped_log<sched_test_log> log_global{};

/*******************
 *     Dummies     *
 *******************/

constexpr uint32_t CARRIER_IDX = 0;

struct sched_sim_args {
  struct tti_event_t {
    struct user_event_t {
      uint32_t sr_data      = 0;
      uint32_t dl_data      = 0;
      uint32_t dl_nof_retxs = 0;
    };
    std::map<uint16_t, user_event_t> users;
    bool                             new_user = false;
    bool                             rem_user = false;
    uint32_t                         new_rnti;
    uint32_t                         rem_rnti;
  };

  std::vector<tti_event_t> tti_events2;
  sim_sched_args           sim_args;
  //  std::vector<tti_ev>      sim_events;
};

// Designed for testing purposes
struct sched_tester : public srsenb::common_sched_tester {
  struct tester_user_results {
    uint32_t                                  dl_pending_data     = 0;
    uint32_t                                  ul_pending_data     = 0; ///< data pending for UL
    bool                                      has_dl_retx         = false;
    bool                                      has_dl_tx           = false;
    bool                                      has_ul_tx           = false; ///< has either tx or retx
    bool                                      has_ul_retx         = false;
    bool                                      has_ul_newtx        = false; ///< *no* retx, but has tx
    bool                                      ul_retx_got_delayed = false;
    srsenb::sched_interface::ul_sched_data_t* ul_sched            = nullptr; // fast lookup
    srsenb::sched_interface::dl_sched_data_t* dl_sched            = nullptr; // fast lookup
    srsenb::dl_harq_proc                      dl_harqs[2 * FDD_HARQ_DELAY_MS];
    srsenb::ul_harq_proc                      ul_harq;
  };
  struct sched_tti_data {
    uint32_t                                current_cfi;
    uint32_t                                nof_prachs              = 0;
    bool                                    ul_pending_msg3_present = false;
    srsenb::sf_sched::pending_msg3_t        ul_pending_msg3;
    srslte::bounded_bitset<128, true>       used_cce;
    std::map<uint16_t, tester_user_results> ue_data;   ///< stores buffer state of each user
    tester_user_results                     total_ues; ///< stores combined UL/DL buffer state
    srsenb::sched_interface::ul_sched_res_t sched_result_ul;
    srsenb::sched_interface::dl_sched_res_t sched_result_dl;
  };
  struct ue_info {
    bool                                     drb_cfg_flag = false;
    srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg;
    uint32_t                                 preamble_idx = 0;
  };

  sched_sim_args sim_events;

  // tester control data
  std::map<uint16_t, ue_info> tester_ues;

  // sched results
  sched_tti_data tti_data;

  int  add_user(uint16_t                                 rnti,
                srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg,
                srsenb::sched_interface::ue_cfg_t        ue_cfg_);
  void rem_user(uint16_t rnti);
  int  test_tti_result();
  int  assert_no_empty_allocs();
  int  test_collisions();
  int  test_harqs();
  int  run_tti(const tti_ev& tti_events) final;

private:
  void new_test_tti();
  int  process_tti_args();
  void before_sched();
  int  process_results();
};

int sched_tester::add_user(uint16_t                                 rnti,
                           srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg,
                           srsenb::sched_interface::ue_cfg_t        ue_cfg_)
{
  TESTASSERT(common_sched_tester::add_user(rnti, ue_cfg_) == SRSLTE_SUCCESS);
  ue_info info;
  info.bearer_cfg   = bearer_cfg;
  info.preamble_idx = tti_data.nof_prachs++;
  tester_ues.insert(std::make_pair(rnti, info));

  return SRSLTE_SUCCESS;
}

void sched_tester::rem_user(uint16_t rnti)
{
  common_sched_tester::rem_user(rnti);
  tester_ues.erase(rnti);
  tti_data.ue_data.erase(rnti);
}

void sched_tester::new_test_tti()
{
  common_sched_tester::new_test_tti();
  // NOTE: make a local copy, since some of these variables may be cleared during scheduling
  auto& pending_msg3s = carrier_schedulers[0]->get_sf_sched_ptr(tti_info.tti_params.tti_rx)->get_pending_msg3();
  tti_data.ul_pending_msg3_present = false;
  if (not pending_msg3s.empty()) {
    tti_data.ul_pending_msg3_present = true;
    tti_data.ul_pending_msg3         = pending_msg3s.front();
  }
  tti_data.current_cfi = sched_cfg.nof_ctrl_symbols;
  tti_data.used_cce.resize(srslte_regs_pdcch_ncce(sched_cell_params[CARRIER_IDX].regs.get(), tti_data.current_cfi));
  tti_data.used_cce.reset();
  tti_data.ue_data.clear();
  tti_data.total_ues  = tester_user_results();
  tti_data.nof_prachs = 0;
}

int sched_tester::process_tti_args()
{
  // may add a new user
  if (sim_events.tti_events2[tti_info.tti_params.tti_rx].new_user) {
    CONDERROR(!srslte_prach_tti_opportunity_config_fdd(
                  sched_cell_params[CARRIER_IDX].cfg.prach_config, tti_info.tti_params.tti_rx, -1),
              "[TESTER] New user added in a non-PRACH TTI\n");
    uint16_t rnti = sim_events.tti_events2[tti_info.tti_params.tti_rx].new_rnti;
    add_user(rnti, sim_events.sim_args.bearer_cfg, sim_events.sim_args.ue_cfg);
  }

  // may remove an existing user
  if (sim_events.tti_events2[tti_info.tti_params.tti_rx].rem_user) {
    uint16_t rnti = sim_events.tti_events2[tti_info.tti_params.tti_rx].rem_rnti;
    bearer_ue_rem(rnti, 0);
    ue_rem(rnti);
    rem_user(rnti);
    log_global->info("[TESTER] Removing user rnti=0x%x\n", rnti);
  }

  // push UL SRs and DL packets
  for (auto& e : sim_events.tti_events2[tti_info.tti_params.tti_rx].users) {
    if (e.second.sr_data > 0 and tester_ues[e.first].drb_cfg_flag) {
      uint32_t tot_ul_data = ue_db[e.first].get_pending_ul_new_data(tti_info.tti_params.tti_tx_ul) + e.second.sr_data;
      uint32_t lcid        = 0;
      ul_bsr(e.first, lcid, tot_ul_data, true);
    }
    auto* user = ue_tester->get_user_state(e.first);
    if (e.second.dl_data > 0 and user->msg3_tic.is_valid() and user->msg3_tic.tti_rx() < tti_info.tti_params.tti_rx) {
      // If Msg4 not yet sent, allocate data in SRB0 buffer
      uint32_t lcid                = (user->msg4_tic.is_valid()) ? 2 : 0;
      uint32_t pending_dl_new_data = ue_db[e.first].get_pending_dl_new_data();
      if (lcid == 2 and not tester_ues[e.first].drb_cfg_flag) {
        // If RRCSetup finished
        if (pending_dl_new_data == 0) {
          // setup lcid==2 bearer
          tester_ues[e.first].drb_cfg_flag = true;
          bearer_ue_cfg(e.first, 2, &tester_ues[e.first].bearer_cfg);
        } else {
          // Let SRB0 get emptied
          continue;
        }
      }
      // TODO: Does it need TTI for checking pending data?
      uint32_t tot_dl_data = pending_dl_new_data + e.second.dl_data;
      dl_rlc_buffer_state(e.first, lcid, tot_dl_data, 0);
    }
  }

  return SRSLTE_SUCCESS;
}

void sched_tester::before_sched()
{
  // check pending data buffers
  for (auto& it : ue_db) {
    uint16_t              rnti = it.first;
    srsenb::sched_ue*     user = &it.second;
    tester_user_results   d;
    srsenb::ul_harq_proc* hul = user->get_ul_harq(tti_info.tti_params.tti_tx_ul, CARRIER_IDX);
    d.ul_pending_data         = get_ul_buffer(rnti);
    //        user->get_pending_ul_new_data(tti_info.tti_params.tti_tx_ul) or hul->has_pending_retx(); //
    //        get_ul_buffer(rnti);
    d.dl_pending_data         = get_dl_buffer(rnti);
    d.has_ul_retx             = hul->has_pending_retx();
    d.has_ul_tx               = d.has_ul_retx or d.ul_pending_data > 0;
    srsenb::dl_harq_proc* hdl = user->get_pending_dl_harq(tti_info.tti_params.tti_tx_dl, CARRIER_IDX);
    d.has_dl_retx             = (hdl != nullptr) and hdl->has_pending_retx(0, tti_info.tti_params.tti_tx_dl);
    d.has_dl_tx = (hdl != nullptr) or (it.second.get_empty_dl_harq(CARRIER_IDX) != nullptr and d.dl_pending_data > 0);
    d.has_ul_newtx = not d.has_ul_retx and d.ul_pending_data > 0;
    tti_data.ue_data.insert(std::make_pair(rnti, d));
    tti_data.total_ues.dl_pending_data += d.dl_pending_data;
    tti_data.total_ues.ul_pending_data += d.ul_pending_data;
    tti_data.total_ues.has_ul_tx |= d.has_ul_tx;
    tti_data.total_ues.has_dl_tx |= d.has_dl_tx;
    tti_data.total_ues.has_ul_newtx |= d.has_ul_newtx;

    for (uint32_t i = 0; i < 2 * FDD_HARQ_DELAY_MS; ++i) {
      const srsenb::dl_harq_proc* h      = user->get_dl_harq(i, CARRIER_IDX);
      tti_data.ue_data[rnti].dl_harqs[i] = *h;
    }
    // NOTE: ACK might have just cleared the harq for tti_info.tti_params.tti_tx_ul
    tti_data.ue_data[rnti].ul_harq = *user->get_ul_harq(tti_info.tti_params.tti_tx_ul, CARRIER_IDX);
  }

  // TODO: Check whether pending pending_rar.rar_tti correspond to a prach_tti
}

int sched_tester::process_results()
{
  tti_info.dl_sched_result.resize(1);
  tti_info.dl_sched_result[0] = tti_data.sched_result_dl;
  tti_info.ul_sched_result.resize(1);
  tti_info.ul_sched_result[0] = tti_data.sched_result_ul;
  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    uint16_t rnti                   = tti_data.sched_result_ul.pusch[i].dci.rnti;
    tti_data.ue_data[rnti].ul_sched = &tti_data.sched_result_ul.pusch[i];
    CONDERROR(tester_ues.count(rnti) == 0,
              "[TESTER] [%d] The user rnti=0x%x that no longer exists got allocated.\n",
              tti_info.tti_params.tti_rx,
              rnti);
  }
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_data_elems; ++i) {
    uint16_t rnti                   = tti_data.sched_result_dl.data[i].dci.rnti;
    tti_data.ue_data[rnti].dl_sched = &tti_data.sched_result_dl.data[i];
    CONDERROR(tester_ues.count(rnti) == 0,
              "[TESTER] [%d] The user rnti=0x%x that no longer exists got allocated.\n",
              tti_info.tti_params.tti_rx,
              rnti);
  }

  test_tti_result();
  ue_tester->test_ra(0, tti_info.dl_sched_result[CARRIER_IDX], tti_info.ul_sched_result[CARRIER_IDX]);
  test_collisions();
  assert_no_empty_allocs();
  test_harqs();
  output_tester[CARRIER_IDX].test_sib_scheduling(tti_info.tti_params, tti_data.sched_result_dl);

  return SRSLTE_SUCCESS;
}

int sched_tester::run_tti(const tti_ev& tti_events)
{
  new_test_tti();
  log_global->info("[TESTER] ---- tti=%u | nof_ues=%zd ----\n", tic.tti_rx(), ue_db.size());

  process_tti_args();

  process_ack_txs();
  before_sched();

  dl_sched(tti_info.tti_params.tti_tx_dl, CARRIER_IDX, tti_data.sched_result_dl);
  ul_sched(tti_info.tti_params.tti_tx_ul, CARRIER_IDX, tti_data.sched_result_ul);

  process_results();
  return SRSLTE_SUCCESS;
}

int sched_tester::assert_no_empty_allocs()
{
  // Test if allocations only take place for users with pending data or in RAR
  for (auto& iter : tti_data.ue_data) {
    uint16_t rnti = iter.first;
    //    srsenb::sched_ue* user = &ue_db[rnti];

    if (!iter.second.has_ul_tx and tti_data.ue_data[rnti].ul_sched != nullptr and
        tti_data.ue_data[rnti].ul_sched->needs_pdcch) {
      // TODO: This test does not work for adaptive re-tx
      TESTERROR("[TESTER] There was a user without data that got allocated in UL\n");
    }
    //    srsenb::ul_harq_proc* hul       = user->get_ul_harq(tti_info.tti_params.tti_tx_ul);
    iter.second.ul_retx_got_delayed = iter.second.has_ul_retx and iter.second.ul_harq.is_empty(0);
    tti_data.total_ues.ul_retx_got_delayed |= iter.second.ul_retx_got_delayed;
    // Retxs cannot give space to newtx allocations
    CONDERROR(
        tti_data.total_ues.ul_retx_got_delayed, "[TESTER] There was a retx that was erased for user rnti=0x%x\n", rnti);
  }

  // There must be allocations if there is pending data/retxs.
  //  bool no_dl_allocs = true;
  //  for (auto& it : tti_data.ue_data) {
  //    if (it.second.dl_sched != nullptr) {
  //      no_dl_allocs = false;
  //    }
  //  }
  //  CONDERROR(tti_data.total_ues.has_dl_tx and no_dl_allocs, "There was pending DL data but no user got allocated\n");
  // TODO: You have to verify if there is space for the retx since it is non-adaptive
  return SRSLTE_SUCCESS;
}

/**
 * Tests whether there were collisions in the DCI allocations
 */
int sched_tester::test_tti_result()
{
  /* TEST: Check if there are collisions in the PDCCH */
  TESTASSERT(output_tester[CARRIER_IDX].test_pdcch_collisions(
                 tti_data.sched_result_dl, tti_data.sched_result_ul, &tti_data.used_cce) == SRSLTE_SUCCESS);

  /* TEST: Check whether dci values are correct */
  TESTASSERT(output_tester[CARRIER_IDX].test_dci_values_consistency(tti_data.sched_result_dl,
                                                                    tti_data.sched_result_ul) == SRSLTE_SUCCESS);

  const srsenb::sf_sched* tti_sched = carrier_schedulers[0]->get_sf_sched_ptr(tti_info.tti_params.tti_rx);

  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    const auto& pusch = tti_data.sched_result_ul.pusch[i];
    CONDERROR(ue_db.count(pusch.dci.rnti) == 0, "The allocated rnti=0x%x does not exist\n", pusch.dci.rnti);
  }
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_data_elems; ++i) {
    auto& data = tti_data.sched_result_dl.data[i];
    CONDERROR(ue_db.count(data.dci.rnti) == 0, "Allocated rnti=0x%x that does not exist\n", data.dci.rnti);
  }
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_rar_elems; ++i) {
    const auto& rar = tti_data.sched_result_dl.rar[i];
    for (uint32_t j = 0; j < rar.nof_grants; ++j) {
      const auto& msg3_grant = rar.msg3_grant[j];
      const auto& msg3_list =
          carrier_schedulers[0]->get_sf_sched_ptr(tti_sched->get_tti_rx() + MSG3_DELAY_MS)->get_pending_msg3();
      const auto& p = msg3_list.front();
      CONDERROR(msg3_list.empty(), "Pending Msg3 should have been set\n");
      uint32_t rba = srslte_ra_type2_to_riv(p.L, p.n_prb, sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
      CONDERROR(msg3_grant.grant.rba != rba, "Pending Msg3 RBA is not valid\n");
    }
  }

  /* verify if sched_result "used_cce" coincide with sched "used_cce" */
  auto* tti_alloc = carrier_schedulers[0]->get_sf_sched_ptr(tti_info.tti_params.tti_rx);
  if (tti_data.used_cce != tti_alloc->get_pdcch_mask()) {
    std::string mask_str = tti_alloc->get_pdcch_mask().to_string();
    TESTERROR(
        "[TESTER] The used_cce do not match: (%s!=%s)\n", mask_str.c_str(), tti_data.used_cce.to_string().c_str());
  }

  // TODO: Check postponed retxs

  //  typedef std::map<uint16_t, srsenb::sched_ue>::iterator it_t;
  //  // There must be allocations if there is pending data/retxs.
  //  if(total_ues.has_ul_tx and ul_sched_result.empty()) {
  //    for (it_t it = ue_db.begin(); it != ue_db.end(); ++it) {
  //      uint32_t aggr_level = it->second.get_aggr_level(srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT0, cfg.cell.nof_prb,
  //      cfg.cell.nof_ports)); if (find_empty_dci(it->second.get_locations(current_cfi, sf_idx), aggr_level) > 0) {
  //        TESTERROR("[%d] There was pending UL data and free CCEs, but no user got allocated\n",
  //        tti_info.tti_params.tti_rx);
  //      }
  //    }
  //  }
  return SRSLTE_SUCCESS;
}

int sched_tester::test_harqs()
{
  /* check consistency of DL harq procedures and allocations */
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_data_elems; ++i) {
    const auto&                 data = tti_data.sched_result_dl.data[i];
    uint32_t                    h_id = data.dci.pid;
    uint16_t                    rnti = data.dci.rnti;
    const srsenb::dl_harq_proc* h    = ue_db[rnti].get_dl_harq(h_id, CARRIER_IDX);
    CONDERROR(h == nullptr, "[TESTER] scheduled DL harq pid=%d does not exist\n", h_id);
    CONDERROR(h->is_empty(), "[TESTER] Cannot schedule an empty harq proc\n");
    CONDERROR(h->get_tti() != tti_info.tti_params.tti_tx_dl,
              "[TESTER] The scheduled DL harq pid=%d does not a valid tti=%u\n",
              h_id,
              tti_info.tti_params.tti_tx_dl);
    CONDERROR(h->get_n_cce() != data.dci.location.ncce, "[TESTER] Harq DCI location does not match with result\n");
    if (tti_data.ue_data[rnti].dl_harqs[h_id].has_pending_retx(0, tti_info.tti_params.tti_tx_dl)) { // retx
      CONDERROR(tti_data.ue_data[rnti].dl_harqs[h_id].nof_retx(0) + 1 != h->nof_retx(0),
                "[TESTER] A dl harq of user rnti=0x%x was likely overwritten.\n",
                rnti);
      CONDERROR(h->nof_retx(0) >= sim_events.sim_args.ue_cfg.maxharq_tx,
                "[TESTER] The number of retx=%d exceeded its max=%d\n",
                h->nof_retx(0),
                sim_events.sim_args.ue_cfg.maxharq_tx);
    } else { // newtx
      CONDERROR(h->nof_retx(0) != 0, "[TESTER] A new harq was scheduled but with invalid number of retxs\n");
    }
  }

  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    const auto&                 pusch   = tti_data.sched_result_ul.pusch[i];
    uint16_t                    rnti    = pusch.dci.rnti;
    const auto&                 ue_data = tti_data.ue_data[rnti];
    const srsenb::ul_harq_proc* h       = ue_db[rnti].get_ul_harq(tti_info.tti_params.tti_tx_ul, CARRIER_IDX);
    CONDERROR(h == nullptr or h->is_empty(), "[TESTER] scheduled UL harq does not exist or is empty\n");
    CONDERROR(h->get_tti() != tti_info.tti_params.tti_tx_ul,
              "[TESTER] The scheduled UL harq does not a valid tti=%u\n",
              tti_info.tti_params.tti_tx_ul);
    CONDERROR(h->has_pending_ack(), "[TESTER] At the end of the TTI, there shouldnt be any pending ACKs\n");

    if (h->has_pending_retx()) {
      // retx
      CONDERROR(ue_data.ul_harq.is_empty(0), "[TESTER] reTx in an UL harq that was empty\n");
      CONDERROR(h->nof_retx(0) != ue_data.ul_harq.nof_retx(0) + 1,
                "[TESTER] A retx UL harq was scheduled but with invalid number of retxs\n");
      CONDERROR(h->is_adaptive_retx() and not pusch.needs_pdcch, "[TESTER] Adaptive retxs need PDCCH alloc\n");
    } else {
      CONDERROR(h->nof_retx(0) != 0, "[TESTER] A new harq was scheduled but with invalid number of retxs\n");
      CONDERROR(not ue_data.ul_harq.is_empty(0), "[TESTER] UL new tx in a UL harq that was not empty\n");
    }
  }

  /* Check PHICH allocations */
  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_phich_elems; ++i) {
    const auto& phich = tti_data.sched_result_ul.phich[i];
    CONDERROR(tti_data.ue_data.count(phich.rnti) == 0, "[TESTER] Allocated PHICH rnti no longer exists\n");
    const auto& hprev = tti_data.ue_data[phich.rnti].ul_harq;
    const auto* h     = ue_db[phich.rnti].get_ul_harq(tti_info.tti_params.tti_tx_ul, CARRIER_IDX);
    CONDERROR(not hprev.has_pending_ack(), "[TESTER] Alloc PHICH did not have any pending ack\n");
    bool maxretx_flag = hprev.nof_retx(0) + 1 >= hprev.max_nof_retx();
    if (phich.phich == sched_interface::ul_sched_phich_t::ACK) {
      CONDERROR(!hprev.is_empty(), "[TESTER] ack phich for UL harq that is not empty\n");
    } else {
      CONDERROR(h->get_pending_data() == 0 and !maxretx_flag, "[TESTER] NACKed harq has no pending data\n");
    }
  }
  for (const auto& ue : ue_db) {
    const auto& hprev = tti_data.ue_data[ue.first].ul_harq;
    if (not hprev.has_pending_ack()) {
      continue;
    }
    uint32_t i = 0;
    for (; i < tti_data.sched_result_ul.nof_phich_elems; ++i) {
      const auto& phich = tti_data.sched_result_ul.phich[i];
      if (phich.rnti == ue.first) {
        break;
      }
    }
    CONDERROR(i == tti_data.sched_result_ul.nof_phich_elems,
              "[TESTER] harq had pending ack but no phich was allocked\n");
  }

  // schedule future acks
  TESTASSERT(schedule_acks() == SRSLTE_SUCCESS);

  // Check whether some pids got old
  if (check_old_pids) {
    for (auto& user : ue_db) {
      for (int i = 0; i < 2 * FDD_HARQ_DELAY_MS; i++) {
        if (not(user.second.get_dl_harq(i, CARRIER_IDX)->is_empty(0) and user.second.get_dl_harq(1, CARRIER_IDX))) {
          if (srslte_tti_interval(tti_info.tti_params.tti_tx_dl, user.second.get_dl_harq(i, CARRIER_IDX)->get_tti()) >
              49) {
            TESTERROR("[TESTER] The pid=%d for rnti=0x%x got old.\n",
                      user.second.get_dl_harq(i, CARRIER_IDX)->get_id(),
                      user.first);
          }
        }
      }
    }
  }

  return SRSLTE_SUCCESS;
}

int sched_tester::test_collisions()
{
  const srsenb::sf_sched* tti_sched = carrier_schedulers[0]->get_sf_sched_ptr(tti_info.tti_params.tti_rx);
  srsenb::prbmask_t       ul_allocs(sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);

  /* TEST: any collision in PUCCH and PUSCH */
  TESTASSERT(output_tester[CARRIER_IDX].test_pusch_collisions(
                 tti_info.tti_params, tti_data.sched_result_ul, ul_allocs) == SRSLTE_SUCCESS);

  /* TEST: check whether cumulative UL PRB masks coincide */
  if (ul_allocs != tti_sched->get_ul_mask()) {
    TESTERROR("[TESTER] The UL PRB mask and the scheduler result UL mask are not consistent\n");
  }

  /* TEST: Check if there is a collision with Msg3 or Msg3 alloc data is not consistent */
  if (tti_data.ul_pending_msg3_present) {
    bool passed = false;
    for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
      if (tti_data.ul_pending_msg3.rnti == tti_data.sched_result_ul.pusch[i].dci.rnti) {
        CONDERROR(passed, "[TESTER] There can only be one msg3 allocation per UE\n");
        CONDERROR(tti_data.sched_result_ul.pusch[i].needs_pdcch, "[TESTER] Msg3 allocations do not need PDCCH DCI\n");
        uint32_t L, RBstart;
        srslte_ra_type2_from_riv(tti_data.sched_result_ul.pusch[i].dci.type2_alloc.riv,
                                 &L,
                                 &RBstart,
                                 sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb,
                                 sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
        if (RBstart != tti_data.ul_pending_msg3.n_prb or L != tti_data.ul_pending_msg3.L) {
          TESTERROR("[TESTER] The Msg3 allocation does not coincide with the expected.\n");
        }
        passed = true;
      }
    }
    CONDERROR(not passed, "[TESTER] No Msg3 allocation was found in the sched_result\n");
  }

  // update ue stats with number of allocated UL PRBs
  for (uint32_t i = 0; i < tti_data.sched_result_ul.nof_dci_elems; ++i) {
    uint32_t L, RBstart;
    srslte_ra_type2_from_riv(tti_data.sched_result_ul.pusch[i].dci.type2_alloc.riv,
                             &L,
                             &RBstart,
                             sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb,
                             sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
    ue_stats[tti_data.sched_result_ul.pusch[i].dci.rnti].nof_ul_rbs += L;
  }

  /* TEST: check any collision in PDSCH */
  srsenb::rbgmask_t rbgmask(sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
  TESTASSERT(output_tester[CARRIER_IDX].test_pdsch_collisions(tti_info.tti_params, tti_data.sched_result_dl, rbgmask) ==
             SRSLTE_SUCCESS);

  // update ue stats with number of DL RB allocations
  srslte::bounded_bitset<100, true> alloc_mask(sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
  for (uint32_t i = 0; i < tti_data.sched_result_dl.nof_data_elems; ++i) {
    TESTASSERT(srsenb::extract_dl_prbmask(sched_cell_params[CARRIER_IDX].cfg.cell,
                                          tti_data.sched_result_dl.data[i].dci,
                                          &alloc_mask) == SRSLTE_SUCCESS);
    ue_stats[tti_data.sched_result_dl.data[i].dci.rnti].nof_dl_rbs += alloc_mask.count();
  }

  // TEST: check if resulting DL mask is equal to scheduler internal DL mask
  if (rbgmask != carrier_schedulers[0]->get_sf_sched_ptr(tti_info.tti_params.tti_rx)->get_dl_mask()) {
    TESTERROR("[TESTER] The UL PRB mask and the scheduler result UL mask are not consistent (%s!=%s)\n",
              rbgmask.to_string().c_str(),
              carrier_schedulers[CARRIER_IDX]
                  ->get_sf_sched_ptr(tti_info.tti_params.tti_rx)
                  ->get_dl_mask()
                  .to_string()
                  .c_str());
  }
  return SRSLTE_SUCCESS;
}

srsenb::sched_interface::cell_cfg_t generate_cell_cfg()
{
  srsenb::sched_interface::cell_cfg_t cell_cfg     = {};
  srslte_cell_t&                      cell_cfg_phy = cell_cfg.cell;

  std::uniform_int_distribution<uint32_t> dist_prb_idx(0, 5);
  uint32_t                                prb_idx = dist_prb_idx(srsenb::get_rand_gen());

  /* Set PHY cell configuration */
  cell_cfg_phy.id              = 1;
  cell_cfg_phy.cp              = SRSLTE_CP_NORM;
  cell_cfg_phy.nof_ports       = 1;
  cell_cfg_phy.nof_prb         = std::array<uint32_t, 6>({6, 15, 25, 50, 75, 100})[prb_idx];
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

void test_scheduler_rand(const sched_sim_args& args)
{
  // Create classes
  sched_tester  tester;
  srsenb::sched my_sched;

  log_global->set_level(srslte::LOG_LEVEL_INFO);

  tester.sim_events       = args;
  sim_sched_args sim_args = args.sim_args;
  sim_args.ue_cfg         = args.sim_args.ue_cfg;
  sim_args.bearer_cfg     = args.sim_args.bearer_cfg;
  sim_args.start_tti      = 0;
  sim_args.sim_log        = log_global.get();

  tester.init(nullptr);
  tester.sim_cfg(sim_args);

  uint32_t tti      = 0;
  uint32_t nof_ttis = 0;
  while (nof_ttis <= args.tti_events2.size()) {
    log_global->step(tti);

    tester.run_tti({});

    nof_ttis++;
    tti = (tti + 1) % 10240;
  }
}

sched_sim_args rand_sim_params(uint32_t nof_ttis)
{
  sched_sim_args                      sim_gen;
  std::vector<std::vector<uint32_t> > current_rntis;
  uint16_t                            rnti_start   = 70;
  uint32_t                            max_conn_dur = 10000, min_conn_dur = 5000;
  float                               P_ul_sr = srsenb::randf() * 0.5, P_dl = srsenb::randf() * 0.5;
  float                               P_prach        = 0.99f;  // 0.1f + randf()*0.3f;
  float                               ul_sr_exps[]   = {1, 4}; // log rand
  float                               dl_data_exps[] = {1, 4}; // log rand
  uint32_t                            max_nof_users  = 5;
  std::uniform_int_distribution<>     connection_dur_dist(min_conn_dur, max_conn_dur);

  sim_gen.sim_args.cell_cfg = {generate_cell_cfg()};

  sim_gen.sim_args.ue_cfg = generate_default_ue_cfg();

  bzero(&sim_gen.sim_args.bearer_cfg, sizeof(srsenb::sched_interface::ue_bearer_cfg_t));
  sim_gen.sim_args.bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;

  sim_gen.sim_args.P_retx = 0.1;
  sim_gen.tti_events2.resize(nof_ttis);
  //  sim_gen.sim_events.resize(nof_ttis);

  for (uint32_t tti = 0; tti < sim_gen.tti_events2.size(); ++tti) {
    if (not current_rntis.empty()) {
      // may rem user
      for (uint32_t i = 0; i < current_rntis.size(); ++i) {
        if (current_rntis[i][2] + current_rntis[i][1] <= tti) {
          auto it_to_rem                    = current_rntis.begin() + i;
          sim_gen.tti_events2[tti].rem_user = true;
          sim_gen.tti_events2[tti].rem_rnti = (*it_to_rem)[0];
          current_rntis.erase(it_to_rem);
        }
      }

      for (auto& current_rnti : current_rntis) {
        uint32_t rnti = current_rnti[0];
        if (srsenb::randf() < P_ul_sr) {
          float exp = ul_sr_exps[0] + srsenb::randf() * (ul_sr_exps[1] - ul_sr_exps[0]);
          sim_gen.tti_events2[tti].users[rnti].sr_data = (uint32_t)pow(10, exp);
        }
        if (srsenb::randf() < P_dl) {
          float exp = dl_data_exps[0] + srsenb::randf() * (dl_data_exps[1] - dl_data_exps[0]);
          sim_gen.tti_events2[tti].users[rnti].dl_data = (uint32_t)pow(10, exp);
        }
      }
    }

    // may add new user (For now, we only support one UE per PRACH)
    bool is_prach_tti =
        srslte_prach_tti_opportunity_config_fdd(sim_gen.sim_args.cell_cfg[CARRIER_IDX].prach_config, tti, -1);
    if (is_prach_tti and current_rntis.size() < max_nof_users and srsenb::randf() < P_prach) {
      std::vector<uint32_t> elem(3);
      elem[0] = rnti_start;
      elem[1] = tti;
      elem[2] = connection_dur_dist(srsenb::get_rand_gen());
      current_rntis.push_back(elem);
      sim_gen.tti_events2[tti].new_user = true;
      sim_gen.tti_events2[tti].new_rnti = rnti_start;
      rnti_start++;
    }
  }

  return sim_gen;
}

int main()
{
  // Setup seed
  srsenb::set_randseed(seed);

  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_INFO);
  printf("[TESTER] This is the chosen seed: %u\n", seed);
  /* initialize random seed: */
  uint32_t N_runs = 1, nof_ttis = 10240 + 10;

  for (uint32_t n = 0; n < N_runs; ++n) {
    printf("Sim run number: %u\n", n + 1);
    sched_sim_args sim_args = rand_sim_params(nof_ttis);
    test_scheduler_rand(sim_args);
  }

  return 0;
}
