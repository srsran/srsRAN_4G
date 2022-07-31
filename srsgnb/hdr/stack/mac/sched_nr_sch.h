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

#ifndef SRSRAN_SCHED_NR_SCH_H
#define SRSRAN_SCHED_NR_SCH_H

#include "srsenb/hdr/stack/mac/sched_common.h"
#include "srsgnb/hdr/stack/mac/sched_nr_cfg.h"
#include "srsgnb/hdr/stack/mac/sched_ue/ue_cfg_manager.h"

namespace srsenb {

namespace sched_nr_impl {

using pdsch_alloc_result = srsran::expected<pdsch_t*, alloc_result>;

class pdsch_allocator
{
public:
  pdsch_allocator(const bwp_params_t& cfg_, uint32_t sl_index, pdsch_list_t& pdsch_lst);

  /// Get available RBGs for allocation
  rbg_bitmap occupied_rbgs() const
  {
    // Note: in case, RBGs are used, dci format is not 1_0
    return dl_prbs.rbgs();
  }
  /// Get available PRBs for allocation
  prb_bitmap occupied_prbs(uint32_t ss_id, srsran_dci_format_nr_t dci_fmt) const
  {
    if (dci_fmt == srsran_dci_format_nr_1_0) {
      const srsran_search_space_t* ss = bwp_cfg.get_ss(ss_id);
      if (ss != nullptr and SRSRAN_SEARCH_SPACE_IS_COMMON(ss->type)) {
        return (dl_prbs | bwp_cfg.dci_fmt_1_0_excluded_prbs(ss->coreset_id)).prbs();
      }
    }
    return dl_prbs.prbs();
  }

  /// Marks a range of PRBS as occupied, preventing further allocations
  void reserve_prbs(const prb_grant& grant) { dl_prbs |= grant; }

  /// Verifies if the input arguments are valid for an SI allocation and grant doesnt collide with other grants
  alloc_result is_si_grant_valid(uint32_t ss_id, const prb_grant& grant) const;

  /// Verifies if the input arguments are valid for an RAR allocation and grant doesnt collide with other grants
  alloc_result is_rar_grant_valid(const prb_grant& grant) const;

  /// Verifies if the input arguments are valid for an UE allocation and grant doesnt collide with other grants
  alloc_result is_ue_grant_valid(const ue_carrier_params_t& ue,
                                 uint32_t                   ss_id,
                                 srsran_dci_format_nr_t     dci_fmt,
                                 const prb_grant&           grant) const;

  /**
   * @brief Tries to allocate UE PDSCH grant. Ensures that there are no collisions with other previous PDSCH allocations
   * @param ss_id[in]  SearchSpaceId used for allocation
   * @param dci_fmt[in] Chosen DL DCI format
   * @param grant[in]  PRBs used for the grant
   * @param ue[in]  UE carrier parameters
   * @param dci[out] DCI where frequency_assignment and time_assignment get stored.
   * @return pdsch_t* of allocated PDSCH in case of success. alloc_result error code in case of failure
   */
  pdsch_alloc_result alloc_ue_pdsch(uint32_t                   ss_id,
                                    srsran_dci_format_nr_t     dci_fmt,
                                    const prb_grant&           grant,
                                    const ue_carrier_params_t& ue,
                                    srsran_dci_dl_nr_t&        dci);

  /// Similar to alloc_ue_pdsch, but it doesn't verify if input parameters are valid
  pdsch_t& alloc_ue_pdsch_unchecked(uint32_t                   ss_id,
                                    srsran_dci_format_nr_t     dci_fmt,
                                    const prb_grant&           grant,
                                    const ue_carrier_params_t& ue,
                                    srsran_dci_dl_nr_t&        dci);

  /**
   * @brief Tries to allocate SI PDSCH grant. Ensures that there are no collisions with other previous PDSCH allocations
   * @param ss_id[in] SearchSpaceId used for allocation
   * @param grant[in] PRBs used for the grant
   * @param dci[out] DCI where frequency_assignment and time_assignment get stored.
   * @return pdsch_t* of allocated PDSCH in case of success. alloc_result error code in case of failure
   */
  pdsch_alloc_result alloc_si_pdsch(uint32_t ss_id, const prb_grant& grant, srsran_dci_dl_nr_t& dci);
  /// Similar to alloc_si_pdsch, but it doesn't verify if input parameters are valid
  pdsch_t& alloc_si_pdsch_unchecked(uint32_t ss_id, const prb_grant& grant, srsran_dci_dl_nr_t& dci);

  /**
   * @brief Tries to allocate RAR PDSCH grant. Ensures that there are no collisions with other previous PDSCH
   * allocations
   * @param grant[in] PRBs used for the grant
   * @param dci[out] DCI where frequency_assignment and time_assignment get stored.
   * @return pdsch_t* of allocated PDSCH in case of success. alloc_result error code in case of failure
   */
  pdsch_alloc_result alloc_rar_pdsch(const prb_grant& grant, srsran_dci_dl_nr_t& dci);
  /// Similar to alloc_rar_pdsch, but it doesn't verify if input parameters are valid
  pdsch_t& alloc_rar_pdsch_unchecked(const prb_grant& grant, srsran_dci_dl_nr_t& dci);

  /// Cancel last PDSCH allocation
  void cancel_last_pdsch();

  /// Clear all PDSCHs
  void reset();

private:
  alloc_result is_grant_valid_common(srsran_search_space_type_t ss_type,
                                     srsran_dci_format_nr_t     dci_fmt,
                                     uint32_t                   coreset_id,
                                     const prb_grant&           grant) const;
  pdsch_t&     alloc_pdsch_unchecked(uint32_t                   coreset_id,
                                     srsran_search_space_type_t ss_type,
                                     srsran_dci_format_nr_t     dci_fmt,
                                     const prb_grant&           grant,
                                     srsran_dci_dl_nr_t&        dci);

  const bwp_params_t& bwp_cfg;
  uint32_t            slot_idx = 0;

  pdsch_list_t& pdschs;
  bwp_rb_bitmap dl_prbs;
};

using pusch_alloc_result = srsran::expected<pusch_t*, alloc_result>;

class pusch_allocator
{
public:
  pusch_allocator(const bwp_params_t& cfg_, uint32_t sl_index, pusch_list_t& pusch_lst);

  /// Get available RBGs for allocation
  const rbg_bitmap& occupied_rbgs() const { return ul_prbs.rbgs(); }
  /// Get available PRBs for allocation
  const prb_bitmap& occupied_prbs() const { return ul_prbs.prbs(); }

  alloc_result has_grant_space(uint32_t nof_grants = 1, bool verbose = true) const;

  /// Checks if provided PDSCH arguments produce a valid PDSCH that fits into cell PRBs and does not collide with other
  /// allocations
  alloc_result is_grant_valid(srsran_search_space_type_t ss_type, const prb_grant& grant, bool verbose = true) const;

  /**
   * @brief Tries to allocate PDSCH grant. Ensures that there are no collisions with other previous PDSCH allocations
   * @param ss_type[in] PDCCH chosen search space type
   * @param grant[in]  PRBs used for the grant
   * @param pdcch[out] DCI where frequency_assignment and time_assignment get stored.
   * @return pdsch_t object pointer in case of success. alloc_result error code in case of failure
   */
  pusch_alloc_result
  alloc_pusch(const srsran_search_space_type_t ss_type, const prb_grant& grant, srsran_dci_ul_nr_t& dci);

  /**
   * @brief Allocates PDSCH grant without verifying for collisions. Useful to avoid redundant is_grant_valid(...) calls
   * @param dci_ctx[in] PDCCH DL DCI context information
   * @param grant[in]  PRBs used for the grant
   * @param pdcch[out] DCI where frequency and time assignment get stored.
   */
  pusch_t& alloc_pusch_unchecked(const prb_grant& grant, srsran_dci_ul_nr_t& dci);

  void cancel_last_pusch();

  void reset();

private:
  const bwp_params_t& bwp_cfg;
  uint32_t            slot_idx = 0;

  pusch_list_t& puschs;
  bwp_rb_bitmap ul_prbs;
};

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_SCH_H
