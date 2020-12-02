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

#include "srsenb/hdr/stack/mac/sched.h"
#include "srsenb/hdr/stack/mac/sched_carrier.h"
#include "srsenb/hdr/stack/mac/sched_ue.h"
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

#include "sched_common_test_suite.h"
#include "sched_sim_ue.h"
#include "sched_test_common.h"
#include "sched_test_utils.h"
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

struct ue_stats_t {
  uint32_t nof_dl_rbs   = 0;
  uint32_t nof_ul_rbs   = 0;
  uint64_t nof_dl_bytes = 0;
  uint64_t nof_ul_bytes = 0;
  uint32_t nof_ttis     = 0;
};
std::map<uint16_t, ue_stats_t> ue_stats;
ue_stats_t                     ue_tot_stats;

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
    info("all: {DL/UL RBs: %" PRIu32 "/%" PRIu32 ", DL/UL bitrates: %0.2f/%0.2f Mbps}\n",
         ue_tot_stats.nof_dl_rbs,
         ue_tot_stats.nof_ul_rbs,
         ue_tot_stats.nof_dl_bytes * 8 * 0.001 / ue_tot_stats.nof_ttis,
         ue_tot_stats.nof_ul_bytes * 8 * 0.001 / ue_tot_stats.nof_ttis);
    for (auto& e : ue_stats) {
      info("0x%x: {DL/UL RBs: %" PRIu32 "/%" PRIu32 ", DL/UL bitrates: %0.2f/%0.2f Mbps}\n",
           e.first,
           e.second.nof_dl_rbs,
           e.second.nof_ul_rbs,
           e.second.nof_dl_bytes * 8 * 0.001 / e.second.nof_ttis,
           e.second.nof_ul_bytes * 8 * 0.001 / e.second.nof_ttis);
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
  int  assert_no_empty_allocs();
  int  test_harqs();

private:
  void new_test_tti() override;
  void before_sched() override;
  int  process_results() override;
  int  update_ue_stats();
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
    srsenb::ul_harq_proc* hul = user->get_ul_harq(srsenb::to_tx_ul(tti_rx), CARRIER_IDX);
    d.ul_pending_data         = get_ul_buffer(rnti);
    //        user->get_pending_ul_new_data(tti_info.tti_params.tti_tx_ul) or hul->has_pending_retx(); //
    //        get_ul_buffer(rnti);
    d.dl_pending_data         = get_dl_buffer(rnti);
    d.has_ul_retx             = hul->has_pending_retx();
    d.has_ul_tx               = d.has_ul_retx or d.ul_pending_data > 0;
    srsenb::dl_harq_proc* hdl = user->get_pending_dl_harq(srsenb::to_tx_dl(tti_rx), CARRIER_IDX);
    d.has_dl_tx = (hdl != nullptr) or (it.second.get_empty_dl_harq(srsenb::to_tx_dl(tti_rx), CARRIER_IDX) != nullptr and
                                       d.dl_pending_data > 0);
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
    tti_data.ue_data[rnti].ul_harq = *user->get_ul_harq(srsenb::to_tx_ul(tti_rx), CARRIER_IDX);
  }

  // TODO: Check whether pending pending_rar.rar_tti correspond to a prach_tti
}

int sched_tester::process_results()
{
  const srsenb::cc_sched_result* cc_result = sched_results.get_cc(tti_rx, CARRIER_IDX);
  srsenb::sf_output_res_t        sf_out{sched_cell_params, tti_rx, tti_info.ul_sched_result, tti_info.dl_sched_result};
  TESTASSERT(tti_rx == cc_result->tti_rx);

  // Common tests
  TESTASSERT(test_pdcch_collisions(sf_out, CARRIER_IDX, &cc_result->pdcch_mask) == SRSLTE_SUCCESS);
  TESTASSERT(test_dci_content_common(sf_out, CARRIER_IDX) == SRSLTE_SUCCESS);
  TESTASSERT(test_sib_scheduling(sf_out, CARRIER_IDX) == SRSLTE_SUCCESS);
  TESTASSERT(test_pusch_collisions(sf_out, CARRIER_IDX, &cc_result->ul_mask) == SRSLTE_SUCCESS);
  TESTASSERT(test_pdsch_collisions(sf_out, CARRIER_IDX, &cc_result->dl_mask) == SRSLTE_SUCCESS);

  // UE dedicated tests
  TESTASSERT(ue_tester->test_all(sf_out) == SRSLTE_SUCCESS);
  assert_no_empty_allocs();
  test_harqs();
  update_ue_stats();

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
    CONDERROR(h.get_tti() != srsenb::to_tx_dl(tti_rx),
              "The scheduled DL harq pid=%d does not a valid tti=%u\n",
              h_id,
              srsenb::to_tx_dl(tti_rx).to_uint());
    CONDERROR(h.get_n_cce() != data.dci.location.ncce, "Harq DCI location does not match with result\n");
  }

  /* Check PHICH allocations */
  for (uint32_t i = 0; i < tti_info.ul_sched_result[CARRIER_IDX].nof_phich_elems; ++i) {
    const auto& phich = tti_info.ul_sched_result[CARRIER_IDX].phich[i];
    const auto& hprev = tti_data.ue_data[phich.rnti].ul_harq;
    const auto* h     = ue_db[phich.rnti].get_ul_harq(srsenb::to_tx_ul(tti_rx), CARRIER_IDX);
    CONDERROR(not hprev.has_pending_phich(), "Alloc PHICH did not have any pending ack\n");
    bool maxretx_flag = hprev.nof_retx(0) + 1 >= hprev.max_nof_retx();
    if (phich.phich == sched_interface::ul_sched_phich_t::ACK) {
      // The harq can be either ACKed or Resumed
      if (not hprev.is_empty()) {
        // In case it was resumed
        CONDERROR(h == nullptr or h->is_empty(), "Cannot resume empty UL harq\n");
        for (uint32_t j = 0; j < tti_info.ul_sched_result[CARRIER_IDX].nof_dci_elems; ++j) {
          auto& pusch = tti_info.ul_sched_result[CARRIER_IDX].pusch[j];
          CONDERROR(pusch.dci.rnti == phich.rnti, "Cannot send PHICH::ACK for same harq that got UL grant.\n");
        }
      }
    } else {
      CONDERROR(h->get_pending_data() == 0 and !maxretx_flag, "NACKed harq has no pending data\n");
    }
  }

  return SRSLTE_SUCCESS;
}

int sched_tester::update_ue_stats()
{
  // update ue stats with number of allocated UL PRBs
  for (uint32_t i = 0; i < tti_info.ul_sched_result[CARRIER_IDX].nof_dci_elems; ++i) {
    const auto& pusch = tti_info.ul_sched_result[CARRIER_IDX].pusch[i];
    uint32_t    L, RBstart;
    srslte_ra_type2_from_riv(pusch.dci.type2_alloc.riv,
                             &L,
                             &RBstart,
                             sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb,
                             sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
    ue_stats[pusch.dci.rnti].nof_ul_rbs += L;
    ue_stats[pusch.dci.rnti].nof_ul_bytes += pusch.tbs;
    ue_tot_stats.nof_ul_rbs += L;
    ue_tot_stats.nof_ul_bytes += pusch.tbs;
  }

  // update ue stats with number of DL RB allocations
  srslte::bounded_bitset<100, true> alloc_mask(sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
  for (uint32_t i = 0; i < tti_info.dl_sched_result[CARRIER_IDX].nof_data_elems; ++i) {
    auto& data = tti_info.dl_sched_result[CARRIER_IDX].data[i];
    TESTASSERT(srsenb::extract_dl_prbmask(sched_cell_params[CARRIER_IDX].cfg.cell,
                                          tti_info.dl_sched_result[CARRIER_IDX].data[i].dci,
                                          alloc_mask) == SRSLTE_SUCCESS);
    ue_stats[data.dci.rnti].nof_dl_rbs += alloc_mask.count();
    ue_stats[data.dci.rnti].nof_dl_bytes += data.tbs[0] + data.tbs[1];
    ue_tot_stats.nof_dl_rbs += alloc_mask.count();
    ue_tot_stats.nof_dl_bytes += data.tbs[0] + data.tbs[1];
  }

  for (auto& u : ue_db) {
    ue_stats[u.first].nof_ttis++;
  }
  ue_tot_stats.nof_ttis++;

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

template <typename T>
T pick_random_uniform(std::initializer_list<T> v)
{
  return *(v.begin() + std::uniform_int_distribution<size_t>{0, v.size() - 1}(srsenb::get_rand_gen()));
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
  printf("Number of PRBs is %u\n", nof_prb);

  sched_sim_event_generator generator;

  sim_gen.sim_args.cell_cfg                             = {generate_default_cell_cfg(nof_prb)};
  sim_gen.sim_args.cell_cfg[0].target_ul_sinr           = pick_random_uniform({10, 15, 20, -1});
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg            = generate_default_ue_cfg();
  sim_gen.sim_args.default_ue_sim_cfg.periodic_cqi      = true;
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.maxharq_tx = std::uniform_int_distribution<>{1, 5}(srsenb::get_rand_gen());
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.measgap_period = pick_random_uniform({0, 40, 80});
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.measgap_offset = std::uniform_int_distribution<uint32_t>{
      0, sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.measgap_period}(srsenb::get_rand_gen());
  sim_gen.sim_args.start_tti = 0;
  sim_gen.sim_args.sim_log   = log_global.get();
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
