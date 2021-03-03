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

#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_dci.h"
#include "srsenb/hdr/stack/mac/sched_common.h"
#include <cmath>
#include <cstdint>

namespace srsenb {

/// Compute max TBS based on max coderate
int coderate_to_tbs(float max_coderate, uint32_t nof_re)
{
  return floorf(nof_re * max_coderate - 24);
}

tbs_info compute_mcs_and_tbs_recursive(uint32_t nof_prb,
                                       uint32_t nof_re,
                                       uint32_t max_mcs,
                                       bool     is_ul,
                                       bool     use_tbs_index_alt,
                                       float    max_coderate)
{
  // compute max MCS based on max coderate
  int max_tbs = coderate_to_tbs(max_coderate, nof_re);
  if (max_tbs <= 0) {
    return tbs_info{};
  }
  int tbs_idx = srslte_ra_tbs_to_table_idx(max_tbs, nof_prb);
  if (tbs_idx <= 0) {
    return tbs_info{};
  }
  tbs_idx -= 1; // get TBS index lower bound
  uint32_t mcs = srslte_ra_mcs_from_tbs_idx(tbs_idx, use_tbs_index_alt, is_ul);
  if (mcs > max_mcs) {
    // bound mcs
    mcs     = max_mcs;
    tbs_idx = srslte_ra_tbs_idx_from_mcs(mcs, use_tbs_index_alt, is_ul);
  }

  // compute real TBS based on max MCS
  int   tbs      = srslte_ra_tbs_from_idx(tbs_idx, nof_prb);
  float coderate = srslte_coderate(tbs, nof_re);

  srslte_mod_t mod = (is_ul) ? srslte_ra_ul_mod_from_mcs(mcs) : srslte_ra_dl_mod_from_mcs(mcs, use_tbs_index_alt);
  float        new_maxcoderate = std::min(0.930f * srslte_mod_bits_x_symbol(mod), max_coderate);
  if (new_maxcoderate > coderate) {
    tbs_info tb;
    tb.tbs_bytes = tbs / 8u;
    tb.mcs       = mcs;
    return tb;
  }
  if (mcs == 0) {
    // solution not found
    return tbs_info{};
  }
  return compute_mcs_and_tbs_recursive(nof_prb, nof_re, mcs - 1, is_ul, use_tbs_index_alt, new_maxcoderate);
}

tbs_info compute_mcs_and_tbs(uint32_t nof_prb,
                             uint32_t nof_re,
                             uint32_t cqi,
                             uint32_t max_mcs,
                             bool     is_ul,
                             bool     ulqam64_enabled,
                             bool     use_tbs_index_alt)
{
  assert((not is_ul or not use_tbs_index_alt) && "UL cannot use Alt CQI Table");
  assert((is_ul or not ulqam64_enabled) && "DL cannot use UL-QAM64 enable flag");

  float    max_coderate = srslte_cqi_to_coderate(std::min(cqi + 1u, 15u), use_tbs_index_alt);
  uint32_t max_Qm       = (is_ul) ? (ulqam64_enabled ? 6 : 4) : (use_tbs_index_alt ? 8 : 6);
  max_coderate          = std::min(max_coderate, 0.930f * max_Qm);

  return compute_mcs_and_tbs_recursive(nof_prb, nof_re, max_mcs, is_ul, use_tbs_index_alt, max_coderate);
}

} // namespace srsenb
