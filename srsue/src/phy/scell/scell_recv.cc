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

#include "srsue/hdr/phy/scell/scell_recv.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->debug(fmt, ##__VA_ARGS__)

namespace srsue {
namespace scell {

/**********
 * Secondary cell receiver
 */

void scell_recv::init(srslte::log* _log_h, bool _sic_pss_enabled, uint32_t max_sf_window, phy_common* worker_com)
{
  log_h           = _log_h;
  sic_pss_enabled = _sic_pss_enabled;

  // and a separate ue_sync instance

  uint32_t max_fft_sz  = srslte_symbol_sz(100);
  uint32_t max_sf_size = SRSLTE_SF_LEN(max_fft_sz);

  sf_buffer[0] = (cf_t*)srslte_vec_malloc(sizeof(cf_t) * max_sf_size);
  if (!sf_buffer[0]) {
    ERROR("Error allocating %d samples for scell\n", max_sf_size);
    return;
  }
  measure_p.init(sf_buffer, log_h, 1, worker_com, max_sf_window);

  // do this different we don't need all this search window.
  if (srslte_sync_init(&sync_find, max_sf_window * max_sf_size, 5 * max_sf_size, max_fft_sz)) {
    ERROR("Error initiating sync_find\n");
    return;
  }
  srslte_sync_set_sss_algorithm(&sync_find, SSS_FULL);
  srslte_sync_set_cfo_cp_enable(&sync_find, false, 0);
  srslte_sync_cp_en(&sync_find, false);
  srslte_sync_set_cfo_pss_enable(&sync_find, false);
  srslte_sync_set_threshold(&sync_find, 2.0f); // The highest the best for avoiding ghost cells
  srslte_sync_set_em_alpha(&sync_find, 0.3f);
  srslte_sss_set_threshold(&sync_find.sss, 300.0); // A higher value will avoid false alarms but reduce detection

  // Configure FIND object behaviour (this configuration is always the same)
  srslte_sync_set_cfo_ema_alpha(&sync_find, 1.0);
  srslte_sync_set_cfo_i_enable(&sync_find, false);
  srslte_sync_set_pss_filt_enable(&sync_find, true);
  srslte_sync_set_sss_eq_enable(&sync_find, true);

  sync_find.pss.chest_on_filter  = true;
  sync_find.sss_channel_equalize = false;

  reset();
}

void scell_recv::deinit()
{
  srslte_sync_free(&sync_find);
  free(sf_buffer[0]);
}

void scell_recv::reset()
{
  current_fft_sz = 0;
  measure_p.reset();
}

int scell_recv::find_cells(
    cf_t* input_buffer, float rx_gain_offset, srslte_cell_t cell, uint32_t nof_sf, cell_info_t cells[MAX_CELLS])
{
  uint32_t fft_sz = srslte_symbol_sz(cell.nof_prb);
  uint32_t sf_len = SRSLTE_SF_LEN(fft_sz);

  if (fft_sz != current_fft_sz) {
    if (srslte_sync_resize(&sync_find, nof_sf * sf_len, 5 * sf_len, fft_sz)) {
      ERROR("Error resizing sync nof_sf=%d, sf_len=%d, fft_sz=%d\n", nof_sf, sf_len, fft_sz);
      return SRSLTE_ERROR;
    }
    current_fft_sz = fft_sz;
  }

  int      nof_cells = 0;
  uint32_t peak_idx  = 0;
  uint32_t sf_idx    = 0;
  int      cell_id   = 0;

  srslte_cell_t found_cell;
  found_cell = cell;

  measure_p.set_rx_gain_offset(rx_gain_offset);

  for (uint32_t n_id_2 = 0; n_id_2 < 3; n_id_2++) {

    found_cell.id = 10000;

    if (n_id_2 != (cell.id % 3) || sic_pss_enabled) {
      srslte_sync_set_N_id_2(&sync_find, n_id_2);

      srslte_sync_find_ret_t sync_res;

      do {
        srslte_sync_reset(&sync_find);
        srslte_sync_cfo_reset(&sync_find);

        sync_res            = SRSLTE_SYNC_NOFOUND;
        bool sss_detected   = false;
        cell_id             = 0;
        float    max_peak   = -1;
        uint32_t max_sf5    = 0;
        uint32_t max_sf_idx = 0;

        float sss_correlation_peak_max = 0.0f;

        for (uint32_t sf5_cnt = 0; sf5_cnt < nof_sf / 5; sf5_cnt++) {
          sync_res = srslte_sync_find(&sync_find, input_buffer, sf5_cnt * 5 * sf_len, &peak_idx);
          Debug("INTRA: n_id_2=%d, cnt=%d/%d, sync_res=%d, sf_idx=%d, peak_idx=%d, peak_value=%f, sss_detected=%d\n",
                n_id_2,
                sf5_cnt,
                nof_sf / 5,
                sync_res,
                srslte_sync_get_sf_idx(&sync_find),
                peak_idx,
                sync_find.peak_value,
                srslte_sync_sss_detected(&sync_find));

          if (sync_find.peak_value > max_peak && sync_res == SRSLTE_SYNC_FOUND &&
              srslte_sync_sss_detected(&sync_find)) {
            max_sf5    = sf5_cnt;
            max_sf_idx = srslte_sync_get_sf_idx(&sync_find);

            // Uses the cell ID from the highest SSS correlation peak
            if (sss_correlation_peak_max < srslte_sync_sss_correlation_peak(&sync_find)) {
              // Set the cell ID
              cell_id = srslte_sync_get_cell_id(&sync_find);

              // Update the maximum value
              sss_correlation_peak_max = srslte_sync_sss_correlation_peak(&sync_find);
            }
            sss_detected = true;
          }
        }

        // If the SSS was not detected, the cell id is not reliable. So, consider no sync found
        if (!sss_detected) {
          sync_res = SRSLTE_SYNC_NOFOUND;
        }

        switch (sync_res) {
          case SRSLTE_SYNC_ERROR:
            return SRSLTE_ERROR;
            ERROR("Error finding correlation peak\n");
            return SRSLTE_ERROR;
          case SRSLTE_SYNC_FOUND:

            sf_idx = (10 - max_sf_idx - 5 * (max_sf5 % 2)) % 10;

            if (cell_id >= 0) {
              // We found the same cell as before, look another N_id_2
              if ((uint32_t)cell_id == found_cell.id || (uint32_t)cell_id == cell.id) {
                Debug("INTRA: n_id_2=%d, PCI=%d, found_cell.id=%d, cell.id=%d\n",
                      n_id_2,
                      cell_id,
                      found_cell.id,
                      cell.id);
                sync_res = SRSLTE_SYNC_NOFOUND;
              } else {
                // We found a new cell ID
                found_cell.id        = (uint32_t)cell_id;
                found_cell.nof_ports = 1; // Use port 0 only for measurement
                measure_p.set_cell(found_cell);

                // Correct CFO
                /*
                srslte_cfo_correct(&sync_find.cfo_corr_frame,
                                   input_buffer,
                                   input_cfo_corrected,
                                   -srslte_sync_get_cfo(&sync_find)/sync_find.fft_size);
                */

                switch (measure_p.run_multiple_subframes(input_buffer, peak_idx, sf_idx, nof_sf)) {
                  default:
                    // Consider a cell to be detectable 8.1.2.2.1.1 from 36.133. Currently only using first condition
                    if (measure_p.rsrp() > ABSOLUTE_RSRP_THRESHOLD_DBM) {
                      cells[nof_cells].pci    = found_cell.id;
                      cells[nof_cells].rsrp   = measure_p.rsrp();
                      cells[nof_cells].rsrq   = measure_p.rsrq();
                      cells[nof_cells].offset = measure_p.frame_st_idx();

                      Info("INTRA: Found neighbour cell %d: PCI=%03d, RSRP=%5.1f dBm, SNR=%5.1f dB, peak_idx=%5d, "
                           "peak_value=%3.2f, "
                           "sf=%d, max_sf=%d, n_id_2=%d, CFO=%6.1f/%6.1fHz\n",
                           nof_cells,
                           cell_id,
                           measure_p.rsrp(),
                           measure_p.snr(),
                           measure_p.frame_st_idx(),
                           sync_find.peak_value,
                           sf_idx,
                           max_sf5,
                           n_id_2,
                           15000 * srslte_sync_get_cfo(&sync_find),
                           15000 * measure_p.cfo());

                      nof_cells++;

                      /*
                      if (sic_pss_enabled) {
                        srslte_pss_sic(&sync_find.pss, &input_buffer[sf5_cnt * 5 * sf_len + sf_len / 2 - fft_sz]);
                      }*/
                    } else {
                      Info("INTRA: Found neighbour cell but RSRP=%.1f dBm is below threshold (%.1f dBm)\n",
                           measure_p.rsrp(),
                           ABSOLUTE_RSRP_THRESHOLD_DBM);
                    }
                    break;
                  case measure::ERROR:
                    Error("INTRA: Measuring neighbour cell\n");
                    return SRSLTE_ERROR;
                }
              }
            } else {
              sync_res = SRSLTE_SYNC_NOFOUND;
            }
            break;
          case SRSLTE_SYNC_FOUND_NOSPACE:
            /* If a peak was found but there is not enough space for SSS/CP detection, discard a few samples */
            break;
          default:
            break;
        }
      } while (sync_res == SRSLTE_SYNC_FOUND && sic_pss_enabled && nof_cells < MAX_CELLS);
    }
  }
  return nof_cells;
}

} // namespace scell
} // namespace srsue
