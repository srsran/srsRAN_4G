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

#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_ue_cell.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include <numeric>

namespace srsenb {

/*******************************************************
 *                sched_ue_cell
 *******************************************************/

sched_ue_cell::sched_ue_cell(uint16_t rnti_, const sched_cell_params_t& cell_cfg_, tti_point current_tti_) :
  rnti(rnti_),
  cell_cfg(&cell_cfg_),
  dci_locations(generate_cce_location_table(rnti_, cell_cfg_)),
  harq_ent(SCHED_MAX_HARQ_PROC, SCHED_MAX_HARQ_PROC),
  tpc_fsm(cell_cfg->nof_prb(), cell_cfg->cfg.target_ul_sinr, cell_cfg->cfg.enable_phr_handling),
  fixed_mcs_dl(cell_cfg_.sched_cfg->pdsch_mcs),
  fixed_mcs_ul(cell_cfg_.sched_cfg->pusch_mcs),
  current_tti(current_tti_)
{
  max_aggr_level = cell_cfg->sched_cfg->max_aggr_level >= 0 ? cell_cfg->sched_cfg->max_aggr_level : 3;
}

void sched_ue_cell::set_ue_cfg(const sched_interface::ue_cfg_t& ue_cfg_)
{
  cfg_tti   = current_tti;
  ue_cfg    = &ue_cfg_;
  ue_cc_idx = -1;
  for (size_t i = 0; i < ue_cfg_.supported_cc_list.size(); ++i) {
    if (ue_cfg_.supported_cc_list[i].enb_cc_idx == cell_cfg->enb_cc_idx) {
      ue_cc_idx = i;
    }
  }

  // set max mcs
  max_mcs_ul = cell_cfg->sched_cfg->pusch_max_mcs >= 0 ? cell_cfg->sched_cfg->pusch_max_mcs : 28u;
  if (cell_cfg->cfg.enable_64qam) {
    const uint32_t max_64qam_mcs[] = {20, 24, 28};
    max_mcs_ul                     = std::min(max_mcs_ul, max_64qam_mcs[(size_t)ue_cfg->support_ul64qam]);
  }
  max_mcs_dl = cell_cfg->sched_cfg->pdsch_max_mcs >= 0 ? std::min(cell_cfg->sched_cfg->pdsch_max_mcs, 28) : 28u;
  if (ue_cfg->use_tbs_index_alt) {
    max_mcs_dl = std::min(max_mcs_dl, 27u);
  }

  // Update carrier state
  if (ue_cc_idx == 0) {
    if (cc_state() == cc_st::idle) {
      reset();
      // PCell is always active
      cc_state_ = cc_st::active;

      // set initial DL CQI
      dl_cqi = cell_cfg->cfg.initial_dl_cqi;
    }
  } else {
    // SCell case
    switch (cc_state()) {
      case cc_st::activating:
      case cc_st::active:
        if (not ue_cfg->supported_cc_list[ue_cc_idx].active) {
          cc_state_ = cc_st::deactivating;
          log_h->info("SCHED: Deactivating rnti=0x%x, SCellIndex=%d...\n", rnti, ue_cc_idx);
        }
        break;
      case cc_st::deactivating:
      case cc_st::idle:
        if (ue_cfg->supported_cc_list[ue_cc_idx].active) {
          reset();
          cc_state_ = cc_st::activating;
          dl_cqi    = 0;
          log_h->info("SCHED: Activating rnti=0x%x, SCellIndex=%d...\n", rnti, ue_cc_idx);
        }
        break;
      default:
        break;
    }
  }
}

void sched_ue_cell::new_tti(tti_point tti_rx)
{
  current_tti = tti_rx;

  // Check if cell state needs to be updated
  if (ue_cc_idx > 0 and cc_state_ == cc_st::deactivating) {
    // wait for all ACKs to be received before completely deactivating SCell
    if (current_tti > to_tx_dl_ack(cfg_tti)) {
      cc_state_ = cc_st::idle;
    }
  }
}

void sched_ue_cell::reset()
{
  dl_ri         = 0;
  dl_ri_tti_rx  = tti_point{};
  dl_pmi        = 0;
  dl_pmi_tti_rx = tti_point{};
  dl_cqi        = 1;
  dl_cqi_tti_rx = tti_point{};
  dl_cqi_rx     = false;
  ul_cqi        = 1;
  ul_cqi_tti_rx = tti_point{};
  harq_ent.reset();
}

void sched_ue_cell::finish_tti(tti_point tti_rx)
{
  // reset PIDs with pending data or blocked
  harq_ent.reset_pending_data(tti_rx);
}

void sched_ue_cell::set_dl_cqi(tti_point tti_rx, uint32_t dl_cqi_)
{
  dl_cqi        = dl_cqi_;
  dl_cqi_tti_rx = tti_rx;
  dl_cqi_rx     = dl_cqi_rx or dl_cqi > 0;
  if (ue_cc_idx > 0 and cc_state_ == cc_st::activating and dl_cqi_rx) {
    // Wait for SCell to receive a positive CQI before activating it
    cc_state_ = cc_st::active;
    log_h->info("SCHED: SCell index=%d is now active\n", ue_cc_idx);
  }
}

/*************************************************************
 *                    TBS/MCS derivation
 ************************************************************/

template <typename YType, typename Callable, typename ErrorDetect>
std::tuple<int, YType, int, YType>
false_position_method(int x1, int x2, YType y0, const Callable& f, const ErrorDetect& is_error)
{
  static_assert(std::is_same<YType, decltype(f(x1))>::value,
                "The type of the final result and callable return do not match\n");
  YType y1 = f(x1);
  if (is_error(y1) or y1 >= y0) {
    return std::make_tuple(x1, y1, x1, y1);
  }
  YType y2 = f(x2);
  if (is_error(y2) or y2 <= y0) {
    return std::make_tuple(x2, y2, x2, y2);
  }
  YType y3;
  while (x2 > x1 + 1) {
    int x3 = round(x1 - ((x2 - x1) * (y1 - y0) / (float)(y2 - y1)));
    if (x3 == x2) {
      y3 = y2;
      // check if in frontier
      YType y3_1 = f(x3 - 1);
      if (not is_error(y3_1) and y3_1 < y0) {
        return std::make_tuple(x3 - 1, y3_1, x3, y3);
      } else {
        x3--;
        y3 = y3_1;
      }
    } else if (x3 == x1) {
      y3 = y1;
      // check if in frontier
      YType y3_1 = f(x3 + 1);
      if (not is_error(y3_1) and y3_1 >= y0) {
        return std::make_tuple(x3, y3, x3 + 1, y3_1);
      } else {
        x3++;
        y3 = y3_1;
      }
    } else {
      y3 = f(x3);
      if (is_error(y3) or y3 == y0) {
        return std::make_tuple(x3, y3, x3, y3);
      }
    }
    if (y3 < y0) {
      x1 = x3;
      y1 = y3;
    } else {
      x2 = x3;
      y2 = y3;
    }
  }
  return std::make_tuple(x1, y1, x2, y2);
}
template <typename YType, typename Callable>
std::tuple<int, YType, int, YType> false_position_method(int x1, int x2, YType y0, const Callable& f)
{
  return false_position_method(x1, x2, y0, f, [](int x) { return false; });
}

float diff_coderate_maxcoderate(int      mcs,
                                uint32_t nof_prb,
                                uint32_t nof_re,
                                uint32_t max_Qm,
                                float    max_coderate,
                                bool     use_tbs_index_alt,
                                bool     is_ul)
{
  uint32_t     tbs_idx  = srslte_ra_tbs_idx_from_mcs(mcs, use_tbs_index_alt, is_ul);
  int          tbs      = srslte_ra_tbs_from_idx(tbs_idx, nof_prb);
  float        coderate = srslte_coderate(tbs, nof_re);
  srslte_mod_t mod      = (is_ul) ? srslte_ra_ul_mod_from_mcs(mcs) : srslte_ra_dl_mod_from_mcs(mcs, use_tbs_index_alt);
  uint32_t     Qm       = std::min(max_Qm, srslte_mod_bits_x_symbol(mod));
  return coderate - std::min(max_coderate, 0.930f * Qm);
}

tbs_info cqi_to_tbs(const sched_ue_cell& cell, uint32_t nof_prb, uint32_t nof_re, bool is_ul)
{
  using ul64qam_cap = sched_interface::ue_cfg_t::ul64qam_cap;
  uint32_t max_Qm;
  int      max_mcs;
  float    max_coderate;
  if (is_ul) {
    max_mcs      = cell.max_mcs_ul;
    max_Qm       = cell.get_ue_cfg()->support_ul64qam == ul64qam_cap::enabled ? 6 : 4;
    max_coderate = srslte_cqi_to_coderate(std::min(cell.ul_cqi + 1u, 15u), false);
  } else {
    max_mcs      = cell.max_mcs_dl;
    max_Qm       = cell.get_ue_cfg()->use_tbs_index_alt ? 8 : 6;
    max_coderate = srslte_cqi_to_coderate(std::min(cell.dl_cqi + 1u, 15u), cell.get_ue_cfg()->use_tbs_index_alt);
  }

  // function with sign-flip at solution
  auto test_mcs = [&](int sel_mcs) -> float {
    return diff_coderate_maxcoderate(
        sel_mcs, nof_prb, nof_re, max_Qm, max_coderate, cell.get_ue_cfg()->use_tbs_index_alt, is_ul);
  };

  std::tuple<int, float, int, float> ret;
  if (nof_prb > 1) {
    // for non-voip case
    ret = false_position_method(0, max_mcs, 0.0f, test_mcs);
  } else {
    // avoid 6 prbs (voip case), where function is not monotonic
    ret = false_position_method(7, max_mcs, 0.0f, test_mcs);
    if (std::get<1>(ret) > 0) {
      ret = false_position_method(0, 5, 0.0f, test_mcs);
    }
  }
  tbs_info ret2;
  ret2.mcs       = std::get<0>(ret);
  ret2.tbs_bytes = get_tbs_bytes(ret2.mcs, nof_prb, cell.get_ue_cfg()->use_tbs_index_alt, is_ul);

  // If coderate > SRSLTE_MIN(max_coderate, 0.930 * Qm) we should set TBS=0. We don't because it's not correctly
  // handled by the scheduler, but we might be scheduling undecodable codewords at very low SNR

  return ret2;
}

/* In this scheduler we tend to use all the available bandwidth and select the MCS
 * that approximates the minimum between the capacity and the requested rate
 */
tbs_info alloc_tbs(const sched_ue_cell& cell, uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, bool is_ul)
{
  // TODO: Compute real spectral efficiency based on PUSCH-UCI configuration
  tbs_info ret = cqi_to_tbs(cell, nof_prb, nof_re, is_ul);

  /* If less bytes are requested, lower the MCS */
  if (ret.tbs_bytes > (int)req_bytes && req_bytes > 0) {
    int req_tbs_idx = srslte_ra_tbs_to_table_idx(req_bytes * 8, nof_prb);
    int req_mcs     = srslte_ra_mcs_from_tbs_idx(req_tbs_idx, cell.get_ue_cfg()->use_tbs_index_alt, is_ul);
    while (cell.get_ue_cfg()->use_tbs_index_alt and req_mcs < 0 and req_tbs_idx < 33) {
      // some tbs_idx are invalid for 256QAM. See TS 36.213 - Table 7.1.7.1-1A
      req_mcs = srslte_ra_mcs_from_tbs_idx(++req_tbs_idx, cell.get_ue_cfg()->use_tbs_index_alt, is_ul);
    }

    if (req_mcs >= 0 and req_mcs < (int)ret.mcs) {
      uint32_t max_Qm =
          (is_ul) ? (cell.get_ue_cfg()->support_ul64qam == sched_interface::ue_cfg_t::ul64qam_cap::enabled ? 6 : 4)
                  : (cell.get_ue_cfg()->use_tbs_index_alt ? 8 : 6);
      float max_coderate =
          (is_ul) ? srslte_cqi_to_coderate(std::min(cell.ul_cqi + 1u, 15u), false)
                  : srslte_cqi_to_coderate(std::min(cell.dl_cqi + 1u, 15u), cell.get_ue_cfg()->use_tbs_index_alt);
      if (diff_coderate_maxcoderate(
              req_mcs, nof_prb, nof_re, max_Qm, max_coderate, cell.get_ue_cfg()->use_tbs_index_alt, is_ul) < 0) {
        ret.mcs       = req_mcs;
        ret.tbs_bytes = srslte_ra_tbs_from_idx(req_tbs_idx, nof_prb) / 8;
      }
    }
  }
  // Avoid the unusual case n_prb=1, mcs=6 tbs=328 (used in voip)
  if (nof_prb == 1 && ret.mcs == 6) {
    ret.mcs--;
    ret.tbs_bytes = get_tbs_bytes(ret.mcs, nof_prb, cell.get_ue_cfg()->use_tbs_index_alt, is_ul);
  }

  return ret;
}

tbs_info alloc_tbs_dl(const sched_ue_cell& cell, uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes)
{
  tbs_info ret;

  // Use a higher MCS for the Msg4 to fit in the 6 PRB case
  if (cell.fixed_mcs_dl < 0 or not cell.dl_cqi_rx) {
    // Dynamic MCS
    ret = alloc_tbs(cell, nof_prb, nof_re, req_bytes, false);
  } else {
    // Fixed MCS
    ret.mcs       = cell.fixed_mcs_dl;
    ret.tbs_bytes = get_tbs_bytes((uint32_t)cell.fixed_mcs_dl, nof_prb, cell.get_ue_cfg()->use_tbs_index_alt, false);
  }
  return ret;
}

tbs_info
alloc_tbs_ul(const sched_ue_cell& cell, uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, int explicit_mcs)
{
  tbs_info ret;
  int      mcs = explicit_mcs >= 0 ? explicit_mcs : cell.fixed_mcs_ul;

  if (mcs < 0) {
    // Dynamic MCS
    ret = alloc_tbs(cell, nof_prb, nof_re, req_bytes, true);
  } else {
    // Fixed MCS
    ret.mcs       = mcs;
    ret.tbs_bytes = get_tbs_bytes((uint32_t)mcs, nof_prb, false, true);
  }

  return ret;
}

int get_required_prb_dl(const sched_ue_cell& cell, tti_point tti_tx_dl, uint32_t req_bytes)
{
  auto compute_tbs_approx = [tti_tx_dl, &cell](uint32_t nof_prb) {
    uint32_t nof_re = cell.cell_cfg->get_dl_lb_nof_re(tti_tx_dl, nof_prb);
    tbs_info tb     = alloc_tbs_dl(cell, nof_prb, nof_re, 0);
    return tb.tbs_bytes;
  };

  std::tuple<uint32_t, int, uint32_t, int> ret = false_position_method(
      1u, cell.cell_cfg->nof_prb(), (int)req_bytes, compute_tbs_approx, [](int y) { return y == SRSLTE_ERROR; });
  int      upper_tbs  = std::get<3>(ret);
  uint32_t upper_nprb = std::get<2>(ret);
  return (upper_tbs < 0) ? 0 : ((upper_tbs < (int)req_bytes) ? -1 : upper_nprb);
}

uint32_t get_required_prb_ul(const sched_ue_cell& cell, uint32_t req_bytes)
{
  if (req_bytes == 0) {
    return 0;
  }
  auto compute_tbs_approx = [&cell](uint32_t nof_prb) {
    const uint32_t N_srs  = 0;
    uint32_t       nof_re = (2 * (SRSLTE_CP_NSYMB(cell.cell_cfg->cfg.cell.cp) - 1) - N_srs) * nof_prb * SRSLTE_NRE;
    return alloc_tbs_ul(cell, nof_prb, nof_re, 0).tbs_bytes;
  };

  // find nof prbs that lead to a tbs just above req_bytes
  int                                      target_tbs = req_bytes + 4;
  uint32_t                                 max_prbs   = std::min(cell.tpc_fsm.max_ul_prbs(), cell.cell_cfg->nof_prb());
  std::tuple<uint32_t, int, uint32_t, int> ret =
      false_position_method(1u, max_prbs, target_tbs, compute_tbs_approx, [](int y) { return y == SRSLTE_ERROR; });
  uint32_t req_prbs = std::get<2>(ret);
  while (!srslte_dft_precoding_valid_prb(req_prbs) && req_prbs < cell.cell_cfg->nof_prb()) {
    req_prbs++;
  }
  return req_prbs;
}

} // namespace srsenb
