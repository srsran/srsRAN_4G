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

#include "sched_nr_common.h"
#include "srsran/common/tti_point.h"
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
  bool has_pending_retx(tti_point tti_rx) const { return not empty() and not tb[0].ack_state and tti_ack <= tti_rx; }
  uint32_t nof_retx() const { return tb[0].n_rtx; }
  uint32_t max_nof_retx() const { return max_retx; }
  uint32_t tbs() const { return tb[0].tbs; }
  uint32_t ndi() const { return tb[0].ndi; }
  uint32_t mcs() const { return tb[0].mcs; }

  bool ack_info(uint32_t tb_idx, bool ack);

  void new_tti(tti_point tti_rx);
  void reset();
  bool
       new_tx(tti_point tti_tx, tti_point tti_ack, const rbgmask_t& rbgmask, uint32_t mcs, uint32_t tbs, uint32_t max_retx);
  bool new_retx(tti_point tti_tx, tti_point tti_ack, const rbgmask_t& rbgmask, int* mcs, int* tbs);

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
  tti_point                         tti_tx;
  tti_point                         tti_ack;
  rbgmask_t                         rbgmask;
  std::array<tb_t, SCHED_NR_MAX_TB> tb;
};

class harq_entity
{
public:
  explicit harq_entity(uint32_t nof_harq_procs = SCHED_NR_MAX_HARQ);
  void new_tti(tti_point tti_rx_);

  void dl_ack_info(uint32_t pid, uint32_t tb_idx, bool ack) { dl_harqs[pid].ack_info(tb_idx, ack); }

  harq_proc* find_pending_dl_retx()
  {
    return find_dl([this](const harq_proc& h) { return h.has_pending_retx(tti_rx); });
  }
  harq_proc* find_pending_ul_retx()
  {
    return find_ul([this](const harq_proc& h) { return h.has_pending_retx(tti_rx); });
  }
  harq_proc* find_empty_dl_harq()
  {
    return find_dl([](const harq_proc& h) { return h.empty(); });
  }
  harq_proc* find_empty_ul_harq()
  {
    return find_ul([](const harq_proc& h) { return h.empty(); });
  }

private:
  template <typename Predicate>
  harq_proc* find_dl(Predicate p)
  {
    auto it = std::find_if(dl_harqs.begin(), dl_harqs.end(), p);
    return (it == dl_harqs.end()) ? nullptr : &(*it);
  }
  template <typename Predicate>
  harq_proc* find_ul(Predicate p)
  {
    auto it = std::find_if(ul_harqs.begin(), ul_harqs.end(), p);
    return (it == ul_harqs.end()) ? nullptr : &(*it);
  }

  tti_point              tti_rx;
  std::vector<harq_proc> dl_harqs;
  std::vector<harq_proc> ul_harqs;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_HARQ_H
