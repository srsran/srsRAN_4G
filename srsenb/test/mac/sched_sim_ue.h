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

#ifndef SRSRAN_SCHED_SIM_UE_H
#define SRSRAN_SCHED_SIM_UE_H

#include "sched_common_test_suite.h"
#include "srsenb/hdr/stack/mac/sched_interface.h"
#include "srsran/srslog/srslog.h"
#include <bitset>
#include <map>

namespace srsenb {

struct ue_harq_ctxt_t {
  bool                  active    = false;
  bool                  ndi       = false;
  uint32_t              pid       = 0;
  uint32_t              nof_txs   = 0;
  uint32_t              nof_retxs = std::numeric_limits<uint32_t>::max();
  uint32_t              riv       = 0;
  srsran_dci_location_t dci_loc   = {};
  uint32_t              tbs       = 0;
  srsran::tti_point     last_tti_rx, first_tti_rx;
};
struct ue_cc_ctxt_t {
  std::array<ue_harq_ctxt_t, SRSRAN_FDD_NOF_HARQ> dl_harqs;
  std::array<ue_harq_ctxt_t, SRSRAN_FDD_NOF_HARQ> ul_harqs;
};
struct sim_ue_ctxt_t {
  bool                      conres_rx = false;
  uint16_t                  rnti;
  uint32_t                  preamble_idx, msg3_riv;
  srsran::tti_point         prach_tti_rx, rar_tti_rx, msg3_tti_rx, msg4_tti_rx;
  sched_interface::ue_cfg_t ue_cfg;
  std::vector<ue_cc_ctxt_t> cc_list;

  const sched_interface::ue_cfg_t::cc_cfg_t* get_cc_cfg(uint32_t enb_cc_idx) const;
  int                                        enb_to_ue_cc_idx(uint32_t enb_cc_idx) const;
  bool                                       is_msg3_harq(uint32_t ue_cc_idx, uint32_t pid) const;
  bool is_last_ul_retx(uint32_t ue_cc_idx, uint32_t pid, uint32_t maxharq_msg3tx) const;
  bool is_last_dl_retx(uint32_t ue_cc_idx, uint32_t pid) const;
};

struct sim_enb_ctxt_t {
  srsran::span<const sched_cell_params_t>  cell_params;
  std::map<uint16_t, const sim_ue_ctxt_t*> ue_db;
};
struct ue_tti_events {
  struct cc_data {
    bool     configured = false;
    uint32_t ue_cc_idx  = 0;
    int      dl_pid     = -1;
    bool     dl_ack     = false;
    int      tb         = 0;
    int      ul_pid     = -1;
    bool     ul_ack     = false;
    int      dl_cqi     = -1;
    int      ul_snr     = -1;
  };
  srsran::tti_point    tti_rx;
  std::vector<cc_data> cc_list;
};

class ue_sim
{
public:
  ue_sim(uint16_t                         rnti_,
         const sched_interface::ue_cfg_t& ue_cfg_,
         srsran::tti_point                prach_tti_rx,
         uint32_t                         preamble_idx);

  void set_cfg(const sched_interface::ue_cfg_t& ue_cfg_);
  void bearer_cfg(uint32_t lc_id, const mac_lc_ch_cfg_t& cfg);

  int update(const sf_output_res_t& sf_out);

  const sim_ue_ctxt_t& get_ctxt() const { return ctxt; }
  sim_ue_ctxt_t&       get_ctxt() { return ctxt; }

private:
  void update_conn_state(const sf_output_res_t& sf_out);
  void update_dl_harqs(const sf_output_res_t& sf_out);
  void update_ul_harqs(const sf_output_res_t& sf_out);

  srslog::basic_logger& logger;
  sim_ue_ctxt_t         ctxt;
};

class sched_sim_base
{
public:
  sched_sim_base(sched_interface*                                sched_ptr_,
                 const sched_interface::sched_args_t&            sched_args,
                 const std::vector<sched_interface::cell_cfg_t>& cell_params_);
  virtual ~sched_sim_base() = default;

  int add_user(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_, uint32_t preamble_idx);
  int ue_recfg(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_);
  int bearer_cfg(uint16_t rnti, uint32_t lc_id, const mac_lc_ch_cfg_t& cfg);
  int rem_user(uint16_t rnti);

  void new_tti(srsran::tti_point tti_rx);
  void update(const sf_output_res_t& sf_out);

  sim_enb_ctxt_t get_enb_ctxt() const;
  ue_sim&        at(uint16_t rnti) { return ue_db.at(rnti); }
  const ue_sim&  at(uint16_t rnti) const { return ue_db.at(rnti); }
  ue_sim*        find_rnti(uint16_t rnti)
  {
    auto it = ue_db.find(rnti);
    return it != ue_db.end() ? &it->second : nullptr;
  }
  const ue_sim* find_rnti(uint16_t rnti) const
  {
    auto it = ue_db.find(rnti);
    return it != ue_db.end() ? &it->second : nullptr;
  }
  bool                             user_exists(uint16_t rnti) const { return ue_db.count(rnti) > 0; }
  const sched_interface::ue_cfg_t* get_user_cfg(uint16_t rnti) const
  {
    const ue_sim* ret = find_rnti(rnti);
    return ret == nullptr ? nullptr : &ret->get_ctxt().ue_cfg;
  }
  sched_interface*                        get_sched() { return sched_ptr; }
  srsran::const_span<sched_cell_params_t> get_cell_params() { return cell_params; }
  tti_point                               get_tti_rx() const { return current_tti_rx; }

  std::map<uint16_t, ue_sim>::iterator begin() { return ue_db.begin(); }
  std::map<uint16_t, ue_sim>::iterator end() { return ue_db.end(); }

  // configurable by simulator concrete implementation
  virtual void set_external_tti_events(const sim_ue_ctxt_t& ue_ctxt, ue_tti_events& pending_events) = 0;

private:
  int set_default_tti_events(const sim_ue_ctxt_t& ue_ctxt, ue_tti_events& pending_events);
  int apply_tti_events(sim_ue_ctxt_t& ue_ctxt, const ue_tti_events& events);

  srslog::basic_logger&            logger;
  sched_interface*                 sched_ptr;
  std::vector<sched_cell_params_t> cell_params;

  srsran::tti_point                             current_tti_rx;
  std::map<uint16_t, ue_sim>                    ue_db;
  std::map<uint16_t, sched_interface::ue_cfg_t> final_ue_cfg;
  uint32_t                                      error_counter = 0;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_SIM_UE_H
