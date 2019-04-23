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

void dl_metric_rr::sched_users(std::map<uint16_t, sched_ue>& ue_db,
                               rbgmask_t*                    dl_mask,
                               uint32_t                      nof_ctrl_symbols_,
                               uint32_t                      tti)
{
  typedef std::map<uint16_t, sched_ue>::iterator it_t;

  current_tti = tti;
  nof_ctrl_symbols = nof_ctrl_symbols_;
  used_rbg         = dl_mask;
  available_rbg    = (uint32_t)(used_rbg->size() - used_rbg->count()); // nof_rbg;

  if (ue_db.empty())
    return;

  // give priority in a time-domain RR basis
  uint32_t priority_idx = current_tti % (uint32_t)ue_db.size();
  it_t     iter         = ue_db.begin();
  std::advance(iter,priority_idx);
  for(uint32_t ue_count = 0 ; ue_count < ue_db.size() ; ++iter, ++ue_count) {
    if(iter==ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue *user = (sched_ue*) &iter->second;
    user->set_dl_alloc(allocate_user(user));
  }
}

bool dl_metric_rr::find_allocation(uint32_t nof_rbg, rbgmask_t* rbgmask)
{
  *rbgmask = ~(*used_rbg);

  uint32_t i = 0;
  for (; i < used_rbg->size() and nof_rbg > 0; ++i) {
    if (rbgmask->test(i)) {
      nof_rbg--;
    }
  }
  rbgmask->fill(i, rbgmask->size(), false);

  return nof_rbg == 0;
}

void dl_metric_rr::update_allocation(rbgmask_t new_rbgmask)
{
  *used_rbg |= new_rbgmask;
}

/**
 * Checks if a mask can fit in the current RBG grid
 * @param mask
 * @return Returns true if all the mask entries set to true are empty
 */
bool dl_metric_rr::allocation_is_valid(rbgmask_t mask)
{
  return (mask & (*used_rbg)).none();
}

dl_harq_proc* dl_metric_rr::allocate_user(sched_ue* user)
{
  // FIXME: First do reTxs for all users. Only then do the rest.
  dl_harq_proc *h = user->get_pending_dl_harq(current_tti);
  uint32_t req_bytes = user->get_pending_dl_new_data_total(current_tti);

  // Schedule retx if we have space
#if ASYNC_DL_SCHED
  if (h) {
#else
  if (h && !h->is_empty()) {
#endif
    rbgmask_t retx_mask = h->get_rbgmask();
    uint32_t  nof_rbg   = retx_mask.count();
    if (nof_rbg <= available_rbg) {
      // Try to reuse the same mask
      if (allocation_is_valid(retx_mask)) {
        update_allocation(retx_mask);
        return h;
      }

      // If previous mask does not fit, find another with exact same number of rbgs
      if (find_allocation(nof_rbg, &retx_mask)) {
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
      rbgmask_t newtx_mask(used_rbg->size());
      find_allocation(pending_rbg, &newtx_mask);
      if (newtx_mask.any()) { // some empty spaces were found
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

void ul_metric_rr::sched_users(std::map<uint16_t, sched_ue>& ue_db, ul_mask_t* start_mask, uint32_t tti)
{
  typedef std::map<uint16_t, sched_ue>::iterator it_t;

  current_tti = tti;
  used_rb     = start_mask;

  if(ue_db.size()==0)
      return;

  // give priority in a time-domain RR basis
  uint32_t priority_idx = (current_tti+(uint32_t)ue_db.size()/2) % (uint32_t)ue_db.size(); // make DL and UL interleaved

  // allocate reTxs first
  it_t iter = ue_db.begin();
  std::advance(iter, priority_idx);
  for(uint32_t ue_count = 0 ; ue_count < ue_db.size() ; ++iter, ++ue_count) {
    if(iter==ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue *user = (sched_ue *) &iter->second;
    if (user->get_ul_alloc() == NULL) { // can already be allocated for msg3
      user->set_ul_alloc(allocate_user_retx_prbs(user));
    }
  }

  // give priority in a time-domain RR basis
  iter = ue_db.begin();
  std::advance(iter,priority_idx);
  for(uint32_t ue_count = 0 ; ue_count < ue_db.size() ; ++iter, ++ue_count) {
    if(iter==ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue *user = (sched_ue*) &iter->second;
    if (user->get_ul_alloc() == NULL) {
      user->set_ul_alloc(allocate_user_newtx_prbs(user));
    }
  }
}

bool ul_metric_rr::allocation_is_valid(ul_harq_proc::ul_alloc_t alloc)
{
  if (alloc.RB_start + alloc.L > used_rb->size()) {
    return false; 
  }
  return not used_rb->any(alloc);
}

/**
 * Finds a range of L contiguous PRBs that are empty
 * @param L Size of the requested UL allocation in PRBs
 * @param alloc Found allocation. It is guaranteed that 0 <= alloc->L <= L
 * @return true if the requested allocation of size L was strictly met
 */
bool ul_metric_rr::find_allocation(uint32_t L, ul_harq_proc::ul_alloc_t* alloc)
{
  bzero(alloc, sizeof(ul_harq_proc::ul_alloc_t));
  for (uint32_t n = 0; n < used_rb->size() && alloc->L < L; n++) {
    if (not used_rb->test(n) && alloc->L == 0) {
      alloc->RB_start = n; 
    }
    if (not used_rb->test(n)) {
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
  bool ret = false;
  if(allocation_is_valid(alloc)) {
    used_rb->fill(alloc);
    return true;
  }
  return ret;
}

ul_harq_proc* ul_metric_rr::allocate_user_retx_prbs(sched_ue *user)
{
  ul_harq_proc *h = user->get_ul_harq(current_tti);

  // if there are procedures and we have space
  if (h->has_pending_retx()) {
    ul_harq_proc::ul_alloc_t alloc = h->get_alloc();

    // If can schedule the same mask, do it
    if (update_allocation(alloc)) {
      h->set_realloc(alloc);
      return h;
    }

    // If not, try to find another mask in the current tti with the same number of PRBs
    if (find_allocation(alloc.L, &alloc)) {
      if(not update_allocation(alloc)) {
        printf("ERROR: Scheduler failed to allocate user\n");
        return NULL;
      }
      h->set_realloc(alloc);
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

    find_allocation(pending_rb, &alloc);
    if (alloc.L > 0) { // at least one PRB was scheduled
      if (not update_allocation(alloc)) {
        printf("ERROR: Scheduler failed to allocate user\n");
        return NULL;
      }
      h->set_alloc(alloc);
      return h;
    }
  }
  return NULL;
}

}
