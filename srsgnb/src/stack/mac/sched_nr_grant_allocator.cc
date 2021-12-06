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

#include "srsgnb/hdr/stack/mac/sched_nr_grant_allocator.h"
#include "srsgnb/hdr/stack/mac/sched_nr_bwp.h"
#include "srsgnb/hdr/stack/mac/sched_nr_helpers.h"

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
  dl_prbs(bwp_cfg_.cfg.rb_width, bwp_cfg_.cfg.start_rb, bwp_cfg_.cfg.pdsch.rbg_size_cfg_1),
  ul_prbs(bwp_cfg_.cfg.rb_width, bwp_cfg_.cfg.start_rb, bwp_cfg_.cfg.pdsch.rbg_size_cfg_1),
  slot_idx(slot_idx_),
  cfg(&bwp_cfg_),
  pdcchs(bwp_cfg_, slot_idx_, dl.phy.pdcch_dl, dl.phy.pdcch_ul),
  rar_softbuffer(harq_softbuffer_pool::get_instance().get_tx(bwp_cfg_.cfg.rb_width))
{}

void bwp_slot_grid::reset()
{
  pdcchs.reset();
  dl_prbs.reset();
  ul_prbs.reset();
  dl.phy.ssb.clear();
  dl.phy.nzp_csi_rs.clear();
  dl.phy.pdcch_dl.clear();
  dl.phy.pdcch_ul.clear();
  dl.phy.pdsch.clear();
  dl.data.clear();
  dl.rar.clear();
  dl.sib_idxs.clear();
  ul.pusch.clear();
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
  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[pdcch_slot];
  alloc_result   ret            = verify_pdsch_space(bwp_pdcch_slot, bwp_pdcch_slot);
  if (ret != alloc_result::success) {
    return ret;
  }
  if (bwp_pdcch_slot.dl_prbs.collides(prbs)) {
    return alloc_result::sch_collision;
  }

  const uint32_t ss_id = 0;
  pdcch_dl_t*    pdcch = bwp_pdcch_slot.pdcchs.alloc_si_pdcch(ss_id, aggr_idx);
  if (pdcch == nullptr) {
    logger.warning("SCHED: Cannot allocate SIB1 due to lack of PDCCH space.");
    return alloc_result::no_cch_space;
  }

  // RAR allocation successful.
  bwp_pdcch_slot.dl_prbs |= prbs;
  // Generate DCI for SIB
  pdcch->dci_cfg.coreset0_bw = srsran_coreset_get_bw(&cfg.cfg.pdcch.coreset[0]);
  if (not fill_dci_sib(prbs, si_idx, si_ntx, *bwp_grid.cfg, pdcch->dci)) {
    // Cancel on-going PDCCH allocation
    bwp_pdcch_slot.pdcchs.rem_last_pdcch(pdcch->dci.ctx);
    return alloc_result::invalid_coderate;
  }

  // Generate PDSCH
  bwp_pdcch_slot.dl.phy.pdsch.emplace_back();
  pdsch_t&          pdsch = bwp_pdcch_slot.dl.phy.pdsch.back();
  srsran_slot_cfg_t slot_cfg;
  slot_cfg.idx = pdcch_slot.to_uint();
  int code     = srsran_ra_dl_dci_to_grant_nr(
      &cfg.cell_cfg.carrier, &slot_cfg, &cfg.cfg.pdsch, &pdcch->dci, &pdsch.sch, &pdsch.sch.grant);
  if (code != SRSRAN_SUCCESS) {
    logger.warning("Error generating SIB PDSCH grant.");
    bwp_pdcch_slot.pdcchs.rem_last_pdcch(pdcch->dci.ctx);
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
  static const uint32_t msg3_nof_prbs = 3, m = 0;

  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[pdcch_slot];
  slot_point     msg3_slot      = pdcch_slot + cfg.pusch_ra_list[m].msg3_delay;
  bwp_slot_grid& bwp_msg3_slot  = bwp_grid[msg3_slot];
  alloc_result   ret            = verify_pusch_space(bwp_msg3_slot);
  if (ret != alloc_result::success) {
    return ret;
  }
  ret = verify_pdsch_space(bwp_pdcch_slot, bwp_pdcch_slot);
  if (ret != alloc_result::success) {
    return ret;
  }
  if (not bwp_pdcch_slot.dl.phy.ssb.empty()) {
    // TODO: support concurrent PDSCH and SSB
    logger.debug("SCHED: skipping RAR allocation. Cause: concurrent PDSCH and SSB not yet supported");
    return alloc_result::no_sch_space;
  }
  if (pending_rachs.size() > bwp_pdcch_slot.dl.rar.capacity() - bwp_pdcch_slot.dl.rar.size()) {
    logger.error("SCHED: Trying to allocate too many Msg3 grants in a single slot (%zd)", pending_rachs.size());
    return alloc_result::invalid_grant_params;
  }
  for (auto& rach : pending_rachs) {
    auto ue_it = slot_ues.find(rach.temp_crnti);
    if (ue_it == slot_ues.end()) {
      logger.info("SCHED: Postponing rnti=0x%x RAR allocation. Cause: The ue object not yet fully created",
                  rach.temp_crnti);
      return alloc_result::no_rnti_opportunity;
    }
  }

  // Check DL RB collision
  if (bwp_pdcch_slot.dl_prbs.collides(interv)) {
    logger.debug("SCHED: Provided RBG mask collides with allocation previously made.");
    return alloc_result::sch_collision;
  }

  // Check Msg3 RB collision
  uint32_t     total_ul_nof_prbs = msg3_nof_prbs * pending_rachs.size();
  uint32_t     total_ul_nof_rbgs = srsran::ceil_div(total_ul_nof_prbs, get_P(bwp_grid.nof_prbs(), false));
  prb_interval msg3_rbs          = find_empty_interval_of_length(bwp_msg3_slot.ul_prbs.prbs(), total_ul_nof_rbgs);
  if (msg3_rbs.length() < total_ul_nof_rbgs) {
    logger.debug("SCHED: No space in PUSCH for Msg3.");
    return alloc_result::sch_collision;
  }

  // Find PDCCH position
  pdcch_dl_t* pdcch = bwp_pdcch_slot.pdcchs.alloc_rar_pdcch(ra_rnti, aggr_idx);
  if (pdcch == nullptr) {
    // Could not find space in PDCCH
    logger.debug("SCHED: No space in PDCCH for DL tx.");
    return alloc_result::no_cch_space;
  }

  // RAR allocation successful.
  bwp_pdcch_slot.dl_prbs |= interv;
  // Generate DCI for RAR with given RA-RNTI
  if (not fill_dci_rar(interv, ra_rnti, *bwp_grid.cfg, pdcch->dci)) {
    // Cancel on-going PDCCH allocation
    bwp_pdcch_slot.pdcchs.rem_last_pdcch(pdcch->dci.ctx);
    return alloc_result::invalid_coderate;
  }
  auto& phy_cfg  = slot_ues[pending_rachs[0].temp_crnti]->phy();
  pdcch->dci_cfg = phy_cfg.get_dci_cfg();
  // Generate RAR PDSCH
  // TODO: Properly fill Msg3 grants
  bwp_pdcch_slot.dl.phy.pdsch.emplace_back();
  pdsch_t&          pdsch = bwp_pdcch_slot.dl.phy.pdsch.back();
  srsran_slot_cfg_t slot_cfg;
  slot_cfg.idx = pdcch_slot.to_uint();
  bool success = phy_cfg.get_pdsch_cfg(slot_cfg, pdcch->dci, pdsch.sch);
  srsran_assert(success, "Error converting DCI to grant");
  pdsch.sch.grant.tb[0].softbuffer.tx = bwp_pdcch_slot.rar_softbuffer->get();

  // Generate Msg3 grants in PUSCH
  uint32_t  last_msg3 = msg3_rbs.start();
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
    prb_interval msg3_interv{last_msg3, last_msg3 + msg3_nof_prbs};
    last_msg3 += msg3_nof_prbs;
    ue.h_ul = ue.find_empty_ul_harq();
    success = ue.h_ul->new_tx(msg3_slot, msg3_slot, msg3_interv, mcs, max_harq_msg3_retx);
    srsran_assert(success, "Failed to allocate Msg3");
    fill_dci_msg3(ue, *bwp_grid.cfg, rar_grant.msg3_dci);

    // Generate PUSCH
    bwp_msg3_slot.ul.pusch.emplace_back();
    pusch_t& pusch = bwp_msg3_slot.ul.pusch.back();
    success        = ue->phy().get_pusch_cfg(slot_cfg, rar_grant.msg3_dci, pusch.sch);
    srsran_assert(success, "Error converting DCI to PUSCH grant");
    pusch.sch.grant.tb[0].softbuffer.rx = ue.h_ul->get_softbuffer().get();
    ue.h_ul->set_tbs(pusch.sch.grant.tb[0].tbs);
  }
  bwp_msg3_slot.ul_prbs.add(msg3_rbs);

  return alloc_result::success;
}

// ue is the UE (1 only) that will be allocated
// func computes the grant allocation for this UE
alloc_result bwp_slot_allocator::alloc_pdsch(slot_ue& ue, prb_grant dl_grant)
{
  static const uint32_t                              aggr_idx = 2;
  static const std::array<srsran_dci_format_nr_t, 2> dci_fmt_list{srsran_dci_format_nr_1_1, srsran_dci_format_nr_1_0};

  bwp_slot_grid& bwp_pdcch_slot = bwp_grid[ue.pdcch_slot];
  bwp_slot_grid& bwp_pdsch_slot = bwp_grid[ue.pdsch_slot];
  bwp_slot_grid& bwp_uci_slot   = bwp_grid[ue.uci_slot]; // UCI : UL control info
  alloc_result   result         = verify_pdsch_space(bwp_pdsch_slot, bwp_pdcch_slot, &bwp_uci_slot);
  if (result != alloc_result::success) {
    return result;
  }
  result = verify_ue_cfg(ue.cfg(), ue.h_dl);
  if (result != alloc_result::success) {
    return result;
  }
  if (not bwp_pdsch_slot.dl.phy.ssb.empty()) {
    // TODO: support concurrent PDSCH and SSB
    logger.debug("SCHED: skipping PDSCH allocation. Cause: concurrent PDSCH and SSB not yet supported");
    return alloc_result::no_sch_space;
  }
  if (bwp_pdsch_slot.dl_prbs.collides(dl_grant)) {
    return alloc_result::sch_collision;
  }

  // Find space in PUCCH
  // TODO

  // Choose SearchSpace + DCI format
  srsran_rnti_type_t rnti_type =
      ue->ue_cfg().ue_bearers[1].direction == mac_lc_ch_cfg_t::IDLE ? srsran_rnti_type_tc : srsran_rnti_type_c;
  // Choose the ss_id the highest number of candidates
  candidate_ss_list_t ss_candidates = find_ss(ue->phy().pdcch, aggr_idx, rnti_type, dci_fmt_list);
  if (ss_candidates.empty()) {
    // Could not find space in PDCCH
    logger.warning("SCHED: No PDCCH candidates for any of the rnti=0x%x search spaces", ue->rnti);
    return alloc_result::no_cch_space;
  }
  const srsran_search_space_t& ss = *ss_candidates[0];

  // Find space and allocate PDCCH
  pdcch_dl_t* pdcch = bwp_pdcch_slot.pdcchs.alloc_dl_pdcch(rnti_type, ss.id, aggr_idx, ue.cfg());
  if (pdcch == nullptr) {
    // Could not find space in PDCCH
    logger.debug("Could not find PDCCH space for rnti=0x%x PDSCH allocation", ue->rnti);
    return alloc_result::no_cch_space;
  }

  // Update PRB grant based on the start and end of CORESET RBs
  reduce_to_dl_coreset_bw(cfg, ss.id, srsran_dci_format_nr_1_0, dl_grant);

  // Allocate HARQ
  int mcs = ue->fixed_pdsch_mcs();
  if (ue.h_dl->empty()) {
    bool ret = ue.h_dl->new_tx(ue.pdsch_slot, ue.uci_slot, dl_grant, mcs, 4);
    srsran_assert(ret, "Failed to allocate DL HARQ");
  } else {
    bool ret = ue.h_dl->new_retx(ue.pdsch_slot, ue.uci_slot, dl_grant);
    mcs      = ue.h_dl->mcs();
    srsran_assert(ret, "Failed to allocate DL HARQ retx");
  }

  // Allocation Successful

  const static float max_R = 0.93;
  while (true) {
    // Generate PDCCH
    fill_dl_dci_ue_fields(ue, *bwp_grid.cfg, ss.id, pdcch->dci.ctx.location, pdcch->dci);
    pdcch->dci.pucch_resource = 0;
    pdcch->dci.dai            = std::count_if(bwp_uci_slot.pending_acks.begin(),
                                   bwp_uci_slot.pending_acks.end(),
                                   [&ue](const harq_ack_t& p) { return p.res.rnti == ue->rnti; });
    pdcch->dci.dai %= 4;
    pdcch->dci_cfg = ue->phy().get_dci_cfg();

    // Generate PUCCH
    bwp_uci_slot.pending_acks.emplace_back();
    bwp_uci_slot.pending_acks.back().phy_cfg = &ue->phy();
    srsran_assert(ue->phy().get_pdsch_ack_resource(pdcch->dci, bwp_uci_slot.pending_acks.back().res),
                  "Error getting ack resource");

    // Generate PDSCH
    bwp_pdsch_slot.dl_prbs |= dl_grant;
    bwp_pdsch_slot.dl.phy.pdsch.emplace_back();
    pdsch_t&          pdsch = bwp_pdsch_slot.dl.phy.pdsch.back();
    srsran_slot_cfg_t slot_cfg;
    slot_cfg.idx = ue.pdsch_slot.to_uint();
    bool ret     = ue->phy().get_pdsch_cfg(slot_cfg, pdcch->dci, pdsch.sch);
    srsran_assert(ret, "Error converting DCI to grant");

    pdsch.sch.grant.tb[0].softbuffer.tx = ue.h_dl->get_softbuffer().get();
    pdsch.data[0]                       = ue.h_dl->get_tx_pdu()->get();
    if (ue.h_dl->nof_retx() == 0) {
      ue.h_dl->set_tbs(pdsch.sch.grant.tb[0].tbs); // update HARQ with correct TBS
    } else {
      srsran_assert(pdsch.sch.grant.tb[0].tbs == (int)ue.h_dl->tbs(), "The TBS did not remain constant in retx");
    }
    if (ue.h_dl->nof_retx() > 0 or bwp_pdsch_slot.dl.phy.pdsch.back().sch.grant.tb[0].R_prime < max_R or mcs <= 0) {
      break;
    }
    // Decrease MCS if first tx and rate is too high
    mcs--;
    ue.h_dl->set_mcs(mcs);
    bwp_pdsch_slot.dl.phy.pdsch.pop_back();
    bwp_uci_slot.pending_acks.pop_back();
  }
  if (mcs == 0) {
    logger.warning("Couldn't find mcs that leads to R<0.9");
  }

  // Select scheduled LCIDs and update UE buffer state
  bwp_pdsch_slot.dl.data.emplace_back();
  ue.build_pdu(ue.h_dl->tbs(), bwp_pdsch_slot.dl.data.back());

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::alloc_pusch(slot_ue& ue, prb_grant ul_grant)
{
  static const uint32_t                              aggr_idx = 2;
  static const std::array<srsran_dci_format_nr_t, 2> dci_fmt_list{srsran_dci_format_nr_0_1, srsran_dci_format_nr_0_0};

  auto&        bwp_pdcch_slot = bwp_grid[ue.pdcch_slot];
  auto&        bwp_pusch_slot = bwp_grid[ue.pusch_slot];
  alloc_result ret            = verify_pusch_space(bwp_pusch_slot);
  if (ret != alloc_result::success) {
    return ret;
  }
  ret = verify_ue_cfg(ue.cfg(), ue.h_ul);
  if (ret != alloc_result::success) {
    return ret;
  }
  pdcch_ul_list_t& pdcchs = bwp_pdcch_slot.dl.phy.pdcch_ul;
  if (bwp_pusch_slot.ul_prbs.collides(ul_grant)) {
    return alloc_result::sch_collision;
  }

  // Choose SearchSpace + DCI format
  srsran_rnti_type_t rnti_type =
      ue->ue_cfg().ue_bearers[1].direction == mac_lc_ch_cfg_t::IDLE ? srsran_rnti_type_tc : srsran_rnti_type_c;
  candidate_ss_list_t ss_candidates = find_ss(ue->phy().pdcch, aggr_idx, rnti_type, dci_fmt_list);
  if (ss_candidates.empty()) {
    // Could not find space in PDCCH
    logger.warning("SCHED: No PDCCH candidates for any of the rnti=0x%x search spaces", ue->rnti);
    return alloc_result::no_cch_space;
  }
  const srsran_search_space_t& ss = *ss_candidates[0];

  pdcch_ul_t* pdcch = bwp_pdcch_slot.pdcchs.alloc_ul_pdcch(ss.id, aggr_idx, ue.cfg());
  if (pdcch == nullptr) {
    // Could not find space in PDCCH
    logger.debug("Could not find PDCCH space for rnti=0x%x PUSCH allocation", ue->rnti);
    return alloc_result::no_cch_space;
  }

  // Allocation Successful

  if (ue.h_ul->empty()) {
    int  mcs     = ue->fixed_pusch_mcs();
    bool success = ue.h_ul->new_tx(ue.pusch_slot, ue.pusch_slot, ul_grant, mcs, ue->ue_cfg().maxharq_tx);
    srsran_assert(success, "Failed to allocate UL HARQ");
  } else {
    bool success = ue.h_ul->new_retx(ue.pusch_slot, ue.pusch_slot, ul_grant);
    srsran_assert(success, "Failed to allocate UL HARQ retx");
  }

  // Generate PDCCH
  fill_ul_dci_ue_fields(ue, *bwp_grid.cfg, ss.id, pdcch->dci.ctx.location, pdcch->dci);
  pdcch->dci_cfg = ue->phy().get_dci_cfg();
  // Generate PUSCH
  bwp_pusch_slot.ul_prbs |= ul_grant;
  bwp_pusch_slot.ul.pusch.emplace_back();
  pusch_t&          pusch = bwp_pusch_slot.ul.pusch.back();
  srsran_slot_cfg_t slot_cfg;
  slot_cfg.idx = ue.pusch_slot.to_uint();
  pusch.pid    = ue.h_ul->pid;
  bool success = ue->phy().get_pusch_cfg(slot_cfg, pdcch->dci, pusch.sch);
  srsran_assert(success, "Error converting DCI to PUSCH grant");
  pusch.sch.grant.tb[0].softbuffer.rx = ue.h_ul->get_softbuffer().get();
  if (ue.h_ul->nof_retx() == 0) {
    ue.h_ul->set_tbs(pusch.sch.grant.tb[0].tbs); // update HARQ with correct TBS
  } else {
    srsran_assert(pusch.sch.grant.tb[0].tbs == (int)ue.h_ul->tbs(), "The TBS did not remain constant in retx");
  }

  return alloc_result::success;
}

alloc_result bwp_slot_allocator::verify_pdsch_space(bwp_slot_grid& pdsch_grid,
                                                    bwp_slot_grid& pdcch_grid,
                                                    bwp_slot_grid* uci_grid) const
{
  if (not pdsch_grid.is_dl() or not pdcch_grid.is_dl()) {
    logger.warning("SCHED: Trying to allocate PDSCH in TDD non-DL slot index=%d", pdsch_grid.slot_idx);
    return alloc_result::no_sch_space;
  }
  if (pdsch_grid.dl.phy.pdsch.full()) {
    logger.warning("SCHED: Maximum number of DL PDSCH grants reached");
    return alloc_result::no_sch_space;
  }
  if (uci_grid != nullptr) {
    if (uci_grid->pending_acks.full()) {
      logger.warning("SCHED: No space for ACK.");
      return alloc_result::no_grant_space;
    }
  }
  return alloc_result::success;
}

alloc_result bwp_slot_allocator::verify_pusch_space(bwp_slot_grid& pusch_grid) const
{
  if (not pusch_grid.is_ul()) {
    logger.warning("SCHED: Trying to allocate PUSCH in TDD non-UL slot index=%d", pusch_grid.slot_idx);
    return alloc_result::no_sch_space;
  }
  if (pusch_grid.ul.pusch.full()) {
    logger.warning("SCHED: Maximum number of PUSCH allocations reached");
    return alloc_result::no_grant_space;
  }
  return alloc_result::success;
}

alloc_result bwp_slot_allocator::verify_ue_cfg(const ue_carrier_params_t& ue_cfg, harq_proc* harq) const
{
  if (ue_cfg.active_bwp().bwp_id != cfg.bwp_id) {
    logger.warning(
        "SCHED: Trying to allocate rnti=0x%x in inactive BWP id=%d", ue_cfg.rnti, ue_cfg.active_bwp().bwp_id);
    return alloc_result::no_rnti_opportunity;
  }
  if (harq == nullptr) {
    logger.warning("SCHED: Trying to allocate rnti=0x%x with no available HARQs", ue_cfg.rnti);
    return alloc_result::no_rnti_opportunity;
  }
  return alloc_result::success;
}

} // namespace sched_nr_impl
} // namespace srsenb
