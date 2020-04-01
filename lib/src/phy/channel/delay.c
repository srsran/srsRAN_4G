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

#include <math.h>
#include <srslte/phy/channel/delay.h>
#include <srslte/srslte.h>

static inline double calculate_delay_us(srslte_channel_delay_t* q, const srslte_timestamp_t* ts)
{
  if (q->period_s) {
    // Convert period from seconds to samples
    uint64_t period_nsamples = (uint64_t)roundf(q->period_s * q->srate_hz);

    // Convert timestamp to samples
    uint64_t ts_nsamples = srslte_timestamp_uint64(ts, q->srate_hz) + (uint64_t)q->init_time_s * q->srate_hz;

    // Calculate time modulus in period
    uint64_t mod_t_nsamples = ts_nsamples - period_nsamples * (ts_nsamples / period_nsamples);
    double   t              = (double)mod_t_nsamples / (double)q->srate_hz;

    double arg      = 2.0 * M_PI * t / (double)q->period_s;
    double delay_us = q->delay_min_us + (q->delay_max_us - q->delay_min_us) * (1.0 + sin(arg)) / 2.0;

    return delay_us;
  } else {
    return q->delay_max_us;
  }
}

static inline uint32_t calculate_delay_nsamples(srslte_channel_delay_t* q)
{
  return (uint32_t)round(q->delay_us * (double)q->srate_hz / 1e6);
}

static inline uint32_t ringbuffer_available_nsamples(srslte_channel_delay_t* q)
{
  return srslte_ringbuffer_status(&q->rb) / sizeof(cf_t);
}

int srslte_channel_delay_init(srslte_channel_delay_t* q,
                              float                   delay_min_us,
                              float                   delay_max_us,
                              float                   period_s,
                              float                   init_time_s,
                              uint32_t                srate_max_hz)
{
  // Calculate buffer size
  uint32_t buff_size = (uint32_t)ceilf(delay_max_us * (float)srate_max_hz / 1e6f);

  // Create ring buffer
  int ret = srslte_ringbuffer_init(&q->rb, sizeof(cf_t) * buff_size);

  // Create zero buffer
  q->zero_buffer = srslte_vec_cf_malloc(buff_size);
  if (!q->zero_buffer) {
    ret = SRSLTE_ERROR;
  }

  // Load initial parameters
  q->delay_min_us = delay_min_us;
  q->delay_max_us = delay_max_us;
  q->srate_max_hz = srate_max_hz;
  q->srate_hz     = srate_max_hz;
  q->period_s     = period_s;
  q->init_time_s  = init_time_s;

  return ret;
}

void srslte_channel_delay_update_srate(srslte_channel_delay_t* q, uint32_t srate_hz)
{
  srslte_ringbuffer_reset(&q->rb);
  q->srate_hz = srate_hz;
}

void srslte_channel_delay_free(srslte_channel_delay_t* q)
{
  srslte_ringbuffer_free(&q->rb);

  if (q->zero_buffer) {
    free(q->zero_buffer);
  }
}

void srslte_channel_delay_execute(srslte_channel_delay_t*   q,
                                  const cf_t*               in,
                                  cf_t*                     out,
                                  uint32_t                  len,
                                  const srslte_timestamp_t* ts)
{
  q->delay_us                 = calculate_delay_us(q, ts);
  q->delay_nsamples           = calculate_delay_nsamples(q);
  uint32_t available_nsamples = ringbuffer_available_nsamples(q);
  uint32_t read_nsamples      = SRSLTE_MIN(q->delay_nsamples, len);
  uint32_t copy_nsamples      = (len > read_nsamples) ? (len - read_nsamples) : 0;

  if (available_nsamples < q->delay_nsamples) {
    uint32_t nzeros = q->delay_nsamples - available_nsamples;
    srslte_vec_cf_zero(q->zero_buffer, nzeros);
    srslte_ringbuffer_write(&q->rb, q->zero_buffer, sizeof(cf_t) * nzeros);
  } else if (available_nsamples > q->delay_nsamples) {
    srslte_ringbuffer_read(&q->rb, q->zero_buffer, sizeof(cf_t) * (available_nsamples - q->delay_nsamples));
  }

  // Read buffered samples
  srslte_ringbuffer_read(&q->rb, out, sizeof(cf_t) * read_nsamples);

  // Read other samples
  if (copy_nsamples) {
    memcpy(&out[read_nsamples], in, sizeof(cf_t) * copy_nsamples);
  }

  // Write new sampels
  srslte_ringbuffer_write(&q->rb, (void*)&in[copy_nsamples], sizeof(cf_t) * read_nsamples);
}
