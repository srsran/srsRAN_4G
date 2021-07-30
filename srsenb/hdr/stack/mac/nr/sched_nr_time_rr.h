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

#ifndef SRSRAN_SCHED_NR_TIME_RR_H
#define SRSRAN_SCHED_NR_TIME_RR_H

#include "sched_nr_grant_allocator.h"
#include "srsran/common/slot_point.h"

namespace srsenb {
namespace sched_nr_impl {

/**
 * Base class for scheduler algorithms implementations
 */
class sched_nr_base
{
public:
  virtual ~sched_nr_base() = default;

  virtual void sched_dl_users(slot_ue_map_t& ue_db, bwp_slot_allocator& slot_alloc) = 0;
  virtual void sched_ul_users(slot_ue_map_t& ue_db, bwp_slot_allocator& slot_alloc) = 0;

protected:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("MAC");
};

class sched_nr_time_rr : public sched_nr_base
{
public:
  void sched_dl_users(slot_ue_map_t& ue_db, bwp_slot_allocator& slot_alloc) override;
  void sched_ul_users(slot_ue_map_t& ue_db, bwp_slot_allocator& slot_alloc) override;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_TIME_RR_H
