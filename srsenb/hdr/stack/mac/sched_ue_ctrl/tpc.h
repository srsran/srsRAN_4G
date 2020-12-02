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
  static constexpr size_t SNR_WINDOW_SIZE_MS = 8;
  static constexpr int    PHR_NEG_NOF_PRB    = 1;

public:
  tpc(uint32_t cell_nof_prb, float target_snr_dB_ = -1.0) :
    nof_prb(cell_nof_prb), target_snr_dB(target_snr_dB_), snr_avg(SNR_WINDOW_SIZE_MS)
  {
    max_prbs_cached = nof_prb;
  }
  void set_cfg(float target_snr_dB_) { target_snr_dB = target_snr_dB_; }

  void set_snr(float snr) { pending_snr = snr; }
  void set_phr(int phr_)
  {
    last_phr             = phr_;
    sum_pusch_tpc_values = 0;
    sum_pucch_tpc_values = 0;

    // compute and cache the max nof UL PRBs that avoids overflowing PHR
    max_prbs_cached = PHR_NEG_NOF_PRB;
    for (int n = nof_prb; n >= PHR_NEG_NOF_PRB - 1; --n) {
      if (last_phr >= 10 * log10(n)) {
        max_prbs_cached = n;
        break;
      }
    }
  }

  void new_tti()
  {
    if (target_snr_dB < 0) {
      return;
    }
    // Enqueue PUSCH/PUCCH TPC sent in last TTI (zero for both Delta_PUSCH/Delta_PUCCH=0 and TPC not sent)
    sum_pusch_tpc_values += pending_pusch_tpc;
    pending_pusch_tpc = 0;
    sum_pucch_tpc_values += pending_pucch_tpc;
    pending_pucch_tpc = 0;

    // Enqueue pending SNR measurement
    snr_avg.push(pending_snr);
    pending_snr = snr_avg.null_value;
  }

  /**
   * Called during DCI format0 encoding to set PUSCH TPC command
   * @remark See TS 36.213 Section 5.1.1
   * @return accumulated TPC value {-1, 0, 1, 3}
   */
  int8_t encode_pusch_tpc()
  {
    pending_pusch_tpc = 0;
    if (target_snr_dB <= 0) {
      // undefined target SINR case. Increase Tx power, while the number of allocable PRBs remains unchanged
      pending_pusch_tpc = (max_prbs_cached == nof_prb) ? 1 : (last_phr < 0 ? -1 : 0);
    } else if (snr_avg.value() != snr_avg.null_value) {
      // target SINR is finite and there is power headroom
      float diff = target_snr_dB - snr_avg.value();
      diff -= sum_pusch_tpc_values;
      pending_pusch_tpc = 0;
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
    pending_pusch_tpc = 0;
    if (target_snr_dB <= 0) {
      // undefined target SINR case. Increase Tx power, while the number of allocable PRBs remains unchanged
      pending_pucch_tpc = (max_prbs_cached == nof_prb) ? 1 : (last_phr < 0 ? -1 : 0);
    } else if (snr_avg.value() != snr_avg.null_value) {
      // target SINR is finite and there is power headroom
      float diff = target_snr_dB - snr_avg.value();
      diff -= sum_pucch_tpc_values;
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
  int                                 sum_pusch_tpc_values = 0;
  int                                 sum_pucch_tpc_values = 0;
  uint32_t                            max_prbs_cached      = 100;
  int                                 last_phr             = std::numeric_limits<int>::max();
  int                                 pending_pusch_tpc = 0, pending_pucch_tpc = 0;
  float                               pending_snr = srslte::null_sliding_average<float>::null_value;
};

} // namespace srsenb

#endif // SRSLTE_TPC_H
