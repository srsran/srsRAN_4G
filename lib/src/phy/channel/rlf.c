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

#include <srsran/phy/channel/rlf.h>
#include <srsran/phy/utils/vector.h>

void srsran_channel_rlf_init(srsran_channel_rlf_t* q, uint32_t t_on_ms, uint32_t t_off_ms)
{
  q->t_on_ms  = t_on_ms;
  q->t_off_ms = t_off_ms;
}

void srsran_channel_rlf_execute(srsran_channel_rlf_t*     q,
                                const cf_t*               in,
                                cf_t*                     out,
                                uint32_t                  nsamples,
                                const srsran_timestamp_t* ts)
{
  // Caulculate full period in MS
  uint64_t period_ms = q->t_on_ms + q->t_off_ms;

  // Convert seconds to ms and reduce it into 32 bit
  uint32_t full_secs_ms = (uint32_t)((ts->full_secs * 1000UL) % period_ms);

  // Convert Fractional seconds into ms and convert it to integer
  uint32_t frac_secs_ms = (uint32_t)round(ts->frac_secs * 1000);

  // Add full seconds and fractional performing period module
  uint32_t time_ms = (full_secs_ms + frac_secs_ms) % period_ms;

  // Decide whether enables or disables channel
  if (time_ms < q->t_on_ms) {
    srsran_vec_sc_prod_cfc(in, 1.0f, out, nsamples);
  } else {
    srsran_vec_sc_prod_cfc(in, 0.0f, out, nsamples);
  }
}

void srsran_channel_rlf_free(srsran_channel_rlf_t* q)
{
  // Do nothing
}
