/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_SCHED_BASE_H
#define SRSLTE_SCHED_BASE_H

#include "srsenb/hdr/stack/mac/sched_grid.h"

namespace srsenb {

/**
 * Base class for scheduler algorithms implementations
 */
class sched_base
{
public:
  virtual ~sched_base() = default;

  virtual void sched_dl_users(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched) = 0;
  virtual void sched_ul_users(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched) = 0;

protected:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("MAC");
};

/**************** Helper methods ****************/

/**
 * Finds a bitmask of available RBG resources for a given UE in a greedy fashion
 * @param ue UE being allocated
 * @param enb_cc_idx carrier index
 * @param current_mask bitmask of occupied RBGs, where to search for available RBGs
 * @return bitmask of found RBGs. If a valid mask wasn't found, bitmask::size() == 0
 */
rbgmask_t compute_user_rbgmask_greedy(sched_ue& ue, uint32_t enb_cc_idx, const rbgmask_t& current_mask);

/**
 * Finds a range of L contiguous PRBs that are empty
 * @param L Size of the requested UL PRBs
 * @param current_mask input prb mask where to search for available PRBs
 * @return found interval of PRBs
 */
prb_interval find_contiguous_ul_prbs(uint32_t L, const prbmask_t& current_mask);

const dl_harq_proc* get_dl_retx_harq(sched_ue& user, sf_sched* tti_sched);
const dl_harq_proc* get_dl_newtx_harq(sched_ue& user, sf_sched* tti_sched);
const ul_harq_proc* get_ul_retx_harq(sched_ue& user, sf_sched* tti_sched);
const ul_harq_proc* get_ul_newtx_harq(sched_ue& user, sf_sched* tti_sched);

/// Helper methods to allocate resources in subframe
alloc_outcome_t try_dl_retx_alloc(sf_sched& tti_sched, sched_ue& ue, const dl_harq_proc& h);
alloc_outcome_t
                try_dl_newtx_alloc_greedy(sf_sched& tti_sched, sched_ue& ue, const dl_harq_proc& h, rbgmask_t* result_mask = nullptr);
alloc_outcome_t try_ul_retx_alloc(sf_sched& tti_sched, sched_ue& ue, const ul_harq_proc& h);

} // namespace srsenb

#endif // SRSLTE_SCHED_BASE_H
