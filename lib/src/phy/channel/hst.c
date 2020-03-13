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

#include "srslte/phy/channel/hst.h"

int srslte_channel_hst_init(srslte_channel_hst_t* q, float fd_hz, float period_d, float init_time_s)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q) {
    q->fd_hz       = fd_hz;       // Hz
    q->ds_m        = 300.0f;      // m
    q->dmin_m      = 2.0f;        // m
    q->period_s    = period_d;    // s
    q->init_time_s = init_time_s; // s
    q->fs_hz       = NAN;
    ret            = SRSLTE_SUCCESS;
  }

  return ret;
}

void srslte_channel_hst_update_srate(srslte_channel_hst_t* q, uint32_t srate)
{
  if (q) {
    q->srate_hz = srate;
  }
}

void srslte_channel_hst_execute(srslte_channel_hst_t*     q,
                                cf_t*                     in,
                                cf_t*                     out,
                                uint32_t                  len,
                                const srslte_timestamp_t* ts)
{
  if (q && q->srate_hz) {
    // Convert period from seconds to samples
    uint64_t period_nsamples = (uint64_t)roundf(q->period_s * q->srate_hz);

    // Convert timestamp to samples
    uint64_t ts_nsamples = srslte_timestamp_uint64(ts, q->srate_hz) + (uint64_t)q->init_time_s * q->srate_hz;

    // Calculate time modulus in period
    uint64_t mod_t_nsamples = ts_nsamples - period_nsamples * (ts_nsamples / period_nsamples);
    float    t              = (float)mod_t_nsamples / (float)q->srate_hz;

    float costheta = 0;

    if (0 <= t && t <= q->period_s / 2.0f) {
      float num = q->period_s / 4.0f - t;
      float den = sqrtf(powf(q->dmin_m * q->period_s / (q->ds_m * 2), 2.0f) + powf(num, 2.0f));
      costheta  = num / den;
    } else if (q->period_s / 2.0f < t && t < q->period_s) {
      float num = -1.5f / 2.0f * q->period_s + t;
      float den = sqrtf(powf(q->dmin_m * q->period_s / (q->ds_m * 2), 2.0f) + powf(num, 2.0f));
      costheta  = num / den;
    }

    // Calculate doppler shift
    q->fs_hz = q->fd_hz * costheta;

    // Apply doppler shift, assume the doppler does not vary in a sub-frame
    srslte_vec_apply_cfo(in, -q->fs_hz / q->srate_hz, out, len);
  }
}

void srslte_channel_hst_free(srslte_channel_hst_t* q)
{
  if (q) {
    bzero(q, sizeof(srslte_channel_hst_t));
  }
}
