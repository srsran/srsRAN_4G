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

#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_ue_cell.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsenb/hdr/stack/mac/sched_phy_ch/sched_dci.h"
#include <numeric>

namespace srsenb {

/*******************************************************
 *                sched_ue_cell
 *******************************************************/

sched_ue_cell::sched_ue_cell(uint16_t rnti_, const sched_cell_params_t& cell_cfg_, tti_point current_tti_) :
  logger(srslog::fetch_basic_logger("MAC")),
  rnti(rnti_),
  cell_cfg(&cell_cfg_),
  dci_locations(generate_cce_location_table(rnti_, cell_cfg_)),
  harq_ent(SCHED_MAX_HARQ_PROC, SCHED_MAX_HARQ_PROC),
  tpc_fsm(cell_cfg->nof_prb(),
          cell_cfg->cfg.target_pucch_ul_sinr,
          cell_cfg->cfg.target_pusch_ul_sinr,
          cell_cfg->cfg.enable_phr_handling),
  fixed_mcs_dl(cell_cfg_.sched_cfg->pdsch_mcs),
  fixed_mcs_ul(cell_cfg_.sched_cfg->pusch_mcs),
  current_tti(current_tti_),
  max_aggr_level(cell_cfg_.sched_cfg->max_aggr_level >= 0 ? cell_cfg_.sched_cfg->max_aggr_level : 3)
{
  clear_feedback();
}

void sched_ue_cell::set_ue_cfg(const sched_interface::ue_cfg_t& ue_cfg_)
{
  static const std::array<uint32_t, 3> max_64qam_mcs{20, 24, 28};

  cfg_tti            = current_tti;
  ue_cfg             = &ue_cfg_;
  int prev_ue_cc_idx = ue_cc_idx;
  ue_cc_idx          = -1;
  for (size_t i = 0; i < ue_cfg_.supported_cc_list.size(); ++i) {
    if (ue_cfg_.supported_cc_list[i].enb_cc_idx == cell_cfg->enb_cc_idx) {
      ue_cc_idx = i;
    }
  }
  if (ue_cc_idx < 0 and prev_ue_cc_idx < 0) {
    return;
  }

  // set max mcs
  max_mcs_ul = cell_cfg->sched_cfg->pusch_max_mcs >= 0 ? cell_cfg->sched_cfg->pusch_max_mcs : 28U;
  if (cell_cfg->cfg.enable_64qam) {
    max_mcs_ul = std::min(max_mcs_ul, max_64qam_mcs[(size_t)ue_cfg->support_ul64qam]);
  }
  max_mcs_dl = cell_cfg->sched_cfg->pdsch_max_mcs >= 0 ? std::min(cell_cfg->sched_cfg->pdsch_max_mcs, 28) : 28U;
  if (ue_cfg->use_tbs_index_alt) {
    max_mcs_dl = std::min(max_mcs_dl, 27U);
  }

  // If new cell configuration, clear Cell HARQs
  if (ue_cc_idx != prev_ue_cc_idx) {
    clear_feedback();
    harq_ent.reset();
  }

  // Update carrier state
  if (ue_cc_idx == 0) {
    // PCell is always active
    cc_state_ = cc_st::active;
  } else {
    // SCell case
    switch (cc_state()) {
      case cc_st::activating:
      case cc_st::active:
        if (ue_cc_idx < 0 or not ue_cfg->supported_cc_list[ue_cc_idx].active) {
          cc_state_ = cc_st::deactivating;
          logger.info("SCHED: Deactivating rnti=0x%x, SCellIndex=%d...", rnti, ue_cc_idx);
        }
        break;
      case cc_st::deactivating:
      case cc_st::idle:
        if (ue_cc_idx > 0 and ue_cfg->supported_cc_list[ue_cc_idx].active) {
          cc_state_ = cc_st::activating;
          dl_cqi    = 0;
          logger.info("SCHED: Activating rnti=0x%x, SCellIndex=%d...", rnti, ue_cc_idx);
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
      clear_feedback();
      harq_ent.reset();
    }
  }
}

void sched_ue_cell::clear_feedback()
{
  dl_ri         = 0;
  dl_ri_tti_rx  = tti_point{};
  dl_pmi        = 0;
  dl_pmi_tti_rx = tti_point{};
  dl_cqi        = ue_cc_idx == 0 ? cell_cfg->cfg.initial_dl_cqi : 1;
  dl_cqi_tti_rx = tti_point{};
  dl_cqi_rx     = false;
  ul_cqi        = 1;
  ul_cqi_tti_rx = tti_point{};
}

void sched_ue_cell::finish_tti(tti_point tti_rx)
{
  // clear_feedback PIDs with pending data or blocked
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
    logger.info("SCHED: SCell index=%d is now active", ue_cc_idx);
  }
}

/*************************************************************
 *                    TBS/MCS derivation
 ************************************************************/

/**
 * Implementation of false position method to iteratively find zero of given monotonic discrete function
 * @tparam YType type of y axis
 * @tparam Callable callable with interface "YType function(int)"
 * @tparam ErrorDetect callable with interface "bool function(YType)"
 * @param x1 min x value of input interval
 * @param x2 max x value of input interval
 * @param y0 target y value
 * @param f monotonic function "YType f(int)" that crosses zero within [x1, x2]
 * @param is_error returns true if an error has been detected
 * @return solution of false position. It contains the interval when "f(x)" crossed y0. In case,
 *         - f(x2) <= y0 -> return is tuple(x2, y2, x2, y2)
 *         - f(x1) >= y0 -> return is tuple(x1, x1, x1, x1)
 *         - x' in ]x1, x2[, such that f(x') < y0 and f(x'+1) > y0 -> return is tuple(x', f(x'), x'+1, f(x'+1))
 *         - x' in ]x1, x2[, such that f(x') == y0 -> return is tuple(x', f(x'), x', f(x'))
 */
template <typename YType, typename Callable, typename ErrorDetect>
std::tuple<int, YType, int, YType>
false_position_method(int x1, int x2, YType y0, const Callable& f, const ErrorDetect& is_error)
{
  static_assert(std::is_same<YType, decltype(f(x1))>::value,
                "The type of the final result and callable return do not match");
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
      }
      x3--;
      y3 = y3_1;
    } else if (x3 == x1) {
      y3 = y1;
      // check if in frontier
      YType y3_1 = f(x3 + 1);
      if (not is_error(y3_1) and y3_1 >= y0) {
        return std::make_tuple(x3, y3, x3 + 1, y3_1);
      }
      x3++;
      y3 = y3_1;
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

tbs_info cqi_to_tbs_dl(const sched_ue_cell& cell,
                       uint32_t             nof_prb,
                       uint32_t             nof_re,
                       srsran_dci_format_t  dci_format,
                       int                  req_bytes)
{
  bool use_tbs_index_alt = cell.get_ue_cfg()->use_tbs_index_alt and dci_format != SRSRAN_DCI_FORMAT1A;

  tbs_info ret;
  if (cell.fixed_mcs_dl < 0 or not cell.dl_cqi_rx) {
    // Dynamic MCS
    ret = compute_min_mcs_and_tbs_from_required_bytes(
        nof_prb, nof_re, cell.dl_cqi, cell.max_mcs_dl, req_bytes, false, false, use_tbs_index_alt);

    // If coderate > SRSRAN_MIN(max_coderate, 0.932 * Qm) we should set TBS=0. We don't because it's not correctly
    // handled by the scheduler, but we might be scheduling undecodable codewords at very low SNR
    if (ret.tbs_bytes < 0) {
      ret.mcs       = 0;
      ret.tbs_bytes = get_tbs_bytes((uint32_t)ret.mcs, nof_prb, use_tbs_index_alt, false);
    }
  } else {
    // Fixed MCS
    ret.mcs       = cell.fixed_mcs_dl;
    ret.tbs_bytes = get_tbs_bytes((uint32_t)cell.fixed_mcs_dl, nof_prb, use_tbs_index_alt, false);
  }
  return ret;
}

tbs_info cqi_to_tbs_ul(const sched_ue_cell& cell, uint32_t nof_prb, uint32_t nof_re, int req_bytes, int explicit_mcs)
{
  using ul64qam_cap    = sched_interface::ue_cfg_t::ul64qam_cap;
  int  mcs             = explicit_mcs >= 0 ? explicit_mcs : cell.fixed_mcs_ul;
  bool ulqam64_enabled = cell.get_ue_cfg()->support_ul64qam == ul64qam_cap::enabled;

  tbs_info ret;
  if (mcs < 0) {
    // Dynamic MCS
    ret = compute_min_mcs_and_tbs_from_required_bytes(
        nof_prb, nof_re, cell.ul_cqi, cell.max_mcs_ul, req_bytes, true, ulqam64_enabled, false);

    // If coderate > SRSRAN_MIN(max_coderate, 0.932 * Qm) we should set TBS=0. We don't because it's not correctly
    // handled by the scheduler, but we might be scheduling undecodable codewords at very low SNR
    if (ret.tbs_bytes < 0) {
      ret.mcs       = 0;
      ret.tbs_bytes = get_tbs_bytes((uint32_t)ret.mcs, nof_prb, false, true);
    }
  } else {
    // Fixed MCS
    ret.mcs       = mcs;
    ret.tbs_bytes = get_tbs_bytes((uint32_t)mcs, nof_prb, false, true);
  }

  return ret;
}

int get_required_prb_dl(const sched_ue_cell& cell,
                        tti_point            tti_tx_dl,
                        srsran_dci_format_t  dci_format,
                        uint32_t             req_bytes)
{
  auto compute_tbs_approx = [tti_tx_dl, &cell, dci_format](uint32_t nof_prb) {
    uint32_t nof_re = cell.cell_cfg->get_dl_lb_nof_re(tti_tx_dl, nof_prb);
    tbs_info tb     = cqi_to_tbs_dl(cell, nof_prb, nof_re, dci_format, -1);
    return tb.tbs_bytes;
  };

  std::tuple<uint32_t, int, uint32_t, int> ret = false_position_method(
      1U, cell.cell_cfg->nof_prb(), (int)req_bytes, compute_tbs_approx, [](int y) { return y == SRSRAN_ERROR; });
  int      upper_tbs  = std::get<3>(ret);
  uint32_t upper_nprb = std::get<2>(ret);
  return (upper_tbs < 0) ? 0 : ((upper_tbs < (int)req_bytes) ? -1 : static_cast<int>(upper_nprb));
}

uint32_t get_required_prb_ul(const sched_ue_cell& cell, uint32_t req_bytes)
{
  if (req_bytes == 0) {
    return 0;
  }
  auto compute_tbs_approx = [&cell](uint32_t nof_prb) {
    const uint32_t N_srs  = 0;
    uint32_t       nof_re = (2 * (SRSRAN_CP_NSYMB(cell.cell_cfg->cfg.cell.cp) - 1) - N_srs) * nof_prb * SRSRAN_NRE;
    return cqi_to_tbs_ul(cell, nof_prb, nof_re, -1).tbs_bytes;
  };

  // find nof prbs that lead to a tbs just above req_bytes
  int                                      target_tbs = static_cast<int>(req_bytes) + 4;
  uint32_t                                 max_prbs   = std::min(cell.tpc_fsm.max_ul_prbs(), cell.cell_cfg->nof_prb());
  std::tuple<uint32_t, int, uint32_t, int> ret =
      false_position_method(1U, max_prbs, target_tbs, compute_tbs_approx, [](int y) { return y == SRSRAN_ERROR; });
  uint32_t req_prbs = std::get<2>(ret);
  while (!srsran_dft_precoding_valid_prb(req_prbs) && req_prbs < cell.cell_cfg->nof_prb()) {
    req_prbs++;
  }
  return req_prbs;
}

} // namespace srsenb
