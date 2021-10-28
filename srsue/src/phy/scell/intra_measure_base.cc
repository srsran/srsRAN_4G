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
#include "srsue/hdr/phy/scell/intra_measure_base.h"

#define Log(level, fmt, ...)                                                                                           \
  do {                                                                                                                 \
    logger.level("INTRA-%s-%d: " fmt, to_string(get_rat()).c_str(), get_earfcn(), ##__VA_ARGS__);                      \
  } while (false)

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

  context.meas_len_ms        = args.len_ms;
  context.meas_period_ms     = args.period_ms;
  context.trigger_tti_period = args.tti_period;
  context.trigger_tti_offset = args.tti_offset;
  rx_gain_offset_db          = args.rx_gain_offset_db;

  // Compute subframe length from the sampling rate if available
  if (std::isnormal(args.srate_hz)) {
    context.sf_len = (uint32_t)round(args.srate_hz / 1000.0);
  } else if (get_rat() == srsran::srsran_rat_t::lte) {
    // Select maximum subframe size for LTE
    context.sf_len = SRSRAN_SF_LEN_PRB(SRSRAN_MAX_PRB);
  } else {
    // No maximum subframe length is defined for other RATs
    ERROR("A sampling rate was expected for %s. Undefined behaviour.", srsran::to_string(get_rat()).c_str());
    return;
  }

  // Calculate the new required bytes
  int max_required_bytes = (int)(sizeof(cf_t) * context.meas_len_ms * context.sf_len);

  // Reallocate only if the required capacity exceds the new requirement
  if (ring_buffer.capacity < max_required_bytes) {
    search_buffer.resize((size_t)context.meas_len_ms * (size_t)context.sf_len);

    srsran_ringbuffer_free(&ring_buffer);

    // Initialise buffer for the maximum number of PRB
    if (srsran_ringbuffer_init(&ring_buffer, max_required_bytes) < SRSRAN_SUCCESS) {
      ERROR("Error initiating ringbuffer");
      return;
    }
  }

  if (state.get_state() == internal_state::initial) {
    state.set_state(internal_state::idle);
    start(INTRA_FREQ_MEAS_PRIO);
  }
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
  rx_gain_offset_db = rx_gain_offset_db_;
}

void intra_measure_base::meas_stop()
{
  // Transition state to idle
  // Ring-buffer shall not be reset, it will automatically be reset as soon as the FSM transitions to receive
  state.set_state(internal_state::idle);
  Log(info, "Disabled neighbour cell search");
}

void intra_measure_base::set_cells_to_meas(const std::set<uint32_t>& pci)
{
  mutex.lock();
  context.active_pci = pci;
  mutex.unlock();
  state.set_state(internal_state::wait_first);
  Log(info, "Received list of %zd neighbour cells to measure", pci.size());
}

void intra_measure_base::write(cf_t* data, uint32_t nsamples)
{
  int nbytes = (int)(nsamples * sizeof(cf_t));

  mutex.lock();
  int required_nbytes = ((int)context.meas_len_ms * (int)context.sf_len * (int)sizeof(cf_t));
  mutex.unlock();

  // As nbytes might not match the sub-frame size, make sure that buffer does not overflow
  nbytes = SRSRAN_MIN(srsran_ringbuffer_space(&ring_buffer), nbytes);

  // Try writing in the buffer
  if (srsran_ringbuffer_write(&ring_buffer, data, nbytes) < nbytes) {
    Log(warning, "Error writing to ringbuffer");

    // Transition to wait, so it can keep receiving without stopping the component operation
    state.set_state(internal_state::wait);
  } else {
    // As soon as there are enough samples in the buffer, transition to measure
    if (srsran_ringbuffer_status(&ring_buffer) >= required_nbytes) {
      Log(debug, "Starting search and measurements");
      state.set_state(internal_state::measure);
    }
  }
}

void intra_measure_base::run_tti(uint32_t tti, cf_t* data, uint32_t nsamples)
{
  logger.set_context(tti);

  switch (state.get_state()) {
    case internal_state::initial:
    case internal_state::idle:
    case internal_state::measure:
    case internal_state::quit:
      // Do nothing
      break;
    case internal_state::wait:
    case internal_state::wait_first:
      // Check measurement trigger condition
      if (receive_tti_trigger(tti)) {
        state.set_state(internal_state::receive);
        last_measure_tti = tti;
        srsran_ringbuffer_reset(&ring_buffer);

        // Write baseband to ensure measurement starts in the right TTI
        Log(debug, "Start writing");
        write(data, nsamples);
      }
      break;
    case internal_state::receive:
      // Write baseband
      write(data, nsamples);
      break;
  }
}

void intra_measure_base::measure_proc()
{
  // Grab a copy of the context and pass it to the measure_rat method.
  measure_context_t context_copy = get_context();

  // Read data from buffer and find cells in it
  int ret = srsran_ringbuffer_read_timed(&ring_buffer,
                                         search_buffer.data(),
                                         ((int)context_copy.meas_len_ms * (int)context_copy.sf_len * (int)sizeof(cf_t)),
                                         1000);

  // As this function is called once the ring-buffer has enough data to process, it is not expected to fail
  if (ret < SRSRAN_SUCCESS) {
    Log(error, "Ringbuffer read returned %d", ret);
    return;
  }

  // Go to receive before finishing, so new samples can be enqueued before the thread finishes
  if (state.get_state() == internal_state::measure) {
    // Prevents transition to wait if state has changed while reading the ring-buffer
    state.set_state(internal_state::wait);
  }

  // Perform measurements for the actual RAT
  if (not measure_rat(std::move(context_copy), search_buffer, rx_gain_offset_db)) {
    Log(error, "Error measuring RAT");
  }
}

void intra_measure_base::run_thread()
{
  bool quit = false;

  do {
    // Get state
    internal_state::state_t s = state.get_state();
    switch (s) {
      case internal_state::initial:
      case internal_state::idle:
      case internal_state::wait:
      case internal_state::wait_first:
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
