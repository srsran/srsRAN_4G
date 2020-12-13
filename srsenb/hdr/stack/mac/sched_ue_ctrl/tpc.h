/**
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_TPC_H
#define SRSLTE_TPC_H

#include "srslte/adt/accumulators.h"
#include "srslte/common/common.h"
#include "srslte/common/logmap.h"

namespace srsenb {

/**
 * Class to handle TPC Commands sent to the UE.
 * The TPC value sent to the UE in each DCI is a result of:
 * - the difference between the target SINR and the windowed average of past UE UL SNR estimates
 * - subtracted by the sum of TPC values sent since the last PHR
 */
class tpc
{
  static constexpr int   undefined_phr = std::numeric_limits<int>::max();
  static constexpr float null_snr      = std::numeric_limits<float>::max();

public:
  static constexpr int PHR_NEG_NOF_PRB = 1;

  tpc(uint32_t cell_nof_prb, float target_snr_dB_ = -1.0) :
    nof_prb(cell_nof_prb),
    target_snr_dB(target_snr_dB_),
    snr_avg(0.1, target_snr_dB_),
    win_pusch_tpc_values(FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS),
    win_pucch_tpc_values(FDD_HARQ_DELAY_DL_MS + FDD_HARQ_DELAY_UL_MS)
  {
    max_prbs_cached = nof_prb;
  }
  void set_cfg(float target_snr_dB_) { target_snr_dB = target_snr_dB_; }

  void set_snr(float snr) { pending_snr = snr; }
  void set_phr(int phr_)
  {
    last_phr       = phr_;
    pucch_phr_flag = false;
    pusch_phr_flag = false;

    // compute and cache the max nof UL PRBs that avoids overflowing PHR
    max_prbs_cached = PHR_NEG_NOF_PRB;
    for (int n = nof_prb; n > PHR_NEG_NOF_PRB; --n) {
      if (last_phr >= 10 * log10(n)) {
        max_prbs_cached = n;
        break;
      }
    }
  }

  void new_tti()
  {
    if (target_snr_dB < 0) {
      pending_pusch_delta = 0;
      pending_pucch_delta = 0;
      return;
    }

    // Enqueue pending SNR measurement
    if (pending_snr == null_snr) {
      last_snr_sample_count++;
      acc_pusch_tpc_values += win_pusch_tpc_values.oldest();
      acc_pucch_tpc_values += win_pusch_tpc_values.oldest();
    } else {
      acc_pucch_tpc_values = 0;
      acc_pusch_tpc_values = 0;
      snr_avg.push(pending_snr, last_snr_sample_count);
      last_snr_sample_count = 1;
    }
    pending_snr = null_snr;

    // Enqueue PUSCH/PUCCH TPC sent in last TTI (zero for both Delta_PUSCH/Delta_PUCCH=0 and TPC not sent)
    win_pusch_tpc_values.push(pending_pusch_delta);
    pending_pusch_delta = 0;
    win_pucch_tpc_values.push(pending_pucch_delta);
    pending_pucch_delta = 0;
  }

  /**
   * Called during DCI format0 encoding to set PUSCH TPC command
   * @remark See TS 36.213 Section 5.1.1
   * @return accumulated TPC value {-1, 0, 1, 3}
   */
  uint8_t encode_pusch_tpc()
  {
    assert(pending_pusch_delta == 0); // ensure called once per {cc,tti}
    if (target_snr_dB < 0) {
      // undefined target SINR case. Increase Tx power once per PHR, considering the number of allocable PRBs remains
      // unchanged
      if (not pusch_phr_flag) {
        pending_pusch_delta = (max_prbs_cached == nof_prb) ? 1 : (last_phr < 0 ? -1 : 0);
        pusch_phr_flag      = true;
      }
    } else {
      // target SINR is finite and there is power headroom
      float diff = target_snr_dB - snr_avg.value();
      diff -= win_pusch_tpc_values.value() + acc_pusch_tpc_values;
      int8_t diff_round = roundf(diff);
      if (diff_round >= 1) {
        pending_pusch_delta = diff_round > 3 ? 3 : 1;
      } else if (diff_round <= -1) {
        pending_pusch_delta = -1;
      }
      if (last_phr <= 0) {
        // In case there is no headroom, forbid power increases
        pending_pusch_delta = std::min(pending_pusch_delta, 0);
      }
    }
    return encode_tpc_delta(pending_pusch_delta);
  }

  /**
   * Called during DCI format1/2A/A encoding to set PUCCH TPC command
   * Note: For now we use the same algorithm for PUCCH and PUSCH
   * @remark See TS 36.213 Section 5.1.2
   * @return accumulated TPC value {-1, 0, 1, 3}
   */
  uint8_t encode_pucch_tpc()
  {
    assert(pending_pucch_delta == 0); // ensure called once per {cc,tti}
    if (target_snr_dB < 0) {
      // undefined target SINR case. Increase Tx power once per PHR, considering the number of allocable PRBs remains
      // unchanged
      if (not pucch_phr_flag) {
        pending_pucch_delta = (max_prbs_cached == nof_prb) ? 1 : (last_phr < 0 ? -1 : 0);
        pucch_phr_flag      = true;
      }
    } else {
      // target SINR is finite and there is power headroom
      float diff = target_snr_dB - snr_avg.value();
      diff -= win_pucch_tpc_values.value() + acc_pucch_tpc_values;
      int8_t diff_round = roundf(diff);
      if (diff_round >= 1) {
        pending_pucch_delta = diff_round > 3 ? 3 : 1;
      } else if (diff_round <= -1) {
        pending_pucch_delta = -1;
      }
      if (last_phr <= 0) {
        // In case there is no headroom, forbid power increases
        pending_pucch_delta = std::min(pending_pucch_delta, 0);
      }
    }
    return encode_tpc_delta(pending_pucch_delta);
  }

  uint32_t max_ul_prbs() const { return max_prbs_cached; }

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
        srslte::logmap::get("MAC")->warning("Invalid TPC delta value=%d\n", delta);
        return 1;
    }
  }

  uint32_t nof_prb;
  float    target_snr_dB;

  // PHR-related variables
  int      last_phr        = undefined_phr;
  uint32_t max_prbs_cached = 100;
  bool     pusch_phr_flag = false, pucch_phr_flag = false;

  // SNR estimation
  srslte::exp_average_irreg_sampling<float> snr_avg;
  float                                     pending_snr           = srslte::null_sliding_average<float>::null_value();
  uint32_t                                  last_snr_sample_count = 1;

  // Accumulation of past TPC commands
  srslte::sliding_sum<int> win_pusch_tpc_values, win_pucch_tpc_values;
  int                      pending_pusch_delta = 0, pending_pucch_delta = 0;
  int                      acc_pusch_tpc_values = 0, acc_pucch_tpc_values = 0;
};

} // namespace srsenb

#endif // SRSLTE_TPC_H
