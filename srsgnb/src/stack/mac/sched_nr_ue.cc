/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsgnb/hdr/stack/mac/sched_nr_ue.h"
#include "srsgnb/hdr/stack/mac/sched_nr_helpers.h"
#include "srsran/common/string_helpers.h"
#include "srsran/mac/mac_sch_pdu_nr.h"

namespace srsenb {
namespace sched_nr_impl {

int ue_buffer_manager::get_dl_tx_total() const
{
  int total_bytes = base_type::get_dl_tx_total();
  for (ue_buffer_manager::ce_t ce : pending_ces) {
    total_bytes += srsran::mac_sch_subpdu_nr::sizeof_ce(ce.lcid, false);
  }
  return total_bytes;
}

/**
 * @brief Allocates LCIDs and update US buffer states depending on available resources and checks if there is SRB0/CCCH
 MAC PDU segmentation

 * @param rem_bytes TBS to be filled with MAC CEs and MAC SDUs [in bytes]
 * @param reset_buf_states If true, when there is SRB0/CCCH MAC PDU segmentation, restore the UE buffers and scheduled
 LCIDs as before running this function
 * @return true if there is no SRB0/CCCH MAC PDU segmentation, false otherwise
 */
bool ue_buffer_manager::pdu_builder::alloc_subpdus(uint32_t rem_bytes, sched_nr_interface::dl_pdu_t& pdu)
{
  // First step: allocate MAC CEs until resources allow
  srsran::deque<ce_t> restore_ces;
  for (ce_t ce : parent->pending_ces) {
    if (ce.cc == cc) {
      // Note: This check also avoids thread collisions across UE carriers
      uint32_t size_ce = srsran::mac_sch_subpdu_nr::sizeof_ce(ce.lcid, false);
      if (size_ce > rem_bytes) {
        break;
      }
      rem_bytes -= size_ce;
      pdu.subpdus.push_back(ce.lcid);
      parent->pending_ces.pop_front();
    }
  }

  // Second step: allocate the remaining LCIDs (LCIDs for MAC CEs are addressed above)
  for (uint32_t lcid = 0; rem_bytes > 0 and is_lcid_valid(lcid); ++lcid) {
    uint32_t pending_lcid_bytes = parent->get_dl_tx_total(lcid);
    // Return false if the TBS is too small to store the entire CCCH buffer without segmentation
    if (lcid == srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::CCCH and pending_lcid_bytes > rem_bytes) {
      pdu.subpdus.push_back(lcid);
      return false;
    }
    if (pending_lcid_bytes > 0) {
      rem_bytes -= std::min(rem_bytes, pending_lcid_bytes);
      pdu.subpdus.push_back(lcid);
    }
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

slot_ue::slot_ue(ue_carrier& ue_, slot_point slot_tx_) : ue(&ue_), pdcch_slot(slot_tx_)
{
  const uint32_t k0 = 0;
  pdsch_slot        = pdcch_slot + k0;
  uint32_t k1       = ue->bwp_cfg.get_k1(pdsch_slot);
  uci_slot          = pdsch_slot + k1;
  uint32_t k2       = ue->bwp_cfg.active_bwp().pusch_ra_list[0].K;
  pusch_slot        = pdcch_slot + k2;

  dl_active = ue->cell_params.bwps[0].slots[pdsch_slot.slot_idx()].is_dl;
  if (dl_active) {
    dl_bytes = ue->common_ctxt.pending_dl_bytes;
    h_dl     = ue->harq_ent.find_pending_dl_retx();
    if (h_dl == nullptr) {
      h_dl = ue->harq_ent.find_empty_dl_harq();
    }
  }
  ul_active = ue->cell_params.bwps[0].slots[pusch_slot.slot_idx()].is_ul;
  if (ul_active) {
    ul_bytes = ue->common_ctxt.pending_ul_bytes;
    h_ul     = ue->harq_ent.find_pending_ul_retx();
    if (h_ul == nullptr) {
      h_ul = ue->harq_ent.find_empty_ul_harq();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_carrier::ue_carrier(uint16_t                              rnti_,
                       const ue_cfg_manager&                 uecfg_,
                       const cell_config_manager&            cell_params_,
                       const ue_context_common&              ctxt,
                       const ue_buffer_manager::pdu_builder& pdu_builder_) :
  rnti(rnti_),
  cc(cell_params_.cc),
  logger(srslog::fetch_basic_logger(cell_params_.sched_args.logger_name)),
  bwp_cfg(rnti_, cell_params_.bwps[0], uecfg_),
  cell_params(cell_params_),
  pdu_builder(pdu_builder_),
  common_ctxt(ctxt),
  harq_ent(rnti_, cell_params_.nof_prb(), SCHED_NR_MAX_HARQ, cell_params_.bwps[0].logger)
{}

void ue_carrier::set_cfg(const ue_cfg_manager& ue_cfg)
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

/// Generate basic UE config at the point of RACH
sched_nr_ue_cfg_t get_rach_ue_cfg_helper(uint32_t cc, const sched_params_t& sched_params)
{
  sched_nr_ue_cfg_t uecfg = {};
  uecfg.carriers.resize(1);
  uecfg.carriers[0].active = true;
  uecfg.carriers[0].cc     = cc;
  uecfg.phy_cfg            = sched_params.cells[cc].default_ue_phy_cfg;
  return uecfg;
}

ue::ue(uint16_t rnti_, uint32_t cc, const sched_params_t& sched_cfg_) :
  ue(rnti_, get_rach_ue_cfg_helper(cc, sched_cfg_), sched_cfg_)
{}

ue::ue(uint16_t rnti_, const sched_nr_ue_cfg_t& uecfg, const sched_params_t& sched_cfg_) :
  rnti(rnti_),
  sched_cfg(sched_cfg_),
  buffers(rnti_, srslog::fetch_basic_logger(sched_cfg_.sched_cfg.logger_name)),
  ue_cfg(uecfg.carriers[0].cc)
{
  set_cfg(uecfg);
}

void ue::set_cfg(const sched_nr_ue_cfg_t& cfg)
{
  ue_cfg.apply_config_request(cfg);
  for (auto& ue_cc_cfg : cfg.carriers) {
    if (ue_cc_cfg.active) {
      if (carriers[ue_cc_cfg.cc] == nullptr) {
        carriers[ue_cc_cfg.cc] = std::make_unique<ue_carrier>(rnti,
                                                              ue_cfg,
                                                              sched_cfg.cells[ue_cc_cfg.cc],
                                                              common_ctxt,
                                                              ue_buffer_manager::pdu_builder{ue_cc_cfg.cc, buffers});
      } else {
        carriers[ue_cc_cfg.cc]->set_cfg(ue_cfg);
      }
    }
  }

  buffers.config_lcids(ue_cfg.ue_bearers);
}

void ue::add_dl_mac_ce(uint32_t ce_lcid, uint32_t nof_cmds)
{
  for (uint32_t i = 0; i < nof_cmds; ++i) {
    // If not specified otherwise, the CE is transmitted in PCell
    buffers.pending_ces.push_back(ue_buffer_manager::ce_t{ce_lcid, cfg().carriers[0].cc});
  }
}

void ue::rlc_buffer_state(uint32_t lcid, uint32_t newtx, uint32_t priotx)
{
  buffers.dl_buffer_state(lcid, newtx, priotx);
}

void ue::new_slot(slot_point pdcch_slot)
{
  last_tx_slot = pdcch_slot;

  for (std::unique_ptr<ue_carrier>& cc : carriers) {
    if (cc != nullptr) {
      cc->harq_ent.new_slot(pdcch_slot - TX_ENB_DELAY);
    }
  }

  // Compute pending DL/UL bytes for {rnti, pdcch_slot}
  if (sched_cfg.sched_cfg.auto_refill_buffer) {
    common_ctxt.pending_dl_bytes = 1000000;
    common_ctxt.pending_ul_bytes = 1000000;
  } else {
    common_ctxt.pending_dl_bytes = buffers.get_dl_tx_total();
    common_ctxt.pending_ul_bytes = buffers.get_bsr();
    for (auto& ue_cc_cfg : ue_cfg.carriers) {
      auto& cc = carriers[ue_cc_cfg.cc];
      if (cc != nullptr) {
        // Discount UL HARQ pending bytes to BSR
        for (uint32_t pid = 0; pid < cc->harq_ent.nof_ul_harqs(); ++pid) {
          if (not cc->harq_ent.ul_harq(pid).empty()) {
            common_ctxt.pending_ul_bytes -= std::min(cc->harq_ent.ul_harq(pid).tbs() / 8, common_ctxt.pending_ul_bytes);
            if (last_sr_slot.valid() and cc->harq_ent.ul_harq(pid).harq_slot_tx() > last_sr_slot) {
              last_sr_slot.clear();
            }
          }
        }
      }
    }
    if (common_ctxt.pending_ul_bytes == 0 and last_sr_slot.valid()) {
      // If unanswered SR is pending
      common_ctxt.pending_ul_bytes = 512;
    }
  }
}

slot_ue ue::make_slot_ue(slot_point pdcch_slot, uint32_t cc)
{
  srsran_assert(carriers[cc] != nullptr, "make_slot_ue() called for unknown rnti=0x%x,cc=%d", rnti, cc);
  return slot_ue(*carriers[cc], pdcch_slot);
}

} // namespace sched_nr_impl
} // namespace srsenb
