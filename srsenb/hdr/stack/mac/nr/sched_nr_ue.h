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
  explicit slot_ue(uint16_t rnti_, slot_point slot_rx_, uint32_t cc);
  slot_ue(slot_ue&&) noexcept = default;
  slot_ue& operator=(slot_ue&&) noexcept = default;
  bool     empty() const { return rnti == SCHED_NR_INVALID_RNTI; }
  void     release() { rnti = SCHED_NR_INVALID_RNTI; }

  uint16_t   rnti = SCHED_NR_INVALID_RNTI;
  slot_point slot_rx;
  uint32_t   cc = SCHED_NR_MAX_CARRIERS;

  // UE parameters common to all sectors
  bool pending_sr = false;

  // UE parameters that are sector specific
  const bwp_ue_cfg* cfg = nullptr;
  slot_point        pdcch_slot;
  slot_point        pdsch_slot;
  slot_point        pusch_slot;
  slot_point        uci_slot;
  uint32_t          dl_cqi = 0;
  uint32_t          ul_cqi = 0;
  dl_harq_proc*     h_dl = nullptr;
  ul_harq_proc*     h_ul = nullptr;
};

class ue_carrier
{
public:
  ue_carrier(uint16_t rnti, const ue_cfg_t& cfg, const sched_cell_params& cell_params_);
  void    new_slot(slot_point pdcch_slot, const ue_cfg_t& uecfg_);
  slot_ue try_reserve(slot_point pdcch_slot);

  const uint16_t rnti;
  const uint32_t cc;

  // Channel state
  uint32_t dl_cqi = 1;
  uint32_t ul_cqi = 0;

  harq_entity harq_ent;

private:
  bwp_ue_cfg               bwp_cfg;
  const sched_cell_params& cell_params;
};

class ue
{
public:
  ue(uint16_t rnti, const ue_cfg_t& cfg, const sched_params& sched_cfg_);

  slot_ue try_reserve(slot_point pdcch_slot, uint32_t cc);

  void            set_cfg(const ue_cfg_t& cfg);
  const ue_cfg_t& cfg() const { return ue_cfg; }

  void ul_sr_info(slot_point slot_rx) { pending_sr = true; }

  bool     has_ca() const { return ue_cfg.carriers.size() > 1; }
  uint32_t pcell_cc() const { return ue_cfg.carriers[0].cc; }

  std::array<std::unique_ptr<ue_carrier>, SCHED_NR_MAX_CARRIERS> carriers;

private:
  const uint16_t      rnti;
  const sched_params& sched_cfg;

  bool pending_sr = false;

  ue_cfg_t ue_cfg;
};

using ue_map_t      = srsran::static_circular_map<uint16_t, std::unique_ptr<ue>, SCHED_NR_MAX_USERS>;
using slot_ue_map_t = srsran::static_circular_map<uint16_t, slot_ue, SCHED_NR_MAX_USERS>;

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_UE_H
