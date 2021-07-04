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

#include "sched_nr_common.h"
#include "sched_nr_harq.h"
#include "sched_nr_interface.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/move_callback.h"
#include "srsran/adt/pool/cached_alloc.h"

namespace srsenb {

namespace sched_nr_impl {

using ue_cfg_t    = sched_nr_interface::ue_cfg_t;
using ue_cc_cfg_t = sched_nr_interface::ue_cc_cfg_t;

class ue_cfg_extended : public ue_cfg_t
{
public:
  struct search_space_params {
    srsran_search_space_t* cfg = nullptr;
  };
  struct coreset_params {
    srsran_coreset_t*                 cfg = nullptr;
    std::vector<search_space_params*> ss_list;
    bwp_cce_pos_list                  cce_positions;
  };
  struct bwp_params {
    std::vector<search_space_params> search_spaces;
    std::vector<coreset_params>      coresets;
  };
  struct cc_params {
    srsran::bounded_vector<bwp_params, SCHED_NR_MAX_BWP_PER_CELL> bwps;
  };

  uint16_t               rnti;
  std::vector<cc_params> cc_params;

  ue_cfg_extended() = default;
  explicit ue_cfg_extended(uint16_t rnti, const ue_cfg_t& uecfg);
};

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
  const ue_cfg_extended* cfg = nullptr;
  bool                   pending_sr;

  // UE parameters that are sector specific
  const ue_cc_cfg_t* cc_cfg = nullptr;
  uint32_t           bwp_id;
  tti_point          pdcch_tti;
  tti_point          pdsch_tti;
  tti_point          pusch_tti;
  tti_point          uci_tti;
  uint32_t           dl_cqi;
  uint32_t           ul_cqi;
  harq_proc*         h_dl = nullptr;
  harq_proc*         h_ul = nullptr;

private:
  resource_guard::token ue_token;
};

class ue_carrier
{
public:
  ue_carrier(uint16_t rnti, uint32_t cc, const ue_cfg_t& cfg);
  slot_ue try_reserve(tti_point pdcch_tti, const ue_cfg_extended& cfg);
  void    push_feedback(srsran::move_callback<void(ue_carrier&)> callback);

  const uint16_t rnti;
  const uint32_t cc;

  // Channel state
  uint32_t dl_cqi = 1;
  uint32_t ul_cqi = 0;

  harq_entity harq_ent;

private:
  const ue_cfg_t* cfg = nullptr;

  resource_guard busy;
  tti_point      last_tti_rx;

  srsran::deque<srsran::move_callback<void(ue_carrier&)> > pending_feedback;
};

class ue
{
public:
  ue(uint16_t rnti, const ue_cfg_t& cfg);

  slot_ue try_reserve(tti_point tti_rx, uint32_t cc);

  void set_cfg(const ue_cfg_t& cfg);

  void ul_sr_info(tti_point tti_rx) { pending_sr = true; }

  std::array<std::unique_ptr<ue_carrier>, SCHED_NR_MAX_CARRIERS> carriers;

private:
  const uint16_t rnti;

  bool pending_sr = false;

  int                            current_idx = 0;
  std::array<ue_cfg_extended, 4> ue_cfgs;
};

using ue_map_t = srsran::static_circular_map<uint16_t, std::unique_ptr<ue>, SCHED_NR_MAX_USERS>;

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_UE_H
