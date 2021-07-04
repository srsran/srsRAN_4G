/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_SCHED_DCI_H
#define SRSRAN_SCHED_DCI_H

#include "../sched_lte_common.h"
#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_phy_resource.h"
#include "srsran/adt/bounded_vector.h"

namespace srsenb {

struct tbs_info {
  int tbs_bytes = -1;
  int mcs       = 0;
  tbs_info()    = default;
  tbs_info(int tbs_bytes_, int mcs_) : tbs_bytes(tbs_bytes_), mcs(mcs_) {}
};
inline bool operator==(const tbs_info& lhs, const tbs_info& rhs)
{
  return lhs.mcs == rhs.mcs and lhs.tbs_bytes == rhs.tbs_bytes;
}
inline bool operator!=(const tbs_info& lhs, const tbs_info& rhs)
{
  return not(lhs == rhs);
}

/**
 * Compute MCS, TBS based on CQI, N_prb
 * \remark See TS 36.213 - Table 7.1.7.1-1/1A
 * @return resulting TBS (in bytes) and mcs. TBS=-1 if no valid solution was found.
 */
tbs_info compute_mcs_and_tbs(uint32_t nof_prb,
                             uint32_t nof_re,
                             uint32_t cqi,
                             uint32_t max_mcs,
                             bool     is_ul,
                             bool     ulqam64_enabled,
                             bool     use_tbs_index_alt);

/**
 * Compute MCS, TBS based on maximum coderate, N_prb
 * \remark See TS 36.213 - Table 7.1.7.1-1/1A
 * @return resulting TBS (in bytes) and mcs. TBS=-1 if no valid solution was found.
 */
tbs_info compute_mcs_and_tbs(uint32_t nof_prb,
                             uint32_t nof_re,
                             float    max_coderate,
                             uint32_t max_mcs,
                             bool     is_ul,
                             bool     ulqam64_enabled,
                             bool     use_tbs_index_alt);

/**
 * Compute lowest MCS, TBS based on CQI, N_prb that satisfies TBS >= req_bytes (best effort)
 * \remark See TS 36.213 - Table 7.1.7.1-1/1A
 * @return resulting TBS (in bytes) and mcs. TBS=-1 if no valid solution was found.
 */
tbs_info compute_min_mcs_and_tbs_from_required_bytes(uint32_t nof_prb,
                                                     uint32_t nof_re,
                                                     uint32_t cqi,
                                                     uint32_t max_mcs,
                                                     uint32_t req_bytes,
                                                     bool     is_ul,
                                                     bool     ulqam64_enabled,
                                                     bool     use_tbs_index_alt);

struct pending_rar_t {
  uint16_t                                                                                    ra_rnti = 0;
  tti_point                                                                                   prach_tti{};
  srsran::bounded_vector<sched_interface::dl_sched_rar_info_t, sched_interface::MAX_RAR_LIST> msg3_grant = {};
};

bool generate_sib_dci(sched_interface::dl_sched_bc_t& bc,
                      tti_point                       tti_tx_dl,
                      uint32_t                        sib_idx,
                      uint32_t                        sib_ntx,
                      rbg_interval                    rbg_range,
                      const sched_cell_params_t&      cell_params,
                      uint32_t                        current_cfi);

bool generate_paging_dci(sched_interface::dl_sched_bc_t& bc,
                         tti_point                       tti_tx_dl,
                         uint32_t                        req_bytes,
                         rbg_interval                    rbg_range,
                         const sched_cell_params_t&      cell_params,
                         uint32_t                        current_cfi);

bool generate_rar_dci(sched_interface::dl_sched_rar_t& rar,
                      tti_point                        tti_tx_dl,
                      const pending_rar_t&             pending_rar,
                      rbg_interval                     rbg_range,
                      uint32_t                         nof_grants,
                      uint32_t                         start_msg3_prb,
                      const sched_cell_params_t&       cell_params,
                      uint32_t                         current_cfi);

void log_broadcast_allocation(const sched_interface::dl_sched_bc_t& bc,
                              rbg_interval                          rbg_range,
                              const sched_cell_params_t&            cell_params);

void log_rar_allocation(const sched_interface::dl_sched_rar_t& rar,
                        rbg_interval                           rbg_range,
                        const sched_cell_params_t&             cell_params);

void log_rar_allocation(const sched_interface::dl_sched_rar_t& rar, rbg_interval rbg_range);

} // namespace srsenb

#endif // SRSRAN_SCHED_DCI_H
