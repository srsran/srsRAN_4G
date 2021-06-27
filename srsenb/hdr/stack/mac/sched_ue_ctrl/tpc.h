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

#ifndef SRSRAN_TPC_H
#define SRSRAN_TPC_H

#include "srsran/adt/accumulators.h"
#include "srsran/common/common.h"
#include "srsran/srslog/srslog.h"

namespace srsenb {

/**
 * Class to handle TPC Commands sent to the UE.
 * The TPC value sent to the UE in each DCI is a result of:
 * - the difference between the target SINR and the windowed average of past UE UL SNR estimates
 * - subtracted by the sum of TPC values sent since the last PHR
 */
class tpc
{
  static constexpr int      undefined_phr  = std::numeric_limits<int>::max();
  static constexpr float    null_snr       = std::numeric_limits<float>::max();
  static constexpr uint32_t nof_ul_ch_code = 2;

public:
  static constexpr uint32_t PUSCH_CODE = 0, PUCCH_CODE = 1;
  static constexpr int      PHR_NEG_NOF_PRB = 1;

  explicit tpc(uint16_t rnti_,
               uint32_t cell_nof_prb,
               float    target_pucch_snr_dB_  = -1.0,
               float    target_pusch_sn_dB_   = -1.0,
               bool     phr_handling_flag_    = false,
               int      min_phr_thres_        = 0,
               uint32_t min_tpc_tti_interval_ = 1,
               float    ul_snr_avg_alpha      = 0.05,
               int      init_ul_snr_value     = 5) :
    rnti(rnti_),
    nof_prb(cell_nof_prb),
    target_pucch_snr_dB(target_pucch_snr_dB_),
    target_pusch_snr_dB(target_pusch_sn_dB_),
    min_phr_thres(min_phr_thres_),
    snr_estim_list(
        {ul_ch_snr_estim{ul_snr_avg_alpha, init_ul_snr_value}, ul_ch_snr_estim{ul_snr_avg_alpha, init_ul_snr_value}}),
    phr_handling_flag(phr_handling_flag_),
    max_prbs_cached(nof_prb),
    min_tpc_tti_interval(min_tpc_tti_interval_),
    logger(srslog::fetch_basic_logger("MAC"))
  {}
  void set_cfg(float target_pusch_snr_dB_, float target_pucch_snr_dB_)
  {
    target_pucch_snr_dB = target_pucch_snr_dB_;
    target_pusch_snr_dB = target_pusch_snr_dB_;
  }

  void set_snr(float snr, uint32_t ul_ch_code)
  {
    static const float MIN_UL_SNR = -4.0f;
    if (snr < MIN_UL_SNR) {
      // Assume signal was not sent
      return;
    }
    if (ul_ch_code < nof_ul_ch_code) {
      snr_estim_list[ul_ch_code].pending_snr = snr;
    }
  }
  void set_phr(int phr_, uint32_t grant_nof_prbs)
  {
    last_phr = phr_;
    for (auto& ch_snr : snr_estim_list) {
      ch_snr.acc_tpc_phr_values = 0;
    }

    // compute and cache the max nof UL PRBs that avoids overflowing PHR
    if (phr_handling_flag) {
      max_prbs_cached = PHR_NEG_NOF_PRB;
      int phr_x_prb   = std::roundf(last_phr + 10.0F * log10f(grant_nof_prbs)); // get what the PHR would be if Nprb=1
      for (int n = nof_prb; n > PHR_NEG_NOF_PRB; --n) {
        if (phr_x_prb >= 10 * log10f(n) + min_phr_thres) {
          max_prbs_cached = n;
          break;
        }
      }
      logger.info("SCHED: rnti=0x%x received PHR=%d for UL Nprb=%d. Max UL Nprb is now=%d",
                  rnti,
                  phr_,
                  grant_nof_prbs,
                  max_prbs_cached);
    }
  }

  void new_tti()
  {
    tti_count++;
    for (size_t chidx = 0; chidx < nof_ul_ch_code; ++chidx) {
      float target_snr_dB = chidx == PUSCH_CODE ? target_pusch_snr_dB : target_pucch_snr_dB;
      auto& ch_snr        = snr_estim_list[chidx];

      // Enqueue pending UL Channel SNR measurement
      if (ch_snr.pending_snr == null_snr) {
        ch_snr.last_snr_sample_count++;
        ch_snr.acc_tpc_values += ch_snr.win_tpc_values.oldest();
        ch_snr.acc_tpc_phr_values += ch_snr.win_tpc_values.oldest();
      } else {
        ch_snr.acc_tpc_values = 0;
        ch_snr.snr_avg.push(ch_snr.pending_snr, ch_snr.last_snr_sample_count);
        ch_snr.last_snr_sample       = ch_snr.pending_snr;
        ch_snr.last_snr_sample_count = 1;
      }
      ch_snr.pending_snr = null_snr;

      // Enqueue PUSCH/PUCCH TPC sent in last TTI (zero for both Delta_PUSCH/Delta_PUCCH=0 and TPC not sent)
      if (target_snr_dB >= 0) {
        ch_snr.win_tpc_values.push(ch_snr.pending_delta);
      }
      ch_snr.pending_delta = 0;
    }
  }

  /**
   * Called during DCI format0 encoding to set PUSCH TPC command
   * @remark See TS 36.213 Section 5.1.1
   * @return accumulated TPC value {-1, 0, 1, 3}
   */
  uint8_t encode_pusch_tpc() { return encode_tpc(PUSCH_CODE); }

  /**
   * Called during DCI format1/2A/A encoding to set PUCCH TPC command
   * @remark See TS 36.213 Section 5.1.2
   * @return accumulated TPC value {-1, 0, 1, 3}
   */
  uint8_t encode_pucch_tpc() { return encode_tpc(PUCCH_CODE); }

  uint32_t max_ul_prbs() const { return max_prbs_cached; }

  float get_ul_snr_estim(uint32_t ul_ch_code = PUSCH_CODE) const { return snr_estim_list[ul_ch_code].snr_avg.value(); }

private:
  uint8_t encode_tpc_delta(int8_t delta)
  {
    switch (delta) {
      case -1:
        return 0;
      case 0:
        return 1;
      case 1:
        return 2;
      case 3:
        return 3;
      default:
        logger.warning("Invalid TPC delta value=%d", delta);
        return 1;
    }
  }
  uint8_t encode_tpc(uint32_t cc)
  {
    auto& ch_snr = snr_estim_list[cc];
    assert(ch_snr.pending_delta == 0); // ensure called once per {cc,tti}

    float target_snr_dB = cc == PUSCH_CODE ? target_pusch_snr_dB : target_pucch_snr_dB;
    if (target_snr_dB < 0) {
      // undefined target SINR case
      return encode_tpc_delta(0);
    }

    // limitation of TPC based on PHR
    int max_delta = 3;
    int eff_phr   = last_phr;
    if (cc == PUSCH_CODE and last_phr != undefined_phr) {
      eff_phr -= ch_snr.win_tpc_values.value() + ch_snr.acc_tpc_phr_values;
      max_delta = std::min(max_delta, eff_phr);
    }

    float snr = ch_snr.last_snr_sample;
    // In case of periodicity of TPCs is > 1 tti, use average SNR to compute SNR diff
    if (min_tpc_tti_interval > 1) {
      ch_snr.tpc_snr_avg.push(snr);
      if ((tti_count - ch_snr.last_tpc_tti_count) < min_tpc_tti_interval) {
        // more time required before sending next TPC
        return encode_tpc_delta(0);
      }
      snr = ch_snr.tpc_snr_avg.value();
    }

    float diff = target_snr_dB - snr;
    diff -= ch_snr.win_tpc_values.value() + ch_snr.acc_tpc_values;
    ch_snr.pending_delta = std::max(std::min((int)floorf(diff), max_delta), -1);
    ch_snr.pending_delta = (ch_snr.pending_delta == 2) ? 1 : ch_snr.pending_delta;
    if (ch_snr.pending_delta != 0) {
      if (min_tpc_tti_interval > 1) {
        ch_snr.last_tpc_tti_count = tti_count;
        ch_snr.tpc_snr_avg.reset();
      }
      logger.debug("TPC: rnti=0x%x, %s command=%d, last SNR=%d, SNR average=%f, diff_acc=%f, eff_phr=%d",
                   rnti,
                   cc == PUSCH_CODE ? "PUSCH" : "PUCCH",
                   encode_tpc_delta(ch_snr.pending_delta),
                   ch_snr.last_snr_sample,
                   ch_snr.snr_avg.value(),
                   diff,
                   eff_phr);
    }
    return encode_tpc_delta(ch_snr.pending_delta);
  }

  uint16_t              rnti;
  uint32_t              nof_prb;
  uint32_t              min_tpc_tti_interval = 1;
  float                 target_pucch_snr_dB, target_pusch_snr_dB;
  int                   min_phr_thres;
  bool                  phr_handling_flag;
  srslog::basic_logger& logger;

  // state
  uint32_t tti_count = 0;

  // PHR-related variables
  int      last_phr        = undefined_phr;
  uint32_t max_prbs_cached = 100;

  // SNR estimation
  struct ul_ch_snr_estim {
    // pending new snr sample
    float pending_snr = srsran::null_sliding_average<float>::null_value();
    // SNR average estimation with irregular sample spacing
    uint32_t                                  last_snr_sample_count = 1; // jump in spacing
    srsran::exp_average_irreg_sampling<float> snr_avg;
    int                                       last_snr_sample;
    // Accumulation of past TPC commands
    srsran::sliding_sum<int>       win_tpc_values;
    int                            acc_tpc_values     = 0;
    int                            acc_tpc_phr_values = 0;
    int8_t                         pending_delta      = 0;
    uint32_t                       last_tpc_tti_count = 0;
    srsran::rolling_average<float> tpc_snr_avg; // average of SNRs since last TPC != 1

    explicit ul_ch_snr_estim(float exp_avg_alpha, int initial_snr) :
      snr_avg(exp_avg_alpha, initial_snr),
      win_tpc_values(FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS),
      last_snr_sample(initial_snr)
    {}
  };
  std::array<ul_ch_snr_estim, nof_ul_ch_code> snr_estim_list;
};

} // namespace srsenb

#endif // SRSRAN_TPC_H
