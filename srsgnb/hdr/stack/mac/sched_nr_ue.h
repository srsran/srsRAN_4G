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
#include "sched_ue/ue_cfg_manager.h"
#include "srsenb/hdr/stack/mac/common/base_ue_buffer_manager.h"
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/move_callback.h"
#include "srsran/adt/pool/cached_alloc.h"

namespace srsenb {

namespace sched_nr_impl {

class ue_buffer_manager : public base_ue_buffer_manager<true>
{
  using base_type = base_ue_buffer_manager<true>;

public:
  // Inherited methods from base_ue_buffer_manager base class
  using base_type::base_type;
  using base_type::config_lcid;
  using base_type::dl_buffer_state;
  using base_type::get_bsr;
  using base_type::get_bsr_state;
  using base_type::get_dl_prio_tx;
  using base_type::get_dl_tx;
  using base_type::get_dl_tx_total;
  using base_type::is_bearer_active;
  using base_type::is_bearer_dl;
  using base_type::is_bearer_ul;
  using base_type::is_lcg_active;
  using base_type::ul_bsr;

  int get_dl_tx_total() const;

  // Control Element Command queue
  struct ce_t {
    uint32_t lcid;
    uint32_t cc;
  };
  srsran::deque<ce_t> pending_ces;

  /// Protected, thread-safe interface of "ue_buffer_manager" for "slot_ue"
  struct pdu_builder {
    pdu_builder() = default;
    explicit pdu_builder(uint32_t cc_, ue_buffer_manager& parent_) : cc(cc_), parent(&parent_) {}
    void alloc_subpdus(uint32_t rem_bytes, sched_nr_interface::dl_pdu_t& pdu);

  private:
    uint32_t           cc     = SRSRAN_MAX_CARRIERS;
    ue_buffer_manager* parent = nullptr;
  };
};

/// Class containing context of UE that is common to all carriers
struct ue_context_common {
  uint32_t pending_dl_bytes = 0;
  uint32_t pending_ul_bytes = 0;
};

class slot_ue;

class ue_carrier
{
public:
  ue_carrier(uint16_t                              rnti,
             const ue_cfg_manager&                 cfg,
             const cell_config_manager&            cell_params_,
             const ue_context_common&              ctxt,
             const ue_buffer_manager::pdu_builder& pdu_builder_);

  void                       set_cfg(const ue_cfg_manager& ue_cfg);
  const ue_carrier_params_t& cfg() const { return bwp_cfg; }

  int dl_ack_info(uint32_t pid, uint32_t tb_idx, bool ack);
  int ul_crc_info(uint32_t pid, bool crc);

  const uint16_t             rnti;
  const uint32_t             cc;
  const cell_config_manager& cell_params;

  // Channel state
  uint32_t dl_cqi = 1;
  uint32_t ul_cqi = 0;

  harq_entity harq_ent;

  ue_buffer_manager::pdu_builder pdu_builder;

  // metrics
  mac_ue_metrics_t metrics = {};

  // common context
  const ue_context_common& common_ctxt;

private:
  friend class slot_ue;

  srslog::basic_logger& logger;
  ue_carrier_params_t   bwp_cfg;
};

class ue
{
public:
  ue(uint16_t rnti, uint32_t cc, const sched_params_t& sched_cfg_);
  ue(uint16_t rnti, const sched_nr_ue_cfg_t& uecfg, const sched_params_t& sched_cfg_);

  void new_slot(slot_point pdcch_slot);

  slot_ue make_slot_ue(slot_point pdcch_slot, uint32_t cc);

  /// Update UE CC configuration
  void                  set_cfg(const sched_nr_ue_cfg_t& cfg);
  const ue_cfg_manager& cfg() const { return ue_cfg; }

  void add_dl_mac_ce(uint32_t ce_lcid, uint32_t nof_cmds = 1);
  void rlc_buffer_state(uint32_t lcid, uint32_t newtx, uint32_t retx);

  /// UE state feedback
  void ul_bsr(uint32_t lcg, uint32_t bsr_val) { buffers.ul_bsr(lcg, bsr_val); }
  void ul_sr_info() { last_sr_slot = last_tx_slot - TX_ENB_DELAY; }

  bool has_ca() const
  {
    return ue_cfg.carriers.size() > 1 and std::count_if(ue_cfg.carriers.begin() + 1,
                                                        ue_cfg.carriers.end(),
                                                        [](const ue_cc_cfg_t& cc) { return cc.active; }) > 0;
  }
  uint32_t pcell_cc() const { return ue_cfg.carriers[0].cc; }

  std::array<std::unique_ptr<ue_carrier>, SCHED_NR_MAX_CARRIERS> carriers;

  const uint16_t rnti;

private:
  const sched_params_t& sched_cfg;

  ue_cfg_manager ue_cfg;

  slot_point        last_tx_slot;
  slot_point        last_sr_slot;
  ue_context_common common_ctxt;

  ue_buffer_manager buffers;
};

class slot_ue
{
public:
  slot_ue() = default;
  explicit slot_ue(ue_carrier& ue, slot_point slot_tx_);
  slot_ue(slot_ue&&) noexcept = default;
  slot_ue& operator=(slot_ue&&) noexcept = default;
  bool     empty() const { return ue == nullptr; }
  void     release() { ue = nullptr; }

  const ue_carrier_params_t& cfg() const { return ue->bwp_cfg; }
  const ue_carrier_params_t* operator->() const { return &ue->bwp_cfg; }

  /// Find available HARQs
  dl_harq_proc* find_empty_dl_harq() { return ue->harq_ent.find_empty_dl_harq(); }
  ul_harq_proc* find_empty_ul_harq() { return ue->harq_ent.find_empty_ul_harq(); }

  /// Build PDU with MAC CEs and MAC SDUs
  void build_pdu(uint32_t rem_bytes, sched_nr_interface::dl_pdu_t& pdu)
  {
    ue->pdu_builder.alloc_subpdus(rem_bytes, pdu);
  }

  /// Channel Information Getters
  uint32_t dl_cqi() const { return ue->dl_cqi; }
  uint32_t ul_cqi() const { return ue->ul_cqi; }

  // UE parameters common to all sectors
  uint32_t dl_bytes = 0, ul_bytes = 0;

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
