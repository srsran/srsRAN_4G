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
