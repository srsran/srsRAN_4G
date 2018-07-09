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

uint32_t harq_proc::get_id()
{
  return id; 
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

bool harq_proc::is_empty(uint32_t tb_idx)
{
  return !active[tb_idx] || (active[tb_idx] && ack[tb_idx] && ack_received[tb_idx]);
}

bool harq_proc::has_pending_retx_common(uint32_t tb_idx)
{
  return !ack[tb_idx] && n_rtx[tb_idx] < max_retx;
}

uint32_t harq_proc::get_tti()
{
  return (uint32_t) tti;
}

bool harq_proc::get_ack(uint32_t tb_idx)
{
  return ack[tb_idx];
}

void harq_proc::set_ack(uint32_t tb_idx, bool ack_)
{
  ack[tb_idx] = ack_;
  ack_received[tb_idx] = true;
  log_h->debug("ACK=%d received pid=%d, tb_idx=%d, n_rtx=%d, max_retx=%d\n", ack_, id, tb_idx, n_rtx[tb_idx], max_retx);
  if (n_rtx[tb_idx] + 1 >= max_retx) {
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

void harq_proc::new_retx(uint32_t tb_idx, uint32_t tti_, int *mcs, int *tbs)
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

uint32_t harq_proc::nof_tx(uint32_t tb_idx)
{
  return tx_cnt[tb_idx];
}

uint32_t harq_proc::nof_retx(uint32_t tb_idx)
{
  return n_rtx[tb_idx];
}

bool harq_proc::get_ndi(uint32_t tb_idx)
{
  return ndi[tb_idx];
}

/****************************************************** 
 *                  UE::DL HARQ class                    *
 ******************************************************/

void dl_harq_proc::new_tx(uint32_t tb_idx, uint32_t tti, int mcs, int tbs, uint32_t n_cce_)
{
  n_cce = n_cce_; 
  new_tx_common(tb_idx, tti, mcs, tbs);
}

uint32_t dl_harq_proc::get_n_cce()
{
  return n_cce; 
}

uint32_t dl_harq_proc::get_rbgmask()
{
  return rbgmask;
}

void dl_harq_proc::set_rbgmask(uint32_t new_mask)
{
  rbgmask = new_mask;
}

bool dl_harq_proc::has_pending_retx(uint32_t tb_idx, uint32_t current_tti)
{
  return srslte_tti_interval(current_tti, tti) >= (2*HARQ_DELAY_MS) && has_pending_retx_common(tb_idx);
}

int dl_harq_proc::get_tbs(uint32_t tb_idx)
{
  return last_tbs[tb_idx];
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
  is_adaptive = false;
  memcpy(&allocation, &alloc, sizeof(ul_alloc_t));
}

void ul_harq_proc::re_alloc(ul_harq_proc::ul_alloc_t alloc)
{
  is_adaptive = true;
  memcpy(&allocation, &alloc, sizeof(ul_alloc_t));
}

bool ul_harq_proc::is_adaptive_retx()
{
  return is_adaptive; 
}

void ul_harq_proc::new_tx(uint32_t tti_, int mcs, int tbs)
{  
  need_ack = true; 
  new_tx_common(0, tti_, mcs, tbs);
  pending_data = tbs; 
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
}

bool ul_harq_proc::get_rar_mcs(int *mcs)
{
  if (has_rar_mcs) {
    if (mcs) {
      *mcs = (int) rar_mcs; 
    }
    has_rar_mcs = false; 
    return true; 
  }
  return false; 
}



}
