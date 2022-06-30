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

#include "srsgnb/hdr/stack/mac/sched_nr_grant_allocator.h"
#include "srsgnb/hdr/stack/mac/sched_nr_bwp.h"
#include "srsgnb/hdr/stack/mac/sched_nr_helpers.h"
#include "srsran/mac/mac_sch_pdu_nr.h"

namespace srsenb {
namespace sched_nr_impl {

using candidate_ss_list_t =
    srsran::bounded_vector<const srsran_search_space_t*, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR>;

candidate_ss_list_t find_ss(const srsran_pdcch_cfg_nr_t&               pdcch,
                            uint32_t                                   aggr_idx,
                            srsran_rnti_type_t                         rnti_type,
                            srsran::const_span<srsran_dci_format_nr_t> prio_dcis)
{
  candidate_ss_list_t ret;
  auto                active_ss_lst = view_active_search_spaces(pdcch);

  auto contains_dci_fmt = [prio_dcis, aggr_idx](const srsran_search_space_t& ss) {
    if (ss.nof_candidates[aggr_idx] > 0 and ss.nof_formats > 0) {
      for (uint32_t i = 0; i < prio_dcis.size(); ++i) {
        for (uint32_t j = 0; j < ss.nof_formats; ++j) {
          if (ss.formats[j] == prio_dcis[i]) {
            return true;
          }
        }
      }
    }
    return false;
  };
  auto is_common_ss_allowed = [rnti_type](srsran_search_space_type_t ss_type) {
    switch (rnti_type) {
      case srsran_rnti_type_c:
        return ss_type == srsran_search_space_type_common_1 or ss_type == srsran_search_space_type_common_3;
      case srsran_rnti_type_tc:
      case srsran_rnti_type_ra:
        // TODO: Fix UE config to not use common3
        return ss_type == srsran_search_space_type_common_1 or ss_type == srsran_search_space_type_common_3;
      case srsran_rnti_type_si:
        return ss_type == srsran_search_space_type_common_0;
      default:
        // TODO: Remaining cases
        break;
    }
    return false;
  };

  if (rnti_type == srsran_rnti_type_c) {
    // First search UE-specific
    for (const srsran_search_space_t& ss : active_ss_lst) {
      if (ss.type == srsran_search_space_type_ue and contains_dci_fmt(ss)) {
        ret.push_back(&ss);
      }
    }
  }
  for (const srsran_search_space_t& ss : active_ss_lst) {
    if (is_common_ss_allowed(ss.type) and contains_dci_fmt(ss)) {
      ret.push_back(&ss);
    }
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bwp_slot_grid::bwp_slot_grid(const bwp_params_t& bwp_cfg_, uint32_t slot_idx_) :
  slot_idx(slot_idx_),
  cfg(&bwp_cfg_),
  pdcchs(bwp_cfg_, slot_idx_, dl.phy.pdcch_dl, dl.phy.pdcch_ul),
  pdschs(bwp_cfg_, slot_idx_, dl.phy.pdsch),
  puschs(bwp_cfg_, slot_idx_, ul.pusch),
  rar_softbuffer(harq_softbuffer_pool::get_instance().get_tx(bwp_cfg_.cfg.rb_width))
{}

void bwp_slot_grid::reset()
{
  pdcchs.reset();
  pdschs.reset();
  puschs.reset();
  dl.phy.ssb.clear();
  dl.phy.nzp_csi_rs.clear();
  dl.data.clear();
  dl.rar.clear();
  dl.sib_idxs.clear();
  ul.pucch.clear();
  pending_acks.clear();
}

bwp_res_grid::bwp_res_grid(const bwp_params_t& bwp_cfg_) : cfg(&bwp_cfg_)
{
  for (uint32_t sl = 0; sl < slots.capacity(); ++sl) {
    slots.emplace_back(*cfg, sl % static_cast<uint32_t>(SRSRAN_NSLOTS_PER_FRAME_NR(bwp_cfg_.cell_cfg.carrier.scs)));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bwp_slot_allocator::bwp_slot_allocator(bwp_res_grid& bwp_grid_, slot_point pdcch_slot_, slot_ue_map_t& ues_) :
  logger(bwp_grid_.cfg->logger), cfg(*bwp_grid_.cfg), bwp_grid(bwp_grid_), pdcch_slot(pdcch_slot_), slot_ues(ues_)
{}

alloc_result bwp_slot_allocator::alloc_si(uint32_t            aggr_idx,
                                          uint32_t            si_idx,
                                          uint32_t            si_ntx,
                                          const prb_interval& prbs,
                                          tx_harq_softbuffer& softbuffer)
{
  static const uint32_t               ss_id   = 0;
  static const srsran_dci_format_nr_t dci_fmt = srsran_dci_format_nr_1_0;

  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[pdcch_slot];

  // Verify there is space in PDSCH
  alloc_result ret = bwp_pdcch_slot.pdschs.is_si_grant_valid(ss_id, prbs);
  if (ret != alloc_result::success) {
    return ret;
  }

  // Allocate PDCCH
  auto pdcch_result = bwp_pdcch_slot.pdcchs.alloc_si_pdcch(ss_id, aggr_idx);
  if (pdcch_result.is_error()) {
    logger.warning("SCHED: Cannot allocate SIB due to lack of PDCCH space.");
    return pdcch_result.error();
  }
  pdcch_dl_t& pdcch = *pdcch_result.value();

  // Allocate PDSCH (no need to verify again if there is space in PDSCH)
  pdsch_t& pdsch = bwp_pdcch_slot.pdschs.alloc_si_pdsch_unchecked(ss_id, prbs, pdcch.dci);

  // Generate DCI for SIB
  pdcch.dci_cfg.coreset0_bw = srsran_coreset_get_bw(&cfg.cfg.pdcch.coreset[0]);
  pdcch.dci.mcs             = 5;
  pdcch.dci.rv              = 0;
  pdcch.dci.sii             = si_idx == 0 ? 0 : 1;

  // Generate PDSCH
  srsran_slot_cfg_t slot_cfg;
  slot_cfg.idx = pdcch_slot.to_uint();
  int code     = srsran_ra_dl_dci_to_grant_nr(
      &cfg.cell_cfg.carrier, &slot_cfg, &cfg.cfg.pdsch, &pdcch.dci, &pdsch.sch, &pdsch.sch.grant);
  if (code != SRSRAN_SUCCESS) {
    logger.warning("Error generating SIB PDSCH grant.");
    bwp_pdcch_slot.pdcchs.cancel_last_pdcch();
    bwp_pdcch_slot.dl.phy.pdsch.pop_back();
    return alloc_result::other_cause;
  }
  pdsch.sch.grant.tb[0].softbuffer.tx = softbuffer.get();

  // Store SI msg index
  bwp_pdcch_slot.dl.sib_idxs.push_back(si_idx);

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::alloc_rar_and_msg3(uint16_t                                ra_rnti,
                                                    uint32_t                                aggr_idx,
                                                    prb_interval                            interv,
                                                    srsran::const_span<dl_sched_rar_info_t> pending_rachs)
{
  static const uint32_t               msg3_nof_prbs = 3, m = 0;
  static const srsran_dci_format_nr_t dci_fmt = srsran_dci_format_nr_1_0;

  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[pdcch_slot];
  slot_point     msg3_slot      = pdcch_slot + cfg.pusch_ra_list[m].msg3_delay;
  bwp_slot_grid& bwp_msg3_slot  = bwp_grid[msg3_slot];

  // Verify there is space in PDSCH for RAR
  alloc_result ret = bwp_pdcch_slot.pdschs.is_rar_grant_valid(interv);
  if (ret != alloc_result::success) {
    return ret;
  }
  for (auto& rach : pending_rachs) {
    auto ue_it = slot_ues.find(rach.temp_crnti);
    if (ue_it == slot_ues.end()) {
      logger.info("SCHED: Postponing rnti=0x%x RAR allocation. Cause: The ue object not yet fully created",
                  rach.temp_crnti);
      return alloc_result::no_rnti_opportunity;
    }
  }
  srsran_sanity_check(not bwp_pdcch_slot.dl.rar.full(), "The #RARs should be below #PDSCHs");
  if (not bwp_pdcch_slot.dl.phy.ssb.empty()) {
    // TODO: support concurrent PDSCH and SSB
    logger.debug("SCHED: skipping RAR allocation. Cause: concurrent PDSCH and SSB not yet supported");
    return alloc_result::no_sch_space;
  }

  // Verify there is space in PUSCH for Msg3s
  ret = bwp_msg3_slot.puschs.has_grant_space(pending_rachs.size());
  if (ret != alloc_result::success) {
    return ret;
  }
  // Check Msg3 RB collision
  uint32_t     total_msg3_nof_prbs = msg3_nof_prbs * pending_rachs.size();
  prb_interval all_msg3_rbs =
      find_empty_interval_of_length(bwp_msg3_slot.puschs.occupied_prbs(), total_msg3_nof_prbs, 0);
  if (all_msg3_rbs.length() < total_msg3_nof_prbs) {
    logger.debug("SCHED: No space in PUSCH for Msg3.");
    return alloc_result::sch_collision;
  }

  // Allocate PDCCH position for RAR
  auto pdcch_result = bwp_pdcch_slot.pdcchs.alloc_rar_pdcch(ra_rnti, aggr_idx);
  if (pdcch_result.is_error()) {
    // Could not find space in PDCCH
    return pdcch_result.error();
  }
  pdcch_dl_t& pdcch = *pdcch_result.value();
  pdcch.dci_cfg     = slot_ues[pending_rachs[0].temp_crnti]->get_dci_cfg();
  pdcch.dci.mcs     = 5;

  // Allocate RAR PDSCH
  pdsch_t& pdsch = bwp_pdcch_slot.pdschs.alloc_rar_pdsch_unchecked(interv, pdcch.dci);

  // Fill RAR PDSCH content
  // TODO: Properly fill Msg3 grants
  srsran_slot_cfg_t slot_cfg;
  slot_cfg.idx = pdcch_slot.to_uint();
  int code     = srsran_ra_dl_dci_to_grant_nr(
      &cfg.cell_cfg.carrier, &slot_cfg, &cfg.cfg.pdsch, &pdcch.dci, &pdsch.sch, &pdsch.sch.grant);
  srsran_assert(code == SRSRAN_SUCCESS, "Error converting DCI to grant");
  pdsch.sch.grant.tb[0].softbuffer.tx = bwp_pdcch_slot.rar_softbuffer->get();

  // Generate Msg3 grants in PUSCH
  uint32_t  last_msg3 = all_msg3_rbs.start();
  const int mcs = 0, max_harq_msg3_retx = 4;
  slot_cfg.idx = msg3_slot.to_uint();
  bwp_pdcch_slot.dl.rar.emplace_back();
  sched_nr_interface::rar_t& rar_out = bwp_pdcch_slot.dl.rar.back();
  for (const dl_sched_rar_info_t& grant : pending_rachs) {
    slot_ue& ue = slot_ues[grant.temp_crnti];

    // Generate RAR grant
    rar_out.grants.emplace_back();
    auto& rar_grant = rar_out.grants.back();
    rar_grant.data  = grant;
    fill_dci_from_cfg(cfg, rar_grant.msg3_dci);
    // Fill Msg3 DCI context
    rar_grant.msg3_dci.ctx.coreset_id = pdcch.dci.ctx.coreset_id;
    rar_grant.msg3_dci.ctx.rnti_type  = srsran_rnti_type_tc;
    rar_grant.msg3_dci.ctx.rnti       = ue->rnti;
    rar_grant.msg3_dci.ctx.ss_type    = srsran_search_space_type_rar;
    rar_grant.msg3_dci.ctx.format     = srsran_dci_format_nr_rar;

    // Allocate Msg3 PUSCH allocation
    prb_interval msg3_interv{last_msg3, last_msg3 + msg3_nof_prbs};
    last_msg3 += msg3_nof_prbs;
    pusch_t& pusch = bwp_msg3_slot.puschs.alloc_pusch_unchecked(msg3_interv, rar_grant.msg3_dci);

    // Allocate UL HARQ
    ue.h_ul = ue.find_empty_ul_harq();
    srsran_sanity_check(ue.h_ul != nullptr, "Failed to allocate Msg3");
    bool success = ue.h_ul->new_tx(msg3_slot, msg3_interv, mcs, max_harq_msg3_retx, rar_grant.msg3_dci);
    srsran_sanity_check(success, "Failed to allocate Msg3");

    // Generate PUSCH content
    success = ue->phy().get_pusch_cfg(slot_cfg, rar_grant.msg3_dci, pusch.sch);
    srsran_assert(success, "Error converting DCI to PUSCH grant");
    pusch.sch.grant.tb[0].softbuffer.rx = ue.h_ul->get_softbuffer().get();
    ue.h_ul->set_tbs(pusch.sch.grant.tb[0].tbs);
  }

  return alloc_result::success;
}

// ue is the UE (1 only) that will be allocated
// func computes the grant allocation for this UE
alloc_result bwp_slot_allocator::alloc_pdsch(slot_ue& ue, uint32_t ss_id, const prb_grant& dl_grant)
{
  static const uint32_t               aggr_idx  = 2;
  static const srsran_dci_format_nr_t dci_fmt   = srsran_dci_format_nr_1_0;
  static const srsran_rnti_type_t     rnti_type = srsran_rnti_type_c;

  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[ue.pdcch_slot];
  bwp_slot_grid& bwp_pdsch_slot = bwp_grid[ue.pdsch_slot];
  bwp_slot_grid& bwp_uci_slot   = bwp_grid[ue.uci_slot]; // UCI : UL control info

  // Verify there is space in PDSCH
  alloc_result ret = bwp_pdcch_slot.pdschs.is_ue_grant_valid(ue.cfg(), ss_id, dci_fmt, dl_grant);
  if (ret != alloc_result::success) {
    return ret;
  }

  alloc_result result = verify_uci_space(bwp_uci_slot);
  if (result != alloc_result::success) {
    return result;
  }
  if (ue.h_dl == nullptr) {
    logger.warning("SCHED: Trying to allocate rnti=0x%x with no available DL HARQs", ue->rnti);
    return result;
  }
  if (not bwp_pdsch_slot.dl.phy.ssb.empty()) {
    // TODO: support concurrent PDSCH and SSB
    logger.debug("SCHED: skipping PDSCH allocation. Cause: concurrent PDSCH and SSB not yet supported");
    return alloc_result::no_sch_space;
  }

  // Check space in PUCCH/PUSCH for UCI
  // TODO

  // Find space and allocate PDCCH
  auto pdcch_result = bwp_pdcch_slot.pdcchs.alloc_dl_pdcch(rnti_type, ss_id, aggr_idx, ue.cfg());
  if (pdcch_result.is_error()) {
    // Could not find space in PDCCH
    return pdcch_result.error();
  }
  pdcch_dl_t& pdcch        = *pdcch_result.value();
  pdcch.dci_cfg            = ue->get_dci_cfg();
  pdcch.dci.pucch_resource = 0;
  pdcch.dci.dai            = std::count_if(bwp_uci_slot.pending_acks.begin(),
                                bwp_uci_slot.pending_acks.end(),
                                [&ue](const harq_ack_t& p) { return p.res.rnti == ue->rnti; });
  pdcch.dci.dai %= 4;

  // Allocate PDSCH
  pdsch_t& pdsch = bwp_pdcch_slot.pdschs.alloc_ue_pdsch_unchecked(ss_id, dci_fmt, dl_grant, ue.cfg(), pdcch.dci);

  // Select MCS and Allocate HARQ
  int              mcs          = ue->fixed_pdsch_mcs();
  const static int min_MCS_ccch = 4;
  if (ue.h_dl->empty()) {
    if (mcs < 0) {
      mcs = srsran_ra_nr_cqi_to_mcs(/* cqi */ ue.dl_cqi(),
                                    /* cqi_table_idx */ ue.cfg().phy().csi.reports->cqi_table,
                                    /* mcs_table */ pdsch.sch.sch_cfg.mcs_table,
                                    /* dci_format */ pdcch.dci.ctx.format,
                                    /* search_space_type*/ pdcch.dci.ctx.ss_type,
                                    /* rnti_type */ rnti_type);
      if (mcs < 0) {
        logger.warning("SCHED: UE rnti=0x%x reported CQI=0 - Using lowest MCS=0", ue->rnti);
        mcs = 0;
      }
    }
    // Overwrite MCS if there are pending bytes for LCID. The optimal way would be to verify that there are pending
    // bytes and that the MAC SDU for CCCH gets segmented. But since the event of segmentation happens at most a couple
    // of times (e.g., to send msg4/RRCSetup), we opt for the less optimal but simpler approach.
    if (ue.get_pending_bytes(srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::CCCH) and mcs < min_MCS_ccch) {
      mcs = min_MCS_ccch;
      logger.info("SCHED: MCS increased to min value %d to allocate SRB0/CCCH for rnti=0x%x", min_MCS_ccch, ue->rnti);
    }
    bool success = ue.h_dl->new_tx(ue.pdsch_slot, ue.uci_slot, dl_grant, mcs, 4, pdcch.dci);
    srsran_assert(success, "Failed to allocate DL HARQ");
  } else {
    bool success = ue.h_dl->new_retx(ue.pdsch_slot, ue.uci_slot, dl_grant, pdcch.dci);
    mcs          = ue.h_dl->mcs();
    srsran_assert(success, "Failed to allocate DL HARQ retx");
  }

  srsran_slot_cfg_t slot_cfg;
  slot_cfg.idx = ue.pdsch_slot.to_uint();
  // Value 0.95 is from TS 38.214 v15.14.00, Section 5.1.3, page 17
  const static float max_R = 0.95;
  double             R_prime;
  // The purpose of the internal loop is to decrease the MCS if the effective coderate is too high. This loop
  // only affects the high MCS values
  while (true) {
    // Generate PDSCH
    bool success = ue->phy().get_pdsch_cfg(slot_cfg, pdcch.dci, pdsch.sch);
    srsran_assert(success, "Error converting DCI to grant");
    if (ue.h_dl->nof_retx() != 0) {
      srsran_assert(pdsch.sch.grant.tb[0].tbs == (int)ue.h_dl->tbs(), "The TBS did not remain constant in retx");
    }
    R_prime = pdsch.sch.grant.tb[0].R_prime;
    if (ue.h_dl->nof_retx() > 0 or R_prime < max_R or mcs <= 0 or
        (ue.get_pending_bytes(srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::CCCH) and mcs <= min_MCS_ccch)) {
      break;
    }
    // Decrease MCS if first tx and rate is too high
    mcs--;
    pdcch.dci.mcs = mcs;
  }
  if (R_prime >= max_R and mcs == 0) {
    logger.warning("Couldn't find mcs that leads to R<0.95");
  }

  ue.h_dl->set_mcs(mcs);
  ue.h_dl->set_tbs(pdsch.sch.grant.tb[0].tbs); // set HARQ TBS
  pdsch.sch.grant.tb[0].softbuffer.tx = ue.h_dl->get_softbuffer().get();
  pdsch.data[0]                       = ue.h_dl->get_tx_pdu()->get();

  // Select scheduled LCIDs and update UE buffer state
  bwp_pdsch_slot.dl.data.emplace_back();
  // NOTE: ue.h_dl->tbs() has to be converted from bits to bytes
  bool segmented_ccch_pdu = not ue.build_pdu(ue.h_dl->tbs() / 8, bwp_pdsch_slot.dl.data.back());
  if (segmented_ccch_pdu) {
    logger.error("SCHED: Insufficient resources to allocate SRB0/CCCH for rnti=0x%x", min_MCS_ccch, ue->rnti);
  }

  // Generate PUCCH
  bwp_uci_slot.pending_acks.emplace_back();
  bwp_uci_slot.pending_acks.back().phy_cfg = &ue->phy();
  bool success = ue->phy().get_pdsch_ack_resource(pdcch.dci, bwp_uci_slot.pending_acks.back().res);
  srsran_assert(success, "Error getting ack resource");

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::alloc_pusch(slot_ue& ue, const prb_grant& ul_grant)
{
  static const uint32_t                              aggr_idx = 2;
  static const std::array<srsran_dci_format_nr_t, 2> dci_fmt_list{srsran_dci_format_nr_0_1, srsran_dci_format_nr_0_0};
  static const srsran_rnti_type_t                    rnti_type = srsran_rnti_type_c;

  auto& bwp_pdcch_slot = bwp_grid[ue.pdcch_slot];
  auto& bwp_pusch_slot = bwp_grid[ue.pusch_slot];

  if (ue.h_ul == nullptr) {
    logger.warning("SCHED: Trying to allocate rnti=0x%x with no available UL HARQs", ue->rnti);
    return alloc_result::no_rnti_opportunity;
  }

  // Choose SearchSpace + DCI format
  candidate_ss_list_t ss_candidates = find_ss(ue->phy().pdcch, aggr_idx, rnti_type, dci_fmt_list);
  if (ss_candidates.empty()) {
    // Could not find space in PDCCH
    logger.warning("SCHED: No PDCCH candidates for any of the rnti=0x%x search spaces", ue->rnti);
    return alloc_result::no_cch_space;
  }
  const srsran_search_space_t& ss = *ss_candidates[0];

  // Verify if PUSCH allocation is valid
  alloc_result ret = bwp_pusch_slot.puschs.is_grant_valid(ss.type, ul_grant);
  if (ret != alloc_result::success) {
    return ret;
  }

  auto pdcch_result = bwp_pdcch_slot.pdcchs.alloc_ul_pdcch(ss.id, aggr_idx, ue.cfg());
  if (pdcch_result.is_error()) {
    // Could not find space in PDCCH
    return pdcch_result.error();
  }

  // Allocation Successful
  pdcch_ul_t& pdcch = *pdcch_result.value();
  pdcch.dci_cfg     = ue->get_dci_cfg();

  // Allocate PUSCH
  pusch_t& pusch = bwp_pusch_slot.puschs.alloc_pusch_unchecked(ul_grant, pdcch.dci);

  if (ue.h_ul->empty()) {
    int  mcs     = ue->fixed_pusch_mcs();
    bool success = ue.h_ul->new_tx(ue.pusch_slot, ul_grant, mcs, ue->ue_cfg().maxharq_tx, pdcch.dci);
    srsran_assert(success, "Failed to allocate UL HARQ");
  } else {
    bool success = ue.h_ul->new_retx(ue.pusch_slot, ul_grant, pdcch.dci);
    srsran_assert(success, "Failed to allocate UL HARQ retx");
  }

  // Generate PUSCH content
  srsran_slot_cfg_t slot_cfg;
  slot_cfg.idx = ue.pusch_slot.to_uint();
  pusch.pid    = ue.h_ul->pid;
  bool success = ue->phy().get_pusch_cfg(slot_cfg, pdcch.dci, pusch.sch);
  srsran_assert(success, "Error converting DCI to PUSCH grant");
  pusch.sch.grant.tb[0].softbuffer.rx = ue.h_ul->get_softbuffer().get();
  if (ue.h_ul->nof_retx() == 0) {
    ue.h_ul->set_tbs(pusch.sch.grant.tb[0].tbs); // update HARQ with correct TBS
  } else {
    srsran_assert(pusch.sch.grant.tb[0].tbs == (int)ue.h_ul->tbs(), "The TBS did not remain constant in retx");
  }

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::verify_uci_space(const bwp_slot_grid& uci_grid) const
{
  if (uci_grid.pending_acks.full()) {
    logger.warning("SCHED: No space for ACK.");
    return alloc_result::no_grant_space;
  }
  return alloc_result::success;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

prb_grant find_optimal_dl_grant(bwp_slot_allocator& slot_alloc, const slot_ue& ue, uint32_t ss_id)
{
  static const srsran_dci_format_nr_t dci_fmt = srsran_dci_format_nr_1_0; // TODO: Support more DCI formats

  prb_bitmap used_prb_mask = slot_alloc.occupied_dl_prbs(ue.pdsch_slot, ss_id, dci_fmt);

  prb_interval prb_interv = find_empty_interval_of_length(used_prb_mask, used_prb_mask.size(), 0);

  return prb_interv;
}

} // namespace sched_nr_impl
} // namespace srsenb
