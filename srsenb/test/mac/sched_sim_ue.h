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

#ifndef SRSLTE_SCHED_SIM_UE_H
#define SRSLTE_SCHED_SIM_UE_H

#include "sched_common_test_suite.h"
#include "srslte/interfaces/sched_interface.h"
#include <bitset>

namespace srsenb {

struct ue_harq_ctxt_t {
  bool                  active    = false;
  bool                  ndi       = false;
  uint32_t              pid       = 0;
  uint32_t              nof_txs   = 0;
  uint32_t              nof_retxs = 0;
  uint32_t              riv       = 0;
  srslte_dci_location_t dci_loc   = {};
  uint32_t              tbs       = 0;
  srslte::tti_point     last_tti_rx, first_tti_rx;
};
struct ue_cc_ctxt_t {
  std::array<ue_harq_ctxt_t, SRSLTE_FDD_NOF_HARQ> dl_harqs;
  std::array<ue_harq_ctxt_t, SRSLTE_FDD_NOF_HARQ> ul_harqs;
};
struct sim_ue_ctxt_t {
  bool                      conres_rx = false;
  uint16_t                  rnti;
  uint32_t                  preamble_idx, msg3_riv;
  srslte::tti_point         prach_tti_rx, rar_tti_rx, msg3_tti_rx, msg4_tti_rx;
  sched_interface::ue_cfg_t ue_cfg;
  std::vector<ue_cc_ctxt_t> cc_list;

  const sched_interface::ue_cfg_t::cc_cfg_t* get_cc_cfg(uint32_t enb_cc_idx) const;
  int                                        enb_to_ue_cc_idx(uint32_t enb_cc_idx) const;
  bool                                       is_msg3_harq(uint32_t ue_cc_idx, uint32_t pid) const;
  bool is_last_ul_retx(uint32_t ue_cc_idx, uint32_t pid, uint32_t maxharq_msg3tx) const;
  bool is_last_dl_retx(uint32_t ue_cc_idx, uint32_t pid) const;
};

struct sim_enb_ctxt_t {
  const std::vector<sched_interface::cell_cfg_t>* cell_params;
  std::map<uint16_t, const sim_ue_ctxt_t*>        ue_db;
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
    int      ul_cqi     = -1;
  };
  srslte::tti_point    tti_rx;
  std::vector<cc_data> cc_list;
};

class ue_sim
{
public:
  ue_sim(uint16_t                                        rnti_,
         const std::vector<sched_interface::cell_cfg_t>& cell_params_,
         const sched_interface::ue_cfg_t&                ue_cfg_,
         srslte::tti_point                               prach_tti_rx,
         uint32_t                                        preamble_idx);

  void set_cfg(const sched_interface::ue_cfg_t& ue_cfg_);
  void bearer_cfg(uint32_t lc_id, const sched_interface::ue_bearer_cfg_t& cfg);

  int update(const sf_output_res_t& sf_out);

  const sim_ue_ctxt_t& get_ctxt() const { return ctxt; }
  sim_ue_ctxt_t&       get_ctxt() { return ctxt; }

  class sync_tti_events
  {
  public:
    sync_tti_events(ue_sim* ue_, sched_interface* sched_) : ue(ue_), sched(sched_) {}
    sync_tti_events(const sync_tti_events&) = delete;
    sync_tti_events(sync_tti_events&&)      = default;
    sync_tti_events& operator=(const sync_tti_events&) = delete;
    sync_tti_events& operator=(sync_tti_events&&) = default;
    ~sync_tti_events() { ue->push_feedback(sched); }
    ue_tti_events* operator->() { return &ue->pending_feedback; }
    ue_tti_events* operator*() { return &ue->pending_feedback; }

  private:
    ue_sim*          ue;
    sched_interface* sched;
  };
  sync_tti_events get_pending_events(srslte::tti_point tti_rx, sched_interface* sched);

private:
  void update_conn_state(const sf_output_res_t& sf_out);
  void update_dl_harqs(const sf_output_res_t& sf_out);
  void update_ul_harqs(const sf_output_res_t& sf_out);
  void push_feedback(sched_interface* sched);

  srslte::log_ref                                 log_h{"MAC"};
  const std::vector<sched_interface::cell_cfg_t>* cell_params;
  sim_ue_ctxt_t                                   ctxt;
  ue_tti_events                                   pending_feedback;
  uint32_t                                        error_count = 0;
};

class ue_db_sim
{
public:
  ue_db_sim(const std::vector<sched_interface::cell_cfg_t>& cell_params_) : cell_params(&cell_params_) {}
  void add_user(uint16_t                         rnti,
                const sched_interface::ue_cfg_t& ue_cfg_,
                srslte::tti_point                prach_tti_rx_,
                uint32_t                         preamble_idx);
  void ue_recfg(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_);
  void bearer_cfg(uint16_t rnti, uint32_t lc_id, const sched_interface::ue_bearer_cfg_t& cfg);
  void rem_user(uint16_t rnti);

  void                                     update(const sf_output_res_t& sf_out);
  std::map<uint16_t, const sim_ue_ctxt_t*> get_ues_ctxt() const;
  ue_sim&                                  at(uint16_t rnti) { return ue_db.at(rnti); }
  const ue_sim&                            at(uint16_t rnti) const { return ue_db.at(rnti); }
  ue_sim*                                  find_rnti(uint16_t rnti)
  {
    auto it = ue_db.find(rnti);
    return it != ue_db.end() ? &it->second : nullptr;
  }
  const ue_sim* find_rnti(uint16_t rnti) const
  {
    auto it = ue_db.find(rnti);
    return it != ue_db.end() ? &it->second : nullptr;
  }

  std::map<uint16_t, ue_sim>::iterator begin() { return ue_db.begin(); }
  std::map<uint16_t, ue_sim>::iterator end() { return ue_db.end(); }

private:
  const std::vector<sched_interface::cell_cfg_t>* cell_params;
  std::map<uint16_t, ue_sim>                      ue_db;
};

} // namespace srsenb

#endif // SRSLTE_SCHED_SIM_UE_H
