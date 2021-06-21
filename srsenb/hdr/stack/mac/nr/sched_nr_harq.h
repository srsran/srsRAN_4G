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

#include "srsran/common/tti_point.h"
#include <array>

namespace srsenb {
namespace sched_nr_impl {

template <size_t NofTbs>
class harq
{
public:
  harq() = default;

  bool empty() const
  {
    return std::all_of(tb.begin(), tb.end(), [](const tb_t t) { return not t.active; });
  }
  bool empty(uint32_t tb_idx) const { return tb[tb_idx].active; }

private:
  struct tb_t {
    bool     active    = false;
    bool     ack_state = false;
    bool     ndi       = false;
    uint32_t n_rtx     = 0;
    uint32_t mcs       = 0;
  };

  uint32_t                 id;
  tti_point                tti_tx;
  std::array<tb_t, NofTbs> tb;
};

class harq_entity
{
public:
  void dl_ack_info(tti_point tti_rx, uint32_t tb_idx, bool ack) {}

private:
  std::array<harq<1>, 16> dl_harqs;
  std::array<harq<1>, 16> ul_harqs;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_HARQ_H
