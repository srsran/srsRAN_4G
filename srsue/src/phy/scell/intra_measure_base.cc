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
#include "srsue/hdr/phy/scell/intra_measure_base.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.error("INTRA-%s: " fmt, to_string(get_rat()).c_str(), ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.warning("INTRA-%s: " fmt, to_string(get_rat()).c_str(), ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.info("INTRA-%s: " fmt, to_string(get_rat()).c_str(), ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.debug("INTRA-%s: " fmt, to_string(get_rat()).c_str(), ##__VA_ARGS__)

namespace srsue {
namespace scell {

intra_measure_base::intra_measure_base(srslog::basic_logger& logger, meas_itf& new_cell_itf_) :
  logger(logger), context(new_cell_itf_), thread("SYNC_INTRA_MEASURE")
{}

intra_measure_base::~intra_measure_base()
{
  srsran_ringbuffer_free(&ring_buffer);
}

void intra_measure_base::init_generic(uint32_t cc_idx_, const args_t& args)
{
  context.cc_idx = cc_idx_;

  context.meas_len_ms       = args.len_ms;
  context.meas_period_ms    = args.period_ms;
  context.rx_gain_offset_db = args.rx_gain_offset_db;

  context.sf_len = SRSRAN_SF_LEN_PRB(SRSRAN_MAX_PRB);
  if (isnormal(args.srate_hz)) {
    context.sf_len = (uint32_t)round(args.srate_hz / 1000.0);
  }

  // Calculate the new required bytes
  int max_required_bytes = (int)(sizeof(cf_t) * context.meas_len_ms * context.sf_len);

  // Reallocate only if the required capacity exceds the new requirement
  if (ring_buffer.capacity < max_required_bytes) {
    search_buffer.resize(context.meas_len_ms * context.sf_len);

    srsran_ringbuffer_free(&ring_buffer);

    // Initialise buffer for the maximum number of PRB
    if (srsran_ringbuffer_init(&ring_buffer, max_required_bytes) < SRSRAN_SUCCESS) {
      return;
    }
  }

  state.set_state(internal_state::idle);
  start(INTRA_FREQ_MEAS_PRIO);
}

void intra_measure_base::stop()
{
  // Notify quit to asynchronous thread. If it is measuring, it will first finish the measure, report to stack and
  // then it will finish
  state.set_state(internal_state::quit);

  // Wait for the asynchronous thread to finish
  wait_thread_finish();

  srsran_ringbuffer_stop(&ring_buffer);
}

void intra_measure_base::set_rx_gain_offset(float rx_gain_offset_db_)
{
  context.rx_gain_offset_db = rx_gain_offset_db_;
}

void intra_measure_base::meas_stop()
{
  // Transition state to idle
  // Ring-buffer shall not be reset, it will automatically be reset as soon as the FSM transitions to receive
  state.set_state(internal_state::idle);
  Info("Disabled neighbour cell search for EARFCN %d", get_earfcn());
}

void intra_measure_base::set_cells_to_meas(const std::set<uint32_t>& pci)
{
  active_pci_mutex.lock();
  context.active_pci = pci;
  active_pci_mutex.unlock();
  state.set_state(internal_state::receive);
  Info("Received list of %zd neighbour cells to measure in EARFCN %d.", pci.size(), get_earfcn());
}

void intra_measure_base::write(uint32_t tti, cf_t* data, uint32_t nsamples)
{
  int      nbytes          = (int)(nsamples * sizeof(cf_t));
  int      required_nbytes = (int)(context.meas_len_ms * context.sf_len * sizeof(cf_t));
  uint32_t elapsed_tti     = TTI_SUB(tti, last_measure_tti);

  switch (state.get_state()) {
    case internal_state::idle:
    case internal_state::measure:
    case internal_state::quit:
      // Do nothing
      break;
    case internal_state::wait:
      if (elapsed_tti >= context.meas_period_ms) {
        state.set_state(internal_state::receive);
        last_measure_tti = tti;
        srsran_ringbuffer_reset(&ring_buffer);
      }
      break;
    case internal_state::receive:
      // As nbytes might not match the sub-frame size, make sure that buffer does not overflow
      nbytes = SRSRAN_MIN(srsran_ringbuffer_space(&ring_buffer), nbytes);

      // Try writing in the buffer
      if (srsran_ringbuffer_write(&ring_buffer, data, nbytes) < nbytes) {
        Warning("Error writing to ringbuffer (EARFCN=%d)", get_earfcn());

        // Transition to wait, so it can keep receiving without stopping the component operation
        state.set_state(internal_state::wait);
      } else {
        // As soon as there are enough samples in the buffer, transition to measure
        if (srsran_ringbuffer_status(&ring_buffer) >= required_nbytes) {
          state.set_state(internal_state::measure);
        }
      }
      break;
  }
}

void intra_measure_base::measure_proc()
{
  std::set<uint32_t> cells_to_measure = {};

  // Read data from buffer and find cells in it
  srsran_ringbuffer_read(&ring_buffer, search_buffer.data(), (int)context.meas_len_ms * context.sf_len * sizeof(cf_t));

  // Go to receive before finishing, so new samples can be enqueued before the thread finishes
  if (state.get_state() == internal_state::measure) {
    // Prevents transition to wait if state has changed while reading the ring-buffer
    state.set_state(internal_state::wait);
  }

  // Perform measurements for the actual RAT
  measure_rat(context, search_buffer);
}

void intra_measure_base::run_thread()
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
