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

#ifndef SRSRAN_SCHED_NR_SIM_UE_H
#define SRSRAN_SCHED_NR_SIM_UE_H

#include "../sched_sim_ue.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr.h"
#include "srsran/adt/circular_array.h"
#include <condition_variable>

namespace srsenb {

const static uint32_t MAX_GRANTS = mac_interface_phy_nr::MAX_GRANTS;

struct ue_nr_harq_ctxt_t {
  bool                  active    = false;
  bool                  ndi       = false;
  uint32_t              pid       = 0;
  uint32_t              nof_txs   = 0;
  uint32_t              nof_retxs = std::numeric_limits<uint32_t>::max();
  uint32_t              riv       = 0;
  srsran_dci_location_t dci_loc   = {};
  uint32_t              tbs       = 0;
  slot_point            last_slot_tx, first_slot_tx, last_slot_ack;
};
struct sched_nr_cc_output_res_t {
  slot_point                                slot;
  uint32_t                                  cc;
  const sched_nr_interface::dl_sched_res_t* dl_cc_result;
  const sched_nr_interface::ul_sched_t*     ul_cc_result;
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
  };
  slot_point           slot_rx;
  std::vector<cc_data> cc_list;
};

struct sim_nr_ue_ctxt_t {
  uint16_t                     rnti;
  uint32_t                     preamble_idx;
  slot_point                   prach_slot_rx;
  sched_nr_interface::ue_cfg_t ue_cfg;
  std::vector<ue_nr_cc_ctxt_t> cc_list;

  bool is_last_dl_retx(uint32_t ue_cc_idx, uint32_t pid) const
  {
    auto& h = cc_list.at(ue_cc_idx).dl_harqs[pid];
    return h.nof_retxs + 1 >= ue_cfg.maxharq_tx;
  }
};
struct sim_nr_enb_ctxt_t {
  srsran::span<const sched_nr_impl::sched_cell_params> cell_params;
  std::map<uint16_t, const sim_nr_ue_ctxt_t*>          ue_db;
};

class sched_nr_ue_sim
{
public:
  sched_nr_ue_sim(uint16_t                            rnti_,
                  const sched_nr_interface::ue_cfg_t& ue_cfg_,
                  slot_point                          prach_slot_rx,
                  uint32_t                            preamble_idx);

  int update(const sched_nr_cc_output_res_t& cc_out);

  const sim_nr_ue_ctxt_t& get_ctxt() const { return ctxt; }
  sim_nr_ue_ctxt_t&       get_ctxt() { return ctxt; }

private:
  void update_dl_harqs(const sched_nr_cc_output_res_t& sf_out);

  srslog::basic_logger& logger;
  sim_nr_ue_ctxt_t      ctxt;
};

class sched_nr_sim_base
{
public:
  sched_nr_sim_base(const sched_nr_interface::sched_cfg_t&             sched_args,
                    const std::vector<sched_nr_interface::cell_cfg_t>& cell_params_,
                    std::string                                        test_name);
  virtual ~sched_nr_sim_base();

  int add_user(uint16_t rnti, const sched_nr_interface::ue_cfg_t& ue_cfg_, slot_point tti_rx, uint32_t preamble_idx);

  void new_slot(slot_point slot_tx);
  void update(sched_nr_cc_output_res_t& cc_out);

  sched_nr_ue_sim&       at(uint16_t rnti) { return ue_db.at(rnti); }
  const sched_nr_ue_sim& at(uint16_t rnti) const { return ue_db.at(rnti); }
  sched_nr_ue_sim*       find_rnti(uint16_t rnti)
  {
    auto it = ue_db.find(rnti);
    return it != ue_db.end() ? &it->second : nullptr;
  }
  const sched_nr_ue_sim* find_rnti(uint16_t rnti) const
  {
    auto it = ue_db.find(rnti);
    return it != ue_db.end() ? &it->second : nullptr;
  }
  bool                                user_exists(uint16_t rnti) const { return ue_db.count(rnti) > 0; }
  const sched_nr_interface::ue_cfg_t* get_user_cfg(uint16_t rnti) const
  {
    const sched_nr_ue_sim* ret = find_rnti(rnti);
    return ret == nullptr ? nullptr : &ret->get_ctxt().ue_cfg;
  }
  sched_nr*                                            get_sched() { return sched_ptr.get(); }
  srsran::const_span<sched_nr_impl::sched_cell_params> get_cell_params() { return cell_params; }
  slot_point                                           get_slot_rx() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return current_slot_tx;
  }

  sim_nr_enb_ctxt_t get_enb_ctxt() const;

  std::map<uint16_t, sched_nr_ue_sim>::iterator begin() { return ue_db.begin(); }
  std::map<uint16_t, sched_nr_ue_sim>::iterator end() { return ue_db.end(); }

  // configurable by simulator concrete implementation
  virtual void set_external_slot_events(const sim_nr_ue_ctxt_t& ue_ctxt, ue_nr_slot_events& pending_events) {}

private:
  int set_default_slot_events(const sim_nr_ue_ctxt_t& ue_ctxt, ue_nr_slot_events& pending_events);
  int apply_slot_events(sim_nr_ue_ctxt_t& ue_ctxt, const ue_nr_slot_events& events);

  std::string                                   test_name;
  srslog::basic_logger&                         logger;
  srslog::basic_logger&                         mac_logger;
  std::unique_ptr<sched_nr>                     sched_ptr;
  std::vector<sched_nr_impl::sched_cell_params> cell_params;

  slot_point current_slot_tx;
  int        cc_finished = 0;

  std::map<uint16_t, sched_nr_ue_sim> ue_db;

  mutable std::mutex      mutex;
  std::condition_variable cvar;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_SIM_UE_H
