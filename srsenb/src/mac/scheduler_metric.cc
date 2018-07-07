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
  for (uint32_t n=0;n<total_rbg;n++) {
    if (mask[n]) {
      rbg_bitmask |= (1<<(total_rbg-1-n));
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
  return user->prb_to_rbg(user->get_required_prb_dl(pending_data, nof_ctrl_symbols));
}

void dl_metric_rr::new_tti(std::map<uint16_t,sched_ue> &ue_db, uint32_t start_rbg, uint32_t nof_rbg, uint32_t nof_ctrl_symbols_, uint32_t tti)
{
  total_rbg = start_rbg+nof_rbg;
  for (uint32_t i=0;i<total_rbg;i++) {
    if (i<start_rbg) {
      used_rbg[i] = true;
    } else {
      used_rbg[i] = false;
    }
  }
  available_rbg = nof_rbg;
  used_rbg_mask = calc_rbg_mask(used_rbg);
  current_tti = tti;
  nof_ctrl_symbols = nof_ctrl_symbols_;

  if(ue_db.size()==0)
    return;

  // give priority in a time-domain RR basis
  uint32_t priority_idx = current_tti % ue_db.size();
  std::map<uint16_t, sched_ue>::iterator iter = ue_db.begin();
  std::advance(iter,priority_idx);
  for(uint32_t ue_count = 0 ; ue_count < ue_db.size() ; ++iter, ++ue_count) {
    if(iter==ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue *user = (sched_ue*) &iter->second;
    user->dl_next_alloc = apply_user_allocation(user);
  }
}

bool dl_metric_rr::new_allocation(uint32_t nof_rbg, uint32_t *rbgmask) {
  bool mask_bit[MAX_RBG]; 
  bzero(mask_bit, sizeof(bool)*MAX_RBG);
  
  for (uint32_t i=0;i<total_rbg && nof_rbg > 0;i++) {
    if (used_rbg[i]) {
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
  used_rbg_mask |= new_mask;
  for (uint32_t n=0;n<total_rbg;n++) {
    if (used_rbg_mask & (1<<(total_rbg-1-n))) {
      used_rbg[n] = true;
    } else {
      used_rbg[n] = false;
    }
  }  
}

bool dl_metric_rr::allocation_is_valid(uint32_t mask) 
{
  return (mask & used_rbg_mask);
}

dl_harq_proc* dl_metric_rr::apply_user_allocation(sched_ue *user) {
  dl_harq_proc *h = user->get_pending_dl_harq(current_tti);
  uint32_t req_bytes = user->get_pending_dl_new_data_total(current_tti);

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
    if (nof_rbg < available_rbg) {
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
    if (req_bytes) {
      uint32_t pending_rbg = user->prb_to_rbg(user->get_required_prb_dl(req_bytes, nof_ctrl_symbols));
      uint32_t newtx_mask = 0;
      new_allocation(pending_rbg, &newtx_mask);
      if (newtx_mask) {
        update_allocation(newtx_mask);
        h->set_rbgmask(newtx_mask);
        return h;
      }
    }
  }

  return NULL;
}

dl_harq_proc* dl_metric_rr::get_user_allocation(sched_ue *user)
{
  return user->dl_next_alloc;
}









/*****************************************************************
 *
 * Uplink Metric 
 *
 *****************************************************************/

void ul_metric_rr::reset_allocation(uint32_t nof_rb_)
{
  nof_rb       = nof_rb_;
  bzero(used_rb, nof_rb*sizeof(bool));
}

void ul_metric_rr::new_tti(std::map<uint16_t,sched_ue> &ue_db, uint32_t nof_rb_, uint32_t tti)
{
  typedef std::map<uint16_t, sched_ue>::iterator it_t;
  current_tti  = tti; 

  if(ue_db.size()==0)
      return;

  for(it_t it = ue_db.begin(); it != ue_db.end(); ++it) {
    it->second.ul_next_alloc = NULL;
  }

  // give priority in a time-domain RR basis
  uint32_t priority_idx = (current_tti+(uint32_t)ue_db.size()/2) % (uint32_t)ue_db.size(); // make DL and UL interleaved

  // allocate reTxs first
  it_t iter = ue_db.begin();
  for(uint32_t ue_count = 0 ; ue_count < ue_db.size() ; ++iter, ++ue_count) {
    if(iter==ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue *user = (sched_ue *) &iter->second;
    user->ul_next_alloc = allocate_user_retx_prbs(user);
  }

  // give priority in a time-domain RR basis
  iter = ue_db.begin();
  std::advance(iter,priority_idx);
  for(uint32_t ue_count = 0 ; ue_count < ue_db.size() ; ++iter, ++ue_count) {
    if(iter==ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue *user = (sched_ue*) &iter->second;
    if (!user->ul_next_alloc) {
      user->ul_next_alloc = allocate_user_newtx_prbs(user);
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
  if (alloc->L==0) {
    return false;
  }
  
  // Make sure L is allowed by SC-FDMA modulation 
  while (!srslte_dft_precoding_valid_prb(alloc->L)) {
    alloc->L--;
  }
  return alloc->L == L; 
}

bool ul_metric_rr::update_allocation(ul_harq_proc::ul_alloc_t alloc)
{
  if(allocation_is_valid(alloc)) {
    for (uint32_t n=alloc.RB_start;n<alloc.RB_start+alloc.L;n++) {
      used_rb[n] = true;
    }
    return true;
  }
  return false;
}

ul_harq_proc* ul_metric_rr::allocate_user_retx_prbs(sched_ue *user)
{
  ul_harq_proc *h = user->get_ul_harq(current_tti);

  // if there are procedures and we have space
  if(!h->is_empty(0)) {
    ul_harq_proc::ul_alloc_t alloc = h->get_alloc();

    // If can schedule the same mask, do it
    if (update_allocation(alloc)) {
      return h;
    }

    // If not, try to find another mask in the current tti
    if (new_allocation(alloc.L, &alloc)) {
      if(not update_allocation(alloc)) {
        printf("SCHED: Computed UL allocation is not valid!\n");
      }
      h->set_alloc(alloc);
      return h;
    }
  }
  return NULL;
}

ul_harq_proc* ul_metric_rr::allocate_user_newtx_prbs(sched_ue* user)
{
  uint32_t pending_data = user->get_pending_ul_new_data(current_tti);
  ul_harq_proc *h = user->get_ul_harq(current_tti);

  // find an empty PID
  if (h->is_empty(0) and pending_data) {
    uint32_t pending_rb = user->get_required_prb_ul(pending_data);
    ul_harq_proc::ul_alloc_t alloc;
    new_allocation(pending_rb, &alloc);
    if (alloc.L) {
      if(!update_allocation(alloc)) {
        printf("SCHED: Computed UL allocation is not valid!\n");
      }
      h->set_alloc(alloc);
      return h;
    }
  }
  return NULL;
}

ul_harq_proc* ul_metric_rr::apply_user_allocation(sched_ue *user, bool retx_only) {
  // Time-domain RR scheduling
  uint32_t pending_data = user->get_pending_ul_new_data(current_tti);
  ul_harq_proc *h = user->get_ul_harq(current_tti);

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

  if (retx_only) {
    return NULL;
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

ul_harq_proc*  ul_metric_rr::get_user_allocation(sched_ue *user)
{
  return user->ul_next_alloc;
}

}
