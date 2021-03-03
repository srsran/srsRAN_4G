/**
 *
 * \section copyright
 *
 * copyright 2013-2020 software radio systems limited
 *
 * by using this file, you agree to the terms and conditions set
 * forth in the license file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_SCHED_DCI_H
#define SRSLTE_SCHED_DCI_H

#include <cstdint>

namespace srsenb {

struct tbs_info {
  int tbs_bytes = -1;
  int mcs       = 0;
};

tbs_info compute_mcs_and_tbs(uint32_t nof_prb,
                             uint32_t nof_re,
                             uint32_t cqi,
                             uint32_t max_mcs,
                             bool     is_ul,
                             bool     ulqam64_enabled,
                             bool     use_tbs_index_alt);

} // namespace srsenb

#endif // SRSLTE_SCHED_DCI_H
