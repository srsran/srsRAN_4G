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

#ifndef SRSLTE_DELAY_H
#define SRSLTE_DELAY_H

#include "srslte/config.h"
#include "srslte/phy/common/timestamp.h"
#include "srslte/phy/utils/ringbuffer.h"
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
