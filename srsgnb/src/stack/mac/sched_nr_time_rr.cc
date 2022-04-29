/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsgnb/hdr/stack/mac/sched_nr_time_rr.h"

namespace srsenb {
namespace sched_nr_impl {

/**
 * @brief Algorithm to select next UE to allocate in a time-domain RR fashion
 * @param ue_db map of "slot_ue"
 * @param rr_count starting index to select next UE
 * @param p callable with signature "bool(slot_ue&)" that returns true if UE allocation was successful
 * @return true if a UE was allocated
 */
template <typename Predicate>
bool round_robin_apply(slot_ue_map_t& ue_db, uint32_t rr_count, Predicate p)
{
  if (ue_db.empty()) {
    return false;
  }
  auto it = ue_db.begin();
  std::advance(it, (rr_count % ue_db.size()));
  for (uint32_t count = 0; count < ue_db.size(); ++count, ++it) {
    if (it == ue_db.end()) {
      // wrap-around
      it = ue_db.begin();
    }
    if (p(it->second)) {
      return true;
    }
  }
  return false;
}

void sched_nr_time_rr::sched_dl_users(slot_ue_map_t& ue_db, bwp_slot_allocator& slot_alloc)
{
  // Start with retxs
  auto retx_ue_function = [&slot_alloc](slot_ue& ue) {
    if (ue.h_dl != nullptr and ue.h_dl->has_pending_retx(slot_alloc.get_tti_rx())) {
      alloc_result res = slot_alloc.alloc_pdsch(ue, ue->find_ss_id(srsran_dci_format_nr_1_0), ue.h_dl->prbs());
      if (res == alloc_result::success) {
        return true;
      }
    }
    return false;
  };
  if (round_robin_apply(ue_db, slot_alloc.get_pdcch_tti().to_uint(), retx_ue_function)) {
    return;
  }

  // Move on to new txs
  auto newtx_ue_function = [&slot_alloc](slot_ue& ue) {
    if (ue.dl_bytes > 0 and ue.h_dl != nullptr and ue.h_dl->empty()) {
      int ss_id = ue->find_ss_id(srsran_dci_format_nr_1_0);
      if (ss_id < 0) {
        return false;
      }
      prb_grant    prbs = find_optimal_dl_grant(slot_alloc, ue, ss_id);
      alloc_result res  = slot_alloc.alloc_pdsch(ue, ss_id, prbs);
      if (res == alloc_result::success) {
        return true;
      }
    }
    return false;
  };
  round_robin_apply(ue_db, slot_alloc.get_pdcch_tti().to_uint(), newtx_ue_function);
}

void sched_nr_time_rr::sched_ul_users(slot_ue_map_t& ue_db, bwp_slot_allocator& slot_alloc)
{
  // Start with retxs
  if (round_robin_apply(ue_db, slot_alloc.get_pdcch_tti().to_uint(), [&slot_alloc](slot_ue& ue) {
        if (ue.h_ul != nullptr and ue.h_ul->has_pending_retx(slot_alloc.get_tti_rx())) {
          alloc_result res = slot_alloc.alloc_pusch(ue, ue.h_ul->prbs());
          if (res == alloc_result::success) {
            return true;
          }
        }
        return false;
      })) {
    return;
  }

  // Move on to new txs
  round_robin_apply(ue_db, slot_alloc.get_pdcch_tti().to_uint(), [&slot_alloc](slot_ue& ue) {
    if (ue.ul_bytes > 0 and ue.h_ul != nullptr and ue.h_ul->empty()) {
      alloc_result res = slot_alloc.alloc_pusch(ue, prb_interval{0, slot_alloc.cfg.cfg.rb_width});
      if (res == alloc_result::success) {
        return true;
      }
    }
    return false;
  });
}

} // namespace sched_nr_impl
} // namespace srsenb
