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
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsenb/hdr/stack/mac/common/ue_buffer_manager.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/move_callback.h"
#include "srsran/adt/pool/cached_alloc.h"

namespace srsenb {

namespace sched_nr_impl {

class slot_ue;

class ue_carrier
{
public:
  ue_carrier(uint16_t rnti, const ue_cfg_t& cfg, const cell_params_t& cell_params_);

  void                       set_cfg(const ue_cfg_t& ue_cfg);
  const ue_carrier_params_t& cfg() const { return bwp_cfg; }

  int dl_ack_info(uint32_t pid, uint32_t tb_idx, bool ack);
  int ul_crc_info(uint32_t pid, bool crc);

  const uint16_t       rnti;
  const uint32_t       cc;
  const cell_params_t& cell_params;

  // Channel state
  uint32_t dl_cqi = 1;
  uint32_t ul_cqi = 0;

  harq_entity harq_ent;

  // metrics
  mac_ue_metrics_t metrics = {};

private:
  friend class slot_ue;

  srslog::basic_logger& logger;
  ue_carrier_params_t   bwp_cfg;
};

class ue
{
public:
  ue(uint16_t rnti, const ue_cfg_t& cfg, const sched_params_t& sched_cfg_);

  void new_slot(slot_point pdcch_slot);

  slot_ue make_slot_ue(slot_point pdcch_slot, uint32_t cc);

  void            set_cfg(const ue_cfg_t& cfg);
  const ue_cfg_t& cfg() const { return ue_cfg; }

  /// UE state feedback
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

  const uint16_t rnti;

private:
  const sched_params_t& sched_cfg;

  slot_point last_pdcch_slot;
  slot_point last_sr_slot;
  int        ul_pending_bytes = 0, dl_pending_bytes = 0;

  ue_cfg_t ue_cfg;
};

class slot_ue
{
public:
  slot_ue() = default;
  explicit slot_ue(ue_carrier& ue, slot_point slot_tx_, uint32_t dl_pending_bytes, uint32_t ul_pending_bytes);
  slot_ue(slot_ue&&) noexcept = default;
  slot_ue& operator=(slot_ue&&) noexcept = default;
  bool     empty() const { return ue == nullptr; }
  void     release() { ue = nullptr; }

  const ue_carrier_params_t& cfg() const { return ue->bwp_cfg; }
  const ue_carrier_params_t& operator*() const { return ue->bwp_cfg; }
  const ue_carrier_params_t* operator->() const { return &ue->bwp_cfg; }

  // mutable interface to ue_carrier state
  dl_harq_proc* find_empty_dl_harq() { return ue->harq_ent.find_empty_dl_harq(); }
  ul_harq_proc* find_empty_ul_harq() { return ue->harq_ent.find_empty_ul_harq(); }

  // UE parameters common to all sectors
  uint32_t dl_pending_bytes = 0, ul_pending_bytes = 0;

  // UE parameters that are sector specific
  bool          dl_active;
  bool          ul_active;
  slot_point    pdcch_slot;
  slot_point    pdsch_slot;
  slot_point    pusch_slot;
  slot_point    uci_slot;
  dl_harq_proc* h_dl = nullptr;
  ul_harq_proc* h_ul = nullptr;

private:
  ue_carrier* ue = nullptr;
};

using ue_map_t      = rnti_map_t<std::unique_ptr<ue> >;
using slot_ue_map_t = rnti_map_t<slot_ue>;

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_UE_H
