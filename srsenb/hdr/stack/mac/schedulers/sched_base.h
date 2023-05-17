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

#ifndef SRSRAN_SCHED_BASE_H
#define SRSRAN_SCHED_BASE_H

#include "srsenb/hdr/stack/mac/sched_grid.h"
#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_phy_resource.h"

namespace srsenb {

/**
 * Base class for scheduler algorithms implementations
 */
class sched_base
{
public:
  virtual ~sched_base() = default;

  virtual void sched_dl_users(sched_ue_list& ue_db, sf_sched* tti_sched) = 0;
  virtual void sched_ul_users(sched_ue_list& ue_db, sf_sched* tti_sched) = 0;

protected:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("MAC");
};

/**************** Helper methods ****************/

const dl_harq_proc* get_dl_retx_harq(sched_ue& user, sf_sched* tti_sched);
const dl_harq_proc* get_dl_newtx_harq(sched_ue& user, sf_sched* tti_sched);
const ul_harq_proc* get_ul_retx_harq(sched_ue& user, sf_sched* tti_sched);
const ul_harq_proc* get_ul_newtx_harq(sched_ue& user, sf_sched* tti_sched);

/// Helper methods to allocate resources in subframe
alloc_result try_dl_retx_alloc(sf_sched& tti_sched, sched_ue& ue, const dl_harq_proc& h);
alloc_result
             try_dl_newtx_alloc_greedy(sf_sched& tti_sched, sched_ue& ue, const dl_harq_proc& h, rbgmask_t* result_mask = nullptr);
alloc_result try_ul_retx_alloc(sf_sched& tti_sched, sched_ue& ue, const ul_harq_proc& h);

} // namespace srsenb

#endif // SRSRAN_SCHED_BASE_H
