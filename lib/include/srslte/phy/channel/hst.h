/*
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
