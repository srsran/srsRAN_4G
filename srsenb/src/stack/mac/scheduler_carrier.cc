/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/stack/mac/scheduler_carrier.h"

#define Error(fmt, ...) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->debug(fmt, ##__VA_ARGS__)

namespace srsenb {

/*******************************************************
 *          TTI resource Scheduling Methods
 *******************************************************/

void sched::carrier_sched::tti_sched_result_t::init(carrier_sched* carrier_)
{
  parent_carrier = carrier_;
  sched_params   = &carrier_->sched_ptr->sched_params;
  log_h          = sched_params->log_h;
  sibs_cfg       = sched_params->cfg->sibs;
  tti_alloc.init(*sched_params);
}

void sched::carrier_sched::tti_sched_result_t::new_tti(uint32_t tti_rx_, uint32_t start_cfi)
{
  tti_alloc.new_tti(tti_rx_, start_cfi);

  // internal state
  rar_allocs.clear();
  bc_allocs.clear();
  data_allocs.clear();
  ul_data_allocs.clear();

  // TTI result
  pdcch_mask.reset();
  pdcch_mask.resize(tti_alloc.get_pdcch_grid().nof_cces());
  bzero(&dl_sched_result, sizeof(dl_sched_result));
  bzero(&ul_sched_result, sizeof(ul_sched_result));
}

bool sched::carrier_sched::tti_sched_result_t::is_dl_alloc(sched_ue* user) const
{
  for (const auto& a : data_allocs) {
    if (a.user_ptr == user) {
      return true;
    }
  }
  return false;
}

bool sched::carrier_sched::tti_sched_result_t::is_ul_alloc(sched_ue* user) const
{
  for (const auto& a : ul_data_allocs) {
    if (a.user_ptr == user) {
      return true;
    }
  }
  return false;
}

sched::carrier_sched::tti_sched_result_t::ctrl_code_t
sched::carrier_sched::tti_sched_result_t::alloc_dl_ctrl(uint32_t aggr_lvl, uint32_t tbs_bytes, uint16_t rnti)
{
  ctrl_alloc_t ctrl_alloc{};

  // based on rnti, check which type of alloc
  alloc_type_t alloc_type = alloc_type_t::DL_RAR;
  if (rnti == SRSLTE_SIRNTI) {
    alloc_type = alloc_type_t::DL_BC;
  } else if (rnti == SRSLTE_PRNTI) {
    alloc_type = alloc_type_t::DL_PCCH;
  }

  /* Allocate space in the DL RBG and PDCCH grids */
  tti_grid_t::dl_ctrl_alloc_t ret = tti_alloc.alloc_dl_ctrl(aggr_lvl, alloc_type);
  if (not ret.outcome) {
    return {ret.outcome, ctrl_alloc};
  }

  // Allocation Successful
  ctrl_alloc.dci_idx    = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  ctrl_alloc.rbg_range  = ret.rbg_range;
  ctrl_alloc.rnti       = rnti;
  ctrl_alloc.req_bytes  = tbs_bytes;
  ctrl_alloc.alloc_type = alloc_type;

  return {ret.outcome, ctrl_alloc};
}

alloc_outcome_t
sched::carrier_sched::tti_sched_result_t::alloc_bc(uint32_t aggr_lvl, uint32_t sib_idx, uint32_t sib_ntx)
{
  uint32_t    sib_len = sibs_cfg[sib_idx].len;
  uint32_t    rv      = get_rvidx(sib_ntx);
  ctrl_code_t ret     = alloc_dl_ctrl(aggr_lvl, sib_len, SRSLTE_SIRNTI);
  if (not ret.first) {
    Warning("SCHED: Could not allocate SIB=%d, L=%d, len=%d, cause=%s\n",
            sib_idx + 1,
            aggr_lvl,
            sib_len,
            ret.first.to_string());
    return ret.first;
  }

  // BC allocation successful
  bc_alloc_t bc_alloc(ret.second);
  bc_alloc.rv      = rv;
  bc_alloc.sib_idx = sib_idx;
  bc_allocs.push_back(bc_alloc);

  return ret.first;
}

alloc_outcome_t sched::carrier_sched::tti_sched_result_t::alloc_paging(uint32_t aggr_lvl, uint32_t paging_payload)
{
  ctrl_code_t ret = alloc_dl_ctrl(aggr_lvl, paging_payload, SRSLTE_PRNTI);
  if (not ret.first) {
    Warning(
        "SCHED: Could not allocate Paging with payload length=%d, cause=%s\n", paging_payload, ret.first.to_string());
    return ret.first;
  }

  // Paging allocation successful
  bc_alloc_t bc_alloc(ret.second);
  bc_allocs.push_back(bc_alloc);

  return ret.first;
}

sched::carrier_sched::tti_sched_result_t::rar_code_t
sched::carrier_sched::tti_sched_result_t::alloc_rar(uint32_t              aggr_lvl,
                                                    const dl_sched_rar_t& rar_grant,
                                                    uint32_t              prach_tti,
                                                    uint32_t              buf_rar)
{
  // RA-RNTI = 1 + t_id + f_id
  // t_id = index of first subframe specified by PRACH (0<=t_id<10)
  // f_id = index of the PRACH within subframe, in ascending order of freq domain (0<=f_id<6) (for FDD, f_id=0)
  uint16_t ra_rnti = 1 + (uint16_t)(prach_tti % 10);

  ctrl_code_t ret = alloc_dl_ctrl(aggr_lvl, buf_rar, ra_rnti);
  if (not ret.first) {
    Warning("SCHED: Could not allocate RAR for L=%d, cause=%s\n", aggr_lvl, ret.first.to_string());
    return {ret.first, nullptr};
  }

  // Allocation successful
  rar_alloc_t rar_alloc(ret.second);
  rar_alloc.rar_grant = rar_grant;
  rar_allocs.push_back(rar_alloc);

  return {ret.first, &rar_allocs.back()};
}

alloc_outcome_t
sched::carrier_sched::tti_sched_result_t::alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid)
{
  if (is_dl_alloc(user)) {
    log_h->warning("SCHED: Attempt to assign multiple harq pids to the same user rnti=0x%x\n", user->get_rnti());
    return alloc_outcome_t::ERROR;
  }

  // Try to allocate RBGs and DCI
  alloc_outcome_t ret = tti_alloc.alloc_dl_data(user, user_mask);
  if (ret != alloc_outcome_t::SUCCESS) {
    return ret;
  }

  // Allocation Successful
  dl_alloc_t alloc;
  alloc.dci_idx   = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  alloc.user_ptr  = user;
  alloc.user_mask = user_mask;
  alloc.pid       = pid;
  data_allocs.push_back(alloc);

  return alloc_outcome_t::SUCCESS;
}

alloc_outcome_t sched::carrier_sched::tti_sched_result_t::alloc_ul(sched_ue*                              user,
                                                                   ul_harq_proc::ul_alloc_t               alloc,
                                                                   tti_sched_result_t::ul_alloc_t::type_t alloc_type,
                                                                   uint32_t                               mcs)
{
  // Check whether user was already allocated
  if (is_ul_alloc(user)) {
    log_h->warning("SCHED: Attempt to assign multiple ul_harq_proc to the same user rnti=0x%x\n", user->get_rnti());
    return alloc_outcome_t::ERROR;
  }

  // Allocate RBGs and DCI space
  bool            needs_pdcch = alloc_type == ul_alloc_t::ADAPT_RETX or alloc_type == ul_alloc_t::NEWTX;
  alloc_outcome_t ret         = tti_alloc.alloc_ul_data(user, alloc, needs_pdcch);
  if (ret != alloc_outcome_t::SUCCESS) {
    return ret;
  }

  ul_alloc_t ul_alloc = {};
  ul_alloc.type       = alloc_type;
  ul_alloc.dci_idx    = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  ul_alloc.user_ptr   = user;
  ul_alloc.alloc      = alloc;
  ul_alloc.mcs        = mcs;
  ul_data_allocs.push_back(ul_alloc);

  return alloc_outcome_t::SUCCESS;
}

alloc_outcome_t sched::carrier_sched::tti_sched_result_t::alloc_ul_user(sched_ue* user, ul_harq_proc::ul_alloc_t alloc)
{
  // check whether adaptive/non-adaptive retx/newtx
  tti_sched_result_t::ul_alloc_t::type_t alloc_type;
  ul_harq_proc*                          h        = user->get_ul_harq(get_tti_tx_ul());
  bool                                   has_retx = h->has_pending_retx();
  if (has_retx) {
    ul_harq_proc::ul_alloc_t prev_alloc = h->get_alloc();
    if (prev_alloc.L == alloc.L and prev_alloc.RB_start == prev_alloc.L) {
      alloc_type = ul_alloc_t::NOADAPT_RETX;
    } else {
      alloc_type = ul_alloc_t::ADAPT_RETX;
    }
  } else {
    alloc_type = ul_alloc_t::NEWTX;
  }

  return alloc_ul(user, alloc, alloc_type);
}

alloc_outcome_t
sched::carrier_sched::tti_sched_result_t::alloc_ul_msg3(sched_ue* user, ul_harq_proc::ul_alloc_t alloc, uint32_t mcs)
{
  return alloc_ul(user, alloc, ul_alloc_t::MSG3, mcs);
}

void sched::carrier_sched::tti_sched_result_t::set_bc_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
{
  for (const auto& bc_alloc : bc_allocs) {
    sched_interface::dl_sched_bc_t* bc = &dl_sched_result.bc[dl_sched_result.nof_bc_elems];

    // assign NCCE/L
    bc->dci.location = dci_result[bc_alloc.dci_idx]->dci_pos;

    /* Generate DCI format1A */
    prb_range_t prb_range = prb_range_t(bc_alloc.rbg_range, sched_params->P);
    int         tbs       = generate_format1a(
        prb_range.prb_start, prb_range.length(), bc_alloc.req_bytes, bc_alloc.rv, bc_alloc.rnti, &bc->dci);

    // Setup BC/Paging processes
    if (bc_alloc.alloc_type == alloc_type_t::DL_BC) {
      if (tbs <= (int)bc_alloc.req_bytes) {
        log_h->warning("SCHED: Error SIB%d, rbgs=(%d,%d), dci=(%d,%d), len=%d\n",
                       bc_alloc.sib_idx + 1,
                       bc_alloc.rbg_range.rbg_start,
                       bc_alloc.rbg_range.rbg_end,
                       bc->dci.location.L,
                       bc->dci.location.ncce,
                       bc_alloc.req_bytes);
        continue;
      }

      // Setup BC process
      bc->index = bc_alloc.sib_idx;
      bc->type  = sched_interface::dl_sched_bc_t::BCCH;
      bc->tbs   = (uint32_t)bc_alloc.req_bytes;

      log_h->info("SCHED: SIB%d, rbgs=(%d,%d), dci=(%d,%d), rv=%d, len=%d, period=%d, mcs=%d\n",
                  bc_alloc.sib_idx + 1,
                  bc_alloc.rbg_range.rbg_start,
                  bc_alloc.rbg_range.rbg_end,
                  bc->dci.location.L,
                  bc->dci.location.ncce,
                  bc_alloc.rv,
                  bc_alloc.req_bytes,
                  sibs_cfg[bc_alloc.sib_idx].period_rf,
                  bc->dci.tb[0].mcs_idx);
    } else {
      // Paging
      if (tbs <= 0) {
        log_h->warning("SCHED: Error Paging, rbgs=(%d,%d), dci=(%d,%d)\n",
                       bc_alloc.rbg_range.rbg_start,
                       bc_alloc.rbg_range.rbg_end,
                       bc->dci.location.L,
                       bc->dci.location.ncce);
        continue;
      }

      // Setup Paging process
      bc->type = sched_interface::dl_sched_bc_t::PCCH;
      bc->tbs  = (uint32_t)tbs;

      log_h->info("SCHED: PCH, rbgs=(%d,%d), dci=(%d,%d), tbs=%d, mcs=%d\n",
                  bc_alloc.rbg_range.rbg_start,
                  bc_alloc.rbg_range.rbg_end,
                  bc->dci.location.L,
                  bc->dci.location.ncce,
                  tbs,
                  bc->dci.tb[0].mcs_idx);
    }

    dl_sched_result.nof_bc_elems++;
  }
}

void sched::carrier_sched::tti_sched_result_t::set_rar_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
{
  for (const auto& rar_alloc : rar_allocs) {
    sched_interface::dl_sched_rar_t* rar = &dl_sched_result.rar[dl_sched_result.nof_rar_elems];

    // Assign NCCE/L
    rar->dci.location = dci_result[rar_alloc.dci_idx]->dci_pos;

    /* Generate DCI format1A */
    prb_range_t prb_range = prb_range_t(rar_alloc.rbg_range, sched_params->P);
    int         tbs =
        generate_format1a(prb_range.prb_start, prb_range.length(), rar_alloc.req_bytes, 0, rar_alloc.rnti, &rar->dci);
    if (tbs <= 0) {
      log_h->warning("SCHED: Error RAR, ra_rnti_idx=%d, rbgs=(%d,%d), dci=(%d,%d)\n",
                     rar_alloc.rnti,
                     rar_alloc.rbg_range.rbg_start,
                     rar_alloc.rbg_range.rbg_end,
                     rar->dci.location.L,
                     rar->dci.location.ncce);
      continue;
    }

    // Setup RAR process
    rar->tbs        = rar_alloc.req_bytes;
    rar->nof_grants = rar_alloc.rar_grant.nof_grants;
    memcpy(rar->msg3_grant, rar_alloc.rar_grant.msg3_grant, sizeof(dl_sched_rar_grant_t) * rar->nof_grants);

    // Print RAR allocation result
    for (uint32_t i = 0; i < rar->nof_grants; ++i) {
      const auto& msg3_grant = rar->msg3_grant[i];
      uint16_t    expected_rnti =
          parent_carrier->ra_sched_ptr->find_pending_msg3(get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY).rnti;
      log_h->info("SCHED: RAR, temp_crnti=0x%x, ra-rnti=%d, rbgs=(%d,%d), dci=(%d,%d), rar_grant_rba=%d, "
                  "rar_grant_mcs=%d\n",
                  expected_rnti,
                  rar_alloc.rnti,
                  rar_alloc.rbg_range.rbg_start,
                  rar_alloc.rbg_range.rbg_end,
                  rar->dci.location.L,
                  rar->dci.location.ncce,
                  msg3_grant.grant.rba,
                  msg3_grant.grant.trunc_mcs);
    }

    dl_sched_result.nof_rar_elems++;
  }
}

void sched::carrier_sched::tti_sched_result_t::set_dl_data_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
{
  for (const auto& data_alloc : data_allocs) {
    sched_interface::dl_sched_data_t* data = &dl_sched_result.data[dl_sched_result.nof_data_elems];

    // Assign NCCE/L
    data->dci.location = dci_result[data_alloc.dci_idx]->dci_pos;

    // Generate DCI Format1/2/2A
    sched_ue*           user        = data_alloc.user_ptr;
    dl_harq_proc*       h           = user->get_dl_harq(data_alloc.pid);
    uint32_t            data_before = user->get_pending_dl_new_data(get_tti_tx_dl());
    srslte_dci_format_t dci_format  = user->get_dci_format();
    bool                is_newtx    = h->is_empty();

    int tbs = 0;
    switch (dci_format) {
      case SRSLTE_DCI_FORMAT1:
        tbs = user->generate_format1(h, data, get_tti_tx_dl(), get_cfi(), data_alloc.user_mask);
        break;
      case SRSLTE_DCI_FORMAT2:
        tbs = user->generate_format2(h, data, get_tti_tx_dl(), get_cfi(), data_alloc.user_mask);
        break;
      case SRSLTE_DCI_FORMAT2A:
        tbs = user->generate_format2a(h, data, get_tti_tx_dl(), get_cfi(), data_alloc.user_mask);
        break;
      default:
        Error("DCI format (%d) not implemented\n", dci_format);
    }

    if (tbs <= 0) {
      log_h->warning("SCHED: Error DL %s rnti=0x%x, pid=%d, mask=%s, tbs=%d, buffer=%d\n",
                     is_newtx ? "tx" : "retx",
                     user->get_rnti(),
                     h->get_id(),
                     data_alloc.user_mask.to_hex().c_str(),
                     tbs,
                     user->get_pending_dl_new_data(get_tti_tx_dl()));
      continue;
    }

    // Print Resulting DL Allocation
    log_h->info("SCHED: DL %s rnti=0x%x, pid=%d, mask=0x%s, dci=(%d,%d), n_rtx=%d, tbs=%d, buffer=%d/%d\n",
                !is_newtx ? "retx" : "tx",
                user->get_rnti(),
                h->get_id(),
                data_alloc.user_mask.to_hex().c_str(),
                data->dci.location.L,
                data->dci.location.ncce,
                h->nof_retx(0) + h->nof_retx(1),
                tbs,
                data_before,
                user->get_pending_dl_new_data(get_tti_tx_dl()));

    dl_sched_result.nof_data_elems++;
  }
}

void sched::carrier_sched::tti_sched_result_t::set_ul_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
{
  /* Set UL data DCI locs and format */
  for (const auto& ul_alloc : ul_data_allocs) {
    sched_interface::ul_sched_data_t* pusch = &ul_sched_result.pusch[ul_sched_result.nof_dci_elems];

    sched_ue* user = ul_alloc.user_ptr;

    srslte_dci_location_t cce_range = {0, 0};
    if (ul_alloc.needs_pdcch()) {
      cce_range = dci_result[ul_alloc.dci_idx]->dci_pos;
    }

    /* Set fixed mcs if specified */
    int fixed_mcs = (ul_alloc.type == ul_alloc_t::MSG3) ? ul_alloc.mcs : -1;

    /* Generate DCI Format1A */
    uint32_t pending_data_before = user->get_pending_ul_new_data(get_tti_tx_ul());
    int      tbs =
        user->generate_format0(pusch, get_tti_tx_ul(), ul_alloc.alloc, ul_alloc.needs_pdcch(), cce_range, fixed_mcs);

    ul_harq_proc* h = user->get_ul_harq(get_tti_tx_ul());
    if (tbs <= 0) {
      log_h->warning("SCHED: Error %s %s rnti=0x%x, pid=%d, dci=(%d,%d), prb=(%d,%d), tbs=%d, bsr=%d\n",
                     ul_alloc.type == ul_alloc_t::MSG3 ? "Msg3" : "UL",
                     ul_alloc.is_retx() ? "retx" : "tx",
                     user->get_rnti(),
                     h->get_id(),
                     pusch->dci.location.L,
                     pusch->dci.location.ncce,
                     ul_alloc.alloc.RB_start,
                     ul_alloc.alloc.RB_start + ul_alloc.alloc.L,
                     tbs,
                     user->get_pending_ul_new_data(get_tti_tx_ul()));
      continue;
    }

    // Allocation was successful
    if (ul_alloc.type == ul_alloc_t::NEWTX) {
      // Un-trigger SR
      user->unset_sr();
    }

    // Print Resulting UL Allocation
    log_h->info("SCHED: %s %s rnti=0x%x, pid=%d, dci=(%d,%d), prb=(%d,%d), n_rtx=%d, tbs=%d, bsr=%d (%d-%d)\n",
                ul_alloc.is_msg3() ? "Msg3" : "UL",
                ul_alloc.is_retx() ? "retx" : "tx",
                user->get_rnti(),
                h->get_id(),
                pusch->dci.location.L,
                pusch->dci.location.ncce,
                ul_alloc.alloc.RB_start,
                ul_alloc.alloc.RB_start + ul_alloc.alloc.L,
                h->nof_retx(0),
                tbs,
                user->get_pending_ul_new_data(get_tti_tx_ul()),
                pending_data_before,
                user->get_pending_ul_old_data());

    ul_sched_result.nof_dci_elems++;
  }
}

void sched::carrier_sched::tti_sched_result_t::generate_dcis()
{
  /* Pick one of the possible DCI masks */
  pdcch_grid_t::alloc_result_t dci_result;
  //  tti_alloc.get_pdcch_grid().result_to_string();
  tti_alloc.get_pdcch_grid().get_allocs(&dci_result, &pdcch_mask);

  /* Register final CFI */
  dl_sched_result.cfi = tti_alloc.get_pdcch_grid().get_cfi();

  /* Generate DCI formats and fill sched_result structs */
  set_bc_sched_result(dci_result);

  set_rar_sched_result(dci_result);

  set_dl_data_sched_result(dci_result);

  set_ul_sched_result(dci_result);
}

uint32_t sched::carrier_sched::tti_sched_result_t::get_nof_ctrl_symbols() const
{
  return tti_alloc.get_cfi() + ((parent_carrier->cfg->cell.nof_prb <= 10) ? 1 : 0);
}

int sched::carrier_sched::tti_sched_result_t::generate_format1a(uint32_t         rb_start,
                                                                uint32_t         l_crb,
                                                                uint32_t         tbs_bytes,
                                                                uint32_t         rv,
                                                                uint16_t         rnti,
                                                                srslte_dci_dl_t* dci)
{
  /* Calculate I_tbs for this TBS */
  int tbs = tbs_bytes * 8;
  int i;
  int mcs = -1;
  for (i = 0; i < 27; i++) {
    if (srslte_ra_tbs_from_idx(i, 2) >= tbs) {
      dci->type2_alloc.n_prb1a = srslte_ra_type2_t::SRSLTE_RA_TYPE2_NPRB1A_2;
      mcs                      = i;
      tbs                      = srslte_ra_tbs_from_idx(i, 2);
      break;
    }
    if (srslte_ra_tbs_from_idx(i, 3) >= tbs) {
      dci->type2_alloc.n_prb1a = srslte_ra_type2_t::SRSLTE_RA_TYPE2_NPRB1A_3;
      mcs                      = i;
      tbs                      = srslte_ra_tbs_from_idx(i, 3);
      break;
    }
  }
  if (i == 28) {
    Error("Can't allocate Format 1A for TBS=%d\n", tbs);
    return -1;
  }

  Debug("ra_tbs=%d/%d, tbs_bytes=%d, tbs=%d, mcs=%d\n",
        srslte_ra_tbs_from_idx(mcs, 2),
        srslte_ra_tbs_from_idx(mcs, 3),
        tbs_bytes,
        tbs,
        mcs);

  dci->alloc_type       = SRSLTE_RA_ALLOC_TYPE2;
  dci->type2_alloc.mode = srslte_ra_type2_t::SRSLTE_RA_TYPE2_LOC;
  dci->type2_alloc.riv  = srslte_ra_type2_to_riv(l_crb, rb_start, parent_carrier->cfg->cell.nof_prb);
  dci->pid              = 0;
  dci->tb[0].mcs_idx    = mcs;
  dci->tb[0].rv         = rv;
  dci->format           = SRSLTE_DCI_FORMAT1A;
  dci->rnti             = rnti;

  return tbs;
}

/*******************************************************
 *        Broadcast (SIB+Paging) scheduling
 *******************************************************/

bc_sched::bc_sched(sched::cell_cfg_t* cfg_) : cfg(cfg_) {}

void bc_sched::init(srsenb::rrc_interface_mac* rrc_)
{
  rrc = rrc_;
}

void bc_sched::dl_sched(sched::carrier_sched::tti_sched_result_t* tti_sched)
{
  current_sf_idx = tti_sched->get_sf_idx();
  current_sfn    = tti_sched->get_sfn();
  current_tti    = tti_sched->get_tti_tx_dl();
  bc_aggr_level  = 2;

  /* Activate/deactivate SI windows */
  update_si_windows(tti_sched);

  /* Allocate DCIs and RBGs for each SIB */
  alloc_sibs(tti_sched);

  /* Allocate Paging */
  // NOTE: It blocks
  alloc_paging(tti_sched);
}

void bc_sched::update_si_windows(sched::carrier_sched::tti_sched_result_t* tti_sched)
{
  uint32_t tti_tx_dl = tti_sched->get_tti_tx_dl();

  for (uint32_t i = 0; i < pending_sibs.size(); ++i) {
    // There is SIB data
    if (cfg->sibs[i].len == 0) {
      continue;
    }

    if (not pending_sibs[i].is_in_window) {
      uint32_t sf = 5;
      uint32_t x  = 0;
      if (i > 0) {
        x  = (i - 1) * cfg->si_window_ms;
        sf = x % 10;
      }
      if ((current_sfn % (cfg->sibs[i].period_rf)) == x / 10 && current_sf_idx == sf) {
        pending_sibs[i].is_in_window = true;
        pending_sibs[i].window_start = tti_tx_dl;
        pending_sibs[i].n_tx         = 0;
      }
    } else {
      if (i > 0) {
        if (srslte_tti_interval(tti_tx_dl, pending_sibs[i].window_start) > cfg->si_window_ms) {
          // the si window has passed
          pending_sibs[i] = {};
        }
      } else {
        // SIB1 is always in window
        if (pending_sibs[0].n_tx == 4) {
          pending_sibs[0].n_tx = 0;
        }
      }
    }
  }
}

void bc_sched::alloc_sibs(sched::carrier_sched::tti_sched_result_t* tti_sched)
{
  for (uint32_t i = 0; i < pending_sibs.size(); i++) {
    if (cfg->sibs[i].len > 0 and pending_sibs[i].is_in_window and pending_sibs[i].n_tx < 4) {
      uint32_t nof_tx = (i > 0) ? SRSLTE_MIN(srslte::ceil_div(cfg->si_window_ms, 10), 4) : 4;
      uint32_t n_sf   = (tti_sched->get_tti_tx_dl() - pending_sibs[i].window_start);

      // Check if there is any SIB to tx
      bool sib1_flag = (i == 0) and (current_sfn % 2) == 0 and current_sf_idx == 5;
      bool other_sibs_flag =
          (i > 0) and (n_sf >= (cfg->si_window_ms / nof_tx) * pending_sibs[i].n_tx) and current_sf_idx == 9;
      if (not sib1_flag and not other_sibs_flag) {
        continue;
      }

      // Schedule SIB
      tti_sched->alloc_bc(bc_aggr_level, i, pending_sibs[i].n_tx);
      pending_sibs[i].n_tx++;
    }
  }
}

void bc_sched::alloc_paging(sched::carrier_sched::tti_sched_result_t* tti_sched)
{
  /* Allocate DCIs and RBGs for paging */
  if (rrc != nullptr) {
    uint32_t paging_payload = 0;
    if (rrc->is_paging_opportunity(current_tti, &paging_payload) and paging_payload > 0) {
      tti_sched->alloc_paging(bc_aggr_level, paging_payload);
    }
  }
}

void bc_sched::reset()
{
  for (auto& sib : pending_sibs) {
    sib = {};
  }
}

/*******************************************************
 *                 RAR scheduling
 *******************************************************/

ra_sched::ra_sched(sched::cell_cfg_t* cfg_) : cfg(cfg_) {}

void ra_sched::init(srslte::log* log_, std::map<uint16_t, sched_ue>& ue_db_)
{
  log_h = log_;
  ue_db = &ue_db_;
}

// Schedules RAR
// On every call to this function, we schedule the oldest RAR which is still within the window. If outside the window we
// discard it.
void ra_sched::dl_sched(srsenb::sched::carrier_sched::tti_sched_result_t* tti_sched)
{
  tti_tx_dl      = tti_sched->get_tti_tx_dl();
  rar_aggr_level = 2;

  // Discard all RARs out of the window. The first one inside the window is scheduled, if we can't we exit
  while (!pending_rars.empty()) {
    dl_sched_rar_info_t rar = pending_rars.front();
    if (not sched_utils::is_in_tti_interval(tti_tx_dl, rar.prach_tti + 3, rar.prach_tti + 3 + cfg->prach_rar_window)) {
      if (tti_tx_dl >= rar.prach_tti + 3 + cfg->prach_rar_window) {
        log_h->console("SCHED: Could not transmit RAR within the window (RA TTI=%d, Window=%d, Now=%d)\n",
                       rar.prach_tti,
                       cfg->prach_rar_window,
                       tti_tx_dl);
        log_h->error("SCHED: Could not transmit RAR within the window (RA TTI=%d, Window=%d, Now=%d)\n",
                     rar.prach_tti,
                     cfg->prach_rar_window,
                     tti_tx_dl);
        // Remove from pending queue and get next one if window has passed already
        pending_rars.pop();
        continue;
      }
      // If window not yet started do not look for more pending RARs
      return;
    }

    /* Since we do a fixed Msg3 scheduling for all RAR, we can only allocate 1 RAR per TTI.
     * If we have enough space in the window, every call to this function we'll allocate 1 pending RAR and associate a
     * Msg3 transmission
     */
    dl_sched_rar_t rar_grant;
    uint32_t       L_prb = 3;
    uint32_t       n_prb = cfg->nrb_pucch > 0 ? cfg->nrb_pucch : 2;
    bzero(&rar_grant, sizeof(rar_grant));
    uint32_t rba = srslte_ra_type2_to_riv(L_prb, n_prb, cfg->cell.nof_prb);

    dl_sched_rar_grant_t* grant = &rar_grant.msg3_grant[0];
    grant->grant.tpc_pusch      = 3;
    grant->grant.trunc_mcs      = 0;
    grant->grant.rba            = rba;
    grant->data                 = rar;
    rar_grant.nof_grants++;

    // Try to schedule DCI + RBGs for RAR Grant
    sched::carrier_sched::tti_sched_result_t::rar_code_t ret =
        tti_sched->alloc_rar(rar_aggr_level,
                             rar_grant,
                             rar.prach_tti,
                             7 * rar_grant.nof_grants); // fixme: check RAR size

    // If we can allocate, schedule Msg3 and remove from pending
    if (!ret.first) {
      return;
    }

    // Schedule Msg3 only if there is a requirement for Msg3 data
    uint32_t pending_tti              = (tti_sched->get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY) % TTIMOD_SZ;
    pending_msg3[pending_tti].enabled = true;
    pending_msg3[pending_tti].rnti    = rar.temp_crnti; // FIXME
    pending_msg3[pending_tti].L       = L_prb;
    pending_msg3[pending_tti].n_prb   = n_prb;
    dl_sched_rar_grant_t* last_msg3   = &rar_grant.msg3_grant[rar_grant.nof_grants - 1];
    pending_msg3[pending_tti].mcs     = last_msg3->grant.trunc_mcs;
    log_h->info("SCHED: Allocating Msg3 for rnti=%d at tti=%d\n",
                rar.temp_crnti,
                tti_sched->get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY);

    // Remove pending RAR and exit
    pending_rars.pop();
    return;
  }
}

// Schedules Msg3
void ra_sched::ul_sched(sched::carrier_sched::tti_sched_result_t* tti_sched)
{
  uint32_t pending_tti = tti_sched->get_tti_tx_ul() % TTIMOD_SZ;

  // check if there is a Msg3 to allocate
  if (not pending_msg3[pending_tti].enabled) {
    return;
  }

  uint16_t rnti    = pending_msg3[pending_tti].rnti;
  auto     user_it = ue_db->find(rnti);
  if (user_it == ue_db->end()) {
    log_h->warning("SCHED: Msg3 allocated for user rnti=0x%x that no longer exists\n", rnti);
    return;
  }

  /* Allocate RBGs and HARQ for Msg3 */
  ul_harq_proc::ul_alloc_t msg3 = {pending_msg3[pending_tti].n_prb, pending_msg3[pending_tti].L};
  if (not tti_sched->alloc_ul_msg3(&user_it->second, msg3, pending_msg3[pending_tti].mcs)) {
    log_h->warning("SCHED: Could not allocate msg3 within (%d,%d)\n", msg3.RB_start, msg3.RB_start + msg3.L);
    return;
  }
  pending_msg3[pending_tti].enabled = false;
}

int ra_sched::dl_rach_info(dl_sched_rar_info_t rar_info)
{
  log_h->info("SCHED: New RAR tti=%d, preamble=%d, temp_crnti=0x%x, ta_cmd=%d, msg3_size=%d\n",
              rar_info.prach_tti,
              rar_info.preamble_idx,
              rar_info.temp_crnti,
              rar_info.ta_cmd,
              rar_info.msg3_size);
  pending_rars.push(rar_info);
  return 0;
}

void ra_sched::reset()
{
  tti_tx_dl = 0;
  for (auto& msg3 : pending_msg3) {
    msg3 = {};
  }
  while (not pending_rars.empty()) {
    pending_rars.pop();
  }
}

const ra_sched::pending_msg3_t& ra_sched::find_pending_msg3(uint32_t tti)
{
  uint32_t pending_tti = tti % TTIMOD_SZ;
  return pending_msg3[pending_tti];
}

/*******************************************************
 *                 Carrier scheduling
 *******************************************************/

sched::carrier_sched::carrier_sched(sched* sched_) : sched_ptr(sched_), cfg(&sched_->cfg)
{
  bc_sched_ptr.reset(new bc_sched{cfg});
  ra_sched_ptr.reset(new ra_sched{cfg});
}

void sched::carrier_sched::reset()
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  if (ra_sched_ptr != nullptr) {
    ra_sched_ptr->reset();
  }
  if (bc_sched_ptr != nullptr) {
    bc_sched_ptr->reset();
  }
}

void sched::carrier_sched::carrier_cfg()
{
  // sched::cfg is now fully set
  log_h = sched_ptr->log_h;
  tti_dl_mask.resize(10, 0);

  // init Broadcast/RA schedulers
  bc_sched_ptr->init(sched_ptr->rrc);
  ra_sched_ptr->init(log_h, sched_ptr->ue_db);

  dl_metric->set_log(log_h);
  ul_metric->set_log(log_h);

  // Setup constant PUCCH/PRACH mask
  pucch_mask.resize(cfg->cell.nof_prb);
  if (cfg->nrb_pucch > 0) {
    pucch_mask.fill(0, (uint32_t)cfg->nrb_pucch);
    pucch_mask.fill(cfg->cell.nof_prb - cfg->nrb_pucch, cfg->cell.nof_prb);
  }
  prach_mask.resize(cfg->cell.nof_prb);
  prach_mask.fill(cfg->prach_freq_offset, cfg->prach_freq_offset + 6);

  // Initiate the tti_scheduler for each TTI
  for (tti_sched_result_t& tti_sched : tti_scheds) {
    tti_sched.init(this);
  }
}

void sched::carrier_sched::set_metric(sched::metric_dl* dl_metric_, sched::metric_ul* ul_metric_)
{
  dl_metric = dl_metric_;
  ul_metric = ul_metric_;
}

void sched::carrier_sched::set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)
{
  tti_dl_mask.assign(tti_mask, tti_mask + nof_sfs);
}

sched::carrier_sched::tti_sched_result_t* sched::carrier_sched::generate_tti_result(uint32_t tti_rx)
{
  tti_sched_result_t* tti_sched = get_tti_sched(tti_rx);

  // if it is the first time tti is run, reset vars
  if (tti_rx != tti_sched->get_tti_rx()) {
    uint32_t start_cfi = sched_ptr->sched_params.sched_cfg.nof_ctrl_symbols;
    tti_sched->new_tti(tti_rx, start_cfi);

    // Protects access to pending_rar[], pending_msg3[], pending_sibs[], rlc buffers
    std::lock_guard<std::mutex> lock(sched_mutex);
    pthread_rwlock_rdlock(&sched_ptr->rwlock);

    /* Schedule PHICH */
    generate_phich(tti_sched);

    /* Schedule DL control data */
    if (tti_dl_mask[tti_sched->get_tti_tx_dl() % tti_dl_mask.size()] == 0) {
      /* Schedule Broadcast data (SIB and paging) */
      if (bc_sched_ptr != nullptr) {
        bc_sched_ptr->dl_sched(tti_sched);
      }

      /* Schedule RAR */
      ra_sched_ptr->dl_sched(tti_sched);
    }

    /* Prioritize PDCCH scheduling for DL and UL data in a RoundRobin fashion */
    if ((tti_rx % 2) == 0) {
      alloc_ul_users(tti_sched);
    }

    /* Schedule DL user data */
    alloc_dl_users(tti_sched);

    if ((tti_rx % 2) == 1) {
      alloc_ul_users(tti_sched);
    }

    /* Select the winner DCI allocation combination */
    tti_sched->generate_dcis();

    /* reset PIDs with pending data or blocked */
    for (auto& user : sched_ptr->ue_db) {
      user.second.reset_pending_pids(tti_rx);
    }

    pthread_rwlock_unlock(&sched_ptr->rwlock);
  }

  return tti_sched;
}

void sched::carrier_sched::generate_phich(tti_sched_result_t* tti_sched)
{
  // Allocate user PHICHs
  uint32_t nof_phich_elems = 0;
  for (auto& ue_pair : sched_ptr->ue_db) {
    sched_ue& user = ue_pair.second;
    uint16_t  rnti = ue_pair.first;

    //    user.has_pucch = false; // FIXME: What is this for?

    ul_harq_proc* h = user.get_ul_harq(tti_sched->get_tti_rx());

    /* Indicate PHICH acknowledgment if needed */
    if (h->has_pending_ack()) {
      tti_sched->ul_sched_result.phich[nof_phich_elems].phich =
          h->get_pending_ack() ? ul_sched_phich_t::ACK : ul_sched_phich_t::NACK;
      tti_sched->ul_sched_result.phich[nof_phich_elems].rnti = rnti;
      log_h->debug("SCHED: Allocated PHICH for rnti=0x%x, value=%d\n",
                   rnti,
                   tti_sched->ul_sched_result.phich[nof_phich_elems].phich);
      nof_phich_elems++;
    }
  }
  tti_sched->ul_sched_result.nof_phich_elems = nof_phich_elems;
}

void sched::carrier_sched::alloc_dl_users(sched::carrier_sched::tti_sched_result_t* tti_result)
{
  if (tti_dl_mask[tti_result->get_tti_tx_dl() % tti_dl_mask.size()] != 0) {
    return;
  }

  // NOTE: In case of 6 PRBs, do not transmit if there is going to be a PRACH in the UL to avoid collisions
  if (cfg->cell.nof_prb == 6) {
    uint32_t tti_rx_ack   = TTI_RX_ACK(tti_result->get_tti_rx());
    bool     msg3_enabled = false;
    if (ra_sched_ptr != nullptr and ra_sched_ptr->find_pending_msg3(tti_rx_ack).enabled) {
      msg3_enabled = true;
    }
    if (srslte_prach_tti_opportunity_config_fdd(cfg->prach_config, tti_rx_ack, -1) or msg3_enabled) {
      tti_result->get_dl_mask().fill(0, tti_result->get_dl_mask().size());
    }
  }

  // call DL scheduler metric to fill RB grid
  dl_metric->sched_users(sched_ptr->ue_db, tti_result);
}

int sched::carrier_sched::alloc_ul_users(sched::carrier_sched::tti_sched_result_t* tti_sched)
{
  uint32_t   tti_tx_ul = tti_sched->get_tti_tx_ul();
  prbmask_t& ul_mask   = tti_sched->get_ul_mask();

  /* reserve PRBs for PRACH */
  if (srslte_prach_tti_opportunity_config_fdd(cfg->prach_config, tti_tx_ul, -1)) {
    ul_mask = prach_mask;
    log_h->debug("SCHED: Allocated PRACH RBs. Mask: 0x%s\n", prach_mask.to_hex().c_str());
  }

  /* Allocate Msg3 if there's a pending RAR */
  ra_sched_ptr->ul_sched(tti_sched);

  /* reserve PRBs for PUCCH */
  if (cfg->cell.nof_prb != 6 and (ul_mask & pucch_mask).any()) {
    log_h->error("There was a collision with the PUCCH. current mask=0x%s, pucch_mask=0x%s\n",
                 ul_mask.to_hex().c_str(),
                 pucch_mask.to_hex().c_str());
  }
  ul_mask |= pucch_mask;

  /* Call scheduler for UL data */
  ul_metric->sched_users(sched_ptr->ue_db, tti_sched);

  /* Update pending data counters after this TTI */
  for (auto& user : sched_ptr->ue_db) {
    user.second.get_ul_harq(tti_tx_ul)->reset_pending_data();
  }

  return SRSLTE_SUCCESS;
}

int sched::carrier_sched::dl_rach_info(dl_sched_rar_info_t rar_info)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  return ra_sched_ptr->dl_rach_info(rar_info);
}

} // namespace srsenb
