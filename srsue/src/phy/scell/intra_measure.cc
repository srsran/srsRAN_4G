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

void intra_measure::init(phy_common* common, rrc_interface_phy_lte* rrc_, srslte::log* log_h_)
{
  rrc   = rrc_;
  log_h = log_h_;

  if (common) {
    intra_freq_meas_len_ms    = common->args->intra_freq_meas_len_ms;
    intra_freq_meas_period_ms = common->args->intra_freq_meas_period_ms;
    rx_gain_offset_db         = common->args->rx_gain_offset;
  }

  // Initialise Reference signal measurement
  srslte_refsignal_dl_sync_init(&refsignal_dl_sync);

  // Start scell
  scell.init(log_h, intra_freq_meas_len_ms);

  search_buffer = srslte_vec_cf_malloc(intra_freq_meas_len_ms * SRSLTE_SF_LEN_PRB(SRSLTE_MAX_PRB));

  if (srslte_ringbuffer_init(&ring_buffer, sizeof(cf_t) * intra_freq_meas_len_ms * SRSLTE_SF_LEN_PRB(SRSLTE_MAX_PRB))) {
    return;
  }

  state.set_state(internal_state::idle);
  start(INTRA_FREQ_MEAS_PRIO);
}

void intra_measure::stop()
{
  state.set_state(internal_state::quit);
  srslte_ringbuffer_stop(&ring_buffer);
  wait_thread_finish();
  srslte_refsignal_dl_sync_free(&refsignal_dl_sync);
}

void intra_measure::set_primary_cell(uint32_t earfcn, srslte_cell_t cell)
{
  current_earfcn = earfcn;
  current_sflen  = (uint32_t)SRSLTE_SF_LEN_PRB(cell.nof_prb);
  serving_cell   = cell;
}

void intra_measure::set_rx_gain_offset(float rx_gain_offset_db_)
{
  rx_gain_offset_db = rx_gain_offset_db_;
}

void intra_measure::meas_stop()
{
  state.set_state(internal_state::idle);
  receive_cnt = 0;
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
  state.set_state(internal_state::receive);
  log_h->info("INTRA: Received list of %zd neighbour cells to measure in EARFCN %d.\n", pci.size(), current_earfcn);
}

void intra_measure::write(uint32_t tti, cf_t* data, uint32_t nsamples)
{
  uint32_t elapsed_tti = ((tti + 10240) - last_measure_tti) % 10240;

  switch (state.get_state()) {

    case internal_state::idle:
    case internal_state::measure:
    case internal_state::quit:
      // Do nothing
      break;
    case internal_state::wait:
      if (elapsed_tti >= intra_freq_meas_period_ms) {
        state.set_state(internal_state::receive);
        receive_cnt      = 0;
        last_measure_tti = tti;
        srslte_ringbuffer_reset(&ring_buffer);
      }
      break;
    case internal_state::receive:
      if (srslte_ringbuffer_write(&ring_buffer, data, nsamples * sizeof(cf_t)) < (int)(nsamples * sizeof(cf_t))) {
        Warning("Error writing to ringbuffer\n");
        state.set_state(internal_state::idle);
      } else {
        receive_cnt++;
        if (receive_cnt == intra_freq_meas_len_ms) {
          // Buffer ready for measuring, start
          state.set_state(internal_state::measure);
        }
      }
      break;
  }
}

void intra_measure::measure_proc()
{
  std::set<uint32_t> cells_to_measure = {};

  // Load cell list to measure
  active_pci_mutex.lock();
  cells_to_measure = active_pci;
  active_pci_mutex.unlock();

  // Read data from buffer and find cells in it
  srslte_ringbuffer_read(&ring_buffer, search_buffer, intra_freq_meas_len_ms * current_sflen * sizeof(cf_t));

  // Go to receive before finishing, so new samples can be enqueued before the thread finishes
  if (state.get_state() == internal_state::measure) {
    // Prevents transition to wait if state has changed while reading the ring-buffer
    state.set_state(internal_state::wait);
  }

  // Detect new cells using PSS/SSS
  std::set<uint32_t> detected_cells = scell.find_cells(search_buffer, serving_cell, intra_freq_meas_len_ms);

  // Add detected cells to the list of cells to measure
  for (auto& c : detected_cells) {
    cells_to_measure.insert(c);
  }

  // Initialise empty neighbour cell list
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
    srslte_refsignal_dl_sync_run(&refsignal_dl_sync, search_buffer, intra_freq_meas_len_ms * current_sflen);

    if (refsignal_dl_sync.found) {
      rrc_interface_phy_lte::phy_meas_t m = {};
      m.pci                               = cell.id;
      m.earfcn                            = current_earfcn;
      m.rsrp                              = refsignal_dl_sync.rsrp_dBfs - rx_gain_offset_db;
      m.rsrq                              = refsignal_dl_sync.rsrq_dB;
      m.cfo_hz                            = refsignal_dl_sync.cfo_Hz;
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

  // Send measurements to RRC if any cell found
  if (not neighbour_cells.empty()) {
    rrc->new_cell_meas(neighbour_cells);
  }

  // Inform that measurement has finished
  meas_sync.increase();
}

void intra_measure::run_thread()
{
  bool quit = false;

  do {
    switch (state.get_state()) {

      case internal_state::idle:
      case internal_state::wait:
      case internal_state::receive:
        // Wait for a state change
        state.wait_change();
        break;
      case internal_state::measure:
        // Run the measurement process
        measure_proc();
        break;
      case internal_state::quit:
        // Quit loop
        quit = true;
        break;
    }
  } while (not quit);
}

} // namespace scell
} // namespace srsue
