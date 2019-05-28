/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include <srslte/phy/channel/rlf.h>
#include <srslte/phy/utils/vector.h>

void srslte_channel_rlf_init(srslte_channel_rlf_t* q, uint32_t t_on_ms, uint32_t t_off_ms)
{
  q->t_on_ms  = t_on_ms;
  q->t_off_ms = t_off_ms;
}

void srslte_channel_rlf_execute(
    srslte_channel_rlf_t* q, const cf_t* in, cf_t* out, uint32_t nsamples, const srslte_timestamp_t* ts)
{
  uint32_t period_ms    = q->t_on_ms + q->t_off_ms;
  double   full_secs_ms = (ts->full_secs * 1000) % period_ms;
  double   frac_secs_ms = (ts->frac_secs * 1000);
  double   time_ms      = full_secs_ms + frac_secs_ms;

  if (time_ms < q->t_on_ms) {
    srslte_vec_sc_prod_cfc(in, 1.0f, out, nsamples);
  } else {
    srslte_vec_sc_prod_cfc(in, 0.0f, out, nsamples);
  }
}

void srslte_channel_rlf_free(srslte_channel_rlf_t* q)
{
  // Do nothing
}