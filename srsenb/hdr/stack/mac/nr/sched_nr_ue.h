/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_SCHED_NR_UE_H
#define SRSRAN_SCHED_NR_UE_H

#include "sched_nr_cfg.h"
#include "sched_nr_harq.h"
#include "sched_nr_interface.h"
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
  explicit slot_ue(resource_guard::token ue_token, uint16_t rnti_, tti_point tti_rx_, uint32_t cc);
  slot_ue(slot_ue&&) noexcept = default;
  slot_ue& operator=(slot_ue&&) noexcept = default;
  bool     empty() const { return ue_token.empty(); }
  void     release() { ue_token.release(); }

  uint16_t  rnti = SCHED_NR_INVALID_RNTI;
  tti_point tti_rx;
  uint32_t  cc = SCHED_NR_MAX_CARRIERS;

  // UE parameters common to all sectors
  const bwp_ue_cfg* cfg = nullptr;
  bool              pending_sr;

  // UE parameters that are sector specific
  const ue_cc_cfg_t* cc_cfg = nullptr;
  tti_point          pdcch_tti;
  tti_point          pdsch_tti;
  tti_point          pusch_tti;
  tti_point          uci_tti;
  uint32_t           dl_cqi;
  uint32_t           ul_cqi;
  dl_harq_proc*      h_dl = nullptr;
  harq_proc*         h_ul = nullptr;

private:
  resource_guard::token ue_token;
};

class ue_carrier
{
public:
  ue_carrier(uint16_t rnti, const ue_cfg_t& cfg, const sched_cell_params& cell_params_);
  slot_ue try_reserve(tti_point pdcch_tti, const ue_cfg_t& cfg);
  void    push_feedback(srsran::move_callback<void(ue_carrier&)> callback);

  const uint16_t rnti;
  const uint32_t cc;

  // Channel state
  uint32_t dl_cqi = 1;
  uint32_t ul_cqi = 0;

  harq_entity harq_ent;

private:
  bwp_ue_cfg               bwp_cfg;
  const sched_cell_params& cell_params;

  resource_guard busy;
  tti_point      last_tti_rx;

  srsran::deque<srsran::move_callback<void(ue_carrier&)> > pending_feedback;
};

class ue
{
public:
  ue(uint16_t rnti, const ue_cfg_t& cfg, const sched_params& sched_cfg_);

  slot_ue try_reserve(tti_point tti_rx, uint32_t cc);

  void set_cfg(const ue_cfg_t& cfg);

  void ul_sr_info(tti_point tti_rx) { pending_sr = true; }

  std::array<std::unique_ptr<ue_carrier>, SCHED_NR_MAX_CARRIERS> carriers;

private:
  const uint16_t      rnti;
  const sched_params& sched_cfg;

  bool pending_sr = false;

  int                     current_idx = 0;
  std::array<ue_cfg_t, 4> ue_cfgs;
};

using ue_map_t = srsran::static_circular_map<uint16_t, std::unique_ptr<ue>, SCHED_NR_MAX_USERS>;

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_UE_H
