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
