/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_SCHED_DCI_H
#define SRSLTE_SCHED_DCI_H

#include <cstdint>

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
 * Compute lowest MCS, TBS based on CQI, N_prb that satisfies TBS >= req_bytes
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

} // namespace srsenb

#endif // SRSLTE_SCHED_DCI_H
