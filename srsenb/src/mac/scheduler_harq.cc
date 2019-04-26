/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include "srslte/srslte.h"
#include "srslte/common/pdu.h"
#include "srsenb/hdr/mac/scheduler.h"

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

namespace srsenb {

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

void harq_proc::set_max_retx(uint32_t max_retx_) {
  log_h->debug("Set max_retx=%d pid=%d\n", max_retx_, id);
  max_retx = max_retx_; 
}

void harq_proc::reset(uint32_t tb_idx)
{
  active[tb_idx] = false;
  ack[tb_idx] = true;
  ack_received[tb_idx] = false;
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
  return !active[tb_idx] || (active[tb_idx] && ack[tb_idx] && ack_received[tb_idx]);
}

bool harq_proc::has_pending_retx_common(uint32_t tb_idx) const
{
  return !ack[tb_idx] && n_rtx[tb_idx] < max_retx;
}

uint32_t harq_proc::get_tti() const
{
  return (uint32_t) tti;
}

bool harq_proc::get_ack(uint32_t tb_idx) const
{
  return ack[tb_idx];
}

void harq_proc::set_ack(uint32_t tb_idx, bool ack_)
{
  ack[tb_idx] = ack_;
  ack_received[tb_idx] = true;
  log_h->info("ACK=%d received pid=%d, tb_idx=%d, n_rtx=%d, max_retx=%d\n", ack_, id, tb_idx, n_rtx[tb_idx], max_retx);
  if (!ack_ && (n_rtx[tb_idx] + 1 >= max_retx)) {
    Warning("SCHED: discarting TB %d pid=%d, tti=%d, maximum number of retx exceeded (%d)\n", tb_idx, id, tti, max_retx);
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

  if (max_retx) {
    active[tb_idx] = true;
  } else {
    active[tb_idx] = false; // Can reuse this process if no retx are allowed
  }
}

void harq_proc::new_retx_common(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs)
{
  ack_received[tb_idx] = false;
  tti = tti_; 
  n_rtx[tb_idx]++;
  if (mcs) {
    *mcs = last_mcs[tb_idx];
  }
  if (tbs) {
    *tbs = last_tbs[tb_idx];
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

/******************************************************
 *                  UE::DL HARQ class                    *
 ******************************************************/

dl_harq_proc::dl_harq_proc()
{
  n_cce   = 0;
  rbgmask = 0;
}

void dl_harq_proc::new_tx(uint32_t tb_idx, uint32_t tti, int mcs, int tbs, uint32_t n_cce_)
{
  n_cce = n_cce_; 
  new_tx_common(tb_idx, tti, mcs, tbs);
}

void dl_harq_proc::new_retx(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs)
{
  new_retx_common(tb_idx, tti_, mcs, tbs);
}

uint32_t dl_harq_proc::get_n_cce()
{
  return n_cce; 
}

rbgmask_t dl_harq_proc::get_rbgmask()
{
  return rbgmask;
}

void dl_harq_proc::set_rbgmask(rbgmask_t new_mask)
{
  rbgmask = new_mask;
}

bool dl_harq_proc::has_pending_retx(uint32_t tb_idx, uint32_t current_tti) const
{
  uint32_t tti_diff = srslte_tti_interval(current_tti, tti);
  // NOTE: tti may be ahead of current_tti due to thread flip
  return (tti_diff < (10240 / 2)) and (tti_diff >= SRSLTE_FDD_NOF_HARQ) and !is_empty(tb_idx);
}

int dl_harq_proc::get_tbs(uint32_t tb_idx) const
{
  return last_tbs[tb_idx];
}

/******************************************************
 *                  UE::UL RB MASK                    *
 ******************************************************/

bool ul_mask_t::any(ul_harq_proc::ul_alloc_t alloc) const noexcept
{
  return base_type::any(alloc.RB_start, alloc.RB_start + alloc.L);
}

void ul_mask_t::fill(srsenb::ul_harq_proc::ul_alloc_t alloc) noexcept
{
  base_type::fill(alloc.RB_start, alloc.RB_start + alloc.L, true);
}

/****************************************************** 
 *                  UE::UL HARQ class                    *
 ******************************************************/

ul_harq_proc::ul_alloc_t ul_harq_proc::get_alloc()
{
  return allocation;
}

void ul_harq_proc::set_alloc(ul_harq_proc::ul_alloc_t alloc)
{
  if (not is_empty(0)) {
    log_h->error("Trying to overwrite an on-going harq procedure\n");
    return;
  }
  is_rar      = false; // can be set to true through set_rar_mcs()
  is_adaptive = false;
  allocation  = alloc;
}

void ul_harq_proc::set_realloc(ul_harq_proc::ul_alloc_t alloc)
{
  if (is_empty(0)) {
    log_h->error("Trying to reallocate an empty harq procedure\n");
    return;
  }
  if (alloc.L != allocation.L or alloc.RB_start != allocation.RB_start) {
    is_adaptive = true;
  }
  allocation = alloc;
}

bool ul_harq_proc::has_pending_retx()
{
  return active[0] and has_pending_retx_common(0) and need_ack;
}

bool ul_harq_proc::is_adaptive_retx()
{
  return is_adaptive and has_pending_retx();
}

bool ul_harq_proc::is_rar_tx()
{
  return is_rar;
}

bool ul_harq_proc::is_new_tx()
{
  return active[0] and not has_pending_retx();
}

void ul_harq_proc::new_tx(uint32_t tti_, int mcs, int tbs)
{  
  need_ack = true; 
  new_tx_common(0, tti_, mcs, tbs);
  pending_data = tbs;
}

void ul_harq_proc::new_retx(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs)
{
  new_retx_common(tb_idx, tti_, mcs, tbs);
}

bool ul_harq_proc::has_pending_ack()
{
  bool ret = need_ack; 
  
  // Reset if already received a positive ACK
  if (active[0] && ack[0]) {
    active[0] = false;
  }
  if (!active[0]) {
    need_ack = false;
  }
  return ret; 
}



void ul_harq_proc::reset_pending_data()
{
  if (!active[0]) {
    pending_data = 0;
  }
}


uint32_t ul_harq_proc::get_pending_data()
{
  return (uint32_t) pending_data;
}

void ul_harq_proc::set_rar_mcs(uint32_t mcs)
{
  rar_mcs     = mcs;
  has_rar_mcs = true;
  is_rar      = true;
}

bool ul_harq_proc::get_rar_mcs(int *mcs)
{
  if (has_rar_mcs) {
    if (mcs) {
      *mcs = (int) rar_mcs; 
    }
    has_rar_mcs = false;
    is_rar      = false;
    return true; 
  }
  return false; 
}

}
