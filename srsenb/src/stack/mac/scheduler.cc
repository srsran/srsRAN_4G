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

#include <srsenb/hdr/stack/mac/scheduler_ue.h>
#include <string.h>

#include "srsenb/hdr/stack/mac/scheduler.h"
#include "srslte/common/pdu.h"
#include "srslte/srslte.h"

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

namespace srsenb {

/*******************************************************
 *          TTI resource Scheduling Methods
 *******************************************************/

void sched::tti_sched_t::init(sched* parent_)
{
  parent   = parent_;
  log_h    = parent->log_h;
  P        = parent->P;
  sibs_cfg = parent->cfg.sibs;

  pdcch_grid_t pdcch_alloc;
  pdcch_alloc.init(log_h, &parent->regs, parent->common_locations, parent->rar_locations);
  tti_alloc.init(log_h, &parent->cfg, pdcch_alloc);
}

void sched::tti_sched_t::new_tti(uint32_t tti_rx_, uint32_t start_cfi)
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

bool sched::tti_sched_t::is_dl_alloc(sched_ue* user) const
{
  for (const auto& a : data_allocs) {
    if (a.user_ptr == user) {
      return true;
    }
  }
  return false;
}

bool sched::tti_sched_t::is_ul_alloc(sched_ue* user) const
{
  for (const auto& a : ul_data_allocs) {
    if (a.user_ptr == user) {
      return true;
    }
  }
  return false;
}

sched::tti_sched_t::ctrl_code_t sched::tti_sched_t::alloc_dl_ctrl(uint32_t aggr_lvl, uint32_t tbs_bytes, uint16_t rnti)
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
  tti_grid_t::ctrl_alloc_t ret = tti_alloc.alloc_dl_ctrl(aggr_lvl, alloc_type);
  if (not ret.first) {
    return {ret.first, ctrl_alloc};
  }

  // Allocation Successful
  ctrl_alloc.dci_idx   = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  ctrl_alloc.rbg_range = ret.second;
  ctrl_alloc.rnti       = rnti;
  ctrl_alloc.req_bytes  = tbs_bytes;
  ctrl_alloc.alloc_type = alloc_type;

  return {ret.first, ctrl_alloc};
}

alloc_outcome_t sched::tti_sched_t::alloc_bc(uint32_t aggr_lvl, uint32_t sib_idx, uint32_t sib_ntx)
{
  uint32_t                        sib_len = sibs_cfg[sib_idx].len;
  uint32_t                        rv      = get_rvidx(sib_ntx);
  ctrl_code_t                     ret     = alloc_dl_ctrl(aggr_lvl, sib_len, SRSLTE_SIRNTI);
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
  bc_alloc.rv     = rv;
  bc_alloc.sib_idx = sib_idx;
  bc_allocs.push_back(bc_alloc);

  return ret.first;
}

alloc_outcome_t sched::tti_sched_t::alloc_paging(uint32_t aggr_lvl, uint32_t paging_payload)
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

sched::tti_sched_t::rar_code_t
sched::tti_sched_t::alloc_rar(uint32_t aggr_lvl, const dl_sched_rar_t& rar_grant, uint32_t rar_tti, uint32_t buf_rar)
{
  uint16_t rar_sfidx = (uint16_t)((rar_tti + 1) % 10);

  ctrl_code_t ret = alloc_dl_ctrl(aggr_lvl, buf_rar, rar_sfidx);
  if (not ret.first) {
    Warning("SCHED: Could not allocate RAR for L=%d, cause=%s\n", aggr_lvl, ret.first.to_string());
    return {ret.first, NULL};
  }

  // Allocation successful
  rar_alloc_t rar_alloc(ret.second);
  rar_alloc.rar_grant = rar_grant;
  rar_allocs.push_back(rar_alloc);

  return {ret.first, &rar_allocs.back()};
}

alloc_outcome_t sched::tti_sched_t::alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid)
{
  if (is_dl_alloc(user)) {
    log_h->warning("SCHED: Attempt to assign multiple harq pids to the same user rnti=0x%x\n", user->get_rnti());
    return alloc_outcome_t::ERROR;
  }

  // Try to allocate RBGs and DCI
  alloc_outcome_t ret = tti_alloc.alloc_dl_data(user, user_mask);
  if (not ret) {
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

alloc_outcome_t sched::tti_sched_t::alloc_ul(sched_ue*                       user,
                                             ul_harq_proc::ul_alloc_t        alloc,
                                             tti_sched_t::ul_alloc_t::type_t alloc_type,
                                             uint32_t                        mcs)
{
  // Check whether user was already allocated
  if (is_ul_alloc(user)) {
    log_h->warning("SCHED: Attempt to assign multiple ul_harq_proc to the same user rnti=0x%x\n", user->get_rnti());
    return alloc_outcome_t::ERROR;
  }

  // Allocate RBGs and DCI space
  bool            needs_pdcch = alloc_type == ul_alloc_t::ADAPT_RETX or alloc_type == ul_alloc_t::NEWTX;
  alloc_outcome_t ret         = tti_alloc.alloc_ul_data(user, alloc, needs_pdcch);
  if (not ret) {
    return ret;
  }

  ul_alloc_t ul_alloc = {};
  ul_alloc.type     = alloc_type;
  ul_alloc.dci_idx  = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  ul_alloc.user_ptr = user;
  ul_alloc.alloc    = alloc;
  ul_alloc.mcs      = mcs;
  ul_data_allocs.push_back(ul_alloc);

  return alloc_outcome_t::SUCCESS;
}

alloc_outcome_t sched::tti_sched_t::alloc_ul_user(sched_ue* user, ul_harq_proc::ul_alloc_t alloc)
{
  // check whether adaptive/non-adaptive retx/newtx
  tti_sched_t::ul_alloc_t::type_t alloc_type;
  ul_harq_proc*                   h        = user->get_ul_harq(get_tti_tx_ul());
  bool                            has_retx = h->has_pending_retx();
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

alloc_outcome_t sched::tti_sched_t::alloc_ul_msg3(sched_ue* user, ul_harq_proc::ul_alloc_t alloc, uint32_t mcs)
{
  return alloc_ul(user, alloc, ul_alloc_t::MSG3, mcs);
}

void sched::tti_sched_t::set_bc_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
{
  for (const auto& bc_alloc : bc_allocs) {
    sched_interface::dl_sched_bc_t* bc = &dl_sched_result.bc[dl_sched_result.nof_bc_elems];

    // assign NCCE/L
    bc->dci.location = dci_result[bc_alloc.dci_idx]->dci_pos;

    /* Generate DCI format1A */
    prb_range_t prb_range = prb_range_t(bc_alloc.rbg_range, P);
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

void sched::tti_sched_t::set_rar_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
{
  for (const auto& rar_alloc : rar_allocs) {
    sched_interface::dl_sched_rar_t* rar = &dl_sched_result.rar[dl_sched_result.nof_rar_elems];

    // Assign NCCE/L
    rar->dci.location = dci_result[rar_alloc.dci_idx]->dci_pos;

    /* Generate DCI format1A */
    prb_range_t prb_range = prb_range_t(rar_alloc.rbg_range, P);
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
      const auto& msg3_grant    = rar->msg3_grant[i];
      uint32_t    pending_tti   = (get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY) % 10;
      uint16_t    expected_rnti = parent->pending_msg3[pending_tti].rnti; // FIXME
      log_h->info("SCHED: RAR, ra_id=%d, rnti=0x%x, rarnti_idx=%d, rbgs=(%d,%d), dci=(%d,%d), rar_grant_rba=%d, "
                  "rar_grant_mcs=%d\n",
                  msg3_grant.ra_id,
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

void sched::tti_sched_t::set_dl_data_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
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

    int  tbs      = 0;
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

void sched::tti_sched_t::set_ul_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
{
  /* Set UL data DCI locs and format */
  for (const auto& ul_alloc : ul_data_allocs) {
    sched_interface::ul_sched_data_t* pusch = &ul_sched_result.pusch[ul_sched_result.nof_dci_elems];

    sched_ue*     user = ul_alloc.user_ptr;

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

void sched::tti_sched_t::generate_dcis()
{
  /* Pick one of the possible DCI masks */
  pdcch_grid_t::alloc_result_t dci_result;
  //  tti_alloc.get_pdcch_grid().print_result();
  tti_alloc.get_pdcch_grid().get_allocs(&dci_result, &pdcch_mask);

  /* Register final CFI */
  dl_sched_result.cfi = tti_alloc.get_pdcch_grid().get_cfi();

  /* Generate DCI formats and fill sched_result structs */
  set_bc_sched_result(dci_result);

  set_rar_sched_result(dci_result);

  set_dl_data_sched_result(dci_result);

  set_ul_sched_result(dci_result);
}

uint32_t sched::tti_sched_t::get_nof_ctrl_symbols() const
{
  return tti_alloc.get_cfi() + (parent->cfg.cell.nof_prb <= 10) ? 1 : 0;
}

int sched::tti_sched_t::generate_format1a(
    uint32_t rb_start, uint32_t l_crb, uint32_t tbs_bytes, uint32_t rv, uint16_t rnti, srslte_dci_dl_t* dci)
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
    } else if (srslte_ra_tbs_from_idx(i, 3) >= tbs) {
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
  dci->type2_alloc.riv  = srslte_ra_type2_to_riv(l_crb, rb_start, parent->cfg.cell.nof_prb);
  dci->pid              = 0;
  dci->tb[0].mcs_idx    = mcs;
  dci->tb[0].rv         = rv;
  dci->format           = SRSLTE_DCI_FORMAT1A;
  dci->rnti             = rnti;

  return tbs;
}

/*******************************************************
 * 
 * Initialization and sched configuration functions 
 * 
 *******************************************************/
sched::sched() : bc_aggr_level(0), rar_aggr_level(0), P(0), si_n_rbg(0), rar_n_rbg(0), nof_rbg(0)
{
  current_tti = 0;
  log_h = NULL;
  dl_metric = NULL;
  ul_metric = NULL;
  rrc = NULL;

  bzero(&cfg, sizeof(cfg));
  bzero(&regs, sizeof(regs));
  bzero(&sched_cfg, sizeof(sched_cfg));
  bzero(&common_locations, sizeof(common_locations));
  bzero(&pdsch_re, sizeof(pdsch_re));
  tti_dl_mask.resize(10, 0);

  for (int i = 0; i < 3; i++) {
    bzero(rar_locations[i], sizeof(sched_ue::sched_dci_cce_t) * 10);
  }

  pthread_rwlock_init(&rwlock, NULL);

  reset();
}

sched::~sched()
{
  srslte_regs_free(&regs);
  pthread_rwlock_wrlock(&rwlock);
  pthread_rwlock_unlock(&rwlock);
  pthread_rwlock_destroy(&rwlock);
}

void sched::init(rrc_interface_mac* rrc_, srslte::log* log)
{
  sched_cfg.pdsch_max_mcs    = 28;
  sched_cfg.pdsch_mcs        = -1;
  sched_cfg.pusch_max_mcs    = 28;
  sched_cfg.pusch_mcs        = -1;
  sched_cfg.nof_ctrl_symbols = 3;
  log_h                      = log;
  rrc                        = rrc_;
  reset();
}

int sched::reset()
{
  bzero(pending_msg3, sizeof(pending_msg3_t) * 10);
  bzero(pending_rar, sizeof(sched_rar_t) * SCHED_MAX_PENDING_RAR);
  bzero(pending_sibs, sizeof(sched_sib_t) * MAX_SIBS);
  configured = false;
  pthread_rwlock_wrlock(&rwlock);
  ue_db.clear();
  pthread_rwlock_unlock(&rwlock);
  return 0;
}

void sched::set_sched_cfg(sched_interface::sched_args_t* sched_cfg_)
{
  if (sched_cfg_) {
    sched_cfg = *sched_cfg_;
  }
}

void sched::set_metric(sched::metric_dl* dl_metric_, sched::metric_ul* ul_metric_)
{
  dl_metric = dl_metric_;
  ul_metric = ul_metric_;
  dl_metric->set_log(log_h);
  ul_metric->set_log(log_h);
}

int sched::cell_cfg(sched_interface::cell_cfg_t* cell_cfg)
{
  // Basic cell config checks
  if (cell_cfg->si_window_ms == 0) {
    Error("SCHED: Invalid si-window length 0 ms\n");
    return -1;
  }

  cfg = *cell_cfg;

  // Get DCI locations
  if (srslte_regs_init(&regs, cfg.cell)) {
    Error("Getting DCI locations\n");
    return SRSLTE_ERROR;
  }

  P = srslte_ra_type0_P(cfg.cell.nof_prb);
  si_n_rbg  = srslte::ceil_div(4, P);
  rar_n_rbg = srslte::ceil_div(3, P);
  nof_rbg   = srslte::ceil_div(cfg.cell.nof_prb, P);
  pucch_mask.resize(cfg.cell.nof_prb);
  if (cfg.nrb_pucch > 0) {
    pucch_mask.fill(0, (uint32_t)cfg.nrb_pucch);
    pucch_mask.fill(cfg.cell.nof_prb - cfg.nrb_pucch, cfg.cell.nof_prb);
  }
  prach_mask.resize(cfg.cell.nof_prb);
  prach_mask.fill(cfg.prach_freq_offset, cfg.prach_freq_offset + 6);

  // Compute Common locations for DCI for each CFI
  for (uint32_t cfi = 0; cfi < 3; cfi++) {
    generate_cce_location(&regs, &common_locations[cfi], cfi + 1);
  }

  // Compute UE locations for RA-RNTI
  for (uint32_t cfi = 0; cfi < 3; cfi++) {
    for (uint32_t sf_idx = 0; sf_idx < 10; sf_idx++) {
      generate_cce_location(&regs, &rar_locations[cfi][sf_idx], cfi + 1, sf_idx);
    }
  }

  // Initiate the tti_scheduler for each TTI
  for (uint32_t i = 0; i < nof_sched_ttis; ++i) {
    pdcch_grid_t pdcch_alloc;
    pdcch_alloc.init(log_h, &regs, common_locations, rar_locations);
    tti_scheds[i].init(this);
  }
  configured = true;

  // PRACH has to fit within the PUSCH space
  bool invalid_prach = cfg.cell.nof_prb == 6 and (cfg.prach_freq_offset + 6 > cfg.cell.nof_prb);
  invalid_prach |= cfg.cell.nof_prb > 6 and ((cfg.prach_freq_offset + 6) > (cfg.cell.nof_prb - cfg.nrb_pucch) or
                                             (int) cfg.prach_freq_offset < cfg.nrb_pucch);
  if (invalid_prach) {
    log_h->error("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n", cfg.prach_freq_offset);
    log_h->console("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n",
                   cfg.prach_freq_offset);
    return -1;
  }

  if (common_locations[sched_cfg.nof_ctrl_symbols - 1].nof_loc[2] == 0) {
    Error("SCHED: Current cfi=%d is not valid for broadcast (check scheduler.nof_ctrl_symbols in conf file).\n",
          sched_cfg.nof_ctrl_symbols);
    log_h->console(
        "SCHED: Current cfi=%d is not valid for broadcast (check scheduler.nof_ctrl_symbols in conf file).\n",
        sched_cfg.nof_ctrl_symbols);
  }

  return 0;
}


/*******************************************************
 * 
 * FAPI-like main sched interface. Wrappers to UE object
 * 
 *******************************************************/

int sched::ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* ue_cfg)
{
  // Add or config user
  pthread_rwlock_rdlock(&rwlock);
  ue_db[rnti].set_cfg(rnti, ue_cfg, &cfg, &regs, log_h);
  ue_db[rnti].set_max_mcs(sched_cfg.pusch_max_mcs, sched_cfg.pdsch_max_mcs);
  ue_db[rnti].set_fixed_mcs(sched_cfg.pusch_mcs, sched_cfg.pdsch_mcs);
  pthread_rwlock_unlock(&rwlock);

  return 0;
}

int sched::ue_rem(uint16_t rnti)
{
  int ret = 0;
  pthread_rwlock_wrlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db.erase(rnti);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

bool sched::ue_exists(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  bool ret = (ue_db.count(rnti) == 1);
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

void sched::phy_config_enabled(uint16_t rnti, bool enabled)
{
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].phy_config_enabled(current_tti, enabled);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

int sched::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_bearer_cfg(lc_id, cfg);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].rem_bearer(lc_id);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

uint32_t sched::get_dl_buffer(uint16_t rnti)
{
  uint32_t ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ret = ue_db[rnti].get_pending_dl_new_data(current_tti);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

uint32_t sched::get_ul_buffer(uint16_t rnti)
{
  uint32_t ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ret = ue_db[rnti].get_pending_ul_new_data(current_tti);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].dl_buffer_state(lc_id, tx_queue, retx_queue);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].mac_buffer_state(ce_code);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_ant_info(uint16_t rnti, asn1::rrc::phys_cfg_ded_s::ant_info_c_* dl_ant_info)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_dl_ant_info(dl_ant_info);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ret = ue_db[rnti].set_ack_info(tti, tb_idx, ack);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_crc_info(uint32_t tti, uint16_t rnti, bool crc)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_ul_crc(tti, crc);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_dl_ri(tti, cqi_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_dl_pmi(tti, pmi_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_dl_cqi(tti, cqi_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_rach_info(uint32_t tti, uint32_t ra_id, uint16_t rnti, uint32_t estimated_size)
{
  for (int i = 0; i < SCHED_MAX_PENDING_RAR; i++) {
    if (!pending_rar[i].buf_rar) {
      pending_rar[i].ra_id   = ra_id;
      pending_rar[i].rnti    = rnti;
      pending_rar[i].rar_tti = tti;
      pending_rar[i].buf_rar = estimated_size;
      return 0;
    }
  }
  Warning("SCHED: New RACH discarted because maximum number of pending RAR exceeded (%d)\n", SCHED_MAX_PENDING_RAR);
  return -1;
}

int sched::ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi, uint32_t ul_ch_code)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_ul_cqi(tti, cqi, ul_ch_code);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr, bool set_value)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].ul_buffer_state(lcid, bsr, set_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_recv_len(uint16_t rnti, uint32_t lcid, uint32_t len)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].ul_recv_len(lcid, len);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_phr(uint16_t rnti, int phr)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].ul_phr(phr);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_sr_info(uint32_t tti, uint16_t rnti)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_sr();
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

void sched::set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)
{
  tti_dl_mask.assign(tti_mask, tti_mask + nof_sfs);
}

void sched::tpc_inc(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].tpc_inc();
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

void sched::tpc_dec(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].tpc_dec();
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

/*******************************************************
 * 
 * Main sched functions 
 * 
 *******************************************************/

sched::tti_sched_t* sched::new_tti(uint32_t tti_rx)
{
  tti_sched_t* tti_sched = get_tti_sched(tti_rx);

  // if it is the first time tti is run, reset vars
  if (tti_rx != tti_sched->get_tti_rx()) {
    uint32_t start_cfi = sched_cfg.nof_ctrl_symbols - ((cfg.cell.nof_prb >= 10) ? 0 : 1);
    tti_sched->new_tti(tti_rx, start_cfi);

    // Protects access to pending_rar[], pending_msg3[], pending_sibs[], rlc buffers
    std::lock_guard<std::mutex> lock(sched_mutex);
    pthread_rwlock_rdlock(&rwlock);

    /* Schedule PHICH */
    generate_phich(tti_sched);

    /* Schedule DL */
    if (tti_dl_mask[tti_sched->get_tti_tx_dl() % tti_dl_mask.size()] == 0) {
      generate_dl_sched(tti_sched);
    }

    /* Schedule UL */
    generate_ul_sched(tti_sched);

    /* Generate DCI */
    tti_sched->generate_dcis();

    /* reset PIDs with pending data or blocked */
    for (auto& user : ue_db) {
      user.second.reset_pending_pids(tti_rx);
    }

    pthread_rwlock_unlock(&rwlock);
  }

  return tti_sched;
}

// Schedules Broadcast messages (SIB)
void sched::dl_sched_bc(tti_sched_t* tti_sched)
{
  /* Activate/Deactivate SI windows */
  for (int i = 0; i < MAX_SIBS; i++) {
    // There is SIB data
    if (cfg.sibs[i].len == 0) {
      continue;
    }

    if (!pending_sibs[i].is_in_window) {
      uint32_t sf = 5;
      uint32_t x  = 0;
      if (i > 0) {
        x  = (i - 1) * cfg.si_window_ms;
        sf = x % 10;
      }
      if ((tti_sched->get_sfn() % (cfg.sibs[i].period_rf)) == x / 10 && tti_sched->get_sf_idx() == sf) {
        pending_sibs[i].is_in_window = true;
        pending_sibs[i].window_start = tti_sched->get_tti_tx_dl();
        pending_sibs[i].n_tx         = 0;
      }
    } else {
      if (i > 0) {
        if (srslte_tti_interval(tti_sched->get_tti_tx_dl(), pending_sibs[i].window_start) > cfg.si_window_ms) {
          // the si window has passed
          pending_sibs[i].is_in_window = false;
          pending_sibs[i].window_start = 0;
        }
      } else {
        // SIB1 is always in window
        if (pending_sibs[0].n_tx == 4) {
          pending_sibs[0].n_tx = 0;
        }
      }
    }
  }

  /* Allocate DCIs and RBGs for each SIB */
  for (int i = 0; i < MAX_SIBS; i++) {
    if (cfg.sibs[i].len && pending_sibs[i].is_in_window && pending_sibs[i].n_tx < 4) {
      uint32_t nof_tx = (i > 0) ? SRSLTE_MIN(CEILFRAC(cfg.si_window_ms, 10), 4) : 4;
      uint32_t n_sf   = (tti_sched->get_tti_tx_dl() - pending_sibs[i].window_start);

      // Check if there is any SIB to tx
      bool sib1_flag = i == 0 and (tti_sched->get_sfn() % 2) == 0 and tti_sched->get_sf_idx() == 5;
      bool other_sibs_flag =
          i > 0 and n_sf >= (cfg.si_window_ms / nof_tx) * pending_sibs[i].n_tx and tti_sched->get_sf_idx() == 9;
      if (!sib1_flag and !other_sibs_flag) {
        continue;
      }

      // Schedule SIB
      tti_sched->alloc_bc(bc_aggr_level, i, pending_sibs[i].n_tx);
      pending_sibs[i].n_tx++;
    }
  }

  /* Allocate DCIs and RBGs for paging */
  if (rrc) {
    uint32_t paging_payload = 0;
    if (rrc->is_paging_opportunity(current_tti, &paging_payload) and paging_payload) {
      tti_sched->alloc_paging(bc_aggr_level, paging_payload);
    }
  }
}

bool is_in_tti_interval(uint32_t tti, uint32_t tti1, uint32_t tti2)
{
  tti %= 10240;
  tti1 %= 10240;
  tti2 %= 10240;
  if (tti1 <= tti2) {
    return tti >= tti1 and tti <= tti2;
  }
  return tti >= tti1 or tti <= tti2;
}

// Schedules RAR
void sched::dl_sched_rar(tti_sched_t* tti_sched)
{
  for (uint32_t i = 0; i < SCHED_MAX_PENDING_RAR; i++) {
    // check if the RAR is inactive or was already scheduled
    if (pending_rar[i].buf_rar == 0) {
      continue;
    }
    // Check if we are still within the RAR window, otherwise discard it
    if (not is_in_tti_interval(tti_sched->get_tti_tx_dl(),
                               pending_rar[i].rar_tti + 3,
                               pending_rar[i].rar_tti + 3 + cfg.prach_rar_window)) {
      log_h->console("SCHED: Could not transmit RAR within the window (RA TTI=%d, Window=%d, Now=%d)\n",
                     pending_rar[i].rar_tti,
                     cfg.prach_rar_window,
                     current_tti);
      log_h->error("SCHED: Could not transmit RAR within the window (RA TTI=%d, Window=%d, Now=%d)\n",
                   pending_rar[i].rar_tti,
                   cfg.prach_rar_window,
                   current_tti);
      pending_rar[i].buf_rar = 0;
      pending_rar[i].rar_tti = 0;
      continue;
    }

    /* Group pending RARs with same transmission TTI */
    uint32_t       tti     = pending_rar[i].rar_tti;
    uint32_t       buf_rar = 0;
    dl_sched_rar_t rar_grant;
    uint32_t       L_prb = 3;
    uint32_t       n_prb = 2;
    bzero(&rar_grant, sizeof(rar_grant));
    uint32_t rba = srslte_ra_type2_to_riv(L_prb, n_prb, cfg.cell.nof_prb);
    for (uint32_t j = i; j < SCHED_MAX_PENDING_RAR; ++j) {
      if (pending_rar[j].rar_tti != pending_rar[i].rar_tti) {
        continue;
      }
      if (rar_grant.nof_grants > 0) {
        log_h->warning("Only 1 RA is responded at a time. Found %d for TTI=%d\n", rar_grant.nof_grants + 1, tti);
        continue;
      }

      dl_sched_rar_grant_t* grant = &rar_grant.msg3_grant[rar_grant.nof_grants];
      grant->grant.tpc_pusch      = 3;
      grant->grant.trunc_mcs      = 0;
      grant->grant.rba            = rba;
      grant->ra_id                = pending_rar[j].ra_id;
      buf_rar += pending_rar[j].buf_rar;
      rar_grant.nof_grants++;
    }

    // Try to schedule DCI + RBGs for RAR Grant
    tti_sched_t::rar_code_t ret = tti_sched->alloc_rar(rar_aggr_level, rar_grant, pending_rar[i].rar_tti, buf_rar);
    if (not ret.first) {
      continue;
    }

    // Schedule Msg3
    uint32_t pending_tti              = (tti_sched->get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY) % 10;
    pending_msg3[pending_tti].enabled = true;
    pending_msg3[pending_tti].rnti    = pending_rar[i].rnti; // FIXME
    pending_msg3[pending_tti].L       = L_prb;
    pending_msg3[pending_tti].n_prb   = n_prb;
    dl_sched_rar_grant_t* last_msg3   = &rar_grant.msg3_grant[rar_grant.nof_grants - 1];
    pending_msg3[pending_tti].mcs     = last_msg3->grant.trunc_mcs;

    // Reset allocated RARs
    for (uint32_t j = i; j < SCHED_MAX_PENDING_RAR; ++j) {
      if (pending_rar[j].rar_tti == pending_rar[i].rar_tti) {
        pending_rar[j].buf_rar = 0;
        pending_rar[j].rar_tti = 0;
      }
    }
  }
}

void sched::dl_sched_data(tti_sched_t* tti_sched)
{
  // NOTE: In case of 6 PRBs, do not transmit if there is going to be a PRACH in the UL to avoid collisions
  uint32_t tti_rx_ack  = TTI_RX_ACK(tti_sched->get_tti_rx());
  uint32_t pending_tti = tti_rx_ack % 10;
  if (cfg.cell.nof_prb == 6 and (srslte_prach_tti_opportunity_config_fdd(cfg.prach_config, tti_rx_ack, -1) or
                                 pending_msg3[pending_tti].enabled)) {
    tti_sched->get_dl_mask().fill(0, tti_sched->get_dl_mask().size());
  }

  // call scheduler metric to fill RB grid
  dl_metric->sched_users(ue_db, tti_sched);
}

// Compute DL scheduler result
int sched::generate_dl_sched(tti_sched_t* tti_sched)
{
  /* Initialize variables */
  current_tti    = tti_sched->get_tti_tx_dl();
  bc_aggr_level  = 2;
  rar_aggr_level = 2;

  /* Schedule Broadcast data */
  dl_sched_bc(tti_sched);

  /* Schedule RAR */
  dl_sched_rar(tti_sched);

  /* Schedule pending RLC data */
  dl_sched_data(tti_sched);

  return 0;
}

void sched::generate_phich(tti_sched_t* tti_sched)
{
  // Allocate user PHICHs
  uint32_t nof_phich_elems = 0;
  for (auto& ue_pair : ue_db) {
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

void sched::ul_sched_msg3(tti_sched_t* tti_sched)
{
  uint32_t pending_tti = tti_sched->get_tti_tx_ul() % 10;
  if (not pending_msg3[pending_tti].enabled) {
    return;
  }

  uint16_t rnti = pending_msg3[pending_tti].rnti;
  if (not ue_db.count(rnti)) {
    log_h->warning("SCHED: Msg3 allocated for user rnti=0x%x that no longer exists\n", rnti);
    return;
  }
  sched_ue* user = &ue_db[rnti];

  /* Allocate RBGs and HARQ for Msg3 */
  ul_harq_proc::ul_alloc_t msg3 = {pending_msg3[pending_tti].n_prb, pending_msg3[pending_tti].L};
  if (not tti_sched->alloc_ul_msg3(user, msg3, pending_msg3[pending_tti].mcs)) {
    log_h->warning("SCHED: Could not allocate msg3 within (%d,%d)\n", msg3.RB_start, msg3.RB_start + msg3.L);
    return;
  }
  pending_msg3[pending_tti].enabled = false;
}

// Compute UL scheduler result
int sched::generate_ul_sched(tti_sched_t* tti_sched)
{
  /* Initialize variables */
  current_tti                = tti_sched->get_tti_tx_ul();
  prbmask_t& ul_mask         = tti_sched->get_ul_mask();

  // reserve PRBs for PRACH
  if (srslte_prach_tti_opportunity_config_fdd(cfg.prach_config, tti_sched->get_tti_tx_ul(), -1)) {
    ul_mask = prach_mask;
    log_h->debug("SCHED: Allocated PRACH RBs. Mask: 0x%s\n", prach_mask.to_hex().c_str());
  }

  // Update available allocation if there's a pending RAR
  ul_sched_msg3(tti_sched);

  // reserve PRBs for PUCCH
  if (cfg.cell.nof_prb != 6 and (ul_mask & pucch_mask).any()) {
    log_h->error("There was a collision with the PUCCH. current mask=0x%s, pucch_mask=0x%s\n",
                 ul_mask.to_hex().c_str(),
                 pucch_mask.to_hex().c_str());
  }
  ul_mask |= pucch_mask;

  // Call scheduler for UL data
  ul_metric->sched_users(ue_db, tti_sched);

  // Update pending data counters after this TTI
  for (auto& user : ue_db) {
    user.second.get_ul_harq(tti_sched->get_tti_tx_ul())->reset_pending_data();
  }

  return SRSLTE_SUCCESS;
}

// Downlink Scheduler API
int sched::dl_sched(uint32_t tti, sched_interface::dl_sched_res_t* sched_result)
{
  if (!configured) {
    return 0;
  }

  uint32_t tti_rx = TTI_SUB(tti, TX_DELAY);

  // Compute scheduling Result for tti_rx
  tti_sched_t* tti_sched = new_tti(tti_rx);

  // copy result
  *sched_result = tti_sched->dl_sched_result;

  return 0;
}

// Uplink Scheduler API
int sched::ul_sched(uint32_t tti, srsenb::sched_interface::ul_sched_res_t* sched_result)
{
  if (!configured) {
    return 0;
  }

  // Compute scheduling Result for tti_rx
  uint32_t     tti_rx    = (tti + 10240 - 2 * FDD_HARQ_DELAY_MS) % 10240;
  tti_sched_t* tti_sched = new_tti(tti_rx);

  // Copy results
  *sched_result = tti_sched->ul_sched_result;

  return SRSLTE_SUCCESS;
}

/*******************************************************
 *
 * Helper functions
 *
 *******************************************************/

void sched::generate_cce_location(
    srslte_regs_t* regs_, sched_ue::sched_dci_cce_t* location, uint32_t cfi, uint32_t sf_idx, uint16_t rnti)
{
  bzero(location, sizeof(sched_ue::sched_dci_cce_t));

  srslte_dci_location_t loc[64];
  uint32_t              nloc = 0;
  if (rnti == 0) {
    nloc = srslte_pdcch_common_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), loc, 64);
  } else {
    nloc = srslte_pdcch_ue_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), loc, 64, sf_idx, rnti);
  }

  // Save to different format
  for (uint32_t i = 0; i < nloc; i++) {
    uint32_t l                                   = loc[i].L;
    location->cce_start[l][location->nof_loc[l]] = loc[i].ncce;
    location->nof_loc[l]++;
  }
}

}


