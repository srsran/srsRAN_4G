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

  explicit tpc(uint32_t cell_nof_prb,
               float    target_pucch_snr_dB_  = -1.0,
               float    target_pusch_sn_dB_   = -1.0,
               bool     phr_handling_flag_    = false,
               uint32_t min_tpc_tti_interval_ = 1,
               float    ul_snr_avg_alpha      = 0.05,
               int      init_ul_snr_value     = 5) :
    nof_prb(cell_nof_prb),
    target_pucch_snr_dB(target_pucch_snr_dB_),
    target_pusch_snr_dB(target_pusch_sn_dB_),
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
  void set_phr(int phr_)
  {
    last_phr = phr_;
    for (auto& ch_snr : snr_estim_list) {
      ch_snr.phr_flag = false;
    }

    // compute and cache the max nof UL PRBs that avoids overflowing PHR
    if (phr_handling_flag) {
      max_prbs_cached = PHR_NEG_NOF_PRB;
      for (int n = nof_prb; n > PHR_NEG_NOF_PRB; --n) {
        if (last_phr >= 10 * log10(n)) {
          max_prbs_cached = n;
          break;
        }
      }
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
      } else {
        ch_snr.acc_tpc_values = 0;
        ch_snr.snr_avg.push(ch_snr.pending_snr, ch_snr.last_snr_sample_count);
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
    if (target_snr_dB < 0 or last_phr == 0) {
      // undefined target sinr case, or no more PHR
      return encode_tpc_delta(0);
    }
    if (last_phr < 0) {
      // negative PHR
      encode_tpc_delta(-1);
    }
    if ((tti_count - ch_snr.last_tpc_tti_count) < min_tpc_tti_interval) {
      // more time required before sending next TPC
      return encode_tpc_delta(0);
    }

    // target SINR is finite and there is power headroom
    float diff = target_snr_dB - ch_snr.snr_avg.value();
    diff -= ch_snr.win_tpc_values.value() + ch_snr.acc_tpc_values;
    if (diff >= 1) {
      ch_snr.pending_delta      = diff > 3 ? 3 : 1;
      ch_snr.last_tpc_tti_count = tti_count;
    } else if (diff <= -1) {
      ch_snr.pending_delta      = -1;
      ch_snr.last_tpc_tti_count = tti_count;
    }
    return encode_tpc_delta(ch_snr.pending_delta);
  }

  uint32_t              nof_prb;
  uint32_t              min_tpc_tti_interval = 1;
  float                 target_pucch_snr_dB, target_pusch_snr_dB;
  bool                  phr_handling_flag;
  srslog::basic_logger& logger;

  // state
  uint32_t tti_count = 0;

  // PHR-related variables
  int      last_phr        = undefined_phr;
  uint32_t max_prbs_cached = 100;

  // SNR estimation
  struct ul_ch_snr_estim {
    // flag used in undefined target SINR case
    bool phr_flag = false;
    // pending new snr sample
    float pending_snr = srsran::null_sliding_average<float>::null_value();
    // SNR average estimation with irregular sample spacing
    uint32_t                                  last_snr_sample_count = 1; // jump in spacing
    srsran::exp_average_irreg_sampling<float> snr_avg;
    // Accumulation of past TPC commands
    srsran::sliding_sum<int> win_tpc_values;
    int8_t                   pending_delta      = 0;
    int                      acc_tpc_values     = 0;
    uint32_t                 last_tpc_tti_count = 0;

    explicit ul_ch_snr_estim(float exp_avg_alpha, int initial_snr) :
      snr_avg(exp_avg_alpha, initial_snr), win_tpc_values(FDD_HARQ_DELAY_UL_MS + FDD_HARQ_DELAY_DL_MS)
    {}
  };
  std::array<ul_ch_snr_estim, nof_ul_ch_code> snr_estim_list;
};

} // namespace srsenb

#endif // SRSRAN_TPC_H
