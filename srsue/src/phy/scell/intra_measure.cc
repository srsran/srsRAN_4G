/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "srsue/hdr/phy/scell/intra_measure.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED and log_h != nullptr)                                                                       \
  log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSLTE_DEBUG_ENABLED and log_h != nullptr)                                                                       \
  log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSLTE_DEBUG_ENABLED and log_h != nullptr)                                                                       \
  log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED and log_h != nullptr)                                                                       \
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

void intra_measure::init(uint32_t cc_idx_, phy_common* common, meas_itf* new_cell_itf_, srslte::log* log_h_)
{
  cc_idx       = cc_idx_;
  new_cell_itf = new_cell_itf_;
  log_h        = log_h_;

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

  // Initialise buffer for the maximum number of PRB
  uint32_t max_required_bytes = (uint32_t)sizeof(cf_t) * intra_freq_meas_len_ms * SRSLTE_SF_LEN_PRB(SRSLTE_MAX_PRB);
  if (srslte_ringbuffer_init(&ring_buffer, max_required_bytes)) {
    return;
  }

  state.set_state(internal_state::idle);
  start(INTRA_FREQ_MEAS_PRIO);
}

void intra_measure::stop()
{
  // Notify quit to asynchronous thread. If it is measuring, it will first finish the measure, report to stack and
  // then it will finish
  state.set_state(internal_state::quit);

  // Wait for the asynchronous thread to finish
  wait_thread_finish();

  srslte_ringbuffer_stop(&ring_buffer);
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
  // Transition state to idle
  // Ring-buffer shall not be reset, it will automatically be reset as soon as the FSM transitions to receive
  state.set_state(internal_state::idle);
  Info("INTRA: Disabled neighbour cell search for EARFCN %d\n", get_earfcn());
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
  int      nbytes          = (int)(nsamples * sizeof(cf_t));
  int      required_nbytes = (int)(intra_freq_meas_len_ms * current_sflen * sizeof(cf_t));
  uint32_t elapsed_tti     = TTI_SUB(tti, last_measure_tti);

  switch (state.get_state()) {

    case internal_state::idle:
    case internal_state::measure:
    case internal_state::quit:
      // Do nothing
      break;
    case internal_state::wait:
      if (elapsed_tti >= intra_freq_meas_period_ms) {
        state.set_state(internal_state::receive);
        last_measure_tti = tti;
        srslte_ringbuffer_reset(&ring_buffer);
      }
      break;
    case internal_state::receive:
      // As nbytes might not match the sub-frame size, make sure that buffer does not overflow
      nbytes = SRSLTE_MIN(srslte_ringbuffer_space(&ring_buffer), nbytes);

      // Try writing in the buffer
      if (srslte_ringbuffer_write(&ring_buffer, data, nbytes) < nbytes) {
        Warning("INTRA: Error writing to ringbuffer (EARFCN=%d)\n", current_earfcn);

        // Transition to wait, so it can keep receiving without stopping the component operation
        state.set_state(internal_state::wait);
      } else {
        // As soon as there are enough samples in the buffer, transition to measure
        if (srslte_ringbuffer_status(&ring_buffer) >= required_nbytes) {
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
  srslte_ringbuffer_read(&ring_buffer, search_buffer, (int)intra_freq_meas_len_ms * current_sflen * sizeof(cf_t));

  // Go to receive before finishing, so new samples can be enqueued before the thread finishes
  if (state.get_state() == internal_state::measure) {
    // Prevents transition to wait if state has changed while reading the ring-buffer
    state.set_state(internal_state::wait);
  }

  // Detect new cells using PSS/SSS
  std::set<uint32_t> detected_cells = scell.find_cells(search_buffer, serving_cell, intra_freq_meas_len_ms);

  // Add detected cells to the list of cells to measure
  for (const uint32_t& c : detected_cells) {
    cells_to_measure.insert(c);
  }

  // Initialise empty neighbour cell list
  std::vector<phy_meas_t> neighbour_cells = {};

  new_cell_itf->cell_meas_reset(cc_idx);

  // Use Cell Reference signal to measure cells in the time domain for all known active PCI
  for (const uint32_t& id : cells_to_measure) {
    // Do not measure serving cell here since it's measured by workers
    if (id == serving_cell.id) {
      continue;
    }
    srslte_cell_t cell = serving_cell;
    cell.id            = id;

    srslte_refsignal_dl_sync_set_cell(&refsignal_dl_sync, cell);
    srslte_refsignal_dl_sync_run(&refsignal_dl_sync, search_buffer, intra_freq_meas_len_ms * current_sflen);

    if (refsignal_dl_sync.found) {
      phy_meas_t m = {};
      m.pci        = cell.id;
      m.earfcn     = current_earfcn;
      m.rsrp       = refsignal_dl_sync.rsrp_dBfs - rx_gain_offset_db;
      m.rsrq       = refsignal_dl_sync.rsrq_dB;
      m.cfo_hz     = refsignal_dl_sync.cfo_Hz;
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
    new_cell_itf->new_cell_meas(cc_idx, neighbour_cells);
  }
}

void intra_measure::run_thread()
{
  bool quit = false;

  do {
    // Get state
    internal_state::state_t s = state.get_state();
    switch (s) {

      case internal_state::idle:
      case internal_state::wait:
      case internal_state::receive:
        // Wait for a different state
        state.wait_change(s);
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
