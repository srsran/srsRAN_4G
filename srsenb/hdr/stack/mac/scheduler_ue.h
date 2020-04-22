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

#ifndef SRSENB_SCHEDULER_UE_H
#define SRSENB_SCHEDULER_UE_H

#include "scheduler_common.h"
#include "srslte/common/log.h"
#include "srslte/mac/pdu.h"
#include <map>
#include <vector>

#include "scheduler_harq.h"
#include <deque>

namespace srsenb {

struct sched_ue_carrier {
  const static int SCHED_MAX_HARQ_PROC = FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS;

  sched_ue_carrier(const sched_interface::ue_cfg_t& cfg_,
                   const sched_cell_params_t&       cell_cfg_,
                   uint16_t                         rnti_,
                   uint32_t                         ue_cc_idx);
  void reset();
  void set_cfg(const sched_interface::ue_cfg_t& cfg); ///< reconfigure ue carrier

  uint32_t                   get_aggr_level(uint32_t nof_bits);
  int                        alloc_tbs(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, bool is_ul, int* mcs);
  int                        alloc_tbs_dl(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, int* mcs);
  int                        alloc_tbs_ul(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, int* mcs);
  int                        get_required_prb_dl(uint32_t req_bytes, uint32_t nof_ctrl_symbols);
  uint32_t                   get_required_prb_ul(uint32_t req_bytes);
  const sched_cell_params_t* get_cell_cfg() const { return cell_params; }
  bool                       is_active() const { return active; }
  void                       set_dl_cqi(uint32_t tti_tx_dl, uint32_t dl_cqi);

  harq_entity harq_ent;

  uint32_t dl_ri      = 0;
  uint32_t dl_ri_tti  = 0;
  uint32_t dl_pmi     = 0;
  uint32_t dl_pmi_tti = 0;
  uint32_t dl_cqi     = 1;
  uint32_t dl_cqi_tti = 0;
  uint32_t ul_cqi     = 1;
  uint32_t ul_cqi_tti = 0;
  bool     dl_cqi_rx  = false;

  uint32_t max_mcs_dl = 28, max_mcs_dl_alt = 27, max_mcs_ul = 28;
  uint32_t max_aggr_level = 3;
  int      fixed_mcs_ul = 0, fixed_mcs_dl = 0;

  // Allowed DCI locations per per CFI and per subframe
  std::array<std::array<sched_dci_cce_t, 10>, 3> dci_locations = {};

private:
  // config
  srslte::log_ref                  log_h;
  const sched_interface::ue_cfg_t* cfg         = nullptr;
  const sched_cell_params_t*       cell_params = nullptr;
  uint16_t                         rnti;
  uint32_t                         ue_cc_idx = 0;
  bool                             active    = false;
};

/** This class is designed to be thread-safe because it is called from workers through scheduler thread and from
 * higher layers and mac threads.
 */
class sched_ue
{
public:
  /*************************************************************
   *
   * FAPI-like Interface
   *
   ************************************************************/
  sched_ue();
  void reset();
  void phy_config_enabled(uint32_t tti, bool enabled);
  void init(uint16_t rnti, const std::vector<sched_cell_params_t>& cell_list_params_);
  void set_cfg(const sched_interface::ue_cfg_t& cfg);

  void set_bearer_cfg(uint32_t lc_id, srsenb::sched_interface::ue_bearer_cfg_t* cfg);
  void rem_bearer(uint32_t lc_id);

  void dl_buffer_state(uint8_t lc_id, uint32_t tx_queue, uint32_t retx_queue);
  void ul_buffer_state(uint8_t lc_id, uint32_t bsr, bool set_value = true);
  void ul_phr(int phr);
  void mac_buffer_state(uint32_t ce_code, uint32_t nof_cmds);
  void ul_recv_len(uint32_t lcid, uint32_t len);

  void set_ul_cqi(uint32_t tti, uint32_t enb_cc_idx, uint32_t cqi, uint32_t ul_ch_code);
  void set_dl_ri(uint32_t tti, uint32_t enb_cc_idx, uint32_t ri);
  void set_dl_pmi(uint32_t tti, uint32_t enb_cc_idx, uint32_t ri);
  void set_dl_cqi(uint32_t tti, uint32_t enb_cc_idx, uint32_t cqi);
  int  set_ack_info(uint32_t tti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack);
  void set_ul_crc(srslte::tti_point tti_rx, uint32_t enb_cc_idx, bool crc_res);

  /*******************************************************
   * Custom functions
   *******************************************************/

  void tpc_inc();
  void tpc_dec();

  const dl_harq_proc&              get_dl_harq(uint32_t idx, uint32_t cc_idx) const;
  uint16_t                         get_rnti() const { return rnti; }
  std::pair<bool, uint32_t>        get_cell_index(uint32_t enb_cc_idx) const;
  const sched_interface::ue_cfg_t& get_ue_cfg() const { return cfg; }
  uint32_t                         get_aggr_level(uint32_t ue_cc_idx, uint32_t nof_bits);

  /*******************************************************
   * Functions used by scheduler metric objects
   *******************************************************/

  uint32_t get_required_prb_ul(uint32_t cc_idx, uint32_t req_bytes);

  rbg_range_t                   get_required_dl_rbgs(uint32_t ue_cc_idx);
  std::pair<uint32_t, uint32_t> get_requested_dl_bytes(uint32_t ue_cc_idx);
  uint32_t                      get_pending_dl_new_data();
  uint32_t                      get_pending_ul_new_data(uint32_t tti);
  uint32_t                      get_pending_ul_old_data(uint32_t cc_idx);
  uint32_t                      get_pending_dl_new_data_total();

  dl_harq_proc* get_pending_dl_harq(uint32_t tti_tx_dl, uint32_t cc_idx);
  dl_harq_proc* get_empty_dl_harq(uint32_t tti_tx_dl, uint32_t cc_idx);
  ul_harq_proc* get_ul_harq(uint32_t tti, uint32_t ue_cc_idx);

  /*******************************************************
   * Functions used by the scheduler carrier object
   *******************************************************/

  void finish_tti(const tti_params_t& tti_params, uint32_t enb_cc_idx);

  /*******************************************************
   * Functions used by the scheduler object
   *******************************************************/

  void set_sr();
  void unset_sr();

  int generate_dl_dci_format(uint32_t                          pid,
                             sched_interface::dl_sched_data_t* data,
                             uint32_t                          tti,
                             uint32_t                          ue_cc_idx,
                             uint32_t                          cfi,
                             const rbgmask_t&                  user_mask);
  int generate_format0(sched_interface::ul_sched_data_t* data,
                       uint32_t                          tti,
                       uint32_t                          cc_idx,
                       ul_harq_proc::ul_alloc_t          alloc,
                       bool                              needs_pdcch,
                       srslte_dci_location_t             cce_range,
                       int                               explicit_mcs = -1);

  srslte_dci_format_t get_dci_format();
  sched_dci_cce_t*    get_locations(uint32_t enb_cc_idx, uint32_t current_cfi, uint32_t sf_idx);
  sched_ue_carrier*   get_ue_carrier(uint32_t enb_cc_idx);

  bool     needs_cqi(uint32_t tti, uint32_t cc_idx, bool will_send = false);
  uint32_t get_max_retx();

  bool pucch_sr_collision(uint32_t current_tti, uint32_t n_cce);

  static int cqi_to_tbs(uint32_t  cqi,
                        uint32_t  nof_prb,
                        uint32_t  nof_re,
                        uint32_t  max_mcs,
                        uint32_t  max_Qm,
                        bool      use_tbs_index_alt,
                        bool      is_ul,
                        uint32_t* mcs);

private:
  struct ue_bearer_t {
    sched_interface::ue_bearer_cfg_t cfg      = {};
    int                              buf_tx   = 0;
    int                              buf_retx = 0;
    int                              bsr      = 0;
  };

  void set_bearer_cfg_unlocked(uint32_t lc_id, const sched_interface::ue_bearer_cfg_t& cfg_);

  bool is_sr_triggered();

  int                 alloc_rlc_pdu(sched_interface::dl_sched_pdu_t* mac_sdu, int rem_tbs);
  uint32_t            allocate_mac_sdus(sched_interface::dl_sched_data_t* data, uint32_t total_tbs, uint32_t tbidx);
  uint32_t            allocate_mac_ces(sched_interface::dl_sched_data_t* data, uint32_t total_tbs, uint32_t ue_cc_idx);
  std::pair<int, int> allocate_new_dl_mac_pdu(sched_interface::dl_sched_data_t* data,
                                              dl_harq_proc*                     h,
                                              const rbgmask_t&                  user_mask,
                                              uint32_t                          tti_tx_dl,
                                              uint32_t                          ue_cc_idx,
                                              uint32_t                          cfi,
                                              uint32_t                          tb,
                                              const char*                       dci_format);

  std::pair<int, int> compute_mcs_and_tbs(uint32_t               ue_cc_idx,
                                          uint32_t               tti_tx_dl,
                                          uint32_t               nof_alloc_prbs,
                                          uint32_t               cfi,
                                          const srslte_dci_dl_t& dci);

  static bool bearer_is_ul(const ue_bearer_t* lch);
  static bool bearer_is_dl(const ue_bearer_t* lch);

  uint32_t get_pending_ul_old_data_unlocked(uint32_t cc_idx);
  uint32_t get_pending_ul_new_data_unlocked(uint32_t tti);

  bool needs_cqi_unlocked(uint32_t tti, uint32_t cc_idx, bool will_send = false);

  int generate_format1(uint32_t                          pid,
                       sched_interface::dl_sched_data_t* data,
                       uint32_t                          tti,
                       uint32_t                          cc_idx,
                       uint32_t                          cfi,
                       const rbgmask_t&                  user_mask);
  int generate_format2a(uint32_t                          pid,
                        sched_interface::dl_sched_data_t* data,
                        uint32_t                          tti,
                        uint32_t                          cc_idx,
                        uint32_t                          cfi,
                        const rbgmask_t&                  user_mask);
  int generate_format2(uint32_t                          pid,
                       sched_interface::dl_sched_data_t* data,
                       uint32_t                          tti,
                       uint32_t                          cc_idx,
                       uint32_t                          cfi,
                       const rbgmask_t&                  user_mask);

  /* Args */
  sched_interface::ue_cfg_t               cfg  = {};
  srslte_cell_t                           cell = {};
  srslte::log_ref                         log_h;
  const std::vector<sched_cell_params_t>* cell_params_list = nullptr;
  const sched_cell_params_t*              main_cc_params   = nullptr;

  /* Buffer states */
  bool                                             sr  = false;
  std::array<ue_bearer_t, sched_interface::MAX_LC> lch = {};

  int      power_headroom  = 0;
  uint32_t cqi_request_tti = 0;
  uint16_t rnti            = 0;
  uint32_t max_msg3retx    = 0;

  /* User State */
  int next_tpc_pusch = 0;
  int next_tpc_pucch = 0;

  bool phy_config_dedicated_enabled = false;

  std::vector<sched_ue_carrier> carriers; ///< map of UE CellIndex to carrier configuration

  // Control Element Command queue
  using ce_cmd = srslte::dl_sch_lcid;
  std::deque<ce_cmd> pending_ces;
};
} // namespace srsenb

#endif // SRSENB_SCHEDULER_UE_H
