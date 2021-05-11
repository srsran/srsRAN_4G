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

#ifndef SRSRAN_SCHED_DL_CQI_H
#define SRSRAN_SCHED_DL_CQI_H

#include "srsenb/hdr/stack/mac/sched_common.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsran/adt/accumulators.h"
#include "srsran/common/common_lte.h"
#include "srsran/phy/phch/cqi.h"

namespace srsenb {

/**
 * Class that handles DL CQI state of a given {rnti,sector}
 * - The cell bandwidth is divided into J parts. J = f(nof_cell_prbs)
 * - UE reports wideband CQI every H.Np msec, where Np is the CQI period and H=JK + 1, where K is configured in RRC
 * - Thus, for K==0, only wideband CQI is active
 */
class sched_dl_cqi
{
public:
  sched_dl_cqi(uint32_t cell_nof_prb_, uint32_t K_, uint32_t init_dl_cqi) :
    cell_nof_prb(cell_nof_prb_),
    cell_nof_rbg(cell_nof_prb_to_rbg(cell_nof_prb_)),
    K(K_),
    wb_cqi_avg(init_dl_cqi),
    bp_list(nof_bandwidth_parts(cell_nof_prb_), bandwidth_part_context(init_dl_cqi)),
    subband_cqi(srsran_cqi_hl_get_no_subbands(cell_nof_prb), 0)
  {
    srsran_assert(K <= 4, "K=%d outside of {0, 4}", K);
  }

  void set_K(uint32_t K_)
  {
    srsran_assert(K <= 4, "K=%d outside of {0, 4}", K);
    K = K_;
  }

  void cqi_wb_info(tti_point tti, uint32_t cqi_value)
  {
    if (cqi_value > 0) {
      last_pos_cqi_tti = tti;
    }

    last_wb_tti = tti;
    wb_cqi_avg  = static_cast<float>(cqi_value);
  }

  void cqi_sb_info(tti_point tti, uint32_t sb_index, uint32_t cqi_value)
  {
    if (cqi_value > 0) {
      last_pos_cqi_tti = tti;
    }

    uint32_t bp_idx                      = get_bp_index(sb_index);
    bp_list[bp_idx].last_feedback_tti    = tti;
    bp_list[bp_idx].last_cqi_subband_idx = sb_index;
    bp_list[bp_idx].cqi_val              = static_cast<float>(cqi_value);

    // just cap all sub-bands in the same bandwidth part
    srsran::interval<uint32_t> interv = get_bp_sb_indexes(bp_idx);
    for (uint32_t sb_index2 = interv.start(); sb_index2 < interv.stop(); ++sb_index2) {
      subband_cqi[sb_index2] = bp_list[bp_idx].cqi_val;
    }
  }

  /// Resets CQI to provided value
  void reset_cqi(uint32_t dl_cqi)
  {
    last_pos_cqi_tti = {};
    last_wb_tti      = {};
    wb_cqi_avg       = dl_cqi;
    for (bandwidth_part_context& bp : bp_list) {
      bp = bandwidth_part_context(dl_cqi);
    }
  }

  int get_avg_cqi() const { return get_grant_avg_cqi(rbg_interval(0, cell_nof_rbg)); }

  /// Get average CQI in given RBG interval
  int get_grant_avg_cqi(rbg_interval interv) const
  {
    if (not subband_cqi_enabled()) {
      return static_cast<int>(wb_cqi_avg);
    }
    float    cqi     = 0;
    uint32_t sbstart = rbg_to_sb_index(interv.start()), sbend = rbg_to_sb_index(interv.stop() - 1) + 1;
    for (uint32_t sb = sbstart; sb < sbend; ++sb) {
      cqi += bp_list[get_bp_index(sb)].last_feedback_tti.is_valid() ? subband_cqi[sb] : wb_cqi_avg;
    }
    return static_cast<int>(cqi / (sbend - sbstart));
  }

  /// Get average CQI in given PRB interval
  int get_grant_avg_cqi(prb_interval prb_interv) const
  {
    return get_grant_avg_cqi(rbg_interval::prbs_to_rbgs(prb_interv, cell_nof_prb));
  }

  /// Get average CQI in given RBG mask
  int get_grant_avg_cqi(const rbgmask_t& mask) const
  {
    if (not subband_cqi_enabled()) {
      return static_cast<int>(wb_cqi_avg);
    }
    float    cqi   = 0;
    uint32_t count = 0;
    for (int rbg = mask.find_lowest(0, mask.size()); rbg != -1; rbg = mask.find_lowest(rbg, mask.size())) {
      uint32_t sb = rbg_to_sb_index(rbg);
      cqi += bp_list[get_bp_index(sb)].last_feedback_tti.is_valid() ? subband_cqi[sb] : wb_cqi_avg;
      count++;
      rbg = static_cast<int>(((sb + 1U) * cell_nof_rbg + N() - 1U) / N()); // skip to next subband index
    }
    return static_cast<int>(cqi / count);
  }

  /// Get CQI-optimal RBG mask
  rbgmask_t get_optim_rbg_mask(uint32_t req_rbgs) const
  {
    req_rbgs = std::min(req_rbgs, cell_nof_rbg);
    rbgmask_t mask(cell_nof_rbg);
    if (not subband_cqi_enabled()) {
      mask.fill(0, req_rbgs);
      return mask;
    }
    srsran::bounded_vector<float, max_nof_subbands> sorted_cqis = subband_cqi;
    std::partial_sort(sorted_cqis.begin(), sorted_cqis.begin() + req_rbgs, sorted_cqis.end());
    for (uint32_t i = 0; i < req_rbgs; ++i) {
      mask.set(i);
    }
    return mask;
  }

  /// TS 36.321, 7.2.2 - Parameter N
  uint32_t nof_subbands() const { return subband_cqi.size(); }

  /// TS 36.321, 7.2.2 - Parameter J
  uint32_t nof_bandwidth_parts() const { return bp_list.size(); }

  bool subband_cqi_enabled() const { return K > 0; }

  bool is_cqi_info_received() const { return last_pos_cqi_tti.is_valid(); }

  tti_point last_cqi_info_tti() const { return last_pos_cqi_tti; }

private:
  static const uint32_t max_subband_size    = 8;
  static const uint32_t max_nof_subbands    = 13;
  static const uint32_t max_bandwidth_parts = 4;

  /// TS 36.321, Table 7.2.2-2
  static uint32_t nof_bandwidth_parts(uint32_t nof_prb)
  {
    static const uint32_t nrb[] = {0, 2, 2, 3, 4, 4};
    return nrb[srsran::lte_cell_nof_prb_to_index(nof_prb)];
  }

  uint32_t J() const { return nof_bandwidth_parts(); }
  uint32_t N() const { return nof_subbands(); }

  uint32_t get_bp_index(uint32_t sb_index) const { return sb_index * J() / N(); }

  uint32_t get_sb_index(uint32_t prb_index) const { return prb_index * N() / cell_nof_prb; }

  uint32_t rbg_to_sb_index(uint32_t rbg_index) const { return rbg_index * N() / cell_nof_rbg; }

  srsran::interval<uint32_t> get_bp_sb_indexes(uint32_t bp_idx) const
  {
    return srsran::interval<uint32_t>{bp_idx * N() / J(), (bp_idx + 1) * N() / J()};
  }

  uint32_t cell_nof_prb;
  uint32_t cell_nof_rbg;
  uint32_t K; ///< set in RRC

  /// context of bandwidth part
  struct bandwidth_part_context {
    tti_point last_feedback_tti{};
    uint32_t  last_cqi_subband_idx;
    float     cqi_val;

    explicit bandwidth_part_context(uint32_t init_dl_cqi) : cqi_val(init_dl_cqi), last_cqi_subband_idx(max_nof_subbands)
    {}
  };

  tti_point last_pos_cqi_tti;

  tti_point last_wb_tti;
  float     wb_cqi_avg;

  srsran::bounded_vector<bandwidth_part_context, max_bandwidth_parts> bp_list;
  srsran::bounded_vector<float, max_nof_subbands>                     subband_cqi;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_DL_CQI_H
