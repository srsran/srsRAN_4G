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

#ifndef SRSRAN_SCHED_NR_PDSCH_H
#define SRSRAN_SCHED_NR_PDSCH_H

#include "srsenb/hdr/stack/mac/sched_common.h"
#include "srsgnb/hdr/stack/mac/sched_nr_cfg.h"

namespace srsenb {

namespace sched_nr_impl {

using pdsch_alloc_result = srsran::expected<pdsch_t*, alloc_result>;

class pdsch_allocator
{
public:
  pdsch_allocator(const bwp_params_t& cfg_, uint32_t sl_index, pdsch_list_t& pdsch_lst);

  /// Get available RBGs for allocation
  rbg_bitmap occupied_rbgs(uint32_t ss_id, srsran_dci_format_nr_t dci_fmt) const
  {
    return (dl_prbs | bwp_cfg.coreset_prb_limits(ss_id, dci_fmt)).rbgs();
  }
  /// Get available PRBs for allocation
  prb_bitmap occupied_prbs(uint32_t ss_id, srsran_dci_format_nr_t dci_fmt) const
  {
    return (dl_prbs | bwp_cfg.coreset_prb_limits(ss_id, dci_fmt)).prbs();
  }

  /// Checks if provided PDSCH arguments produce a valid PDSCH that fits into cell PRBs and does not collide with other
  /// allocations
  alloc_result is_grant_valid(uint32_t               ss_id,
                              srsran_dci_format_nr_t dci_fmt,
                              const prb_grant&       grant,
                              ue_carrier_params_t*   ue = nullptr) const;

  /**
   * @brief Tries to allocate PDSCH grant. Ensures that there are no collisions with other previous PDSCH allocations
   * @param dci_ctx[in] PDCCH DL DCI context information
   * @param ss_id[in]  SearchSpaceId used for allocation
   * @param grant[in]  PRBs used for the grant
   * @param pdcch[out] DCI where frequency_assignment and time_assignment get stored.
   * @return pdsch_t object pointer in case of success. alloc_result error code in case of failure
   */
  pdsch_alloc_result
  alloc_pdsch(const srsran_dci_ctx_t& dci_ctx, uint32_t ss_id, const prb_grant& grant, srsran_dci_dl_nr_t& dci);

  /**
   * @brief Allocates PDSCH grant without verifying for collisions. Useful to avoid redundant is_grant_valid(...) calls
   * @param dci_ctx[in] PDCCH DL DCI context information
   * @param grant[in]  PRBs used for the grant
   * @param pdcch[out] DCI where frequency and time assignment get stored.
   */
  pdsch_t& alloc_pdsch_unchecked(const srsran_dci_ctx_t& dci_ctx, const prb_grant& grant, srsran_dci_dl_nr_t& dci);

  void cancel_last_pdsch();

  void reset();

private:
  const bwp_params_t& bwp_cfg;
  uint32_t            slot_idx = 0;

  pdsch_list_t& pdschs;
  bwp_rb_bitmap dl_prbs;
};

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_PDSCH_H
