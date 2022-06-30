/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_DELAY_H
#define SRSRAN_DELAY_H

#include "srsran/config.h"
#include "srsran/phy/common/timestamp.h"
#include "srsran/phy/utils/ringbuffer.h"
#include <stdint.h>

typedef struct {
  float    delay_min_us;
  float    delay_max_us;
  float    period_s;
  float    init_time_s;
  uint32_t srate_max_hz;
  uint32_t srate_hz;
  float    delay_us;
  uint32_t delay_nsamples;

  srsran_ringbuffer_t rb;
  cf_t*               zero_buffer;
} srsran_channel_delay_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSRAN_API int srsran_channel_delay_init(srsran_channel_delay_t* q,
                                         float                   delay_min_us,
                                         float                   delay_max_us,
                                         float                   period_s,
                                         float                   init_time_s,
                                         uint32_t                srate_max_hz);

SRSRAN_API void srsran_channel_delay_update_srate(srsran_channel_delay_t* q, uint32_t srate_hz);

SRSRAN_API void srsran_channel_delay_free(srsran_channel_delay_t* q);

SRSRAN_API void srsran_channel_delay_execute(srsran_channel_delay_t*   q,
                                             const cf_t*               in,
                                             cf_t*                     out,
                                             uint32_t                  len,
                                             const srsran_timestamp_t* ts);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_DELAY_H
