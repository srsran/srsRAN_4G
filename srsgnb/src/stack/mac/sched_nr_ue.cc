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

#include "srsgnb/hdr/stack/mac/sched_nr_ue.h"
#include "srsgnb/hdr/stack/mac/sched_nr_pdcch.h"
#include "srsran/common/string_helpers.h"
#include "srsran/mac/mac_sch_pdu_nr.h"

namespace srsenb {
namespace sched_nr_impl {

int ue_buffer_manager::get_dl_tx_total() const
{
  int total_bytes = base_type::get_dl_tx_total();
  for (uint32_t lcid : pending_ces) {
    total_bytes += srsran::mac_sch_subpdu_nr::sizeof_ce(lcid, false);
  }
  return total_bytes;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

slot_ue::slot_ue(ue_carrier& ue_, slot_point slot_tx_, uint32_t dl_pending_bytes, uint32_t ul_pending_bytes) :
  ue(&ue_), pdcch_slot(slot_tx_)
{
  const uint32_t k0 = 0;
  pdsch_slot        = pdcch_slot + k0;
  uint32_t k1       = ue->bwp_cfg.get_k1(pdsch_slot);
  uci_slot          = pdsch_slot + k1;
  uint32_t k2       = ue->bwp_cfg.active_bwp().pusch_ra_list[0].K;
  pusch_slot        = pdcch_slot + k2;

  const srsran_duplex_config_nr_t& tdd_cfg = ue->cell_params.cfg.duplex;

  dl_active = srsran_duplex_nr_is_dl(&tdd_cfg, 0, pdsch_slot.slot_idx());
  if (dl_active) {
    dl_bytes = dl_pending_bytes;
    h_dl     = ue->harq_ent.find_pending_dl_retx();
    if (h_dl == nullptr) {
      h_dl = ue->harq_ent.find_empty_dl_harq();
    }
  }
  ul_active = srsran_duplex_nr_is_ul(&tdd_cfg, 0, pusch_slot.slot_idx());
  if (ul_active) {
    ul_bytes = ul_pending_bytes;
    h_ul     = ue->harq_ent.find_pending_ul_retx();
    if (h_ul == nullptr) {
      h_ul = ue->harq_ent.find_empty_ul_harq();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_carrier::ue_carrier(uint16_t rnti_, const ue_cfg_t& uecfg_, const cell_params_t& cell_params_) :
  rnti(rnti_),
  cc(cell_params_.cc),
  logger(srslog::fetch_basic_logger(cell_params_.sched_args.logger_name)),
  bwp_cfg(rnti_, cell_params_.bwps[0], uecfg_),
  cell_params(cell_params_),
  harq_ent(rnti_, cell_params_.nof_prb(), SCHED_NR_MAX_HARQ, cell_params_.bwps[0].logger)
{}

void ue_carrier::set_cfg(const ue_cfg_t& ue_cfg)
{
  bwp_cfg = ue_carrier_params_t(rnti, cell_params.bwps[0], ue_cfg);
}

int ue_carrier::dl_ack_info(uint32_t pid, uint32_t tb_idx, bool ack)
{
  int tbs = harq_ent.dl_ack_info(pid, tb_idx, ack);
  if (tbs < 0) {
    logger.warning("SCHED: rnti=0x%x received DL HARQ-ACK for empty pid=%d", rnti, pid);
    return tbs;
  }
  if (ack) {
    metrics.tx_brate += tbs;
  } else {
    metrics.tx_errors++;
  }
  metrics.tx_pkts++;
  return tbs;
}

int ue_carrier::ul_crc_info(uint32_t pid, bool crc)
{
  int ret = harq_ent.ul_crc_info(pid, crc);
  if (ret < 0) {
    logger.warning("SCHED: rnti=0x%x,cc=%d received CRC for empty pid=%d", rnti, cc, pid);
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue::ue(uint16_t rnti_, const ue_cfg_t& cfg, const sched_params_t& sched_cfg_) :
  rnti(rnti_), sched_cfg(sched_cfg_), buffers(rnti_, srslog::fetch_basic_logger(sched_cfg_.sched_cfg.logger_name))
{
  set_cfg(cfg);
}

void ue::set_cfg(const ue_cfg_t& cfg)
{
  bool conres_needed = cfg.is_temp_crnti != ue_cfg.is_temp_crnti;

  ue_cfg = cfg;
  for (auto& ue_cc_cfg : cfg.carriers) {
    if (ue_cc_cfg.active) {
      if (carriers[ue_cc_cfg.cc] == nullptr) {
        carriers[ue_cc_cfg.cc].reset(new ue_carrier(rnti, ue_cfg, sched_cfg.cells[ue_cc_cfg.cc]));
      } else {
        carriers[ue_cc_cfg.cc]->set_cfg(ue_cfg);
      }
    }
  }

  buffers.config_lcids(cfg.ue_bearers);

  if (conres_needed) {
    mac_buffer_state(62, 1);
  }
}

void ue::mac_buffer_state(uint32_t ce_lcid, uint32_t nof_cmds)
{
  for (uint32_t i = 0; i < nof_cmds; ++i) {
    buffers.pending_ces.push_back(ce_lcid);
  }
}

void ue::new_slot(slot_point pdcch_slot)
{
  last_pdcch_slot = pdcch_slot;

  for (std::unique_ptr<ue_carrier>& cc : carriers) {
    if (cc != nullptr) {
      cc->harq_ent.new_slot(pdcch_slot - TX_ENB_DELAY);
    }
  }

  // Compute pending DL/UL bytes for {rnti, pdcch_slot}
  if (sched_cfg.sched_cfg.auto_refill_buffer) {
    dl_pending_bytes = 1000000;
    ul_pending_bytes = 1000000;
  } else {
    dl_pending_bytes = buffers.get_dl_tx_total();
    ul_pending_bytes = buffers.get_bsr();
    for (auto& ue_cc_cfg : ue_cfg.carriers) {
      auto& cc = carriers[ue_cc_cfg.cc];
      if (cc != nullptr) {
        // Discount UL HARQ pending bytes to BSR
        for (uint32_t pid = 0; pid < cc->harq_ent.nof_ul_harqs(); ++pid) {
          if (not cc->harq_ent.ul_harq(pid).empty()) {
            ul_pending_bytes -= cc->harq_ent.ul_harq(pid).tbs();
            if (last_sr_slot.valid() and cc->harq_ent.ul_harq(pid).harq_slot_tx() > last_sr_slot) {
              last_sr_slot.clear();
            }
          }
        }
      }
    }
    ul_pending_bytes = std::max(0, ul_pending_bytes);
    if (ul_pending_bytes == 0 and last_sr_slot.valid()) {
      // If unanswered SR is pending
      ul_pending_bytes = 512;
    }
  }
}

slot_ue ue::make_slot_ue(slot_point pdcch_slot, uint32_t cc)
{
  srsran_assert(carriers[cc] != nullptr, "make_slot_ue() called for inexistent rnti=0x%x,cc=%d", rnti, cc);
  return slot_ue(*carriers[cc], pdcch_slot, dl_pending_bytes, ul_pending_bytes);
}

} // namespace sched_nr_impl
} // namespace srsenb
