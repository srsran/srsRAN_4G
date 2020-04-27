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

#include "srsenb/hdr/phy/prach_worker.h"
#include "srslte/srslte.h"

namespace srsenb {

int prach_worker::init(const srslte_cell_t&      cell_,
                       const srslte_prach_cfg_t& prach_cfg_,
                       stack_interface_phy_lte*  stack_,
                       srslte::log*              log_h_,
                       int                       priority)
{
  log_h     = log_h_;
  stack     = stack_;
  prach_cfg = prach_cfg_;
  cell      = cell_;

  max_prach_offset_us = 50;

  if (srslte_prach_init(&prach, srslte_symbol_sz(cell.nof_prb))) {
    return -1;
  }

  if (srslte_prach_set_cfg(&prach, &prach_cfg, cell.nof_prb)) {
    ERROR("Error initiating PRACH\n");
    return -1;
  }

  srslte_prach_set_detect_factor(&prach, 60);

  nof_sf = (uint32_t)ceilf(prach.T_tot * 1000);

  start(priority);
  initiated = true;

  sf_cnt = 0;
  return 0;
}

void prach_worker::stop()
{
  srslte_prach_free(&prach);

  running      = false;
  sf_buffer* s = nullptr;
  pending_buffers.push(s);
  wait_thread_finish();
}

void prach_worker::set_max_prach_offset_us(float delay_us)
{
  max_prach_offset_us = delay_us;
}

int prach_worker::new_tti(uint32_t tti_rx, cf_t* buffer_rx)
{
  // Save buffer only if it's a PRACH TTI
  if (srslte_prach_tti_opportunity(&prach, tti_rx, -1) || sf_cnt) {
    if (sf_cnt == 0) {
      current_buffer = buffer_pool.allocate();
      if (!current_buffer) {
        log_h->warning("PRACH skipping tti=%d due to lack of available buffers\n", tti_rx);
        return 0;
      }
    }
    if (!current_buffer) {
      log_h->error("PRACH: Expected available current_buffer\n");
      return -1;
    }
    if (current_buffer->nof_samples + SRSLTE_SF_LEN_PRB(cell.nof_prb) < sf_buffer_sz) {
      memcpy(&current_buffer->samples[sf_cnt * SRSLTE_SF_LEN_PRB(cell.nof_prb)],
             buffer_rx,
             sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
      current_buffer->nof_samples += SRSLTE_SF_LEN_PRB(cell.nof_prb);
      if (sf_cnt == 0) {
        current_buffer->tti = tti_rx;
      }
    } else {
      log_h->error("PRACH: Not enough space in current_buffer\n");
      return -1;
    }
    sf_cnt++;
    if (sf_cnt == nof_sf) {
      sf_cnt = 0;
      pending_buffers.push(current_buffer);
    }
  }
  return 0;
}

int prach_worker::run_tti(sf_buffer* b)
{
  if (srslte_prach_tti_opportunity(&prach, b->tti, -1)) {
    // Detect possible PRACHs
    if (srslte_prach_detect_offset(&prach,
                                   prach_cfg.freq_offset,
                                   &b->samples[prach.N_cp],
                                   nof_sf * SRSLTE_SF_LEN_PRB(cell.nof_prb) - prach.N_cp,
                                   prach_indices,
                                   prach_offsets,
                                   prach_p2avg,
                                   &prach_nof_det)) {
      log_h->error("Error detecting PRACH\n");
      return SRSLTE_ERROR;
    }

    if (prach_nof_det) {
      for (uint32_t i = 0; i < prach_nof_det; i++) {
        log_h->info("PRACH: cc=%d, %d/%d, preamble=%d, offset=%.1f us, peak2avg=%.1f, max_offset=%.1f us\n",
                    cc_idx,
                    i,
                    prach_nof_det,
                    prach_indices[i],
                    prach_offsets[i] * 1e6,
                    prach_p2avg[i],
                    max_prach_offset_us);

        if (prach_offsets[i] * 1e6 < max_prach_offset_us) {
          stack->rach_detected(b->tti, cc_idx, prach_indices[i], (uint32_t)(prach_offsets[i] * 1e6));
        }
      }
    }
  }
  return 0;
}

void prach_worker::run_thread()
{
  running = true;
  while (running) {
    sf_buffer* b = pending_buffers.wait_pop();
    if (running && b) {
      int ret = run_tti(b);
      b->reset();
      buffer_pool.deallocate(b);
      if (ret) {
        running = false;
      }
    }
  }
}

} // namespace srsenb
