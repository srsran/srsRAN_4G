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

#ifndef SRSLTE_TPC_H
#define SRSLTE_TPC_H

#include "srslte/adt/accumulators.h"
#include "srslte/common/common.h"

namespace srsenb {

/**
 * Class to handle TPC Commands sent to the UE.
 * The TPC value sent to the UE in each DCI is a result of:
 * - the difference between the target SINR and the windowed average of past UE UL SNR estimates
 * - subtracted by the sum of TPC values sent since the last PHR
 */
class tpc
{
  static constexpr size_t SNR_WINDOW_SIZE_MS = 20;
  static constexpr int    undefined_phr      = std::numeric_limits<int>::max();

public:
  static constexpr int PHR_NEG_NOF_PRB = 1;

  tpc(uint32_t cell_nof_prb, float target_snr_dB_ = -1.0) :
    nof_prb(cell_nof_prb),
    target_snr_dB(target_snr_dB_),
    snr_avg(SNR_WINDOW_SIZE_MS),
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
      pending_pusch_tpc = 0;
      pending_pucch_tpc = 0;
      return;
    }

    // Enqueue pending SNR measurement
    if (pending_snr == snr_avg.null_value()) {
      acc_pusch_tpc_values += win_pusch_tpc_values.oldest();
      acc_pucch_tpc_values += win_pusch_tpc_values.oldest();
    } else {
      acc_pucch_tpc_values = 0;
      acc_pusch_tpc_values = 0;
    }
    snr_avg.push(pending_snr);
    pending_snr = snr_avg.null_value();

    // Enqueue PUSCH/PUCCH TPC sent in last TTI (zero for both Delta_PUSCH/Delta_PUCCH=0 and TPC not sent)
    win_pusch_tpc_values.push(pending_pusch_tpc);
    pending_pusch_tpc = 0;
    win_pucch_tpc_values.push(pending_pucch_tpc);
    pending_pucch_tpc = 0;
  }

  /**
   * Called during DCI format0 encoding to set PUSCH TPC command
   * @remark See TS 36.213 Section 5.1.1
   * @return accumulated TPC value {-1, 0, 1, 3}
   */
  int8_t encode_pusch_tpc()
  {
    assert(pending_pusch_tpc == 0); // ensure called once per {cc,tti}
    if (target_snr_dB < 0) {
      // undefined target SINR case. Increase Tx power once per PHR, considering the number of allocable PRBs remains
      // unchanged
      if (not pusch_phr_flag) {
        pending_pusch_tpc = (max_prbs_cached == nof_prb) ? 1 : (last_phr < 0 ? -1 : 0);
        pusch_phr_flag    = true;
      }
    } else if (snr_avg.value() != snr_avg.null_value()) {
      // target SINR is finite and there is power headroom
      float diff = target_snr_dB - snr_avg.value();
      diff -= win_pusch_tpc_values.value() + acc_pusch_tpc_values;
      if (diff > 1) {
        pending_pusch_tpc = diff > 3 ? 3 : 1;
      } else if (diff <= -1) {
        pending_pusch_tpc = -1;
      }
      if (last_phr <= 0) {
        // In case there is no headroom, forbid power increases
        pending_pusch_tpc = std::min(pending_pusch_tpc, 0);
      }
    }
    return pending_pusch_tpc;
  }

  /**
   * Called during DCI format1/2A/A encoding to set PUCCH TPC command
   * Note: For now we use the same algorithm for PUCCH and PUSCH
   * @remark See TS 36.213 Section 5.1.2
   * @return accumulated TPC value {-1, 0, 1, 3}
   */
  int8_t encode_pucch_tpc()
  {
    assert(pending_pucch_tpc == 0); // ensure called once per {cc,tti}
    if (target_snr_dB < 0) {
      // undefined target SINR case. Increase Tx power once per PHR, considering the number of allocable PRBs remains
      // unchanged
      if (not pucch_phr_flag) {
        pending_pucch_tpc = (max_prbs_cached == nof_prb) ? 1 : (last_phr < 0 ? -1 : 0);
        pucch_phr_flag    = true;
      }
    } else if (snr_avg.value() != snr_avg.null_value()) {
      // target SINR is finite and there is power headroom
      float diff = target_snr_dB - snr_avg.value();
      diff -= win_pucch_tpc_values.value() + acc_pucch_tpc_values;
      pending_pucch_tpc = 0;
      if (diff > 1) {
        pending_pucch_tpc = diff > 3 ? 3 : 1;
      } else if (diff <= -1) {
        pending_pucch_tpc = -1;
      }
      if (last_phr <= 0) {
        // In case there is no headroom, forbid power increases
        pending_pucch_tpc = std::min(pending_pucch_tpc, 0);
      }
    }
    return pending_pucch_tpc;
  }

  uint32_t max_ul_prbs() const { return max_prbs_cached; }

private:
  uint32_t nof_prb;
  float    target_snr_dB;

  srslte::null_sliding_average<float> snr_avg;
  srslte::sliding_sum<int>            win_pusch_tpc_values, win_pucch_tpc_values;
  uint32_t                            max_prbs_cached   = 100;
  int                                 pending_pusch_tpc = 0, pending_pucch_tpc = 0;
  float                               pending_snr          = srslte::null_sliding_average<float>::null_value();
  int                                 acc_pusch_tpc_values = 0, acc_pucch_tpc_values = 0;
  int                                 last_phr       = undefined_phr;
  bool                                pusch_phr_flag = false, pucch_phr_flag = false;
};

} // namespace srsenb

#endif // SRSLTE_TPC_H
