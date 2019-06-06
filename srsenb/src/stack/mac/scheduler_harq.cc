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

#include <string.h>

#include "srsenb/hdr/stack/mac/scheduler.h"
#include "srslte/common/pdu.h"
#include "srslte/srslte.h"

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

namespace srsenb {

rbg_range_t::rbg_range_t(const prb_range_t& rbgs, uint32_t P) :
  rbg_range_t(srslte::ceil_div(rbgs.prb_start, P), srslte::ceil_div(rbgs.prb_end, P))
{
}

prb_range_t::prb_range_t(const rbg_range_t& rbgs, uint32_t P) : prb_range_t(rbgs.rbg_start * P, rbgs.rbg_end * P) {}

prb_range_t prb_range_t::riv_to_prbs(uint32_t riv, uint32_t nof_prbs, int nof_vrbs)
{
  prb_range_t p;
  if (nof_vrbs < 0) {
    nof_vrbs = nof_prbs;
  }
  srslte_ra_type2_from_riv(riv, &p.prb_end, &p.prb_start, nof_prbs, (uint32_t)nof_vrbs);
  p.prb_end += p.prb_start;
  return p;
}

/****************************************************** 
 * 
 * These classes manage the HARQ Processes. 
 * There is a common class and two child classes for UL and DL.
 * 
 ******************************************************/

void harq_proc::config(uint32_t id_, uint32_t max_retx_, srslte::log* log_h_)
{
  log_h    = log_h_; 
  id       = id_; 
  max_retx = max_retx_; 
  for (int i = 0; i < SRSLTE_MAX_TB; i++) {
    ndi[i] = false;
  }
}

void harq_proc::reset(uint32_t tb_idx)
{
  ack_state[tb_idx] = NULL_ACK;
  active[tb_idx] = false;
  n_rtx[tb_idx] = 0;
  tti = 0; 
  last_mcs[tb_idx] = -1;
  last_tbs[tb_idx] = -1;
  tx_cnt[tb_idx] = 0;
}

uint32_t harq_proc::get_id() const
{
  return id;
}

bool harq_proc::is_empty() const
{
  for (uint32_t i = 0; i < SRSLTE_MAX_TB; ++i) {
    if (not is_empty(i)) {
      return false;
    }
  }
  return true;
}

bool harq_proc::is_empty(uint32_t tb_idx) const
{
  return !active[tb_idx];
}

bool harq_proc::has_pending_retx_common(uint32_t tb_idx) const
{
  return !is_empty(tb_idx) && ack_state[tb_idx] == NACK;
}

uint32_t harq_proc::get_tti() const
{
  return (uint32_t) tti;
}

void harq_proc::set_ack_common(uint32_t tb_idx, bool ack_)
{
  ack_state[tb_idx] = ack_ ? ACK : NACK;
  log_h->debug("ACK=%d received pid=%d, tb_idx=%d, n_rtx=%d, max_retx=%d\n", ack_, id, tb_idx, n_rtx[tb_idx], max_retx);
  if (!ack_ && (n_rtx[tb_idx] + 1 >= max_retx)) {
    Warning("SCHED: discarting TB %d pid=%d, tti=%d, maximum number of retx exceeded (%d)\n", tb_idx, id, tti, max_retx);
    active[tb_idx] = false;
  } else if (ack_) {
    active[tb_idx] = false;
  }
}

void harq_proc::new_tx_common(uint32_t tb_idx, uint32_t tti_, int mcs, int tbs)
{  
  reset(tb_idx);
  ndi[tb_idx] = !ndi[tb_idx];
  tti = tti_;   
  tx_cnt[tb_idx]++;
  last_mcs[tb_idx] = mcs;
  last_tbs[tb_idx] = tbs;

  active[tb_idx] = true;
}

void harq_proc::new_retx_common(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs)
{
  ack_state[tb_idx] = NACK;
  tti               = tti_;
  n_rtx[tb_idx]++;
  if (mcs) {
    *mcs = last_mcs[tb_idx];
  }
  if (tbs) {
    *tbs = last_tbs[tb_idx];
  }
}

void harq_proc::reset_pending_data_common()
{
  // reuse harqs with no retxs
  if (max_retx == 0 and !is_empty()) {
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; ++tb) {
      active[tb] = false;
    }
  }
}

uint32_t harq_proc::nof_tx(uint32_t tb_idx) const
{
  return tx_cnt[tb_idx];
}

uint32_t harq_proc::nof_retx(uint32_t tb_idx) const
{
  return n_rtx[tb_idx];
}

bool harq_proc::get_ndi(uint32_t tb_idx) const
{
  return ndi[tb_idx];
}

uint32_t harq_proc::max_nof_retx() const
{
  return max_retx;
}

/******************************************************
 *                  UE::DL HARQ class                 *
 ******************************************************/

dl_harq_proc::dl_harq_proc() : harq_proc()
{
  n_cce = 0;
}

void dl_harq_proc::new_tx(const rbgmask_t& new_mask, uint32_t tb_idx, uint32_t tti, int mcs, int tbs, uint32_t n_cce_)
{
  n_cce   = n_cce_;
  rbgmask = new_mask;
  new_tx_common(tb_idx, tti, mcs, tbs);
}

void dl_harq_proc::new_retx(
    const rbgmask_t& new_mask, uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs, uint32_t n_cce_)
{
  n_cce   = n_cce_;
  rbgmask = new_mask;
  new_retx_common(tb_idx, tti_, mcs, tbs);
}

void dl_harq_proc::set_ack(uint32_t tb_idx, bool ack)
{
  set_ack_common(tb_idx, ack);
}

uint32_t dl_harq_proc::get_n_cce() const
{
  return n_cce; 
}

rbgmask_t dl_harq_proc::get_rbgmask() const
{
  return rbgmask;
}

bool dl_harq_proc::has_pending_retx(uint32_t tb_idx, uint32_t current_tti) const
{
  uint32_t tti_diff = srslte_tti_interval(current_tti, tti);
  // NOTE: tti may be ahead of current_tti due to thread flip
  return (tti_diff < (10240 / 2)) and (tti_diff >= SRSLTE_FDD_NOF_HARQ) and has_pending_retx_common(tb_idx);
}

int dl_harq_proc::get_tbs(uint32_t tb_idx) const
{
  return last_tbs[tb_idx];
}

void dl_harq_proc::reset_pending_data()
{
  reset_pending_data_common();
}

/****************************************************** 
 *                  UE::UL HARQ class                    *
 ******************************************************/

ul_harq_proc::ul_alloc_t ul_harq_proc::get_alloc() const
{
  return allocation;
}

bool ul_harq_proc::has_pending_retx() const
{
  return has_pending_retx_common(0);
}

bool ul_harq_proc::is_adaptive_retx() const
{
  return is_adaptive and has_pending_retx();
}

void ul_harq_proc::new_tx(uint32_t tti_, int mcs, int tbs, ul_harq_proc::ul_alloc_t alloc, uint32_t max_retx_)
{
  max_retx    = (uint32_t)max_retx_;
  is_adaptive = false;
  allocation  = alloc;
  new_tx_common(0, tti_, mcs, tbs);
  pending_data = tbs;
  pending_ack  = NULL_ACK;
}

void ul_harq_proc::new_retx(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs, ul_harq_proc::ul_alloc_t alloc)
{
  if (alloc.L != allocation.L or alloc.RB_start != allocation.RB_start) {
    is_adaptive = true;
  }
  allocation = alloc;
  new_retx_common(tb_idx, tti_, mcs, tbs);
}

void ul_harq_proc::set_ack(uint32_t tb_idx, bool ack_)
{
  pending_ack = ack_ ? ACK : NACK;
  set_ack_common(tb_idx, ack_);
}

bool ul_harq_proc::has_pending_ack() const
{
  return pending_ack != NULL_ACK;
}

bool ul_harq_proc::get_pending_ack() const
{
  return pending_ack == ACK;
}

void ul_harq_proc::reset_pending_data()
{
  reset_pending_data_common();
  pending_ack = NULL_ACK;
  if (is_empty(0)) {
    pending_data = 0;
  }
}

uint32_t ul_harq_proc::get_pending_data() const
{
  return (uint32_t) pending_data;
}

}
