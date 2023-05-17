/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

#include "srsenb/hdr/stack/mac/sched.h"
#include "srsran/common/log_helper.h"
#include "srsran/mac/pdu.h"

using srsran::tti_point;

namespace srsenb {

/******************************************************
 *
 * These classes manage the HARQ Processes.
 * There is a common class and two child classes for UL and DL.
 *
 ******************************************************/

harq_proc::harq_proc() : logger(&srslog::fetch_basic_logger("MAC")) {}

void harq_proc::init(uint32_t id_)
{
  id = id_;
}

void harq_proc::reset(uint32_t tb_idx)
{
  ack_state[tb_idx] = false;
  active[tb_idx]    = false;
  n_rtx[tb_idx]     = 0;
  tti               = tti_point{0};
  last_mcs[tb_idx]  = -1;
  last_tbs[tb_idx]  = -1;
  tx_cnt[tb_idx]    = 0;
}

bool harq_proc::is_empty() const
{
  for (uint32_t i = 0; i < SRSRAN_MAX_TB; ++i) {
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
  return active[tb_idx] && not ack_state[tb_idx];
}

tti_point harq_proc::get_tti() const
{
  return tti;
}

int harq_proc::set_ack_common(uint32_t tb_idx, bool ack_)
{
  if (is_empty(tb_idx)) {
    logger->warning("Received ACK for inactive harq");
    return SRSRAN_ERROR;
  }
  ack_state[tb_idx] = ack_;
  logger->debug("ACK=%d received pid=%d, tb_idx=%d, n_rtx=%d, max_retx=%d", ack_, id, tb_idx, n_rtx[tb_idx], max_retx);
  if (ack_) {
    active[tb_idx] = false;
  }
  return SRSRAN_SUCCESS;
}

void harq_proc::new_tx_common(uint32_t tb_idx, tti_point tti_, int mcs, int tbs, uint32_t max_retx_)
{
  reset(tb_idx);
  ndi[tb_idx] = !ndi[tb_idx];
  tti         = tti_;
  max_retx    = max_retx_;
  tx_cnt[tb_idx]++;
  last_mcs[tb_idx] = mcs;
  last_tbs[tb_idx] = tbs;

  active[tb_idx] = true;
}

void harq_proc::new_retx_common(uint32_t tb_idx, tti_point tti_, int* mcs, int* tbs)
{
  ack_state[tb_idx] = false;
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
  for (bool& tb : active) {
    tb = false;
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

void dl_harq_proc::new_tti(tti_point tti_tx_dl)
{
  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; ++tb) {
    if (has_pending_retx(tb, tti_tx_dl) and nof_retx(tb) + 1 >= max_nof_retx()) {
      logger->info("SCHED: discarding DL TB=%d pid=%d, tti=%d, maximum number of retx exceeded (%d)",
                   tb,
                   get_id(),
                   tti.to_uint(),
                   max_retx);
      active[tb] = false;
    }
  }
}

void dl_harq_proc::new_tx(const rbgmask_t& new_mask,
                          uint32_t         tb_idx,
                          tti_point        tti_tx_dl,
                          int              mcs,
                          int              tbs,
                          uint32_t         n_cce_,
                          uint32_t         max_retx_)
{
  n_cce   = n_cce_;
  rbgmask = new_mask;
  new_tx_common(tb_idx, tti_tx_dl, mcs, tbs, max_retx_);
}

void dl_harq_proc::new_retx(const rbgmask_t& new_mask,
                            uint32_t         tb_idx,
                            tti_point        tti_tx_dl,
                            int*             mcs,
                            int*             tbs,
                            uint32_t         n_cce_)
{
  n_cce   = n_cce_;
  rbgmask = new_mask;
  new_retx_common(tb_idx, tti_tx_dl, mcs, tbs);
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

bool dl_harq_proc::has_pending_retx(uint32_t tb_idx, tti_point tti_tx_dl) const
{
  return (tti_tx_dl >= to_tx_dl_ack(tti)) and has_pending_retx_common(tb_idx);
}

bool dl_harq_proc::has_pending_retx(tti_point tti_tx_dl) const
{
  return has_pending_retx(0, tti_tx_dl) or has_pending_retx(1, tti_tx_dl);
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
 *                  UE::UL HARQ class                 *
 ******************************************************/

void ul_harq_proc::new_tti()
{
  if (has_pending_retx() and nof_retx(0) + 1 >= max_nof_retx()) {
    logger->info(
        "SCHED: discarding UL pid=%d, tti=%d, maximum number of retx exceeded (%d)", get_id(), tti.to_uint(), max_retx);
    active[0] = false;
    if (not pending_phich) {
      reset_pending_data();
    }
  }
}

prb_interval ul_harq_proc::get_alloc() const
{
  return allocation;
}

bool ul_harq_proc::has_pending_retx() const
{
  return has_pending_retx_common(0);
}

void ul_harq_proc::new_tx(tti_point tti_, int mcs, int tbs, prb_interval alloc, uint32_t max_retx_, bool is_msg3)
{
  allocation = alloc;
  new_tx_common(0, tti_point{tti_}, mcs, tbs, max_retx_);
  pending_data    = tbs;
  pending_phich   = true;
  is_msg3_        = is_msg3;
  pdcch_requested = false;
}

void ul_harq_proc::new_retx(tti_point tti_, int* mcs, int* tbs, prb_interval alloc)
{
  // If PRBs changed, or there was no tx in last oportunity (e.g. HARQ is being resumed)
  allocation = alloc;
  new_retx_common(0, tti_point{tti_}, mcs, tbs);
  pending_phich   = true;
  pdcch_requested = false;
}

bool ul_harq_proc::retx_requires_pdcch(tti_point tti_, prb_interval alloc) const
{
  // Adaptive retx if: (1) PRBs changed, (2) HARQ resumed due to last PUSCH retx being skipped (3) HARQ resumed due to
  // last PHICH alloc being skipped (e.g. due to measGaps)
  return alloc != allocation or pdcch_requested;
}

bool ul_harq_proc::set_ack(uint32_t tb_idx, bool ack_)
{
  if (is_empty()) {
    return false;
  }
  set_ack_common(tb_idx, ack_);
  if (is_empty(0) and not pending_phich) {
    reset_pending_data();
  }
  return true;
}

bool ul_harq_proc::has_pending_phich() const
{
  return pending_phich;
}

void ul_harq_proc::request_pdcch()
{
  pdcch_requested = true;
}

void ul_harq_proc::retx_skipped()
{
  // Note: This function should be called in case of PHICH allocation is successful
  // Flagging "PDCCH required" for next retx, as HARQ is being resumed
  pdcch_requested = true;
  n_rtx[0]++;
}

bool ul_harq_proc::pop_pending_phich()
{
  srsran_assert(pending_phich, "pop_pending_phich called for HARQ with no pending PHICH");
  bool ret      = ack_state[0];
  pending_phich = false;
  if (is_empty(0)) {
    // fully reset HARQ info once PHICH is dispatched for an acked / maxretx reached HARQ
    reset_pending_data();
  }
  return ret;
}

void ul_harq_proc::reset_pending_data()
{
  reset_pending_data_common();
  pending_data    = 0;
  is_msg3_        = false;
  pdcch_requested = false;
}

uint32_t ul_harq_proc::get_pending_data() const
{
  return is_empty() ? 0 : (uint32_t)pending_data;
}

/********************
 *   Harq Entity
 *******************/

harq_entity::harq_entity(size_t nof_dl_harqs, size_t nof_ul_harqs) : dl_harqs(nof_dl_harqs), ul_harqs(nof_ul_harqs)
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
    for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
      h.reset(tb);
    }
  }
  for (auto& h : ul_harqs) {
    for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
      h.reset(tb);
      // The reset_pending_data() is called after reset(), when generating PHICH. However, in the case of full HARQ
      // reset (e.g. during handover) no PHICH is going to be generated.
      h.reset_pending_data();
    }
  }
}

void harq_entity::new_tti(tti_point tti_rx)
{
  last_ttis[tti_rx.to_uint() % last_ttis.size()] = tti_rx;
  get_ul_harq(to_tx_ul(tti_rx))->new_tti();
  for (auto& hdl : dl_harqs) {
    hdl.new_tti(to_tx_dl(tti_rx));
  }
}

dl_harq_proc* harq_entity::get_empty_dl_harq(tti_point tti_tx_dl)
{
  if (not is_async) {
    dl_harq_proc* h = &dl_harqs[tti_tx_dl.to_uint() % nof_dl_harqs()];
    return h->is_empty() ? h : nullptr;
  }

  auto it = std::find_if(dl_harqs.begin(), dl_harqs.end(), [](dl_harq_proc& h) { return h.is_empty(); });
  return it != dl_harqs.end() ? &(*it) : nullptr;
}

dl_harq_proc* harq_entity::get_pending_dl_harq(tti_point tti_tx_dl)
{
  if (not is_async) {
    dl_harq_proc* h = &dl_harqs[tti_tx_dl.to_uint() % nof_dl_harqs()];
    return (h->has_pending_retx(0, tti_tx_dl) or h->has_pending_retx(1, tti_tx_dl)) ? h : nullptr;
  }
  return get_oldest_dl_harq(tti_tx_dl);
}

std::tuple<uint32_t, int, int> harq_entity::set_ack_info(tti_point tti_rx, uint32_t tb_idx, bool ack)
{
  for (auto& h : dl_harqs) {
    if (h.get_tti() + FDD_HARQ_DELAY_DL_MS == tti_rx) {
      if (h.set_ack(tb_idx, ack) == SRSRAN_SUCCESS) {
        return std::make_tuple(h.get_id(), h.get_tbs(tb_idx), h.get_mcs(tb_idx));
      }
      return std::make_tuple(h.get_id(), -1, -1);
    }
  }
  return std::make_tuple(dl_harqs.size(), -1, -1);
}

ul_harq_proc* harq_entity::get_ul_harq(tti_point tti_tx_ul)
{
  return &ul_harqs[tti_tx_ul.to_uint() % ul_harqs.size()];
}

int harq_entity::set_ul_crc(tti_point tti_rx, uint32_t tb_idx, bool ack_)
{
  ul_harq_proc* h   = get_ul_harq(tti_rx);
  uint32_t      pid = h->get_id();
  return h->set_ack(tb_idx, ack_) ? pid : -1;
}

void harq_entity::finish_tti(tti_point tti_rx)
{
  // Reset UL HARQ if no retxs
  auto* hul = get_ul_harq(to_tx_ul(tti_rx));
  if (not hul->is_empty() and hul->max_nof_retx() == 0) {
    hul->reset_pending_data();
  }

  // Reset DL harq which has 0 retxs
  for (auto& h : dl_harqs) {
    if (not h.is_empty() and h.max_nof_retx() == 0) {
      // reuse harqs with no retxs
      h.reset_pending_data();
    }
  }
}

/**
 * Get the oldest DL Harq Proc that has pending retxs
 * @param tti_tx_dl assumed to always be equal or ahead in time in comparison to current harqs
 * @return pointer to found dl_harq
 */
dl_harq_proc* harq_entity::get_oldest_dl_harq(tti_point tti_tx_dl)
{
  int      oldest_idx = -1;
  uint32_t oldest_tti = 0;
  for (const dl_harq_proc& h : dl_harqs) {
    tti_point ack_tti_rx = h.get_tti() + FDD_HARQ_DELAY_DL_MS;
    if (h.has_pending_retx(tti_tx_dl) and (last_ttis[ack_tti_rx.to_uint() % last_ttis.size()] == ack_tti_rx)) {
      uint32_t x = tti_tx_dl - h.get_tti();
      if (x > oldest_tti) {
        oldest_idx = h.get_id();
        oldest_tti = x;
      }
    }
  }
  return (oldest_idx >= 0) ? &dl_harqs[oldest_idx] : nullptr;
}

} // namespace srsenb
