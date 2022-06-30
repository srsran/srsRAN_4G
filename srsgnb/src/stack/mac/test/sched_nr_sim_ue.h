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

#ifndef SRSRAN_SCHED_NR_SIM_UE_H
#define SRSRAN_SCHED_NR_SIM_UE_H

#include "srsenb/test/mac/sched_sim_ue.h"
#include "srsgnb/hdr/stack/mac/sched_nr.h"
#include "srsran/adt/circular_array.h"
#include "srsran/common/test_common.h"
#include <condition_variable>
#include <semaphore.h>

namespace srsran {
class task_worker;
}

namespace srsenb {

const static uint32_t MAX_GRANTS = mac_interface_phy_nr::MAX_GRANTS;

struct ue_nr_harq_ctxt_t {
  bool                  active    = false;
  bool                  ndi       = false;
  bool                  is_msg3   = false;
  uint32_t              pid       = 0;
  uint32_t              nof_txs   = 0;
  uint32_t              nof_retxs = std::numeric_limits<uint32_t>::max();
  uint32_t              riv       = 0;
  srsran_dci_location_t dci_loc   = {};
  uint32_t              tbs       = 0;
  slot_point            last_slot_tx, first_slot_tx, last_slot_ack;
};
struct sched_nr_cc_result_view {
  slot_point                          slot;
  uint32_t                            cc = 0;
  const sched_nr_interface::dl_res_t* dl = nullptr;
  const sched_nr_interface::ul_res_t* ul = nullptr;
};

struct ue_nr_cc_ctxt_t {
  std::array<ue_nr_harq_ctxt_t, SCHED_NR_MAX_HARQ> dl_harqs;
  std::array<ue_nr_harq_ctxt_t, SCHED_NR_MAX_HARQ> ul_harqs;
  srsran::circular_array<uint32_t, TTIMOD_SZ>      pending_acks;
};

struct ue_nr_slot_events {
  struct ack_t {
    uint32_t pid;
    bool     ack;
  };
  struct cc_data {
    bool                                      configured = false;
    srsran::bounded_vector<ack_t, MAX_GRANTS> dl_acks;
    srsran::bounded_vector<ack_t, MAX_GRANTS> ul_acks;
    int                                       cqi = -1;
  };
  slot_point           slot_rx;
  std::vector<cc_data> cc_list;
};

struct sim_nr_ue_ctxt_t {
  uint16_t                      rnti;
  uint32_t                      preamble_idx;
  slot_point                    prach_slot_rx;
  sched_nr_impl::ue_cfg_manager ue_cfg;
  std::vector<ue_nr_cc_ctxt_t>  cc_list;

  bool is_last_dl_retx(uint32_t ue_cc_idx, uint32_t pid) const
  {
    auto& h = cc_list.at(ue_cc_idx).dl_harqs[pid];
    return h.nof_retxs + 1 >= ue_cfg.maxharq_tx;
  }
};
struct sim_nr_enb_ctxt_t {
  srsran::span<const sched_nr_impl::cell_config_manager> cell_params;
  std::map<uint16_t, const sim_nr_ue_ctxt_t*>            ue_db;
};

class sched_nr_ue_sim
{
public:
  sched_nr_ue_sim(uint16_t rnti_, const sched_nr_ue_cfg_t& ue_cfg_);
  sched_nr_ue_sim(uint16_t rnti_, const sched_nr_ue_cfg_t& ue_cfg_, slot_point prach_slot_rx, uint32_t preamble_idx);

  int update(const sched_nr_cc_result_view& cc_out);

  const sim_nr_ue_ctxt_t& get_ctxt() const { return ctxt; }
  sim_nr_ue_ctxt_t&       get_ctxt() { return ctxt; }

private:
  void update_dl_harqs(const sched_nr_cc_result_view& sf_out);

  srslog::basic_logger& logger;
  sim_nr_ue_ctxt_t      ctxt;
};

/// Implementation of features common to parallel and sequential sched nr testers
class sched_nr_base_test_bench
{
public:
  struct cc_result_t {
    sched_nr_cc_result_view  res;
    std::chrono::nanoseconds cc_latency_ns;
  };

  sched_nr_base_test_bench(const sched_nr_interface::sched_args_t& sched_args,
                           const std::vector<sched_nr_cell_cfg_t>& cell_params_,
                           std::string                             test_name,
                           uint32_t                                nof_workers = 1);
  virtual ~sched_nr_base_test_bench();

  void run_slot(slot_point slot_tx);
  void stop();

  slot_point get_slot_tx() const { return current_slot_tx; }

  /// may block waiting for scheduler to finish generating slot result
  std::vector<cc_result_t> get_slot_results() const;

  int rach_ind(uint16_t rnti, uint32_t cc, slot_point tti_rx, uint32_t preamble_idx);

  void user_cfg(uint16_t rnti, const sched_nr_interface::ue_cfg_t& ue_cfg_);

  void add_rlc_dl_bytes(uint16_t rnti, uint32_t lcid, uint32_t pdu_size_bytes);

  srsran::const_span<sched_nr_impl::cell_config_manager> get_cell_params() const { return cell_params; }

  /**
   * @brief Specify external events that will be forwarded to the scheduler (CQI, ACKs, etc.) in the given slot
   *        This method can be overridden by the derived class to simulate the environment of interest.
   * @param[in] ue_ctxt simulated UE context object
   * @param[in/out] pending_events events to be sent to the scheduler. The passed arg is initialized with the
   *                               "default events", sufficient to ensure a stable connection without retxs.
   *                               The derived class can decide to erase/modify/add new events
   */
  virtual void set_external_slot_events(const sim_nr_ue_ctxt_t& ue_ctxt, ue_nr_slot_events& pending_events) {}

  /**
   * @brief Called every slot to process the scheduler output for a given CC.
   * @param enb_ctxt simulated eNB context object
   * @param cc_out scheduler result for a given CC
   */
  virtual void process_slot_result(const sim_nr_enb_ctxt_t& enb_ctxt, srsran::const_span<cc_result_t> cc_out) {}

protected:
  void              generate_cc_result(uint32_t cc);
  sim_nr_enb_ctxt_t get_enb_ctxt() const;

  void dl_buffer_state_diff(uint16_t rnti, uint32_t lcid, int newtx);
  void dl_buffer_state_diff(uint16_t rnti, int newtx);
  void update_sched_buffer_state(uint16_t rnti);
  void update_sched_buffer_state(const sched_nr_cc_result_view& cc_out);

  int set_default_slot_events(const sim_nr_ue_ctxt_t& ue_ctxt, ue_nr_slot_events& pending_events);
  int apply_slot_events(sim_nr_ue_ctxt_t& ue_ctxt, const ue_nr_slot_events& events);

  /// Runs general tests to verify result consistency, and updates UE state
  void process_results();

  std::unique_ptr<srsran::test_delimit_logger>    test_delimiter;
  srslog::basic_logger&                           logger;
  srslog::basic_logger&                           mac_logger;
  std::unique_ptr<sched_nr>                       sched_ptr;
  std::vector<sched_nr_impl::cell_config_manager> cell_params;

  std::vector<std::unique_ptr<srsran::task_worker> > cc_workers;

  // UE context from the UE's point-of-view
  std::map<uint16_t, sched_nr_ue_sim> ue_db;

  // gNB point-of-view of UE state
  struct gnb_ue_ctxt {
    struct channel_ctxt {
      uint32_t rlc_unacked = 0;
      uint32_t rlc_newtx   = 0;
    };
    std::map<uint32_t, channel_ctxt> logical_channels;
  };
  std::map<uint16_t, gnb_ue_ctxt> gnb_ue_db;

  // slot-specific
  slot_point                            current_slot_tx;
  std::chrono::steady_clock::time_point slot_start_tp;
  sim_nr_enb_ctxt_t                     slot_ctxt;
  std::vector<cc_result_t>              cc_results;

  std::atomic<bool>     stopped{false};
  mutable sem_t         slot_sem;
  std::atomic<uint32_t> nof_cc_remaining{0};
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_SIM_UE_H
