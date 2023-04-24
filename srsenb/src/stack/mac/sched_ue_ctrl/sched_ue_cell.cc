/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#define CHECK_VALID_CC(feedback_type)                                                                                  \
  do {                                                                                                                 \
    if (cc_state() == cc_st::idle) {                                                                                   \
      logger.warning("SCHED: rnti=0x%x received " feedback_type " for idle cc=%d", rnti, cell_cfg->enb_cc_idx);        \
      return SRSRAN_ERROR;                                                                                             \
    }                                                                                                                  \
  } while (0)

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
  tpc_fsm(rnti_,
          cell_cfg->nof_prb(),
          cell_cfg->cfg.target_pucch_ul_sinr,
          cell_cfg->cfg.target_pusch_ul_sinr,
          cell_cfg->cfg.enable_phr_handling,
          cell_cfg->cfg.min_phr_thres,
          cell_cfg->sched_cfg->min_tpc_tti_interval,
          cell_cfg->sched_cfg->ul_snr_avg_alpha,
          cell_cfg->sched_cfg->init_ul_snr_value),
  fixed_mcs_dl(cell_cfg_.sched_cfg->pdsch_mcs),
  fixed_mcs_ul(cell_cfg_.sched_cfg->pusch_mcs),
  current_tti(current_tti_),
  max_aggr_level(cell_cfg_.sched_cfg->max_aggr_level >= 0 ? cell_cfg_.sched_cfg->max_aggr_level : 3),
  dl_cqi_ctxt(cell_cfg_.nof_prb(), 0, cell_cfg_.sched_cfg->init_dl_cqi)
{
  float target_bler = cell_cfg->sched_cfg->target_bler;
  dl_delta_inc      = cell_cfg->sched_cfg->adaptive_dl_mcs_step_size; // delta_{down} of OLLA
  dl_delta_dec      = (1 - target_bler) * dl_delta_inc / target_bler;
  ul_delta_inc      = cell_cfg->sched_cfg->adaptive_ul_mcs_step_size; // delta_{down} of OLLA
  ul_delta_dec      = (1 - target_bler) * ul_delta_inc / target_bler;
  max_cqi_coeff     = cell_cfg->sched_cfg->max_delta_dl_cqi;
  max_snr_coeff     = cell_cfg->sched_cfg->max_delta_ul_snr;
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
    // CC was inactive and remain inactive
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

  if (ue_cc_idx >= 0) {
    const auto& cc = ue_cfg_.supported_cc_list[ue_cc_idx];
    if (cc.dl_cfg.cqi_report.periodic_configured) {
      dl_cqi_ctxt.set_K(cc.dl_cfg.cqi_report.subband_wideband_ratio);
    }
  }

  // If new cell configuration, clear Cell HARQs
  if (ue_cc_idx != prev_ue_cc_idx) {
    clear_feedback();
    harq_ent.reset();
    logger.info("SCHED: Resetting rnti=0x%x, cc=%d HARQs and feedback state", rnti, cell_cfg->enb_cc_idx);
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
          if (ue_cc_idx > 0) {
            logger.info(
                "SCHED: Deactivating SCell, rnti=0x%x, cc=%d, SCellIndex=%d...", rnti, cell_cfg->enb_cc_idx, ue_cc_idx);
          } else {
            logger.info("SCHED: Deactivating previous PCell, rnti=0x%x, cc=%d...", rnti, cell_cfg->enb_cc_idx);
          }
        }
        break;
      case cc_st::deactivating:
      case cc_st::idle:
        if (ue_cc_idx > 0 and ue_cfg->supported_cc_list[ue_cc_idx].active) {
          cc_state_ = cc_st::activating;
          dl_cqi_ctxt.reset_cqi(0);
          logger.info(
              "SCHED: Activating SCell, rnti=0x%x, cc=%d, SCellIndex=%d...", rnti, cell_cfg->enb_cc_idx, ue_cc_idx);
        }
        break;
      default:
        break;
    }
  }
}

void sched_ue_cell::new_tti(tti_point tti_rx)
{
  if (not configured()) {
    return;
  }
  current_tti = tti_rx;

  harq_ent.new_tti(tti_rx);
  tpc_fsm.new_tti();

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
  dl_cqi_ctxt.reset_cqi(ue_cc_idx == 0 ? cell_cfg->sched_cfg->init_dl_cqi : 1);
  ul_cqi_tti_rx = tti_point{};
}

void sched_ue_cell::finish_tti(tti_point tti_rx)
{
  // clear_feedback PIDs with pending data or blocked
  harq_ent.finish_tti(tti_rx);
}

void sched_ue_cell::check_cc_activation(uint32_t dl_cqi)
{
  if (ue_cc_idx > 0 and cc_state_ == cc_st::activating and dl_cqi > 0) {
    // Wait for SCell to receive a positive CQI before activating it
    cc_state_ = cc_st::active;
    logger.info("SCHED: SCell index=%d is now active", ue_cc_idx);
  }
}

int sched_ue_cell::set_dl_wb_cqi(tti_point tti_rx, uint32_t dl_cqi_)
{
  CHECK_VALID_CC("DL CQI");
  dl_cqi_ctxt.cqi_wb_info(tti_rx, dl_cqi_);
  check_cc_activation(dl_cqi_);
  logger.debug("SCHED: DL CQI cc=%d, cqi=%d", cell_cfg->enb_cc_idx, dl_cqi_);
  return SRSRAN_SUCCESS;
}

int sched_ue_cell::set_dl_sb_cqi(tti_point tti_rx, uint32_t sb_idx, uint32_t dl_cqi_)
{
  CHECK_VALID_CC("DL CQI");
  dl_cqi_ctxt.cqi_sb_info(tti_rx, sb_idx, dl_cqi_);
  check_cc_activation(dl_cqi_);
  logger.debug("SCHED: DL CQI cc=%d, sb_idx=%d, cqi=%d", cell_cfg->enb_cc_idx, sb_idx, dl_cqi_);
  return SRSRAN_SUCCESS;
}

int sched_ue_cell::set_ul_crc(tti_point tti_rx, bool crc_res)
{
  CHECK_VALID_CC("UL CRC");

  // Adapt UL MCS based on BLER
  if (cell_cfg->sched_cfg->target_bler > 0 and fixed_mcs_ul < 0) {
    auto* ul_harq = harq_ent.get_ul_harq(tti_rx);
    if (ul_harq != nullptr) {
      int mcs = ul_harq->get_mcs(0);
      // Note: Avoid keeping increasing the snr delta offset, if MCS is already is at its limit
      float delta_dec_eff = mcs <= 0 ? 0 : ul_delta_dec;
      float delta_inc_eff = mcs >= (int)max_mcs_ul ? 0 : ul_delta_inc;
      ul_snr_coeff += crc_res ? delta_inc_eff : -delta_dec_eff;
      ul_snr_coeff = std::min(std::max(-max_snr_coeff, ul_snr_coeff), max_snr_coeff);
      logger.info("SCHED: UL adaptive link: rnti=0x%x, snr_estim=%.2f, last_mcs=%d, snr_offset=%f",
                  rnti,
                  tpc_fsm.get_ul_snr_estim(),
                  mcs,
                  ul_snr_coeff);
    }
  }

  // Update HARQ process
  int pid = harq_ent.set_ul_crc(tti_rx, 0, crc_res);
  if (pid < 0) {
    logger.warning("SCHED: rnti=0x%x received UL CRC for invalid tti_rx=%d", rnti, (int)tti_rx.to_uint());
    return SRSRAN_ERROR;
  }

  return pid;
}

int sched_ue_cell::set_ack_info(tti_point tti_rx, uint32_t tb_idx, bool ack)
{
  CHECK_VALID_CC("DL ACK Info");

  std::tuple<uint32_t, int, int> p2        = harq_ent.set_ack_info(tti_rx, tb_idx, ack);
  int                            tbs_acked = std::get<1>(p2);
  if (tbs_acked <= 0) {
    logger.warning("SCHED: Received ACK info for unknown TTI=%d", tti_rx.to_uint());
    return tbs_acked;
  }

  // Adapt DL MCS based on BLER
  if (cell_cfg->sched_cfg->target_bler > 0 and fixed_mcs_dl < 0) {
    int mcs = std::get<2>(p2);
    // Note: Avoid keeping increasing the snr delta offset, if MCS is already is at its limit
    float delta_dec_eff = mcs <= 0 ? 0 : dl_delta_dec;
    float delta_inc_eff = mcs >= (int)max_mcs_dl ? 0 : dl_delta_inc;
    dl_cqi_coeff += ack ? delta_inc_eff : -delta_dec_eff;
    dl_cqi_coeff = std::min(std::max(-max_cqi_coeff, dl_cqi_coeff), max_cqi_coeff);
    logger.info("SCHED: DL adaptive link: rnti=0x%x, cqi=%d, last_mcs=%d, cqi_offset=%f",
                rnti,
                dl_cqi_ctxt.get_avg_cqi(),
                mcs,
                dl_cqi_coeff);
  }
  return tbs_acked;
}

int sched_ue_cell::set_ul_snr(tti_point tti_rx, float ul_snr, uint32_t ul_ch_code)
{
  CHECK_VALID_CC("UL SNR estimate");
  if (ue_cfg->ue_bearers[1].direction == mac_lc_ch_cfg_t::IDLE) {
    // Ignore Msg3 SNR samples as Msg3 uses a separate power control loop
    return SRSRAN_SUCCESS;
  }
  tpc_fsm.set_snr(ul_snr, ul_ch_code);
  if (ul_ch_code == tpc::PUSCH_CODE) {
    ul_cqi_tti_rx = tti_rx;
  }
  return SRSRAN_SUCCESS;
}

int sched_ue_cell::get_ul_cqi() const
{
  if (not ul_cqi_tti_rx.is_valid()) {
    return 1;
  }
  float snr = tpc_fsm.get_ul_snr_estim();
  return srsran_cqi_from_snr(snr + ul_snr_coeff);
}

int sched_ue_cell::get_dl_cqi(const rbgmask_t& rbgs) const
{
  int min_cqi;
  find_min_cqi_rbgs(rbgs, dl_cqi_ctxt, min_cqi);
  return std::max(0, (int)std::min(static_cast<float>(min_cqi) + dl_cqi_coeff, 15.0f));
}

int sched_ue_cell::get_dl_cqi() const
{
  return std::max(0, (int)std::min(dl_cqi_ctxt.get_avg_cqi() + dl_cqi_coeff, 15.0f));
}

uint32_t sched_ue_cell::get_aggr_level(uint32_t nof_bits) const
{
  uint32_t dl_cqi = 0;
  if (cell_cfg->sched_cfg->adaptive_aggr_level) {
    dl_cqi = get_dl_cqi();
  } else {
    dl_cqi = dl_cqi_ctxt.get_avg_cqi();
  }
  dl_cqi = std::max(cell_cfg->sched_cfg->pdcch_cqi_offset + (int)dl_cqi, 0);
  return srsenb::get_aggr_level(nof_bits,
                                dl_cqi,
                                cell_cfg->sched_cfg->min_aggr_level,
                                max_aggr_level,
                                cell_cfg->nof_prb(),
                                ue_cfg->use_tbs_index_alt);
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
                       const rbgmask_t&     rbgs,
                       uint32_t             nof_re,
                       srsran_dci_format_t  dci_format,
                       uint32_t             req_bytes)
{
  bool     use_tbs_index_alt = cell.get_ue_cfg()->use_tbs_index_alt and dci_format != SRSRAN_DCI_FORMAT1A;
  uint32_t nof_prbs          = count_prb_per_tb(rbgs);

  tbs_info ret;
  if (cell.fixed_mcs_dl < 0 or not cell.dl_cqi().is_cqi_info_received()) {
    // Dynamic MCS configured or first Tx
    uint32_t dl_cqi = cell.get_dl_cqi(rbgs);

    ret = compute_min_mcs_and_tbs_from_required_bytes(
        nof_prbs, nof_re, dl_cqi, cell.max_mcs_dl, req_bytes, false, false, use_tbs_index_alt);

    // If coderate > SRSRAN_MIN(max_coderate, 0.932 * Qm) we should set TBS=0. We don't because it's not correctly
    // handled by the scheduler, but we might be scheduling undecodable codewords at very low SNR
    if (ret.tbs_bytes < 0) {
      ret.mcs       = 0;
      ret.tbs_bytes = get_tbs_bytes((uint32_t)ret.mcs, nof_prbs, use_tbs_index_alt, false);
    }
  } else {
    // Fixed MCS configured
    ret.mcs       = cell.fixed_mcs_dl;
    ret.tbs_bytes = get_tbs_bytes((uint32_t)cell.fixed_mcs_dl, nof_prbs, use_tbs_index_alt, false);
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
        nof_prb, nof_re, cell.get_ul_cqi(), cell.max_mcs_ul, req_bytes, true, ulqam64_enabled, false);

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
    uint32_t  nof_re       = cell.cell_cfg->get_dl_lb_nof_re(tti_tx_dl, nof_prb);
    rbgmask_t min_cqi_rbgs = cell.dl_cqi().get_optim_rbgmask(nof_prb, false);
    tbs_info  tb           = cqi_to_tbs_dl(cell, min_cqi_rbgs, nof_re, dci_format);
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
  const static int MIN_ALLOC_BYTES = 10; /// There should be enough space for RLC header + BSR + some payload
  if (req_bytes == 0) {
    return 0;
  }
  auto compute_tbs_approx = [&cell](uint32_t nof_prb) {
    const uint32_t N_srs  = 0;
    uint32_t       nof_re = (2 * (SRSRAN_CP_NSYMB(cell.cell_cfg->cfg.cell.cp) - 1) - N_srs) * nof_prb * SRSRAN_NRE;
    return cqi_to_tbs_ul(cell, nof_prb, nof_re, -1).tbs_bytes;
  };

  // find nof prbs that lead to a tbs just above req_bytes
  int                                      target_tbs = std::max(static_cast<int>(req_bytes) + 4, MIN_ALLOC_BYTES);
  uint32_t                                 max_prbs   = std::min(cell.tpc_fsm.max_ul_prbs(), cell.cell_cfg->nof_prb());
  std::tuple<uint32_t, int, uint32_t, int> ret =
      false_position_method(1U, max_prbs, target_tbs, compute_tbs_approx, [](int y) { return y == SRSRAN_ERROR; });
  uint32_t req_prbs  = std::get<2>(ret);
  uint32_t final_tbs = std::get<3>(ret);
  while (final_tbs < MIN_ALLOC_BYTES and req_prbs < cell.cell_cfg->nof_prb()) {
    // Note: If PHR<0 is limiting the max nof PRBs per UL grant, the UL grant may become too small to fit any
    //       data other than headers + BSR. In this edge-case, force an increase the nof required PRBs.
    req_prbs++;
    final_tbs = compute_tbs_approx(req_prbs);
  }
  while (!srsran_dft_precoding_valid_prb(req_prbs) && req_prbs < cell.cell_cfg->nof_prb()) {
    req_prbs++;
  }
  return req_prbs;
}

/// Computes the minimum TBS/MCS achievable for provided UE cell configuration, RBG mask, TTI, DCI format
tbs_info compute_mcs_and_tbs_lower_bound(const sched_ue_cell& ue_cell,
                                         tti_point            tti_tx_dl,
                                         const rbgmask_t&     rbg_mask,
                                         srsran_dci_format_t  dci_format)
{
  uint32_t nof_prbs = count_prb_per_tb(rbg_mask);
  if (nof_prbs == 0) {
    return tbs_info{};
  }
  uint32_t nof_re_lb = ue_cell.cell_cfg->get_dl_lb_nof_re(tti_tx_dl, nof_prbs);
  return cqi_to_tbs_dl(ue_cell, rbg_mask, nof_re_lb, dci_format);
}

bool find_optimal_rbgmask(const sched_ue_cell&       ue_cell,
                          tti_point                  tti_tx_dl,
                          const rbgmask_t&           dl_mask,
                          srsran_dci_format_t        dci_format,
                          srsran::interval<uint32_t> req_bytes,
                          tbs_info&                  tb,
                          rbgmask_t&                 newtxmask)
{
  // Find the largest set of available RBGs possible
  newtxmask = find_available_rbgmask(dl_mask.size(), dci_format == SRSRAN_DCI_FORMAT1A, dl_mask);

  // Compute MCS/TBS if all available RBGs were allocated
  tb = compute_mcs_and_tbs_lower_bound(ue_cell, tti_tx_dl, newtxmask, dci_format);

  if (not ue_cell.dl_cqi().subband_cqi_enabled()) {
    // Wideband CQI case
    // NOTE: for wideband CQI, the TBS is directly proportional to the nof_prbs, so we can use an iterative method
    //       to compute the best mask given "req_bytes"

    if (tb.tbs_bytes < (int)req_bytes.start()) {
      // the grant is too small. it may lead to srb0 segmentation or not space for headers
      return false;
    }
    if (tb.tbs_bytes <= (int)req_bytes.stop()) {
      // the grant is not sufficiently large to fit max required bytes. Stop search at this point
      return true;
    }
    // Reduce DL grant size to the minimum that can fit the pending DL bytes
    srsran::bounded_vector<tbs_info, MAX_NOF_RBGS> tb_table(newtxmask.count());
    auto compute_tbs_approx = [tti_tx_dl, &ue_cell, dci_format, &tb_table](uint32_t nof_rbgs) {
      rbgmask_t search_mask(ue_cell.cell_cfg->nof_rbgs);
      search_mask.fill(0, nof_rbgs);
      tb_table[nof_rbgs - 1] = compute_mcs_and_tbs_lower_bound(ue_cell, tti_tx_dl, search_mask, dci_format);
      return tb_table[nof_rbgs - 1].tbs_bytes;
    };
    std::tuple<uint32_t, int, uint32_t, int> ret = false_position_method(
        1U, tb_table.size(), (int)req_bytes.stop(), compute_tbs_approx, [](int y) { return y == SRSRAN_ERROR; });
    uint32_t upper_nrbg = std::get<2>(ret);
    int      upper_tbs  = std::get<3>(ret);
    if (upper_tbs >= (int)req_bytes.stop()) {
      tb      = tb_table[upper_nrbg - 1];
      int pos = 0;
      for (uint32_t n_rbgs = newtxmask.count(); n_rbgs > upper_nrbg; --n_rbgs) {
        pos = newtxmask.find_lowest(pos + 1, newtxmask.size());
      }
      newtxmask.from_uint64(~((1U << (uint64_t)pos) - 1U) & ((1U << newtxmask.size()) - 1U));
    }
    return true;
  }

  // Subband CQI case
  // NOTE: There is no monotonically increasing guarantee between TBS and nof allocated prbs.
  //       One single subband CQI could be dropping the CQI of the whole TB.
  //       We start with largest RBG allocation and continue removing RBGs. However, there is no guarantee this is
  //       going to be the optimal solution

  // Subtract RBGs with lowest CQI until objective is not met
  // TODO: can be optimized
  rbgmask_t smaller_mask;
  tbs_info  tb2;
  do {
    smaller_mask = remove_min_cqi_rbgs(newtxmask, ue_cell.dl_cqi());
    tb2          = compute_mcs_and_tbs_lower_bound(ue_cell, tti_tx_dl, smaller_mask, dci_format);
    if (tb2.tbs_bytes >= (int)req_bytes.stop() or tb.tbs_bytes <= tb2.tbs_bytes) {
      tb        = tb2;
      newtxmask = smaller_mask;
    }
  } while (tb2.tbs_bytes > (int)req_bytes.stop());
  if (tb.tbs_bytes <= (int)req_bytes.stop()) {
    return true;
  }

  return true;
}

} // namespace srsenb
