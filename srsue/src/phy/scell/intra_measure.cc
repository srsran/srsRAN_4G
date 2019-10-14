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
#include "srsue/hdr/phy/scell/intra_measure.h"

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

intra_measure::intra_measure() : scell(), thread("SYNC_INTRA_MEASURE")
{
}

intra_measure::~intra_measure()
{
  srslte_ringbuffer_free(&ring_buffer);
  scell.deinit();
  free(search_buffer);
}

void intra_measure::init(phy_common* common, rrc_interface_phy_lte* rrc, srslte::log* log_h)
{
  this->rrc       = rrc;
  this->log_h     = log_h;
  this->common    = common;
  receive_enabled = false;

  // Initialise Reference signal measurement
  srslte_refsignal_dl_sync_init(&refsignal_dl_sync);

  // Start scell
  scell.init(log_h, common->args->sic_pss_enabled, common->args->intra_freq_meas_len_ms, common);

  search_buffer =
      (cf_t*)srslte_vec_malloc(common->args->intra_freq_meas_len_ms * SRSLTE_SF_LEN_PRB(SRSLTE_MAX_PRB) * sizeof(cf_t));

  if (srslte_ringbuffer_init(
          &ring_buffer, sizeof(cf_t) * common->args->intra_freq_meas_len_ms * 2 * SRSLTE_SF_LEN_PRB(SRSLTE_MAX_PRB))) {
    return;
  }

  running = true;
  start(INTRA_FREQ_MEAS_PRIO);
}

void intra_measure::stop()
{
  running = false;
  srslte_ringbuffer_stop(&ring_buffer);
  tti_sync.increase();
  wait_thread_finish();
  srslte_refsignal_dl_sync_free(&refsignal_dl_sync);
}

void intra_measure::set_primay_cell(uint32_t earfcn, srslte_cell_t cell)
{
  this->current_earfcn = earfcn;
  current_sflen        = (uint32_t)SRSLTE_SF_LEN_PRB(cell.nof_prb);
  this->primary_cell   = cell;
}

void intra_measure::clear_cells()
{
  active_pci.clear();
  receive_enabled = false;
  receiving       = false;
  receive_cnt     = 0;
  srslte_ringbuffer_reset(&ring_buffer);
}

void intra_measure::add_cell(int pci)
{
  if (std::find(active_pci.begin(), active_pci.end(), pci) == active_pci.end()) {
    active_pci.push_back(pci);
    receive_enabled = true;
    Info("INTRA: Starting intra-frequency measurement for pci=%d\n", pci);
  } else {
    Debug("INTRA: Requested to start already existing intra-frequency measurement for PCI=%d\n", pci);
  }
}

int intra_measure::get_offset(uint32_t pci)
{
  for (auto& i : info) {
    if (i.pci == pci) {
      return i.offset;
    }
  }
  return -1;
}

void intra_measure::rem_cell(int pci)
{
  auto newEnd = std::remove(active_pci.begin(), active_pci.end(), pci);

  if (newEnd != active_pci.end()) {
    active_pci.erase(newEnd, active_pci.end());
    if (active_pci.empty()) {
      receive_enabled = false;
    }
    Info("INTRA: Stopping intra-frequency measurement for pci=%d. Number of cells: %zu\n", pci, active_pci.size());
  } else {
    Warning("INTRA: Requested to stop non-existing intra-frequency measurement for PCI=%d\n", pci);
  }
}

void intra_measure::write(uint32_t tti, cf_t* data, uint32_t nsamples)
{
  if (receive_enabled) {
    if ((tti % common->args->intra_freq_meas_period_ms) == 0) {
      receiving   = true;
      receive_cnt = 0;
      measure_tti = tti;
      srslte_ringbuffer_reset(&ring_buffer);
    }
    if (receiving) {
      if (srslte_ringbuffer_write(&ring_buffer, data, nsamples * sizeof(cf_t)) < (int)(nsamples * sizeof(cf_t))) {
        Warning("Error writting to ringbuffer\n");
        receiving = false;
      } else {
        receive_cnt++;
        if (receive_cnt == common->args->intra_freq_meas_len_ms) {
          tti_sync.increase();
          receiving = false;
        }
      }
    }
  }
}

void intra_measure::run_thread()
{
  while (running) {
    if (running) {
      tti_sync.wait();
    }

    if (running) {

      // Read data from buffer and find cells in it
      srslte_ringbuffer_read(
          &ring_buffer, search_buffer, common->args->intra_freq_meas_len_ms * current_sflen * sizeof(cf_t));
      int found_cells = scell.find_cells(
          search_buffer, common->rx_gain_offset, primary_cell, common->args->intra_freq_meas_len_ms, info);
      receiving = false;

      // Look for other cells not found automatically
      // Using Cell Reference signal synchronization for all known active PCI
      for (auto q : active_pci) {
        srslte_cell_t cell = primary_cell;
        cell.id            = q;

        srslte_refsignal_dl_sync_set_cell(&refsignal_dl_sync, cell);
        srslte_refsignal_dl_sync_run(
            &refsignal_dl_sync, search_buffer, common->args->intra_freq_meas_len_ms * current_sflen);

        if (refsignal_dl_sync.found) {
          Info("INTRA: Found neighbour cell: PCI=%03d, RSRP=%5.1f dBm, RSRQ=%5.1f, peak_idx=%5d, CFO=%+.1fHz\n",
               cell.id,
               refsignal_dl_sync.rsrp_dBfs,
               refsignal_dl_sync.rsrq_dB,
               refsignal_dl_sync.peak_index,
               refsignal_dl_sync.cfo_Hz);

          bool     found              = false;
          float    weakest_rsrp_value = +INFINITY;
          uint32_t weakest_rsrp_index = 0;

          // Try to find PCI in info list
          for (int i = 0; i < found_cells && !found; i++) {
            // Finds cell, update
            if (info[i].pci == cell.id) {
              info[i].rsrp   = refsignal_dl_sync.rsrp_dBfs;
              info[i].rsrq   = refsignal_dl_sync.rsrq_dB;
              info[i].offset = refsignal_dl_sync.peak_index;
              found          = true;
            } else if (weakest_rsrp_value > info[i].rsrp) {
              // Update weakest
              weakest_rsrp_value = info[i].rsrp;
              weakest_rsrp_index = i;
            }
          }

          if (!found) {
            // If number of cells exceeds
            if (found_cells >= scell_recv::MAX_CELLS) {
              // overwrite weakest cell if stronger
              if (refsignal_dl_sync.rsrp_dBfs > weakest_rsrp_value) {
                info[weakest_rsrp_index].pci    = cell.id;
                info[weakest_rsrp_index].rsrp   = refsignal_dl_sync.rsrp_dBfs;
                info[weakest_rsrp_index].rsrq   = refsignal_dl_sync.rsrq_dB;
                info[weakest_rsrp_index].offset = refsignal_dl_sync.peak_index;
              } else {
                // Ignore measurement
              }
            } else {
              // Otherwise append cell
              info[found_cells].pci    = cell.id;
              info[found_cells].rsrp   = refsignal_dl_sync.rsrp_dBfs;
              info[found_cells].rsrq   = refsignal_dl_sync.rsrq_dB;
              info[found_cells].offset = refsignal_dl_sync.peak_index;
              found_cells++;
            }
          }
        }
      }

      // Send measurements to RRC
      for (int i = 0; i < found_cells; i++) {
        rrc->new_phy_meas(info[i].rsrp, info[i].rsrq, measure_tti, current_earfcn, info[i].pci);
      }
    }
  }
}

} // namespace scell
} // namespace srsue
