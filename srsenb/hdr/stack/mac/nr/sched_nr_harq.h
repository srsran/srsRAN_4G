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

#ifndef SRSRAN_SCHED_NR_HARQ_H
#define SRSRAN_SCHED_NR_HARQ_H

#include "sched_nr_cfg.h"
#include "srsenb/hdr/stack/mac/nr/harq_softbuffer.h"
#include "srsran/common/slot_point.h"
#include <array>

namespace srsenb {
namespace sched_nr_impl {

class harq_proc
{
public:
  explicit harq_proc(uint32_t id_) : pid(id_) {}

  bool empty() const
  {
    return std::all_of(tb.begin(), tb.end(), [](const tb_t& t) { return not t.active; });
  }
  bool empty(uint32_t tb_idx) const { return not tb[tb_idx].active; }
  bool has_pending_retx(slot_point slot_rx) const
  {
    return not empty() and not tb[0].ack_state and slot_ack <= slot_rx;
  }
  uint32_t         nof_retx() const { return tb[0].n_rtx; }
  uint32_t         max_nof_retx() const { return max_retx; }
  uint32_t         tbs() const { return tb[0].tbs; }
  uint32_t         ndi() const { return tb[0].ndi; }
  uint32_t         mcs() const { return tb[0].mcs; }
  const prb_grant& prbs() const { return prbs_; }
  slot_point       harq_slot_ack() const { return slot_ack; }

  bool ack_info(uint32_t tb_idx, bool ack);

  void new_slot(slot_point slot_rx);
  void reset();
  bool new_tx(slot_point       slot_tx,
              slot_point       slot_ack,
              const prb_grant& grant,
              uint32_t         mcs,
              uint32_t         tbs,
              uint32_t         max_retx);
  bool new_retx(slot_point slot_tx, slot_point slot_ack, const prb_grant& grant);
  bool new_retx(slot_point slot_tx, slot_point slot_ack);

  // NOTE: Has to be used before first tx is dispatched
  bool set_tbs(uint32_t tbs);

  const uint32_t pid;

private:
  struct tb_t {
    bool     active    = false;
    bool     ack_state = false;
    bool     ndi       = false;
    uint32_t n_rtx     = 0;
    uint32_t mcs       = 0;
    uint32_t tbs       = 0;
  };

  uint32_t                          max_retx = 1;
  slot_point                        slot_tx;
  slot_point                        slot_ack;
  prb_grant                         prbs_;
  std::array<tb_t, SCHED_NR_MAX_TB> tb;
};

class dl_harq_proc : public harq_proc
{
public:
  dl_harq_proc(uint32_t id_, uint32_t nprb);

  tx_harq_softbuffer&           get_softbuffer() { return *softbuffer; }
  srsran::unique_byte_buffer_t* get_tx_pdu() { return &pdu; }

  bool new_tx(slot_point       slot_tx,
              slot_point       slot_ack,
              const prb_grant& grant,
              uint32_t         mcs,
              uint32_t         tbs,
              uint32_t         max_retx);

private:
  srsran::unique_pool_ptr<tx_harq_softbuffer> softbuffer;
  srsran::unique_byte_buffer_t                pdu;
};

class ul_harq_proc : public harq_proc
{
public:
  ul_harq_proc(uint32_t id_, uint32_t nprb) :
    harq_proc(id_), softbuffer(harq_softbuffer_pool::get_instance().get_rx(nprb))
  {}

  rx_harq_softbuffer& get_softbuffer() { return *softbuffer; }

  bool set_tbs(uint32_t tbs)
  {
    softbuffer->reset(tbs * 8u);
    return harq_proc::set_tbs(tbs);
  }

private:
  srsran::unique_pool_ptr<rx_harq_softbuffer> softbuffer;
};

class harq_entity
{
public:
  explicit harq_entity(uint32_t nprb, uint32_t nof_harq_procs = SCHED_NR_MAX_HARQ);
  void new_slot(slot_point slot_rx_);

  void dl_ack_info(uint32_t pid, uint32_t tb_idx, bool ack) { dl_harqs[pid].ack_info(tb_idx, ack); }
  void ul_crc_info(uint32_t pid, bool ack) { ul_harqs[pid].ack_info(0, ack); }

  dl_harq_proc* find_pending_dl_retx()
  {
    return find_dl([this](const dl_harq_proc& h) { return h.has_pending_retx(slot_rx); });
  }
  ul_harq_proc* find_pending_ul_retx()
  {
    return find_ul([this](const ul_harq_proc& h) { return h.has_pending_retx(slot_rx); });
  }
  dl_harq_proc* find_empty_dl_harq()
  {
    return find_dl([](const dl_harq_proc& h) { return h.empty(); });
  }
  ul_harq_proc* find_empty_ul_harq()
  {
    return find_ul([](const ul_harq_proc& h) { return h.empty(); });
  }

private:
  template <typename Predicate>
  dl_harq_proc* find_dl(Predicate p)
  {
    auto it = std::find_if(dl_harqs.begin(), dl_harqs.end(), p);
    return (it == dl_harqs.end()) ? nullptr : &(*it);
  }
  template <typename Predicate>
  ul_harq_proc* find_ul(Predicate p)
  {
    auto it = std::find_if(ul_harqs.begin(), ul_harqs.end(), p);
    return (it == ul_harqs.end()) ? nullptr : &(*it);
  }

  slot_point                slot_rx;
  std::vector<dl_harq_proc> dl_harqs;
  std::vector<ul_harq_proc> ul_harqs;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_HARQ_H
