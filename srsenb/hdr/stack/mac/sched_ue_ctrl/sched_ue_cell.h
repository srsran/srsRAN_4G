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

#ifndef SRSRAN_SCHED_UE_CELL_H
#define SRSRAN_SCHED_UE_CELL_H

#include "../sched_lte_common.h"
#include "sched_dl_cqi.h"
#include "sched_harq.h"
#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_dci.h"
#include "tpc.h"

namespace srsenb {

enum class cc_st { active, idle, activating, deactivating };

struct sched_ue_cell {
  using ue_cc_cfg                      = sched_interface::ue_cfg_t::cc_cfg_t;
  const static int SCHED_MAX_HARQ_PROC = FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS;

  sched_ue_cell(uint16_t rnti_, const sched_cell_params_t& cell_cfg_, tti_point current_tti);
  void set_ue_cfg(const sched_interface::ue_cfg_t& ue_cfg_);
  void new_tti(tti_point tti_rx);
  void clear_feedback();
  void finish_tti(tti_point tti_rx);

  int set_dl_wb_cqi(tti_point tti_rx, uint32_t dl_cqi_);
  int set_dl_sb_cqi(tti_point tti_rx, uint32_t sb_idx, uint32_t dl_cqi_);

  bool             configured() const { return ue_cc_idx >= 0; }
  int              get_ue_cc_idx() const { return ue_cc_idx; }
  bool             is_pcell() const { return ue_cc_idx == 0; }
  bool             is_scell() const { return ue_cc_idx > 0; }
  const ue_cc_cfg* get_ue_cc_cfg() const { return configured() ? &ue_cfg->supported_cc_list[ue_cc_idx] : nullptr; }
  const sched_interface::ue_cfg_t* get_ue_cfg() const { return configured() ? ue_cfg : nullptr; }
  cc_st                            cc_state() const { return cc_state_; }
  float                            get_ul_snr_offset() const { return ul_snr_coeff; }
  float                            get_dl_cqi_offset() const { return dl_cqi_coeff; }

  int get_dl_cqi() const;
  int get_dl_cqi(const rbgmask_t& rbgs) const;
  int get_ul_cqi() const;

  uint32_t get_aggr_level(uint32_t nof_bits) const;

  int set_ack_info(tti_point tti_rx, uint32_t tb_idx, bool ack);
  int set_ul_crc(tti_point tti_rx, bool crc_res);
  int set_ul_snr(tti_point tti_rx, float ul_snr, uint32_t ul_ch_code);

  const uint16_t rnti;

  /// Cell const configuration
  const sched_cell_params_t* cell_cfg = nullptr;

  /// Allowed DCI locations per per CFI and per subframe
  const cce_frame_position_table dci_locations;

  /// Cell HARQ Entity
  harq_entity harq_ent;

  /// Cell Transmit Power Control state machine
  tpc tpc_fsm;

  /// UCI Feedback
  const sched_dl_cqi& dl_cqi() const { return dl_cqi_ctxt; }
  uint32_t            dl_ri = 0;
  tti_point           dl_ri_tti_rx{};
  uint32_t            dl_pmi = 0;
  tti_point           dl_pmi_tti_rx{};
  tti_point           ul_cqi_tti_rx{};

  uint32_t max_mcs_dl = 28, max_mcs_ul = 28;
  uint32_t max_aggr_level = 3;
  int      fixed_mcs_ul = 0, fixed_mcs_dl = 0;

private:
  void check_cc_activation(uint32_t dl_cqi);

  // args
  srslog::basic_logger&            logger;
  const sched_interface::ue_cfg_t* ue_cfg = nullptr;
  tti_point                        cfg_tti;
  int                              ue_cc_idx = -1;

  // state
  tti_point current_tti;
  cc_st     cc_state_ = cc_st::idle;

  // CQI
  float ul_delta_inc = 0, ul_delta_dec = 0;
  float dl_delta_inc = 0, dl_delta_dec = 0;
  float dl_cqi_coeff = 0, ul_snr_coeff = 0;
  float max_cqi_coeff = -5, max_snr_coeff = 5;

  sched_dl_cqi dl_cqi_ctxt;
};

/*************************************************************
 *                    TBS/MCS derivation
 ************************************************************/

/// Compute DL grant optimal TBS and MCS given UE cell context and DL grant parameters
tbs_info cqi_to_tbs_dl(const sched_ue_cell& cell,
                       const rbgmask_t&     rbgs,
                       uint32_t             nof_re,
                       srsran_dci_format_t  dci_format,
                       uint32_t             req_bytes = std::numeric_limits<uint32_t>::max());

/// Compute UL grant optimal TBS and MCS given UE cell context and UL grant parameters
tbs_info
cqi_to_tbs_ul(const sched_ue_cell& cell, uint32_t nof_prb, uint32_t nof_re, int req_bytes = -1, int explicit_mcs = -1);

int      get_required_prb_dl(const sched_ue_cell& cell,
                             tti_point            tti_tx_dl,
                             srsran_dci_format_t  dci_format,
                             uint32_t             req_bytes);
uint32_t get_required_prb_ul(const sched_ue_cell& cell, uint32_t req_bytes);

tbs_info compute_mcs_and_tbs_lower_bound(const sched_ue_cell& ue_cell,
                                         tti_point            tti_tx_dl,
                                         const rbgmask_t&     rbg_mask,
                                         srsran_dci_format_t  dci_format);

bool find_optimal_rbgmask(const sched_ue_cell&       ue_cell,
                          tti_point                  tti_tx_dl,
                          const rbgmask_t&           dl_mask,
                          srsran_dci_format_t        dci_format,
                          srsran::interval<uint32_t> req_bytes,
                          tbs_info&                  tb,
                          rbgmask_t&                 newtxmask);

} // namespace srsenb

#endif // SRSRAN_SCHED_UE_CELL_H
