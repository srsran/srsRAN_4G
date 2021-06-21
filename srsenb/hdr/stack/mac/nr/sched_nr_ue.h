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

#ifndef SRSRAN_SCHED_NR_UE_H
#define SRSRAN_SCHED_NR_UE_H

#include "sched_nr_common.h"
#include "sched_nr_harq.h"
#include "sched_nr_interface.h"
#include "srsran/adt/move_callback.h"
#include "srsran/adt/pool/cached_alloc.h"

namespace srsenb {

namespace sched_nr_impl {

class ue_carrier;

class bwp_ue
{
public:
  bwp_ue() = default;
  explicit bwp_ue(ue_carrier& carrier_, tti_point tti_rx_);
  ~bwp_ue();
  bwp_ue(bwp_ue&& other) noexcept : carrier(other.carrier) { other.carrier = nullptr; }
  bwp_ue& operator=(bwp_ue&& other) noexcept
  {
    carrier       = other.carrier;
    other.carrier = nullptr;
    return *this;
  }
  bool empty() const { return carrier == nullptr; }

  tti_point tti_rx;
  uint32_t  cc = SCHED_NR_MAX_CARRIERS;

  const sched_nr_ue_cfg* cfg = nullptr;
  bool                   pending_sr;

private:
  ue_carrier* carrier = nullptr;
};

class ue_carrier
{
public:
  ue_carrier(uint16_t rnti, uint32_t cc, const sched_nr_ue_cfg& cfg);
  bwp_ue try_reserve(tti_point tti_rx);
  void   push_feedback(srsran::move_callback<void(ue_carrier&)> callback);

  const uint16_t rnti;
  const uint32_t cc;

  harq_entity harq_ent;

private:
  friend class bwp_ue;
  void release() { busy = false; }

  const sched_nr_ue_cfg* cfg;

  srsran::deque<srsran::move_callback<void(ue_carrier&)> > pending_feedback;
  bool                                                     busy{false};
};

class ue
{
public:
  ue(uint16_t rnti, const sched_nr_ue_cfg& cfg);

  bwp_ue try_reserve(tti_point tti_rx, uint32_t cc);

  void set_cfg(const sched_nr_ue_cfg& cfg);

  void ul_sr_info(tti_point tti_rx) { pending_sr = true; }

  std::array<std::unique_ptr<ue_carrier>, SCHED_NR_MAX_CARRIERS> carriers;

private:
  bool pending_sr = false;

  int                            current_idx = 0;
  std::array<sched_nr_ue_cfg, 4> ue_cfgs;
};

using ue_map_t = srsran::static_circular_map<uint16_t, std::unique_ptr<ue>, SCHED_NR_MAX_USERS>;

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_UE_H
