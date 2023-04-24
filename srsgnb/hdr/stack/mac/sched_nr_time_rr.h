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
