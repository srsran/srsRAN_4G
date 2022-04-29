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

#include "srsue/hdr/phy/scell/scell_recv.h"

namespace srsue {
namespace scell {

/**********
 * Secondary cell receiver
 */

void scell_recv::init(uint32_t max_sf_window)
{
  // and a separate ue_sync instance

  uint32_t max_fft_sz  = (uint32_t)srsran_symbol_sz(100);
  uint32_t max_sf_size = SRSRAN_SF_LEN(max_fft_sz);

  sf_buffer[0] = srsran_vec_cf_malloc(max_sf_size);
  if (!sf_buffer[0]) {
    logger.error("Error allocating %d samples for scell", max_sf_size);
    return;
  }

  // do this different we don't need all this search window.
  if (srsran_sync_init(&sync_find, max_sf_window * max_sf_size, 5 * max_sf_size, max_fft_sz)) {
    logger.error("Error initiating sync_find");
    return;
  }
  srsran_sync_set_sss_algorithm(&sync_find, SSS_FULL);
  srsran_sync_set_cfo_cp_enable(&sync_find, false, 0);
  srsran_sync_cp_en(&sync_find, false);
  srsran_sync_set_cfo_pss_enable(&sync_find, false);
  srsran_sync_set_threshold(&sync_find, 2.0f); // The highest the best for avoiding ghost cells
  srsran_sync_set_em_alpha(&sync_find, 0.3f);
  srsran_sss_set_threshold(&sync_find.sss, 300.0); // A higher value will avoid false alarms but reduce detection

  // Configure FIND object behaviour (this configuration is always the same)
  srsran_sync_set_cfo_ema_alpha(&sync_find, 1.0);
  srsran_sync_set_cfo_i_enable(&sync_find, false);
  srsran_sync_set_pss_filt_enable(&sync_find, true);
  srsran_sync_set_sss_eq_enable(&sync_find, true);

  sync_find.pss.chest_on_filter  = true;
  sync_find.sss_channel_equalize = false;

  reset();
}

void scell_recv::deinit()
{
  srsran_sync_free(&sync_find);
  free(sf_buffer[0]);
}

void scell_recv::reset()
{
  current_fft_sz = 0;
}

void scell_recv::find_cells(const cf_t*          input_buffer,
                            const srsran_cell_t& serving_cell,
                            const uint32_t&      nof_sf,
                            std::set<uint32_t>&  found_cell_ids)
{
  uint32_t fft_sz = srsran_symbol_sz(serving_cell.nof_prb);
  uint32_t sf_len = SRSRAN_SF_LEN(fft_sz);

  if (fft_sz != current_fft_sz) {
    if (srsran_sync_resize(&sync_find, nof_sf * sf_len, 5 * sf_len, fft_sz)) {
      logger.error("Error resizing sync nof_sf=%d, sf_len=%d, fft_sz=%d", nof_sf, sf_len, fft_sz);
      return;
    }
    current_fft_sz = fft_sz;
  }

  uint32_t peak_idx = 0;
  int      cell_id  = 0;

  for (uint32_t n_id_2 = 0; n_id_2 < 3; n_id_2++) {
    if (n_id_2 != (serving_cell.id % 3)) {
      srsran_sync_set_N_id_2(&sync_find, n_id_2);

      srsran_sync_find_ret_t sync_res;

      srsran_sync_reset(&sync_find);
      srsran_sync_cfo_reset(&sync_find, 0.0f);

      sync_res          = SRSRAN_SYNC_NOFOUND;
      bool sss_detected = false;
      cell_id           = 0;
      float max_peak    = -1;

      float sss_correlation_peak_max = 0.0f;

      for (uint32_t sf5_cnt = 0; sf5_cnt < nof_sf / 5; sf5_cnt++) {
        sync_res = srsran_sync_find(&sync_find, input_buffer, sf5_cnt * 5 * sf_len, &peak_idx);
        if (sync_res == SRSRAN_SYNC_ERROR) {
          logger.error("INTRA: Error calling sync_find()");
          return;
        }

        if (sync_find.peak_value > max_peak && sync_res == SRSRAN_SYNC_FOUND && srsran_sync_sss_detected(&sync_find)) {
          // Uses the cell ID from the highest SSS correlation peak
          if (sss_correlation_peak_max < srsran_sync_sss_correlation_peak(&sync_find)) {
            // Set the cell ID
            cell_id = srsran_sync_get_cell_id(&sync_find);

            // Update the maximum value
            sss_correlation_peak_max = srsran_sync_sss_correlation_peak(&sync_find);
          }
          sss_detected = true;
        }

        logger.debug("INTRA: n_id_2=%d, cnt=%d/%d, sync_res=%d, cell_id=%d, sf_idx=%d, peak_idx=%d, peak_value=%f, "
                     "sss_detected=%d",
                     n_id_2,
                     sf5_cnt,
                     nof_sf / 5,
                     sync_res,
                     cell_id,
                     srsran_sync_get_sf_idx(&sync_find),
                     peak_idx,
                     sync_find.peak_value,
                     srsran_sync_sss_detected(&sync_find));
      }

      // If the SSS was not detected, the serving_cell id is not reliable. So, consider no sync found
      if (sync_res == SRSRAN_SYNC_FOUND && sss_detected && cell_id >= 0) {
        // We have found a new cell, add to the list
        found_cell_ids.insert((uint32_t)cell_id);
        logger.debug("INTRA: Detected new cell_id=%d using PSS/SSS", cell_id);
      }
    }
  }
  return;
}

} // namespace scell
} // namespace srsue
