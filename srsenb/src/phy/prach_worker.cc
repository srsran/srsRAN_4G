/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsenb/hdr/phy/prach_worker.h"
#include "srsran/interfaces/enb_mac_interfaces.h"
#include "srsran/srsran.h"

namespace srsenb {

int prach_worker::init(const srsran_cell_t&      cell_,
                       const srsran_prach_cfg_t& prach_cfg_,
                       stack_interface_phy_lte*  stack_,
                       int                       priority,
                       uint32_t                  nof_workers_)
{
  stack       = stack_;
  prach_cfg   = prach_cfg_;
  cell        = cell_;
  nof_workers = nof_workers_;

  max_prach_offset_us = 50;

  if (srsran_prach_init(&prach, srsran_symbol_sz(cell.nof_prb))) {
    return -1;
  }

  if (srsran_prach_set_cfg(&prach, &prach_cfg, cell.nof_prb)) {
    ERROR("Error initiating PRACH");
    return -1;
  }

  srsran_prach_set_detect_factor(&prach, 60);

  nof_sf = (uint32_t)ceilf(prach.T_tot * 1000);

  if (nof_workers > 0) {
    start(priority);
  }

  initiated = true;

  sf_cnt = 0;

#if defined(ENABLE_GUI) and ENABLE_PRACH_GUI
  char title[32] = {};
  snprintf(title, sizeof(title), "PRACH buffer %d", cc_idx);

  sdrgui_init();
  plot_real_init(&plot_real);
  plot_real_setTitle(&plot_real, title);
  plot_real_setXAxisAutoScale(&plot_real, true);
  plot_real_setYAxisAutoScale(&plot_real, true);
  plot_real_addToWindowGrid(&plot_real, (char*)"PRACH", 0, cc_idx);
#endif // defined(ENABLE_GUI) and ENABLE_PRACH_GUI

  return 0;
}

void prach_worker::stop()
{
  running      = false;
  sf_buffer* s = nullptr;
  pending_buffers.push(s);

  if (nof_workers > 0) {
    wait_thread_finish();
  }

  srsran_prach_free(&prach);
}

void prach_worker::set_max_prach_offset_us(float delay_us)
{
  max_prach_offset_us = delay_us;
}

int prach_worker::new_tti(uint32_t tti_rx, cf_t* buffer_rx)
{
  // Save buffer only if it's a PRACH TTI
  if (srsran_prach_tti_opportunity(&prach, tti_rx, -1) || sf_cnt) {
    if (sf_cnt == 0) {
      current_buffer = buffer_pool.allocate();
      if (!current_buffer) {
        logger.warning("PRACH skipping tti=%d due to lack of available buffers", tti_rx);
        return 0;
      }
    }
    if (!current_buffer) {
      logger.error("PRACH: Expected available current_buffer");
      return -1;
    }
    if (current_buffer->nof_samples + SRSRAN_SF_LEN_PRB(cell.nof_prb) < sf_buffer_sz) {
      memcpy(&current_buffer->samples[sf_cnt * SRSRAN_SF_LEN_PRB(cell.nof_prb)],
             buffer_rx,
             sizeof(cf_t) * SRSRAN_SF_LEN_PRB(cell.nof_prb));
      current_buffer->nof_samples += SRSRAN_SF_LEN_PRB(cell.nof_prb);
      if (sf_cnt == 0) {
        current_buffer->tti = tti_rx;
      }
    } else {
      logger.error("PRACH: Not enough space in current_buffer");
      return -1;
    }
    sf_cnt++;
    if (sf_cnt == nof_sf) {
      sf_cnt = 0;
      if (nof_workers == 0) {
        run_tti(current_buffer);
        current_buffer->reset();
        buffer_pool.deallocate(current_buffer);
      } else {
        pending_buffers.push(current_buffer);
      }
    }
  }
  return 0;
}

int prach_worker::run_tti(sf_buffer* b)
{
  uint32_t prach_nof_det = 0;
  if (srsran_prach_tti_opportunity(&prach, b->tti, -1)) {
    // Detect possible PRACHs
    if (srsran_prach_detect_offset(&prach,
                                   prach_cfg.freq_offset,
                                   &b->samples[prach.N_cp],
                                   nof_sf * SRSRAN_SF_LEN_PRB(cell.nof_prb) - prach.N_cp,
                                   prach_indices,
                                   prach_offsets,
                                   prach_p2avg,
                                   &prach_nof_det)) {
      logger.error("Error detecting PRACH");
      return SRSRAN_ERROR;
    }

    if (prach_nof_det) {
      for (uint32_t i = 0; i < prach_nof_det; i++) {
        logger.info("PRACH: cc=%d, %d/%d, preamble=%d, offset=%.1f us, peak2avg=%.1f, max_offset=%.1f us",
                    cc_idx,
                    i,
                    prach_nof_det,
                    prach_indices[i],
                    prach_offsets[i] * 1e6,
                    prach_p2avg[i],
                    max_prach_offset_us);

        if (prach_offsets[i] * 1e6 < max_prach_offset_us) {
          // Convert time offset to Time Alignment command
          uint32_t n_ta = (uint32_t)(prach_offsets[i] / (16 * SRSRAN_LTE_TS));

          stack->rach_detected(b->tti, cc_idx, prach_indices[i], n_ta);

#if defined(ENABLE_GUI) and ENABLE_PRACH_GUI
          uint32_t nof_samples = SRSRAN_MIN(nof_sf * SRSRAN_SF_LEN_PRB(cell.nof_prb), 3 * SRSRAN_SF_LEN_MAX);
          srsran_vec_abs_cf(b->samples, plot_buffer.data(), nof_samples);
          plot_real_setNewData(&plot_real, plot_buffer.data(), nof_samples);
#endif // defined(ENABLE_GUI) and ENABLE_PRACH_GUI
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
