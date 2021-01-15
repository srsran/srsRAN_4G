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

#ifndef SRSENB_SCHEDULER_UE_H
#define SRSENB_SCHEDULER_UE_H

#include "sched_common.h"
#include "srslte/common/log.h"
#include <map>
#include <vector>

#include "sched_ue_ctrl/sched_harq.h"
#include "sched_ue_ctrl/sched_lch.h"
#include "sched_ue_ctrl/sched_ue_cell.h"
#include "sched_ue_ctrl/tpc.h"
#include <bitset>
#include <deque>

namespace srsenb {

typedef enum { UCI_PUSCH_NONE = 0, UCI_PUSCH_CQI, UCI_PUSCH_ACK, UCI_PUSCH_ACK_CQI } uci_pusch_t;
enum class cc_st { active, idle, activating, deactivating };

struct tbs_info {
  int tbs_bytes = -1;
  int mcs       = 0;
};

struct cc_sched_ue {
  const static int SCHED_MAX_HARQ_PROC = FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS;

  cc_sched_ue(const sched_interface::ue_cfg_t& cfg_,
              const sched_ue_cell&             cell_ue_,
              uint16_t                         rnti_,
              uint32_t                         ue_cc_idx,
              srslte::tti_point                current_tti);
  void reset();
  void set_cfg(const sched_interface::ue_cfg_t& cfg); ///< reconfigure ue carrier
  void finish_tti(srslte::tti_point tti_rx);

  uint32_t                   get_aggr_level(uint32_t nof_bits);
  tbs_info                   alloc_tbs(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, bool is_ul);
  tbs_info                   alloc_tbs_dl(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes);
  tbs_info                   alloc_tbs_ul(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, int explicit_mcs = -1);
  int                        get_required_prb_dl(tti_point tti_tx_dl, uint32_t req_bytes);
  uint32_t                   get_required_prb_ul(uint32_t req_bytes);
  const sched_cell_params_t* get_cell_cfg() const { return cell_ue->cell_cfg; }
  uint32_t                   get_ue_cc_idx() const { return ue_cc_idx; }
  void                       set_dl_cqi(tti_point tti_rx, uint32_t dl_cqi);
  int                        cqi_to_tbs(uint32_t nof_prb, uint32_t nof_re, bool is_ul, uint32_t* mcs);
  cc_st                      cc_state() const { return cc_state_; }

  harq_entity harq_ent;

  uint32_t  dl_ri = 0;
  tti_point dl_ri_tti_rx{};
  uint32_t  dl_pmi = 0;
  tti_point dl_pmi_tti_rx{};
  uint32_t  dl_cqi = 1;
  tti_point dl_cqi_tti_rx{0};
  uint32_t  ul_cqi = 1;
  tti_point ul_cqi_tti_rx{};
  bool      dl_cqi_rx = false;

  uint32_t max_mcs_dl = 28, max_mcs_ul = 28;
  uint32_t max_aggr_level = 3;
  int      fixed_mcs_ul = 0, fixed_mcs_dl = 0;
  tpc      tpc_fsm;

private:
  // config
  srslte::log_ref                  log_h;
  const sched_interface::ue_cfg_t* cfg     = nullptr;
  const sched_ue_cell*             cell_ue = nullptr;
  uint16_t                         rnti;
  uint32_t                         ue_cc_idx = 0;
  srslte::tti_point                cfg_tti;

  // state
  srslte::tti_point last_tti;
  cc_st             cc_state_ = cc_st::idle;
};

/** This class is designed to be thread-safe because it is called from workers through scheduler thread and from
 * higher layers and mac threads.
 */
class sched_ue
{
  using ue_cfg_t     = sched_interface::ue_cfg_t;
  using bearer_cfg_t = sched_interface::ue_bearer_cfg_t;

public:
  sched_ue();
  void reset();
  void init(uint16_t rnti, const std::vector<sched_cell_params_t>& cell_list_params_);
  void new_subframe(tti_point tti_rx, uint32_t enb_cc_idx);

  /*************************************************************
   *
   * FAPI-like Interface
   *
   ************************************************************/

  void phy_config_enabled(tti_point tti_rx, bool enabled);
  void set_cfg(const ue_cfg_t& cfg);

  void set_bearer_cfg(uint32_t lc_id, const bearer_cfg_t& cfg);
  void rem_bearer(uint32_t lc_id);

  void dl_buffer_state(uint8_t lc_id, uint32_t tx_queue, uint32_t retx_queue);
  void ul_buffer_state(uint8_t lcg_id, uint32_t bsr);
  void ul_phr(int phr);
  void mac_buffer_state(uint32_t ce_code, uint32_t nof_cmds);

  void set_ul_snr(tti_point tti_rx, uint32_t enb_cc_idx, float snr, uint32_t ul_ch_code);
  void set_dl_ri(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t ri);
  void set_dl_pmi(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t ri);
  void set_dl_cqi(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t cqi);
  int  set_ack_info(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack);
  void set_ul_crc(tti_point tti_rx, uint32_t enb_cc_idx, bool crc_res);

  /*******************************************************
   * Custom functions
   *******************************************************/

  const dl_harq_proc&       get_dl_harq(uint32_t idx, uint32_t cc_idx) const;
  uint16_t                  get_rnti() const { return rnti; }
  std::pair<bool, uint32_t> get_active_cell_index(uint32_t enb_cc_idx) const;
  const ue_cfg_t&           get_ue_cfg() const { return cfg; }
  uint32_t                  get_aggr_level(uint32_t ue_cc_idx, uint32_t nof_bits);
  void                      ul_buffer_add(uint8_t lcid, uint32_t bytes);

  /*******************************************************
   * Functions used by scheduler metric objects
   *******************************************************/

  uint32_t get_required_prb_ul(uint32_t cc_idx, uint32_t req_bytes);

  rbg_interval               get_required_dl_rbgs(uint32_t ue_cc_idx);
  srslte::interval<uint32_t> get_requested_dl_bytes(uint32_t ue_cc_idx);
  uint32_t                   get_pending_dl_rlc_data() const;
  uint32_t                   get_expected_dl_bitrate(uint32_t ue_cc_idx, int nof_rbgs = -1) const;

  uint32_t get_pending_ul_data_total(tti_point tti_tx_ul, int this_ue_cc_idx);
  uint32_t get_pending_ul_new_data(tti_point tti_tx_ul, int this_ue_cc_idx);
  uint32_t get_pending_ul_old_data();
  uint32_t get_pending_ul_old_data(uint32_t cc_idx);
  uint32_t get_expected_ul_bitrate(uint32_t ue_cc_idx, int nof_prbs = -1) const;

  dl_harq_proc* get_pending_dl_harq(tti_point tti_tx_dl, uint32_t cc_idx);
  dl_harq_proc* get_empty_dl_harq(tti_point tti_tx_dl, uint32_t cc_idx);
  ul_harq_proc* get_ul_harq(tti_point tti_tx_ul, uint32_t ue_cc_idx);

  /*******************************************************
   * Functions used by the scheduler carrier object
   *******************************************************/

  void finish_tti(tti_point tti_rx, uint32_t enb_cc_idx);

  /*******************************************************
   * Functions used by the scheduler object
   *******************************************************/

  void set_sr();
  void unset_sr();

  int generate_dl_dci_format(uint32_t                          pid,
                             sched_interface::dl_sched_data_t* data,
                             tti_point                         tti_tx_dl,
                             uint32_t                          ue_cc_idx,
                             uint32_t                          cfi,
                             const rbgmask_t&                  user_mask);
  int generate_format0(sched_interface::ul_sched_data_t* data,
                       tti_point                         tti_tx_ul,
                       uint32_t                          cc_idx,
                       prb_interval                      alloc,
                       bool                              needs_pdcch,
                       srslte_dci_location_t             cce_range,
                       int                               explicit_mcs = -1,
                       uci_pusch_t                       uci_type     = UCI_PUSCH_NONE);

  srslte_dci_format_t    get_dci_format();
  const sched_dci_cce_t* get_locations(uint32_t enb_cc_idx, uint32_t current_cfi, uint32_t sf_idx) const;

  cc_sched_ue*                     find_ue_carrier(uint32_t enb_cc_idx);
  size_t                           nof_carriers_configured() const { return carriers.size(); }
  std::bitset<SRSLTE_MAX_CARRIERS> scell_activation_mask() const;
  int                              enb_to_ue_cc_idx(uint32_t enb_cc_idx) const;

  uint32_t get_max_retx();

  bool pucch_sr_collision(tti_point tti_tx_dl, uint32_t n_cce);
  bool pdsch_enabled(tti_point tti_rx, uint32_t enb_cc_idx) const;
  bool pusch_enabled(tti_point tti_rx, uint32_t enb_cc_idx, bool needs_pdcch) const;

private:
  bool is_sr_triggered();

  tbs_info allocate_new_dl_mac_pdu(sched_interface::dl_sched_data_t* data,
                                   dl_harq_proc*                     h,
                                   const rbgmask_t&                  user_mask,
                                   tti_point                         tti_tx_dl,
                                   uint32_t                          ue_cc_idx,
                                   uint32_t                          cfi,
                                   uint32_t                          tb);

  tbs_info compute_mcs_and_tbs(uint32_t               ue_cc_idx,
                               tti_point              tti_tx_dl,
                               uint32_t               nof_alloc_prbs,
                               uint32_t               cfi,
                               const srslte_dci_dl_t& dci);

  bool needs_cqi(uint32_t tti, uint32_t cc_idx, bool will_send = false);

  int generate_format1(uint32_t                          pid,
                       sched_interface::dl_sched_data_t* data,
                       tti_point                         tti_tx_dl,
                       uint32_t                          ue_cc_idx,
                       uint32_t                          cfi,
                       const rbgmask_t&                  user_mask);
  int generate_format2a(uint32_t                          pid,
                        sched_interface::dl_sched_data_t* data,
                        tti_point                         tti_tx_dl,
                        uint32_t                          cc_idx,
                        uint32_t                          cfi,
                        const rbgmask_t&                  user_mask);
  int generate_format2(uint32_t                          pid,
                       sched_interface::dl_sched_data_t* data,
                       tti_point                         tti_tx_dl,
                       uint32_t                          cc_idx,
                       uint32_t                          cfi,
                       const rbgmask_t&                  user_mask);

  /* Args */
  ue_cfg_t                   cfg  = {};
  srslte_cell_t              cell = {};
  mutable srslte::log_ref    log_h;
  const sched_cell_params_t* main_cc_params = nullptr;

  /* Buffer states */
  bool           sr = false;
  lch_ue_manager lch_handler;

  uint32_t cqi_request_tti = 0;
  uint16_t rnti            = 0;
  uint32_t max_msg3retx    = 0;

  bool phy_config_dedicated_enabled = false;

  tti_point                current_tti;
  std::vector<cc_sched_ue> carriers; ///< map of UE CellIndex to carrier configuration

  std::vector<sched_ue_cell> cells; ///< List of eNB cells that may be configured/activated/deactivated for the UE
};

using sched_ue_list = std::map<uint16_t, sched_ue>;

} // namespace srsenb

#endif // SRSENB_SCHEDULER_UE_H
