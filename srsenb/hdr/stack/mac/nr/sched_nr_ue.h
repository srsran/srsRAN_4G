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

#ifndef SRSRAN_SCHED_NR_UE_H
#define SRSRAN_SCHED_NR_UE_H

#include "sched_nr_cfg.h"
#include "sched_nr_harq.h"
#include "sched_nr_interface.h"
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsenb/hdr/stack/mac/common/ue_buffer_manager.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/move_callback.h"
#include "srsran/adt/pool/cached_alloc.h"

namespace srsenb {

namespace sched_nr_impl {

class ue_carrier;

class slot_ue
{
public:
  slot_ue() = default;
  explicit slot_ue(uint16_t rnti_, slot_point slot_rx_, uint32_t cc);
  slot_ue(slot_ue&&) noexcept = default;
  slot_ue& operator=(slot_ue&&) noexcept = default;
  bool     empty() const { return rnti == SCHED_NR_INVALID_RNTI; }
  void     release() { rnti = SCHED_NR_INVALID_RNTI; }

  uint16_t   rnti = SCHED_NR_INVALID_RNTI;
  slot_point slot_rx;
  uint32_t   cc = SCHED_NR_MAX_CARRIERS;

  // UE parameters common to all sectors
  int dl_pending_bytes = 0, ul_pending_bytes = 0;

  // UE parameters that are sector specific
  const bwp_ue_cfg*   cfg      = nullptr;
  harq_entity*        harq_ent = nullptr;
  slot_point          pdcch_slot;
  slot_point          pdsch_slot;
  slot_point          pusch_slot;
  slot_point          uci_slot;
  uint32_t            dl_cqi  = 0;
  uint32_t            ul_cqi  = 0;
  dl_harq_proc*       h_dl    = nullptr;
  ul_harq_proc*       h_ul    = nullptr;
  srsran_uci_cfg_nr_t uci_cfg = {};
};

class ue_carrier
{
public:
  ue_carrier(uint16_t rnti, const ue_cfg_t& cfg, const cell_params_t& cell_params_);
  void set_cfg(const ue_cfg_t& ue_cfg);

  /// Called after CC Feedback has been processed
  void new_slot(slot_point slot_tx);

  slot_ue try_reserve(slot_point pdcch_slot, uint32_t dl_harq_bytes, uint32_t ul_harq_bytes);

  const uint16_t rnti;
  const uint32_t cc;

  // Channel state
  uint32_t dl_cqi = 1;
  uint32_t ul_cqi = 0;

  harq_entity harq_ent;

  // metrics
  mac_ue_metrics_t metrics = {};
  std::mutex       metrics_mutex;

private:
  bwp_ue_cfg           bwp_cfg;
  const cell_params_t& cell_params;
};

class ue
{
public:
  ue(uint16_t rnti, const ue_cfg_t& cfg, const sched_params& sched_cfg_);

  void new_slot(slot_point pdcch_slot);

  slot_ue try_reserve(slot_point pdcch_slot, uint32_t cc);

  void            set_cfg(const ue_cfg_t& cfg);
  const ue_cfg_t& cfg() const { return ue_cfg; }

  void rlc_buffer_state(uint32_t lcid, uint32_t newtx, uint32_t retx) { buffers.dl_buffer_state(lcid, newtx, retx); }
  void ul_bsr(uint32_t lcg, uint32_t bsr_val) { buffers.ul_bsr(lcg, bsr_val); }
  void ul_sr_info() { last_sr_slot = last_pdcch_slot - TX_ENB_DELAY; }

  bool has_ca() const
  {
    return ue_cfg.carriers.size() > 1 and std::count_if(ue_cfg.carriers.begin() + 1,
                                                        ue_cfg.carriers.end(),
                                                        [](const ue_cc_cfg_t& cc) { return cc.active; }) > 0;
  }
  uint32_t pcell_cc() const { return ue_cfg.carriers[0].cc; }

  ue_buffer_manager<true>                                        buffers;
  std::array<std::unique_ptr<ue_carrier>, SCHED_NR_MAX_CARRIERS> carriers;

private:
  const uint16_t      rnti;
  const sched_params& sched_cfg;

  slot_point last_pdcch_slot;
  slot_point last_sr_slot;
  int        ul_pending_bytes = 0, dl_pending_bytes = 0;

  ue_cfg_t ue_cfg;
};

using ue_map_t      = rnti_map_t<std::unique_ptr<ue> >;
using slot_ue_map_t = rnti_map_t<slot_ue>;

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_UE_H
