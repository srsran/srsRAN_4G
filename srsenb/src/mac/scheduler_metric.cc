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
#include "srsenb/hdr/mac/scheduler_harq.h"
#include "srsenb/hdr/mac/scheduler_metric.h"

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

namespace srsenb {
    
  
  
/*****************************************************************
 *
 * Downlink Metric 
 *
 *****************************************************************/  
  
uint32_t dl_metric_rr::calc_rbg_mask(bool mask[MAX_RBG]) 
{
  // Build RBG bitmask  
  uint32_t rbg_bitmask = 0; 
  for (uint32_t n=0;n<total_rb;n++) {
    if (mask[n]) {
      rbg_bitmask |= (1<<(total_rb-1-n)); 
    }
  }  
  return rbg_bitmask; 
}

uint32_t dl_metric_rr::count_rbg(uint32_t mask) {
  uint32_t count = 0; 
  while(mask > 0) {
    if ((mask & 1) == 1) {
      count++; 
    }
    mask >>= 1; 
  }
  return count; 
}

uint32_t dl_metric_rr::get_required_rbg(sched_ue *user, uint32_t tti) 
{
  dl_harq_proc *h = user->get_pending_dl_harq(tti);
  if (h) {
    return count_rbg(h->get_rbgmask());
  }
  uint32_t pending_data = user->get_pending_dl_new_data(current_tti); 
  return user->get_required_prb_dl(pending_data, nof_ctrl_symbols); 
}

void dl_metric_rr::new_tti(std::map<uint16_t,sched_ue> &ue_db, uint32_t start_rb, uint32_t nof_rb, uint32_t nof_ctrl_symbols_, uint32_t tti)
{
  
  total_rb = start_rb+nof_rb; 
  for (uint32_t i=0;i<total_rb;i++) {
    if (i<start_rb) {
      used_rb[i] = true; 
    } else {
      used_rb[i] = false; 
    }
  }
  available_rb = nof_rb; 
  used_rb_mask = calc_rbg_mask(used_rb);
  current_tti = tti; 
  nof_ctrl_symbols = nof_ctrl_symbols_; 
  
  nof_users_with_data = 0; 
  for(std::map<uint16_t, sched_ue>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    sched_ue *user      = (sched_ue*) &iter->second;
    if (user->get_pending_dl_new_data(current_tti) || user->get_pending_dl_harq(current_tti)) {
      user->ue_idx = nof_users_with_data;
      nof_users_with_data++; 
    }
  }
}

bool dl_metric_rr::new_allocation(uint32_t nof_rbg, uint32_t *rbgmask) {
  bool mask_bit[MAX_RBG]; 
  bzero(mask_bit, sizeof(bool)*MAX_RBG);
  
  for (uint32_t i=0;i<total_rb && nof_rbg > 0;i++) {
    if (used_rb[i]) {
      mask_bit[i] = false; 
    } else {
      mask_bit[i] = true; 
      nof_rbg--;
    }
  }
  if (rbgmask) {
    *rbgmask = calc_rbg_mask(mask_bit); 
  }
  return (nof_rbg == 0); 
}

void dl_metric_rr::update_allocation(uint32_t new_mask) {
  used_rb_mask |= new_mask; 
  for (uint32_t n=0;n<total_rb;n++) {
    if (used_rb_mask & (1<<(total_rb-1-n))) {
      used_rb[n] = true; 
    } else {
      used_rb[n] = false; 
    }
  }  
}

bool dl_metric_rr::allocation_is_valid(uint32_t mask) 
{
  return (mask & used_rb_mask); 
}

dl_harq_proc* dl_metric_rr::get_user_allocation(sched_ue *user)
{
  uint32_t pending_data = user->get_pending_dl_new_data(current_tti); 
  dl_harq_proc *h = user->get_pending_dl_harq(current_tti);

  // Time-domain RR scheduling
#if ASYNC_DL_SCHED
  if (pending_data || h) {
#else
  if (pending_data || (h && !h->is_empty())) {
#endif
    if (nof_users_with_data) {
      if ((current_tti%nof_users_with_data) != user->ue_idx) {
        return NULL; 
      }    
    }
  }
  
  // Schedule retx if we have space 
#if ASYNC_DL_SCHED
  if (h) {
#else
  if (h && !h->is_empty()) {
#endif
    uint32_t retx_mask = h->get_rbgmask();
    // If can schedule the same mask, do it
    if (!allocation_is_valid(retx_mask)) {
      update_allocation(retx_mask);
      return h; 
    }
    // If not, try to find another mask in the current tti 
    uint32_t nof_rbg = count_rbg(retx_mask);
    if (nof_rbg < available_rb) {
      if (new_allocation(nof_rbg, &retx_mask)) {
        update_allocation(retx_mask);
        h->set_rbgmask(retx_mask);
        return h; 
      }
    }
  } 
  // If could not schedule the reTx, or there wasn't any pending retx, find an empty PID
#if ASYNC_DL_SCHED
  h = user->get_empty_dl_harq(); 
  if (h) {
#else
  if (h && h->is_empty()) {
#endif
    // Allocate resources based on pending data
    if (pending_data) {
      uint32_t pending_rb = user->get_required_prb_dl(pending_data, nof_ctrl_symbols);
      uint32_t newtx_mask = 0; 
      new_allocation(pending_rb, &newtx_mask);
      if (newtx_mask) {
        update_allocation(newtx_mask);
        h->set_rbgmask(newtx_mask);
        return h; 
      }
    }    
  }
  return NULL; 
}









/*****************************************************************
 *
 * Uplink Metric 
 *
 *****************************************************************/  

void ul_metric_rr::new_tti(std::map<uint16_t,sched_ue> &ue_db, uint32_t nof_rb_, uint32_t tti)
{
  current_tti  = tti; 
  nof_rb       = nof_rb_; 
  available_rb = nof_rb_; 
  bzero(used_rb, nof_rb*sizeof(bool));
  
  nof_users_with_data = 0; 
  for(std::map<uint16_t, sched_ue>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    sched_ue *user      = (sched_ue*) &iter->second;
    if (user->get_pending_ul_new_data(current_tti) || !user->get_ul_harq(current_tti)->is_empty(0)) {
      user->ue_idx    = nof_users_with_data;
      nof_users_with_data++;
    }
  }

}

bool ul_metric_rr::allocation_is_valid(ul_harq_proc::ul_alloc_t alloc)
{
  if (alloc.RB_start+alloc.L > nof_rb) {
    return false; 
  }
  for (uint32_t n=alloc.RB_start;n<alloc.RB_start+alloc.L;n++) {
    if (used_rb[n]) {
      return false; 
    }
  }
  return true; 
}

bool ul_metric_rr::new_allocation(uint32_t L, ul_harq_proc::ul_alloc_t* alloc)
{
  
  bzero(alloc, sizeof(ul_harq_proc::ul_alloc_t));
  for (uint32_t n=0;n<nof_rb && alloc->L < L;n++) {
    if (!used_rb[n] && alloc->L == 0) {
      alloc->RB_start = n; 
    }
    if (!used_rb[n]) {
      alloc->L++; 
    } else if (alloc->L > 0) {
      // avoid edges 
      if (n < 3) {
        alloc->RB_start = 0; 
        alloc->L = 0; 
      } else {
        break;
      }
    }
  }
  if (!alloc->L) {
    return 0; 
  }
  
  // Make sure L is allowed by SC-FDMA modulation 
  while (!srslte_dft_precoding_valid_prb(alloc->L)) {
    alloc->L--;
  }
  return alloc->L == L; 
}

void ul_metric_rr::update_allocation(ul_harq_proc::ul_alloc_t alloc)
{
  if (alloc.L > available_rb) {
    return; 
  }
  if (alloc.RB_start + alloc.L > nof_rb) {
    return; 
  }
  for (uint32_t n=alloc.RB_start;n<alloc.RB_start+alloc.L;n++) {
    used_rb[n] = true;
  }
  available_rb -= alloc.L; 
}

ul_harq_proc*  ul_metric_rr::get_user_allocation(sched_ue *user)
{
  // Time-domain RR scheduling
  uint32_t pending_data = user->get_pending_ul_new_data(current_tti); 
  ul_harq_proc *h = user->get_ul_harq(current_tti);
  
  if (pending_data || !h->is_empty(0)) {
    if (nof_users_with_data) {
      if ((current_tti%nof_users_with_data) != user->ue_idx) {
        return NULL; 
      }    
    }    
  }

  // Schedule retx if we have space 
  
  if (!h->is_empty(0)) {
    
    ul_harq_proc::ul_alloc_t alloc = h->get_alloc();
    
    // If can schedule the same mask, do it
    if (allocation_is_valid(alloc)) {
      update_allocation(alloc);
      return h;
    }
    
    // If not, try to find another mask in the current tti 
    if (new_allocation(alloc.L, &alloc)) {
      update_allocation(alloc);
      h->set_alloc(alloc);
      return h;
    }
  } 
  // If could not schedule the reTx, or there wasn't any pending retx, find an empty PID 
  if (h->is_empty(0)) {
    // Allocate resources based on pending data 
    if (pending_data) {
      uint32_t pending_rb = user->get_required_prb_ul(pending_data);
      ul_harq_proc::ul_alloc_t alloc; 
      new_allocation(pending_rb, &alloc);
      if (alloc.L) {
        update_allocation(alloc);
        h->set_alloc(alloc);
        return h; 
      }
    }    
  }
  return NULL; 
}



}
