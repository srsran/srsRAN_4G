/*
 * Copyright 2013-2020 Software Radio Systems Limited
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
#include "srslte/common/log_helper.h"
#include "srslte/mac/pdu.h"
#include "srslte/srslte.h"

using srslte::tti_point;

namespace srsenb {

/******************************************************
 *
 * These classes manage the HARQ Processes.
 * There is a common class and two child classes for UL and DL.
 *
 ******************************************************/

void harq_proc::init(uint32_t id_)
{
  log_h = srslte::logmap::get("MAC ");
  id    = id_;
}

void harq_proc::set_cfg(uint32_t max_retx_)
{
  max_retx = max_retx_;
}

void harq_proc::reset(uint32_t tb_idx)
{
  ack_state[tb_idx] = NULL_ACK;
  active[tb_idx]    = false;
  n_rtx[tb_idx]     = 0;
  tti               = tti_point{0};
  last_mcs[tb_idx]  = -1;
  last_tbs[tb_idx]  = -1;
  tx_cnt[tb_idx]    = 0;
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

tti_point harq_proc::get_tti() const
{
  return tti;
}

int harq_proc::set_ack_common(uint32_t tb_idx, bool ack_)
{
  if (is_empty(tb_idx)) {
    log_h->warning("Received ACK for inactive harq\n");
    return SRSLTE_ERROR;
  }
  ack_state[tb_idx] = ack_ ? ACK : NACK;
  log_h->debug("ACK=%d received pid=%d, tb_idx=%d, n_rtx=%d, max_retx=%d\n", ack_, id, tb_idx, n_rtx[tb_idx], max_retx);
  if (!ack_ && (n_rtx[tb_idx] + 1 >= max_retx)) {
    Warning("SCHED: discarding TB %d pid=%d, tti=%d, maximum number of retx exceeded (%d)\n",
            tb_idx,
            id,
            tti.to_uint(),
            max_retx);
    active[tb_idx] = false;
  } else if (ack_) {
    active[tb_idx] = false;
  }
  return SRSLTE_SUCCESS;
}

void harq_proc::new_tx_common(uint32_t tb_idx, tti_point tti_, int mcs, int tbs)
{
  reset(tb_idx);
  ndi[tb_idx] = !ndi[tb_idx];
  tti         = tti_;
  tx_cnt[tb_idx]++;
  last_mcs[tb_idx] = mcs;
  last_tbs[tb_idx] = tbs;

  active[tb_idx] = true;
}

void harq_proc::new_retx_common(uint32_t tb_idx, tti_point tti_, int* mcs, int* tbs)
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
  if (max_retx == 0 and not is_empty()) {
    for (bool& tb : active) {
      tb = false;
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
  new_tx_common(tb_idx, tti_point{tti}, mcs, tbs);
}

void dl_harq_proc::new_retx(const rbgmask_t& new_mask,
                            uint32_t         tb_idx,
                            uint32_t         tti_,
                            int*             mcs,
                            int*             tbs,
                            uint32_t         n_cce_)
{
  n_cce   = n_cce_;
  rbgmask = new_mask;
  new_retx_common(tb_idx, tti_point{tti_}, mcs, tbs);
}

int dl_harq_proc::set_ack(uint32_t tb_idx, bool ack)
{
  return set_ack_common(tb_idx, ack);
}

uint32_t dl_harq_proc::get_n_cce() const
{
  return n_cce;
}

rbgmask_t dl_harq_proc::get_rbgmask() const
{
  return rbgmask;
}

bool dl_harq_proc::has_pending_retx(uint32_t tb_idx, uint32_t tti_tx_dl) const
{
  return (tti_point{tti_tx_dl} >= tti + FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS) and
         has_pending_retx_common(tb_idx);
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
  new_tx_common(0, tti_point{tti_}, mcs, tbs);
  pending_data = tbs;
  pending_ack  = NULL_ACK;
}

void ul_harq_proc::new_retx(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs, ul_harq_proc::ul_alloc_t alloc)
{
  is_adaptive = alloc.L != allocation.L or alloc.RB_start != allocation.RB_start;
  allocation  = alloc;
  new_retx_common(tb_idx, tti_point{tti_}, mcs, tbs);
}

bool ul_harq_proc::set_ack(uint32_t tb_idx, bool ack_)
{
  if (is_empty()) {
    return false;
  }
  pending_ack = ack_ ? ACK : NACK;
  set_ack_common(tb_idx, ack_);
  return true;
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
  return (uint32_t)pending_data;
}

/********************
 *   Harq Entity
 *******************/

harq_entity::harq_entity(size_t nof_dl_harqs, size_t nof_ul_harqs) :
  dl_harqs(nof_dl_harqs),
  ul_harqs(nof_ul_harqs),
  log_h(srslte::logmap::get("MAC "))
{
  for (uint32_t i = 0; i < dl_harqs.size(); ++i) {
    dl_harqs[i].init(i);
  }
  for (uint32_t i = 0; i < ul_harqs.size(); ++i) {
    ul_harqs[i].init(i);
  }
}

void harq_entity::reset()
{
  for (auto& h : dl_harqs) {
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
      h.reset(tb);
    }
  }
  for (auto& h : ul_harqs) {
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
      h.reset(tb);
    }
  }
}

void harq_entity::set_cfg(uint32_t max_retx)
{
  for (auto& h : dl_harqs) {
    h.set_cfg(max_retx);
  }
  for (auto& h : ul_harqs) {
    h.set_cfg(max_retx);
  }
}

dl_harq_proc* harq_entity::get_empty_dl_harq(uint32_t tti_tx_dl)
{
  if (not is_async) {
    dl_harq_proc* h = &dl_harqs[tti_tx_dl % nof_dl_harqs()];
    return h->is_empty() ? h : nullptr;
  }

  auto it = std::find_if(dl_harqs.begin(), dl_harqs.end(), [](dl_harq_proc& h) { return h.is_empty(); });
  return it != dl_harqs.end() ? &(*it) : nullptr;
}

dl_harq_proc* harq_entity::get_pending_dl_harq(uint32_t tti_tx_dl)
{
  if (not is_async) {
    dl_harq_proc* h = &dl_harqs[tti_tx_dl % nof_dl_harqs()];
    return (h->has_pending_retx(0, tti_tx_dl) or h->has_pending_retx(1, tti_tx_dl)) ? h : nullptr;
  }
  return get_oldest_dl_harq(tti_tx_dl);
}

std::pair<uint32_t, int> harq_entity::set_ack_info(uint32_t tti_rx, uint32_t tb_idx, bool ack)
{
  for (auto& h : dl_harqs) {
    if (h.get_tti() + FDD_HARQ_DELAY_DL_MS == tti_point{tti_rx}) {
      if (h.set_ack(tb_idx, ack) == SRSLTE_SUCCESS) {
        return {h.get_id(), h.get_tbs(tb_idx)};
      }
      return {h.get_id(), -1};
    }
  }
  return {dl_harqs.size(), -1};
}

ul_harq_proc* harq_entity::get_ul_harq(uint32_t tti_tx_ul)
{
  return &ul_harqs[tti_tx_ul % ul_harqs.size()];
}

std::pair<bool, uint32_t> harq_entity::set_ul_crc(srslte::tti_point tti_rx, uint32_t tb_idx, bool ack_)
{
  ul_harq_proc* h   = get_ul_harq(tti_rx.to_uint());
  uint32_t      pid = h->get_id();
  return {h->set_ack(tb_idx, ack_), pid};
}

void harq_entity::reset_pending_data(uint32_t tti_rx)
{
  tti_point tti_tx_ul = srslte::to_tx_ul(tti_point{tti_rx});
  tti_point tti_tx_dl = srslte::to_tx_dl(tti_point{tti_rx});

  // Reset ACK state of UL Harq
  get_ul_harq(tti_tx_ul.to_uint())->reset_pending_data();

  // Reset any DL harq which has 0 retxs
  for (auto& h : dl_harqs) {
    h.reset_pending_data();
  }

  // delete old DL harq procs
  for (auto& h : dl_harqs) {
    if (not h.is_empty()) {
      if (tti_tx_dl > h.get_tti() + 100) {
        srslte::logmap::get("MAC")->info("SCHED: pid=%d is old. tti_pid=%d, now is %d, resetting\n",
                                         h.get_id(),
                                         h.get_tti().to_uint(),
                                         tti_tx_dl.to_uint());
        for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
          h.reset(tb);
        }
      }
    }
  }
}

/**
 * Get the oldest DL Harq Proc that has pending retxs
 * @param tti_tx_dl assumed to always be equal or ahead in time in comparison to current harqs
 * @return pointer to found dl_harq
 */
dl_harq_proc* harq_entity::get_oldest_dl_harq(uint32_t tti_tx_dl)
{
  tti_point t_tx_dl{tti_tx_dl};
  int       oldest_idx = -1;
  uint32_t  oldest_tti = 0;
  for (const dl_harq_proc& h : dl_harqs) {
    if (h.has_pending_retx(0, tti_tx_dl) or h.has_pending_retx(1, tti_tx_dl)) {
      uint32_t x = t_tx_dl - h.get_tti();
      if (x > oldest_tti) {
        oldest_idx = h.get_id();
        oldest_tti = x;
      }
    }
  }
  return (oldest_idx >= 0) ? &dl_harqs[oldest_idx] : nullptr;
}

} // namespace srsenb
