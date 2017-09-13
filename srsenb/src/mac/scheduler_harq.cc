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
#include <boost/concept_check.hpp>

#include "srslte/srslte.h"
#include "srslte/common/pdu.h"
#include "mac/scheduler.h"

#define Error(fmt, ...)   log_h->error_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

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
  ndi = false; 
}

void harq_proc::set_max_retx(uint32_t max_retx_) {
  log_h->debug("Set max_retx=%d pid=%d\n", max_retx_, id);
  max_retx = max_retx_; 
}

uint32_t harq_proc::get_id()
{
  return id; 
}

void harq_proc::reset()
{
  active = false; 
  ack = true; 
  ack_received = false; 
  n_rtx = 0; 
  tti = 0; 
  last_mcs = -1; 
  last_tbs = -1; 
  tx_cnt = 0; 
}

bool harq_proc::is_empty()
{
  return !active || (active && ack && ack_received);
}

bool harq_proc::has_pending_retx_common()
{
  return !ack && n_rtx < max_retx; 
}

uint32_t harq_proc::get_tti()
{
  return tti; 
}

bool harq_proc::get_ack()
{
  return ack; 
}

void harq_proc::set_ack(bool ack_)
{
  ack = ack_;   
  ack_received = true;
  log_h->debug("ACK=%d received pid=%d, n_rtx=%d, max_retx=%d\n", ack_, id, n_rtx, max_retx);
  if (n_rtx + 1 >= max_retx) {
    Warning("SCHED: discarting TB pid=%d, tti=%d, maximum number of retx exceeded (%d)\n", id, tti, max_retx);
    active = false;      
  }
}

void harq_proc::new_tx_common(uint32_t tti_, int mcs, int tbs)
{  
  reset();
  ndi = !ndi; 
  tti = tti_;   
  tx_cnt++;   
  last_mcs = mcs; 
  last_tbs = tbs; 

  if (max_retx) {
    active = true; 
  } else {
    active = false; // Can reuse this process if no retx are allowed 
  }
}

void harq_proc::new_retx(uint32_t tti_, int *mcs, int *tbs)
{
  ack_received = false; 
  tti = tti_; 
  n_rtx++;
  if (mcs) {
    *mcs = last_mcs; 
  }
  if (tbs) {
    *tbs = last_tbs; 
  }
}

uint32_t harq_proc::nof_tx()
{
  return tx_cnt; 
}

uint32_t harq_proc::nof_retx()
{
  return n_rtx; 
}

bool harq_proc::get_ndi() 
{
  return ndi; 
}

/****************************************************** 
 *                  UE::DL HARQ class                    *
 ******************************************************/

void dl_harq_proc::new_tx(uint32_t tti, int mcs, int tbs, uint32_t n_cce_)
{
  n_cce = n_cce_; 
  new_tx_common(tti, mcs, tbs);
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

bool dl_harq_proc::has_pending_retx(uint32_t current_tti)
{
  return srslte_tti_interval(current_tti, tti) >= 8 && has_pending_retx_common(); 
}

int dl_harq_proc::get_tbs()
{
  return last_tbs; 
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

void ul_harq_proc::same_alloc()
{
  is_adaptive = true; 
}

bool ul_harq_proc::is_adaptive_retx()
{
  return is_adaptive; 
}

void ul_harq_proc::new_tx(uint32_t tti_, int mcs, int tbs)
{  
  need_ack = true; 
  new_tx_common(tti_, mcs, tbs);
  pending_data = tbs; 
}


bool ul_harq_proc::has_pending_ack()
{
  bool ret = need_ack; 
  
  // Reset if already received a positive ACK
  if (active && ack) {
    active = false;     
  }
  if (!active) {
    need_ack = false;
  }
  return ret; 
}



void ul_harq_proc::reset_pending_data()
{
  if (!active) {
    pending_data = 0;
  }
}


  uint32_t ul_harq_proc::get_pending_data()
{
  return pending_data; 
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
