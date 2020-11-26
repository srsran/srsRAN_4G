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

#ifndef SRSLTE_HST_H_
#define SRSLTE_HST_H_

#include <srslte/srslte.h>

typedef struct {
  // System parameters
  uint32_t srate_hz; // Sampling rate

  // Model Parameters
  float fd_hz;       // Maximum Doppler Frequency
  float ds_m;        // eNb distance [m]
  float dmin_m;      // eNb Rail-track distance [m]
  float period_s;    // 2 * Ds / speed [s]
  float init_time_s; // Time offset [s]

  // State
  float fs_hz; // Last doppler dispersion [Hz]
} srslte_channel_hst_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSLTE_API int srslte_channel_hst_init(srslte_channel_hst_t* q, float fd_hz, float period_d, float init_time_s);

SRSLTE_API void srslte_channel_hst_update_srate(srslte_channel_hst_t* q, uint32_t srate);

SRSLTE_API void
srslte_channel_hst_execute(srslte_channel_hst_t* q, cf_t* in, cf_t* out, uint32_t len, const srslte_timestamp_t* ts);

SRSLTE_API void srslte_channel_hst_free(srslte_channel_hst_t* q);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_HST_H_
