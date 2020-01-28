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

intra_measure::intra_measure() : scell(), thread("SYNC_INTRA_MEASURE") {}

intra_measure::~intra_measure()
{
  srslte_ringbuffer_free(&ring_buffer);
  scell.deinit();
  free(search_buffer);
}

void intra_measure::init(phy_common* common_, rrc_interface_phy_lte* rrc_, srslte::log* log_h_)
{
  this->rrc       = rrc_;
  this->log_h     = log_h_;
  this->common    = common_;
  receive_enabled = false;

  // Initialise Reference signal measurement
  srslte_refsignal_dl_sync_init(&refsignal_dl_sync);

  // Start scell
  scell.init(log_h, common->args->intra_freq_meas_len_ms);

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

void intra_measure::set_primary_cell(uint32_t earfcn, srslte_cell_t cell)
{
  this->current_earfcn = earfcn;
  current_sflen        = (uint32_t)SRSLTE_SF_LEN_PRB(cell.nof_prb);
  this->serving_cell   = cell;
}

void intra_measure::meas_stop()
{
  receive_enabled = false;
  receiving       = false;
  receive_cnt     = 0;
  srslte_ringbuffer_reset(&ring_buffer);
  if (log_h) {
    log_h->info("INTRA: Disabled neighbour cell search for EARFCN %d\n", get_earfcn());
  }
}

void intra_measure::set_cells_to_meas(const std::set<uint32_t>& pci)
{
  active_pci_mutex.lock();
  active_pci = pci;
  active_pci_mutex.unlock();
  receive_enabled = true;
  log_h->info("INTRA: Received list of %lu neighbour cells to measure in EARFCN %d.\n", pci.size(), current_earfcn);
}

void intra_measure::write(uint32_t tti, cf_t* data, uint32_t nsamples)
{
  if (receive_enabled) {
    if ((tti % common->args->intra_freq_meas_period_ms) == 0) {
      receiving   = true;
      receive_cnt = 0;
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
  std::set<uint32_t> cells_to_measure = {};

  while (running) {
    if (running) {
      tti_sync.wait();
    }

    if (running) {

      active_pci_mutex.lock();
      cells_to_measure = active_pci;
      active_pci_mutex.unlock();

      // Read data from buffer and find cells in it
      srslte_ringbuffer_read(
          &ring_buffer, search_buffer, common->args->intra_freq_meas_len_ms * current_sflen * sizeof(cf_t));

      // Detect new cells using PSS/SSS
      std::set<uint32_t> detected_cells =
          scell.find_cells(search_buffer, serving_cell, common->args->intra_freq_meas_len_ms);

      // Add detected cells to the list of cells to measure
      for (auto& c : detected_cells) {
        cells_to_measure.insert(c);
      }

      receiving = false;

      std::vector<rrc_interface_phy_lte::phy_meas_t> neighbour_cells = {};

      // Use Cell Reference signal to measure cells in the time domain for all known active PCI
      for (auto id : cells_to_measure) {
        // Do not measure serving cell here since it's measured by workers
        if (id == serving_cell.id) {
          continue;
        }
        srslte_cell_t cell = serving_cell;
        cell.id            = id;

        srslte_refsignal_dl_sync_set_cell(&refsignal_dl_sync, cell);
        srslte_refsignal_dl_sync_run(
            &refsignal_dl_sync, search_buffer, common->args->intra_freq_meas_len_ms * current_sflen);

        if (refsignal_dl_sync.found) {
          rrc_interface_phy_lte::phy_meas_t m = {};
          m.pci                               = cell.id;
          m.earfcn                            = current_earfcn;
          m.rsrp                              = refsignal_dl_sync.rsrp_dBfs - common->rx_gain_offset;
          m.rsrq                              = refsignal_dl_sync.rsrq_dB;
          neighbour_cells.push_back(m);

          Info("INTRA: Found neighbour cell: EARFCN=%d, PCI=%03d, RSRP=%5.1f dBm, RSRQ=%5.1f, peak_idx=%5d, "
               "CFO=%+.1fHz\n",
               m.earfcn,
               m.pci,
               m.rsrp,
               m.rsrq,
               refsignal_dl_sync.peak_index,
               refsignal_dl_sync.cfo_Hz);
        }
      }

      // Send measurements to RRC
      if (not neighbour_cells.empty()) {
        rrc->new_cell_meas(neighbour_cells);
      }

      meas_sync.increase();
    }
  }
}

} // namespace scell
} // namespace srsue
