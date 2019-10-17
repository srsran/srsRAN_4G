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
#include "srsenb/hdr/stack/mac/scheduler_ctrl.h"
#include "srslte/common/pdu.h"
#include "srslte/srslte.h"

#define Error(fmt, ...) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->debug(fmt, ##__VA_ARGS__)

namespace srsenb {

namespace sched_utils {

uint32_t tti_subtract(uint32_t tti1, uint32_t tti2)
{
  return (tti1 + 10240 - tti2) % 10240;
}

uint32_t max_tti(uint32_t tti1, uint32_t tti2)
{
  return ((tti1 - tti2) > 10240 / 2) ? SRSLTE_MIN(tti1, tti2) : SRSLTE_MAX(tti1, tti2);
}

} // namespace sched_utils

/*******************************************************
 *          TTI resource Scheduling Methods
 *******************************************************/

void sched::tti_sched_result_t::init(sched* parent_)
{
  parent   = parent_;
  log_h    = parent->log_h;
  P        = parent->P;
  sibs_cfg = parent->cfg.sibs;

  pdcch_grid_t pdcch_alloc;
  pdcch_alloc.init(log_h, &parent->regs, parent->common_locations, parent->rar_locations);
  tti_alloc.init(log_h, &parent->cfg, pdcch_alloc);
}

void sched::tti_sched_result_t::new_tti(uint32_t tti_rx_, uint32_t start_cfi)
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

bool sched::tti_sched_result_t::is_dl_alloc(sched_ue* user) const
{
  for (const auto& a : data_allocs) {
    if (a.user_ptr == user) {
      return true;
    }
  }
  return false;
}

bool sched::tti_sched_result_t::is_ul_alloc(sched_ue* user) const
{
  for (const auto& a : ul_data_allocs) {
    if (a.user_ptr == user) {
      return true;
    }
  }
  return false;
}

sched::tti_sched_result_t::ctrl_code_t
sched::tti_sched_result_t::alloc_dl_ctrl(uint32_t aggr_lvl, uint32_t tbs_bytes, uint16_t rnti)
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

alloc_outcome_t sched::tti_sched_result_t::alloc_bc(uint32_t aggr_lvl, uint32_t sib_idx, uint32_t sib_ntx)
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

alloc_outcome_t sched::tti_sched_result_t::alloc_paging(uint32_t aggr_lvl, uint32_t paging_payload)
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

sched::tti_sched_result_t::rar_code_t sched::tti_sched_result_t::alloc_rar(uint32_t              aggr_lvl,
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

alloc_outcome_t sched::tti_sched_result_t::alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid)
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

alloc_outcome_t sched::tti_sched_result_t::alloc_ul(sched_ue*                              user,
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

alloc_outcome_t sched::tti_sched_result_t::alloc_ul_user(sched_ue* user, ul_harq_proc::ul_alloc_t alloc)
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

alloc_outcome_t sched::tti_sched_result_t::alloc_ul_msg3(sched_ue* user, ul_harq_proc::ul_alloc_t alloc, uint32_t mcs)
{
  return alloc_ul(user, alloc, ul_alloc_t::MSG3, mcs);
}

void sched::tti_sched_result_t::set_bc_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
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

void sched::tti_sched_result_t::set_rar_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
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
      log_h->warning("SCHED: Error RAR, ra-rnti=%d, rbgs=(%d,%d), dci=(%d,%d)\n",
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
      uint16_t    expected_rnti = parent->carrier_schedulers[0].ra_sched->find_pending_msg3(get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY).rnti;
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

void sched::tti_sched_result_t::set_dl_data_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
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

void sched::tti_sched_result_t::set_ul_sched_result(const pdcch_grid_t::alloc_result_t& dci_result)
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

void sched::tti_sched_result_t::generate_dcis()
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

uint32_t sched::tti_sched_result_t::get_nof_ctrl_symbols() const
{
  return tti_alloc.get_cfi() + ((parent->cfg.cell.nof_prb <= 10) ? 1 : 0);
}

int sched::tti_sched_result_t::generate_format1a(
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
sched::sched() : P(0), nof_rbg(0)
{
  current_tti = 0;
  log_h       = nullptr;
  rrc         = nullptr;

  bzero(&cfg, sizeof(cfg));
  bzero(&regs, sizeof(regs));
  bzero(&sched_cfg, sizeof(sched_cfg));
  bzero(&common_locations, sizeof(common_locations));
  bzero(&pdsch_re, sizeof(pdsch_re));

  for (int i = 0; i < 3; i++) {
    bzero(rar_locations[i], sizeof(sched_ue::sched_dci_cce_t) * 10);
  }

  pthread_rwlock_init(&rwlock, nullptr);

  // Initialize Independent carrier schedulers
  carrier_schedulers.emplace_back(this);

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
  sched_cfg.max_aggr_level   = 3;
  log_h                      = log;
  rrc                        = rrc_;

  for (carrier_sched& c : carrier_schedulers) {
    c.init();
  }
  reset();
}

int sched::reset()
{
  configured = false;
  for (carrier_sched& c : carrier_schedulers) {
    c.reset();
  }
  pthread_rwlock_wrlock(&rwlock);
  ue_db.clear();
  pthread_rwlock_unlock(&rwlock);
  return 0;
}

void sched::set_sched_cfg(sched_interface::sched_args_t* sched_cfg_)
{
  if (sched_cfg_ != nullptr) {
    sched_cfg = *sched_cfg_;
  }
}

void sched::set_metric(sched::metric_dl* dl_metric_, sched::metric_ul* ul_metric_)
{
  for (carrier_sched& c : carrier_schedulers) {
    c.set_metric(dl_metric_, ul_metric_);
  }
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
  if (srslte_regs_init(&regs, cfg.cell) != LIBLTE_SUCCESS) {
    Error("Getting DCI locations\n");
    return SRSLTE_ERROR;
  }

  P       = srslte_ra_type0_P(cfg.cell.nof_prb);
  nof_rbg = srslte::ceil_div(cfg.cell.nof_prb, P);
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
  for (carrier_sched& c : carrier_schedulers) {
    c.cell_cfg();
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
  ue_db[rnti].set_max_mcs(sched_cfg.pusch_max_mcs, sched_cfg.pdsch_max_mcs, sched_cfg.max_aggr_level);
  ue_db[rnti].set_fixed_mcs(sched_cfg.pusch_mcs, sched_cfg.pdsch_mcs);
  pthread_rwlock_unlock(&rwlock);

  return 0;
}

int sched::ue_rem(uint16_t rnti)
{
  int ret = 0;
  pthread_rwlock_wrlock(&rwlock);
  if (ue_db.count(rnti) > 0) {
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
  return ue_db_access(rnti, [](sched_ue& ue) {}) >= 0;
}

void sched::ue_needs_ta_cmd(uint16_t rnti, uint32_t nof_ta_cmd) {
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_needs_ta_cmd(nof_ta_cmd);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

void sched::phy_config_enabled(uint16_t rnti, bool enabled)
{
  // FIXME: Check if correct use of current_tti
  ue_db_access(rnti, [this, enabled](sched_ue& ue) { ue.phy_config_enabled(current_tti, enabled); });
}

int sched::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg_)
{
  return ue_db_access(rnti, [lc_id, cfg_](sched_ue& ue) { ue.set_bearer_cfg(lc_id, cfg_); });
}

int sched::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  return ue_db_access(rnti, [lc_id](sched_ue& ue) { ue.rem_bearer(lc_id); });
}

uint32_t sched::get_dl_buffer(uint16_t rnti)
{
  // FIXME: Check if correct use of current_tti
  uint32_t ret = 0;
  ue_db_access(rnti, [this, &ret](sched_ue& ue) { ret = ue.get_pending_dl_new_data(current_tti); });
  return ret;
}

uint32_t sched::get_ul_buffer(uint16_t rnti)
{
  // FIXME: Check if correct use of current_tti
  uint32_t ret = 0;
  ue_db_access(rnti, [this, &ret](sched_ue& ue) { ret = ue.get_pending_ul_new_data(current_tti); });
  return ret;
}

int sched::dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  return ue_db_access(rnti,
                      [lc_id, tx_queue, retx_queue](sched_ue& ue) { ue.dl_buffer_state(lc_id, tx_queue, retx_queue); });
}

int sched::dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code)
{
  return ue_db_access(rnti, [ce_code](sched_ue& ue) { ue.mac_buffer_state(ce_code); });
}

int sched::dl_ant_info(uint16_t rnti, asn1::rrc::phys_cfg_ded_s::ant_info_c_* dl_ant_info)
{
  return ue_db_access(rnti, [dl_ant_info](sched_ue& ue) { ue.set_dl_ant_info(dl_ant_info); });
}

int sched::dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack)
{
  int ret = -1;
  ue_db_access(rnti, [tti, tb_idx, ack, &ret](sched_ue& ue) { ret = ue.set_ack_info(tti, tb_idx, ack); });
  return ret;
}

int sched::ul_crc_info(uint32_t tti, uint16_t rnti, bool crc)
{
  return ue_db_access(rnti, [tti, crc](sched_ue& ue) { ue.set_ul_crc(tti, crc); });
}

int sched::dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t ri_value)
{
  return ue_db_access(rnti, [tti, ri_value](sched_ue& ue) { ue.set_dl_ri(tti, ri_value); });
}

int sched::dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value)
{
  return ue_db_access(rnti, [tti, pmi_value](sched_ue& ue) { ue.set_dl_pmi(tti, pmi_value); });
}

int sched::dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value)
{
  return ue_db_access(rnti, [tti, cqi_value](sched_ue& ue) { ue.set_dl_cqi(tti, cqi_value); });
}

int sched::dl_rach_info(dl_sched_rar_info_t rar_info)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  return carrier_schedulers[0].ra_sched->dl_rach_info(rar_info);
}

int sched::ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi, uint32_t ul_ch_code)
{
  return ue_db_access(rnti, [tti, cqi, ul_ch_code](sched_ue& ue) { ue.set_ul_cqi(tti, cqi, ul_ch_code); });
}

int sched::ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr, bool set_value)
{
  return ue_db_access(rnti, [lcid, bsr, set_value](sched_ue& ue) { ue.ul_buffer_state(lcid, bsr, set_value); });
}

int sched::ul_recv_len(uint16_t rnti, uint32_t lcid, uint32_t len)
{
  return ue_db_access(rnti, [lcid, len](sched_ue& ue) { ue.ul_recv_len(lcid, len); });
}

int sched::ul_phr(uint16_t rnti, int phr)
{
  return ue_db_access(rnti, [phr](sched_ue& ue) { ue.ul_phr(phr); });
}

int sched::ul_sr_info(uint32_t tti, uint16_t rnti)
{
  return ue_db_access(rnti, [](sched_ue& ue) { ue.set_sr(); });
}

void sched::set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)
{
  carrier_schedulers[0].set_dl_tti_mask(tti_mask, nof_sfs);
}

void sched::tpc_inc(uint16_t rnti)
{
  ue_db_access(rnti, [](sched_ue& ue) { ue.tpc_inc(); });
}

void sched::tpc_dec(uint16_t rnti)
{
  ue_db_access(rnti, [](sched_ue& ue) { ue.tpc_dec(); });
}

/*******************************************************
 *
 * Main sched functions
 *
 *******************************************************/

// Downlink Scheduler API
int sched::dl_sched(uint32_t tti, sched_interface::dl_sched_res_t* sched_result)
{
  if (!configured) {
    return 0;
  }

  uint32_t tti_rx = sched_utils::tti_subtract(tti, TX_DELAY);
  current_tti     = sched_utils::max_tti(current_tti, tti_rx);

  // Compute scheduling Result for tti_rx
  tti_sched_result_t* tti_sched = carrier_schedulers[0].generate_tti_result(tti_rx);

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
  uint32_t            tti_rx    = sched_utils::tti_subtract(tti, 2 * FDD_HARQ_DELAY_MS);
  tti_sched_result_t* tti_sched = carrier_schedulers[0].generate_tti_result(tti_rx);

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

// Common way to access ue_db elements in a read locking way
template <typename Func>
int sched::ue_db_access(uint16_t rnti, Func f)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  auto it = ue_db.find(rnti);
  if (it != ue_db.end()) {
    f(it->second);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

} // namespace srsenb
