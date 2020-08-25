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

#include "scheduler_test_common.h"
#include "scheduler_test_utils.h"
#include "srslte/common/test_common.h"

using srslte::tti_point;

/********************************************************
 * Random Tester for Scheduler.
 * Current Checks:
 * - Check correct timing of PRACH, RAR, and Msg3
 * - Check whether Msg4 contains ConRes
 * - Check allocs of users that no longer exist
 * - Check collisions in PDCCH, PUSCH, and PDSCH
 * - Unexpected Msg3, RAR allocs or with wrong values
 * - Users without data to Tx cannot be allocated in UL
 * - Retxs always take precedence
 * - Invalid BC SIB index or TBS
 * - Harqs:
 *   - invalid pids scheduled
 *   - empty harqs scheduled
 *   - invalid harq TTI
 *   - consistent NCCE loc
 *   - invalid retx number
 *   - DL adaptive retx/new tx <=> PDCCH alloc
 *******************************************************/

uint32_t const seed = std::chrono::system_clock::now().time_since_epoch().count();
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
    info("UE stats:\n");
    for (auto& e : ue_stats) {
      info("0x%x: {DL RBs: %" PRIu64 ", UL RBs: %" PRIu64 "}\n", e.first, e.second.nof_dl_rbs, e.second.nof_ul_rbs);
    }
    info("Number of assertion warnings: %u\n", warn_counter);
    info("Number of assertion errors: %u\n", error_counter);
    info("This was the seed: %u\n", seed);
  }
};
srslte::scoped_log<sched_test_log> log_global{};

/*******************
 *     Dummies     *
 *******************/

constexpr uint32_t CARRIER_IDX = 0;

// Designed for testing purposes
struct sched_tester : public srsenb::common_sched_tester {
  struct tester_user_results {
    uint32_t             dl_pending_data     = 0;
    uint32_t             ul_pending_data     = 0; ///< data pending for UL
    bool                 has_dl_tx           = false;
    bool                 has_ul_tx           = false; ///< has either tx or retx
    bool                 has_ul_retx         = false;
    bool                 has_ul_newtx        = false; ///< *no* retx, but has tx
    bool                 ul_retx_got_delayed = false;
    srsenb::dl_harq_proc dl_harqs[srsenb::cc_sched_ue::SCHED_MAX_HARQ_PROC];
    srsenb::ul_harq_proc ul_harq;
  };
  struct sched_tti_data {
    std::map<uint16_t, tester_user_results> ue_data;   ///< stores buffer state of each user
    tester_user_results                     total_ues; ///< stores combined UL/DL buffer state
  };

  // sched results
  sched_tti_data tti_data;

  void rem_user(uint16_t rnti) override;
  int  test_pdcch_collisions();
  int  assert_no_empty_allocs();
  int  test_sch_collisions();
  int  test_harqs();

private:
  void new_test_tti() override;
  void before_sched() override;
  int  process_results() override;
};

void sched_tester::rem_user(uint16_t rnti)
{
  common_sched_tester::rem_user(rnti);
  tti_data.ue_data.erase(rnti);
}

void sched_tester::new_test_tti()
{
  common_sched_tester::new_test_tti();
  // NOTE: make a local copy, since some of these variables may be cleared during scheduling
  tti_data.ue_data.clear();
  tti_data.total_ues = tester_user_results();
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
    d.has_dl_tx =
        (hdl != nullptr) or
        (it.second.get_empty_dl_harq(tti_info.tti_params.tti_tx_dl, CARRIER_IDX) != nullptr and d.dl_pending_data > 0);
    d.has_ul_newtx = not d.has_ul_retx and d.ul_pending_data > 0;
    tti_data.ue_data.insert(std::make_pair(rnti, d));
    tti_data.total_ues.dl_pending_data += d.dl_pending_data;
    tti_data.total_ues.ul_pending_data += d.ul_pending_data;
    tti_data.total_ues.has_ul_tx |= d.has_ul_tx;
    tti_data.total_ues.has_dl_tx |= d.has_dl_tx;
    tti_data.total_ues.has_ul_newtx |= d.has_ul_newtx;

    for (uint32_t i = 0; i < srsenb::cc_sched_ue::SCHED_MAX_HARQ_PROC; ++i) {
      const srsenb::dl_harq_proc& h      = user->get_dl_harq(i, CARRIER_IDX);
      tti_data.ue_data[rnti].dl_harqs[i] = h;
    }
    // NOTE: ACK might have just cleared the harq for tti_info.tti_params.tti_tx_ul
    tti_data.ue_data[rnti].ul_harq = *user->get_ul_harq(tti_info.tti_params.tti_tx_ul, CARRIER_IDX);
  }

  // TODO: Check whether pending pending_rar.rar_tti correspond to a prach_tti
}

int sched_tester::process_results()
{
  const srsenb::cc_sched_result* cc_result =
      sched_results.get_cc(srslte::tti_point{tti_info.tti_params.tti_rx}, CARRIER_IDX);
  TESTASSERT(tti_info.tti_params.tti_rx == cc_result->tti_params.tti_rx);

  test_pdcch_collisions();
  TESTASSERT(ue_tester->test_all(0, tti_info.dl_sched_result[CARRIER_IDX], tti_info.ul_sched_result[CARRIER_IDX]) ==
             SRSLTE_SUCCESS);
  test_sch_collisions();
  assert_no_empty_allocs();
  test_harqs();
  output_tester[CARRIER_IDX].test_sib_scheduling(tti_info.tti_params, tti_info.dl_sched_result[CARRIER_IDX]);

  return SRSLTE_SUCCESS;
}

int sched_tester::assert_no_empty_allocs()
{
  // Test if allocations only take place for users with pending data or in RAR
  for (auto& iter : tti_data.ue_data) {
    uint16_t rnti = iter.first;
    //    srsenb::sched_ue* user = &ue_db[rnti];

    if (not iter.second.has_ul_tx) {
      for (uint32_t i = 0; i < tti_info.ul_sched_result[CARRIER_IDX].nof_dci_elems; ++i) {
        auto& pusch = tti_info.ul_sched_result[CARRIER_IDX].pusch[i];
        if (pusch.dci.rnti == rnti and pusch.needs_pdcch) {
          // TODO: This test does not work for adaptive re-tx
          TESTERROR("There was a user without data that got allocated in UL\n");
        }
      }
    }
    //    srsenb::ul_harq_proc* hul       = user->get_ul_harq(tti_info.tti_params.tti_tx_ul);
    iter.second.ul_retx_got_delayed = iter.second.has_ul_retx and iter.second.ul_harq.is_empty(0);
    tti_data.total_ues.ul_retx_got_delayed |= iter.second.ul_retx_got_delayed;
    // Retxs cannot give space to newtx allocations
    CONDERROR(tti_data.total_ues.ul_retx_got_delayed, "There was a retx that was erased for user rnti=0x%x\n", rnti);
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
int sched_tester::test_pdcch_collisions()
{
  srslte::bounded_bitset<128, true> used_cce;
  used_cce.resize(srslte_regs_pdcch_ncce(sched_cell_params[CARRIER_IDX].regs.get(), sched_cfg.max_nof_ctrl_symbols));

  /* TEST: Check if there are collisions in the PDCCH */
  TESTASSERT(output_tester[CARRIER_IDX].test_pdcch_collisions(tti_info.dl_sched_result[CARRIER_IDX],
                                                              tti_info.ul_sched_result[CARRIER_IDX],
                                                              &used_cce) == SRSLTE_SUCCESS);

  /* TEST: Check whether dci values are correct */
  TESTASSERT(output_tester[CARRIER_IDX].test_dci_values_consistency(
                 tti_info.dl_sched_result[CARRIER_IDX], tti_info.ul_sched_result[CARRIER_IDX]) == SRSLTE_SUCCESS);

  /* verify if sched_result "used_cce" coincide with sched "used_cce" */
  const srsenb::cc_sched_result* cc_result = sched_results.get_cc(tti_point{tti_info.tti_params.tti_rx}, CARRIER_IDX);
  if (used_cce != cc_result->pdcch_mask) {
    std::string mask_str = cc_result->pdcch_mask.to_string();
    TESTERROR("The used_cce do not match: (%s!=%s)\n", mask_str.c_str(), used_cce.to_string().c_str());
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
  for (uint32_t i = 0; i < tti_info.dl_sched_result[CARRIER_IDX].nof_data_elems; ++i) {
    const auto&                 data = tti_info.dl_sched_result[CARRIER_IDX].data[i];
    uint32_t                    h_id = data.dci.pid;
    uint16_t                    rnti = data.dci.rnti;
    const srsenb::dl_harq_proc& h    = ue_db[rnti].get_dl_harq(h_id, CARRIER_IDX);
    CONDERROR(h.is_empty(), "Cannot schedule an empty harq proc\n");
    CONDERROR(h.get_tti() != tti_point{tti_info.tti_params.tti_tx_dl},
              "The scheduled DL harq pid=%d does not a valid tti=%u\n",
              h_id,
              tti_info.tti_params.tti_tx_dl);
    CONDERROR(h.get_n_cce() != data.dci.location.ncce, "Harq DCI location does not match with result\n");
  }

  for (uint32_t i = 0; i < tti_info.ul_sched_result[CARRIER_IDX].nof_dci_elems; ++i) {
    const auto&                 pusch   = tti_info.ul_sched_result[CARRIER_IDX].pusch[i];
    uint16_t                    rnti    = pusch.dci.rnti;
    const auto&                 ue_data = tti_data.ue_data[rnti];
    const srsenb::ul_harq_proc* h       = ue_db[rnti].get_ul_harq(tti_info.tti_params.tti_tx_ul, CARRIER_IDX);
    CONDERROR(h == nullptr or h->is_empty(), "scheduled UL harq does not exist or is empty\n");
    CONDERROR(h->get_tti() != tti_point{tti_info.tti_params.tti_tx_ul},
              "The scheduled UL harq does not a valid tti=%u\n",
              tti_info.tti_params.tti_tx_ul);
    CONDERROR(h->has_pending_ack(), "At the end of the TTI, there shouldnt be any pending ACKs\n");

    if (h->has_pending_retx()) {
      // retx
      CONDERROR(ue_data.ul_harq.is_empty(0), "reTx in an UL harq that was empty\n");
      CONDERROR(h->nof_retx(0) != ue_data.ul_harq.nof_retx(0) + 1,
                "A retx UL harq was scheduled but with invalid number of retxs\n");
      CONDERROR(h->is_adaptive_retx() and not pusch.needs_pdcch, "Adaptive retxs need PDCCH alloc\n");
    } else {
      CONDERROR(h->nof_retx(0) != 0, "A new harq was scheduled but with invalid number of retxs\n");
      CONDERROR(not ue_data.ul_harq.is_empty(0), "UL new tx in a UL harq that was not empty\n");
    }
  }

  /* Check PHICH allocations */
  for (uint32_t i = 0; i < tti_info.ul_sched_result[CARRIER_IDX].nof_phich_elems; ++i) {
    const auto& phich = tti_info.ul_sched_result[CARRIER_IDX].phich[i];
    CONDERROR(tti_data.ue_data.count(phich.rnti) == 0, "Allocated PHICH rnti no longer exists\n");
    const auto& hprev = tti_data.ue_data[phich.rnti].ul_harq;
    const auto* h     = ue_db[phich.rnti].get_ul_harq(tti_info.tti_params.tti_tx_ul, CARRIER_IDX);
    CONDERROR(not hprev.has_pending_ack(), "Alloc PHICH did not have any pending ack\n");
    bool maxretx_flag = hprev.nof_retx(0) + 1 >= hprev.max_nof_retx();
    if (phich.phich == sched_interface::ul_sched_phich_t::ACK) {
      CONDERROR(!hprev.is_empty(), "ack phich for UL harq that is not empty\n");
    } else {
      CONDERROR(h->get_pending_data() == 0 and !maxretx_flag, "NACKed harq has no pending data\n");
    }
  }
  for (const auto& ue : ue_db) {
    const auto& hprev = tti_data.ue_data[ue.first].ul_harq;
    if (not hprev.has_pending_ack()) {
      continue;
    }
    uint32_t i = 0;
    for (; i < tti_info.ul_sched_result[CARRIER_IDX].nof_phich_elems; ++i) {
      const auto& phich = tti_info.ul_sched_result[CARRIER_IDX].phich[i];
      if (phich.rnti == ue.first) {
        break;
      }
    }
    CONDERROR(i == tti_info.ul_sched_result[CARRIER_IDX].nof_phich_elems,
              "harq had pending ack but no phich was allocked\n");
  }

  // Check whether some pids got old
  if (check_old_pids) {
    for (auto& user : ue_db) {
      for (int i = 0; i < srsenb::cc_sched_ue::SCHED_MAX_HARQ_PROC; i++) {
        if (not user.second.get_dl_harq(i, CARRIER_IDX).is_empty(0)) {
          if (tti_point{tti_info.tti_params.tti_tx_dl} > user.second.get_dl_harq(i, CARRIER_IDX).get_tti() + 49) {
            TESTERROR(
                "The pid=%d for rnti=0x%x got old.\n", user.second.get_dl_harq(i, CARRIER_IDX).get_id(), user.first);
          }
        }
      }
    }
  }

  return SRSLTE_SUCCESS;
}

int sched_tester::test_sch_collisions()
{
  const srsenb::cc_sched_result* cc_result = sched_results.get_cc(tti_point{tti_info.tti_params.tti_rx}, CARRIER_IDX);

  srsenb::prbmask_t ul_allocs(sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);

  /* TEST: any collision in PUCCH and PUSCH */
  TESTASSERT(output_tester[CARRIER_IDX].test_pusch_collisions(
                 tti_info.tti_params, tti_info.ul_sched_result[CARRIER_IDX], ul_allocs) == SRSLTE_SUCCESS);

  /* TEST: check whether cumulative UL PRB masks coincide */
  if (ul_allocs != cc_result->ul_mask) {
    TESTERROR("The UL PRB mask and the scheduler result UL mask are not consistent\n");
  }

  // update ue stats with number of allocated UL PRBs
  for (uint32_t i = 0; i < tti_info.ul_sched_result[CARRIER_IDX].nof_dci_elems; ++i) {
    uint32_t L, RBstart;
    srslte_ra_type2_from_riv(tti_info.ul_sched_result[CARRIER_IDX].pusch[i].dci.type2_alloc.riv,
                             &L,
                             &RBstart,
                             sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb,
                             sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
    ue_stats[tti_info.ul_sched_result[CARRIER_IDX].pusch[i].dci.rnti].nof_ul_rbs += L;
  }

  /* TEST: check any collision in PDSCH */
  srsenb::rbgmask_t rbgmask(sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
  TESTASSERT(output_tester[CARRIER_IDX].test_pdsch_collisions(
                 tti_info.tti_params, tti_info.dl_sched_result[CARRIER_IDX], rbgmask) == SRSLTE_SUCCESS);

  // update ue stats with number of DL RB allocations
  srslte::bounded_bitset<100, true> alloc_mask(sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
  for (uint32_t i = 0; i < tti_info.dl_sched_result[CARRIER_IDX].nof_data_elems; ++i) {
    TESTASSERT(srsenb::extract_dl_prbmask(sched_cell_params[CARRIER_IDX].cfg.cell,
                                          tti_info.dl_sched_result[CARRIER_IDX].data[i].dci,
                                          &alloc_mask) == SRSLTE_SUCCESS);
    ue_stats[tti_info.dl_sched_result[CARRIER_IDX].data[i].dci.rnti].nof_dl_rbs += alloc_mask.count();
  }

  // TEST: check if resulting DL mask is equal to scheduler internal DL mask
  if (rbgmask != cc_result->dl_mask) {
    TESTERROR("The DL PRB mask and the scheduler result DL mask are not consistent (%s!=%s)\n",
              rbgmask.to_string().c_str(),
              cc_result->dl_mask.to_string().c_str());
  }
  return SRSLTE_SUCCESS;
}

void test_scheduler_rand(sched_sim_events sim)
{
  // Create classes
  sched_tester  tester;
  srsenb::sched my_sched;

  tester.init(nullptr);
  tester.sim_cfg(std::move(sim.sim_args));

  tester.test_next_ttis(sim.tti_events);
}

sched_sim_events rand_sim_params(uint32_t nof_ttis)
{
  auto             boolean_dist = []() { return std::uniform_int_distribution<>{0, 1}(srsenb::get_rand_gen()); };
  sched_sim_events sim_gen;
  uint32_t         max_conn_dur = 10000, min_conn_dur = 500;
  float            P_ul_sr = srsenb::randf() * 0.5, P_dl = srsenb::randf() * 0.5;
  float            P_prach        = 0.99f;  // 0.1f + randf()*0.3f;
  float            ul_sr_exps[]   = {1, 4}; // log rand
  float            dl_data_exps[] = {1, 4}; // log rand
  uint32_t         max_nof_users  = 5;
  std::uniform_int_distribution<>         connection_dur_dist(min_conn_dur, max_conn_dur);
  std::uniform_int_distribution<uint32_t> dist_prb_idx(0, 5);
  uint32_t                                prb_idx = dist_prb_idx(srsenb::get_rand_gen());
  uint32_t                                nof_prb = std::array<uint32_t, 6>({6, 15, 25, 50, 75, 100})[prb_idx];

  sched_sim_event_generator generator;

  sim_gen.sim_args.cell_cfg                             = {generate_default_cell_cfg(nof_prb)};
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg            = generate_default_ue_cfg();
  sim_gen.sim_args.default_ue_sim_cfg.periodic_cqi      = true;
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.maxharq_tx = std::uniform_int_distribution<>{1, 5}(srsenb::get_rand_gen());
  sim_gen.sim_args.start_tti                            = 0;
  sim_gen.sim_args.sim_log                              = log_global.get();
  sim_gen.sim_args.sched_args.pdsch_mcs =
      boolean_dist() ? -1 : std::uniform_int_distribution<>{0, 24}(srsenb::get_rand_gen());
  sim_gen.sim_args.sched_args.pusch_mcs =
      boolean_dist() ? -1 : std::uniform_int_distribution<>{0, 24}(srsenb::get_rand_gen());

  generator.tti_events.resize(nof_ttis);

  for (uint32_t tti = 0; tti < nof_ttis; ++tti) {
    for (auto& u : generator.current_users) {
      uint32_t rnti = u.first;
      if (srsenb::randf() < P_ul_sr) {
        float exp = ul_sr_exps[0] + srsenb::randf() * (ul_sr_exps[1] - ul_sr_exps[0]);
        generator.add_ul_data(rnti, (uint32_t)pow(10, exp));
      }
      if (srsenb::randf() < P_dl) {
        float exp = dl_data_exps[0] + srsenb::randf() * (dl_data_exps[1] - dl_data_exps[0]);
        generator.add_dl_data(rnti, (uint32_t)pow(10, exp));
      }
    }

    // may add new user (For now, we only support one UE per PRACH)
    bool is_prach_tti =
        srslte_prach_tti_opportunity_config_fdd(sim_gen.sim_args.cell_cfg[CARRIER_IDX].prach_config, tti, -1);
    if (is_prach_tti and generator.current_users.size() < max_nof_users and srsenb::randf() < P_prach) {
      generator.add_new_default_user(connection_dur_dist(srsenb::get_rand_gen()),
                                     sim_gen.sim_args.default_ue_sim_cfg.ue_cfg);
    }
    generator.step_tti();
  }

  sim_gen.tti_events = std::move(generator.tti_events);

  return sim_gen;
}

int main()
{
  // Setup seed
  srsenb::set_randseed(seed);
  printf("This is the chosen seed: %u\n", seed);

  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_INFO);
  uint32_t N_runs = 1, nof_ttis = 10240 + 10;

  for (uint32_t n = 0; n < N_runs; ++n) {
    printf("Sim run number: %u\n", n + 1);
    sched_sim_events sim = rand_sim_params(nof_ttis);
    test_scheduler_rand(std::move(sim));
  }

  return 0;
}
