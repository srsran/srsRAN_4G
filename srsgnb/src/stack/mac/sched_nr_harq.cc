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

#include "srsgnb/hdr/stack/mac/sched_nr_harq.h"
#include "srsran/common/buffer_pool.h"

namespace srsenb {
namespace sched_nr_impl {

int harq_proc::ack_info(uint32_t tb_idx, bool ack)
{
  if (empty(tb_idx)) {
    return SRSRAN_ERROR;
  }
  tb[tb_idx].ack_state = ack;
  if (ack) {
    tb[tb_idx].active = false;
  }
  return ack ? tb[tb_idx].tbs : 0;
}

bool harq_proc::clear_if_maxretx(slot_point slot_rx)
{
  if (has_pending_retx(slot_rx) and nof_retx() + 1 > max_nof_retx()) {
    tb[0].active = false;
    return true;
  }
  return false;
}

void harq_proc::reset()
{
  tb[0].ack_state = false;
  tb[0].active    = false;
  tb[0].n_rtx     = 0;
  tb[0].mcs       = std::numeric_limits<uint32_t>::max();
  tb[0].tbs       = std::numeric_limits<uint32_t>::max();
}

bool harq_proc::new_tx(slot_point       slot_tx_,
                       slot_point       slot_ack_,
                       const prb_grant& grant,
                       uint32_t         mcs,
                       uint32_t         max_retx_)
{
  if (not empty()) {
    return false;
  }
  reset();
  max_retx     = max_retx_;
  slot_tx      = slot_tx_;
  slot_ack     = slot_ack_;
  prbs_        = grant;
  tb[0].ndi    = !tb[0].ndi;
  tb[0].mcs    = mcs;
  tb[0].tbs    = 0;
  tb[0].active = true;
  return true;
}

bool harq_proc::set_tbs(uint32_t tbs)
{
  if (empty() or nof_retx() > 0) {
    return false;
  }
  tb[0].tbs = tbs;
  return true;
}

bool harq_proc::set_mcs(uint32_t mcs)
{
  if (empty() or nof_retx() > 0) {
    return false;
  }
  tb[0].mcs = mcs;
  return true;
}

bool harq_proc::new_retx(slot_point slot_tx_, slot_point slot_ack_, const prb_grant& grant)
{
  if (grant.is_alloc_type0() != prbs_.is_alloc_type0() or
      (grant.is_alloc_type0() and grant.rbgs().count() != prbs_.rbgs().count()) or
      (grant.is_alloc_type1() and grant.prbs().length() != prbs_.prbs().length())) {
    return false;
  }
  if (new_retx(slot_tx_, slot_ack_)) {
    prbs_ = grant;
    return true;
  }
  return false;
}

bool harq_proc::new_retx(slot_point slot_tx_, slot_point slot_ack_)
{
  if (empty()) {
    return false;
  }
  slot_tx         = slot_tx_;
  slot_ack        = slot_ack_;
  tb[0].ack_state = false;
  tb[0].n_rtx++;
  return true;
}

dl_harq_proc::dl_harq_proc(uint32_t id_, uint32_t nprb) :
  harq_proc(id_), softbuffer(harq_softbuffer_pool::get_instance().get_tx(nprb)), pdu(srsran::make_byte_buffer())
{}

void dl_harq_proc::fill_dci(srsran_dci_dl_nr_t& dci)
{
  const static uint32_t rv_idx[4] = {0, 2, 3, 1};

  dci.pid = pid;
  dci.ndi = ndi();
  dci.mcs = mcs();
  dci.rv  = rv_idx[nof_retx() % 4];
  if (dci.ctx.format == srsran_dci_format_nr_1_0) {
    dci.harq_feedback = (slot_ack - slot_tx) - 1;
  } else {
    dci.harq_feedback = slot_tx.to_uint();
  }
}

bool dl_harq_proc::new_tx(slot_point          slot_tx,
                          slot_point          slot_ack,
                          const prb_grant&    grant,
                          uint32_t            mcs_,
                          uint32_t            max_retx,
                          srsran_dci_dl_nr_t& dci)
{
  const static uint32_t rv_idx[4] = {0, 2, 3, 1};

  if (harq_proc::new_tx(slot_tx, slot_ack, grant, mcs_, max_retx)) {
    pdu->clear();
    fill_dci(dci);
    return true;
  }
  return false;
}

bool dl_harq_proc::new_retx(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant, srsran_dci_dl_nr_t& dci)
{
  if (harq_proc::new_retx(slot_tx, slot_ack, grant)) {
    fill_dci(dci);
    return true;
  }
  return false;
}

void ul_harq_proc::fill_dci(srsran_dci_ul_nr_t& dci)
{
  const static uint32_t rv_idx[4] = {0, 2, 3, 1};

  dci.pid = pid;
  dci.ndi = ndi();
  dci.mcs = mcs();
  dci.rv  = rv_idx[nof_retx() % 4];
}

bool ul_harq_proc::new_tx(slot_point          slot_tx,
                          const prb_grant&    grant,
                          uint32_t            mcs_,
                          uint32_t            max_retx,
                          srsran_dci_ul_nr_t& dci)
{
  const static uint32_t rv_idx[4] = {0, 2, 3, 1};

  if (harq_proc::new_tx(slot_tx, slot_tx, grant, mcs_, max_retx)) {
    fill_dci(dci);
    return true;
  }
  return false;
}

bool ul_harq_proc::new_retx(slot_point slot_tx, const prb_grant& grant, srsran_dci_ul_nr_t& dci)
{
  if (harq_proc::new_retx(slot_tx, slot_tx, grant)) {
    fill_dci(dci);
    return true;
  }
  return false;
}

harq_entity::harq_entity(uint16_t rnti_, uint32_t nprb, uint32_t nof_harq_procs, srslog::basic_logger& logger_) :
  rnti(rnti_), logger(logger_)
{
  // Create HARQs
  dl_harqs.reserve(nof_harq_procs);
  ul_harqs.reserve(nof_harq_procs);
  for (uint32_t pid = 0; pid < nof_harq_procs; ++pid) {
    dl_harqs.emplace_back(pid, nprb);
    ul_harqs.emplace_back(pid, nprb);
  }
}

void harq_entity::new_slot(slot_point slot_rx_)
{
  slot_rx = slot_rx_;
  for (harq_proc& dl_h : dl_harqs) {
    if (dl_h.clear_if_maxretx(slot_rx)) {
      logger.info("SCHED: discarding rnti=0x%x, DL TB pid=%d. Cause: Maximum number of retx exceeded (%d)",
                  rnti,
                  dl_h.pid,
                  dl_h.max_nof_retx());
    }
  }
  for (harq_proc& ul_h : ul_harqs) {
    if (ul_h.clear_if_maxretx(slot_rx)) {
      logger.info("SCHED: discarding rnti=0x%x, UL TB pid=%d. Cause: Maximum number of retx exceeded (%d)",
                  rnti,
                  ul_h.pid,
                  ul_h.max_nof_retx());
    }
  }
}

} // namespace sched_nr_impl
} // namespace srsenb