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
 * - subtracted by the sum of TPC values sent in the last TX_UL_DELAY milliseconds (i.e. 8ms), which the UE hasn't yet
 *   applied
 */
class tpc
{
  static constexpr size_t SNR_WINDOW_SIZE_MS = 8;

public:
  tpc(float target_snr_dB_ = -1.0) :
    target_snr_dB(target_snr_dB_),
    pusch_tpc_values(FDD_HARQ_DELAY_DL_MS + FDD_HARQ_DELAY_UL_MS),
    pucch_tpc_values(FDD_HARQ_DELAY_DL_MS + FDD_HARQ_DELAY_UL_MS),
    snr_avg(SNR_WINDOW_SIZE_MS)
  {
    pending_snr = snr_avg.null_value();
  }
  void set_cfg(float target_snr_dB_) { target_snr_dB = target_snr_dB_; }

  void set_snr(float snr) { pending_snr = snr; }
  void set_phr(int phr_)
  {
    last_phr = phr_;

    // compute and cache the max nof UL PRBs that avoids overflowing PHR
    max_prbs_cached = 1;
    for (int nof_prbs = 100; nof_prbs >= 0; --nof_prbs) {
      if (last_phr >= 10 * log10(nof_prbs)) {
        max_prbs_cached = last_phr;
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
    pusch_tpc_values.push(pending_pusch_tpc);
    pending_pusch_tpc = 0;
    pusch_tpc_values.push(pending_pucch_tpc);
    pending_pucch_tpc = 0;

    // Enqueue pending SNR measurement
    snr_avg.push(pending_snr);
    pending_snr = snr_avg.null_value();
  }

  /**
   * Called during DCI format0 encoding to set TPC command
   * @return accumulated TPC value {-3, -1, 0, 1, 3}
   */
  int8_t encode_pusch_tpc()
  {
    if (target_snr_dB < 0) {
      return 0;
    }
    float diff = target_snr_dB - snr_avg.value();
    diff -= pusch_tpc_values.value();
    int8_t ret = 0;
    if (diff > 1) {
      ret = diff > 3 ? 3 : 1;
    } else if (diff <= -1) {
      ret = -1;
    }
    pending_pusch_tpc = ret;
    return ret;
  }

  int8_t encode_pucch_tpc()
  {
    float diff = target_snr_dB - snr_avg.value();
    diff -= pucch_tpc_values.value();
    int8_t ret = 0;
    if (diff > 1) {
      ret = diff > 3 ? 3 : 1;
    } else if (diff <= -1) {
      ret = -1;
    }
    pending_pucch_tpc = ret;
    return ret;
  }

  uint32_t max_ul_prbs() const { return max_prbs_cached; }

private:
  float                               target_snr_dB;
  srslte::null_sliding_average<float> snr_avg;
  srslte::sliding_sum<int>            pusch_tpc_values, pucch_tpc_values;
  uint32_t                            max_prbs_cached   = 100;
  int                                 last_phr          = 0;
  int                                 pending_pusch_tpc = 0, pending_pucch_tpc = 0;
  float                               pending_snr = 0;
};

} // namespace srsenb

#endif // SRSLTE_TPC_H
