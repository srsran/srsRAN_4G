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

#ifndef SRSLTE_DELAY_H
#define SRSLTE_DELAY_H

#include <srslte/srslte.h>

typedef struct {
  float    delay_min_us;
  float    delay_max_us;
  float    period_s;
  float    init_time_s;
  uint32_t srate_max_hz;
  uint32_t srate_hz;
  float    delay_us;
  uint32_t delay_nsamples;

  srslte_ringbuffer_t rb;
  cf_t*               zero_buffer;
} srslte_channel_delay_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSLTE_API int srslte_channel_delay_init(srslte_channel_delay_t* q,
                                         float                   delay_min_us,
                                         float                   delay_max_us,
                                         float                   period_s,
                                         float                   init_time_s,
                                         uint32_t                srate_max_hz);

SRSLTE_API void srslte_channel_delay_update_srate(srslte_channel_delay_t* q, uint32_t srate_hz);

SRSLTE_API void srslte_channel_delay_free(srslte_channel_delay_t* q);

SRSLTE_API void srslte_channel_delay_execute(srslte_channel_delay_t*   q,
                                             const cf_t*               in,
                                             cf_t*                     out,
                                             uint32_t                  len,
                                             const srslte_timestamp_t* ts);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_DELAY_H
