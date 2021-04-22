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

#ifndef SRSRAN_SCHED_UE_CELL_H
#define SRSRAN_SCHED_UE_CELL_H

#include "../sched_common.h"
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

  void set_dl_cqi(tti_point tti_rx, uint32_t dl_cqi_);

  bool             configured() const { return ue_cc_idx >= 0; }
  int              get_ue_cc_idx() const { return ue_cc_idx; }
  bool             is_pcell() const { return ue_cc_idx == 0; }
  bool             is_scell() const { return ue_cc_idx > 0; }
  const ue_cc_cfg* get_ue_cc_cfg() const { return configured() ? &ue_cfg->supported_cc_list[ue_cc_idx] : nullptr; }
  const sched_interface::ue_cfg_t* get_ue_cfg() const { return configured() ? ue_cfg : nullptr; }
  cc_st                            cc_state() const { return cc_state_; }

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

private:
  srslog::basic_logger& logger;

  const sched_interface::ue_cfg_t* ue_cfg = nullptr;
  tti_point                        cfg_tti;
  int                              ue_cc_idx = -1;

  // state
  tti_point current_tti;
  cc_st     cc_state_ = cc_st::idle;
};

/*************************************************************
 *                    TBS/MCS derivation
 ************************************************************/

tbs_info cqi_to_tbs_dl(const sched_ue_cell& cell,
                       uint32_t             nof_prb,
                       uint32_t             nof_re,
                       srsran_dci_format_t  dci_format,
                       int                  req_bytes = -1);
tbs_info
cqi_to_tbs_ul(const sched_ue_cell& cell, uint32_t nof_prb, uint32_t nof_re, int req_bytes = -1, int explicit_mcs = -1);

int      get_required_prb_dl(const sched_ue_cell& cell,
                             tti_point            tti_tx_dl,
                             srsran_dci_format_t  dci_format,
                             uint32_t             req_bytes);
uint32_t get_required_prb_ul(const sched_ue_cell& cell, uint32_t req_bytes);

} // namespace srsenb

#endif // SRSRAN_SCHED_UE_CELL_H
