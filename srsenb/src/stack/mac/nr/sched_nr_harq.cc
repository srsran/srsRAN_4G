/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/mac/nr/sched_nr_harq.h"
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
  return tb[tb_idx].tbs;
}

void harq_proc::new_slot(slot_point slot_rx)
{
  if (has_pending_retx(slot_rx) and nof_retx() + 1 >= max_nof_retx()) {
    tb[0].active = false;
  }
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
                       uint32_t         tbs,
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
  tb[0].tbs    = tbs;
  tb[0].active = true;
  return true;
}

bool harq_proc::set_tbs(uint32_t tbs, int mcs)
{
  if (empty() or nof_retx() > 0) {
    return false;
  }
  tb[0].tbs = tbs;
  if (mcs >= 0) {
    tb[0].mcs = mcs;
  }
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

bool dl_harq_proc::new_tx(slot_point       slot_tx,
                          slot_point       slot_ack,
                          const prb_grant& grant,
                          uint32_t         mcs,
                          uint32_t         tbs,
                          uint32_t         max_retx)
{
  if (harq_proc::new_tx(slot_tx, slot_ack, grant, mcs, tbs, max_retx)) {
    softbuffer->reset();
    pdu->clear();
    return true;
  }
  return false;
}

harq_entity::harq_entity(uint32_t nprb, uint32_t nof_harq_procs)
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
    dl_h.new_slot(slot_rx);
  }
  for (harq_proc& ul_h : ul_harqs) {
    ul_h.new_slot(slot_rx);
  }
}

} // namespace sched_nr_impl
} // namespace srsenb