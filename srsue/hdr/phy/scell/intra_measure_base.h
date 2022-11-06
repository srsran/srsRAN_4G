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
#ifndef SRSUE_INTRA_MEASURE_BASE_H
#define SRSUE_INTRA_MEASURE_BASE_H

#include "srsran/interfaces/ue_phy_interfaces.h"
#include <condition_variable>
#include <mutex>
#include <srsran/common/common.h>
#include <srsran/common/threads.h>
#include <srsran/common/tti_sync_cv.h>
#include <vector>

namespace srsue {
namespace scell {

/**
 * @brief Describes a generic base class to perform intra-frequency measurements
 */
class intra_measure_base : public srsran::thread
{
  /*
   * The intra-cell measurement has 5 different states:
   *  - idle: it has been initiated and it is waiting to get configured to start capturing samples. From any state
   *          except quit can transition to idle.
   *  - wait: waits for the TTI trigger to transition to receive
   *  - receive: captures base-band samples for intra_freq_meas_len_ms and goes to measure.
   *  - measure: enables the inner thread to start the measuring function. The asynchronous buffer will transition to
   *             wait as soon as it has read the data from the buffer.
   *  - quit: stops the inner thread and quits. Transition from any state measure state.
   *
   * FSM abstraction:
   *
   *  +------+   set_cells_to_meas   +------+      receive_tti_trigger      +---------+
   *  | Idle | --------------------->| Wait |------------------------------>| Receive |
   *  +------+                       +------+                               +---------+
   *     ^                              ^                                        |          stop  +------+
   *     |                  Read buffer |                                        |          ----->| Quit |
   *   init                        +---------+    intra_freq_meas_len_ms         |                +------+
   * meas_stop                     | Measure |<----------------------------------+
   *                               +---------+
   *
   * This class has been designed to be thread safe. Any method can be called from different threads as long as
   * init_generic is called when the FSM is in idle.
   */
public:
  /**
   * @brief Describes an interface for reporting new cell measurements
   */
  class meas_itf
  {
  public:
    virtual void cell_meas_reset(uint32_t cc_idx)                                    = 0;
    virtual void new_cell_meas(uint32_t cc_idx, const std::vector<phy_meas_t>& meas) = 0;
  };

  /**
   * @brief Describes the default generic configuration arguments
   */
  struct args_t {
    double   srate_hz          = 0.0;  ///< Sampling rate in Hz, optional for LTE, compulsory for NR
    uint32_t len_ms            = 20;   ///< Amount of time to accumulate
    uint32_t period_ms         = 200;  ///< Minimum time interval between measurements, set to 0 for free-run
    uint32_t tti_period        = 0;    ///< Measurement TTI trigger period, set to 0 to trigger at any TTI
    uint32_t tti_offset        = 0;    ///< Measurement TTI trigger offset
    float    rx_gain_offset_db = 0.0f; ///< Gain offset, for calibrated measurements
  };

  /**
   * @brief Stops the operation of this component and it cannot be started again
   * @note use meas_stop() method to stop measurements temporally
   */
  void stop();

  /**
   * @brief Updates the receiver gain offset to convert estimated dBFs to dBm in RSRP
   * @param rx_gain_offset Gain offset in dB
   */
  void set_rx_gain_offset(float rx_gain_offset_db);

  /**
   * @brief Sets the PCI list of the cells this components needs to measure and starts the FSM for measuring
   * @param pci is the list of PCIs to measure
   */
  void set_cells_to_meas(const std::set<uint32_t>& pci);

  /**
   * @brief Stops the measurement FSM, setting the inner state to idle.
   */
  void meas_stop();

  /**
   * @brief Inputs the baseband IQ samples into the component, internal state dictates whether it will be written or
   * not.
   * @param tti The current physical layer TTI, used for calculating the buffer write
   * @param data buffer with baseband IQ samples
   * @param nsamples number of samples to write
   */
  void run_tti(uint32_t tti, cf_t* data, uint32_t nsamples);

  /**
   * @brief Get EARFCN of this component
   * @return EARFCN
   */
  virtual uint32_t get_earfcn() const = 0;

  /**
   * @brief Synchronous wait mechanism, blocks the writer thread while it is in measure state. If the asynchronous
   * thread is too slow, use this method for stalling the writing thread and wait the asynchronous thread to clear the
   * buffer.
   */
  void wait_meas()
  { // Only used by scell_search_test
    state.wait_change(internal_state::measure);
  }

protected:
  struct measure_context_t {
    uint32_t           cc_idx             = 0;   ///< Component carrier index
    std::set<uint32_t> active_pci         = {};  ///< Set with the active PCIs
    uint32_t           sf_len             = 0;   ///< Subframe length in samples
    uint32_t           meas_len_ms        = 20;  ///< Measure length in milliseconds/sub-frames
    uint32_t           meas_period_ms     = 200; ///< Minimum time between measurements
    uint32_t           trigger_tti_period = 0;   ///< Measurement TTI trigger period
    uint32_t           trigger_tti_offset = 0;   ///< Measurement TTI trigger offset
    meas_itf&          new_cell_itf;

    explicit measure_context_t(meas_itf& new_cell_itf_) : new_cell_itf(new_cell_itf_) {}
  };

  std::atomic<float> rx_gain_offset_db = {0.0f}; ///< Current gain offset

  /**
   * @brief Generic initialization method, necessary to configure main parameters
   * @param cc_idx_ Indicates the component carrier index linked to the intra frequency measurement instance
   * @param args Generic configuration arguments
   */
  void init_generic(uint32_t cc_idx_, const args_t& args);

  /**
   * @brief Constructor is only accessible through inherited classes
   */
  intra_measure_base(srslog::basic_logger& logger, meas_itf& new_cell_itf_);

  /**
   * @brief Destructor is only accessible through inherited classes
   */
  ~intra_measure_base() override;

  /**
   * @brief Subframe length setter, the inherited class shall set the subframe length
   * @param new_sf_len New subframe length
   */
  void set_current_sf_len(uint32_t new_sf_len)
  {
    std::lock_guard<std::mutex> lock(mutex);
    context.sf_len = new_sf_len;
  }

private:
  /**
   * @brief Describes the internal state class, provides thread safe state management
   */
  class internal_state
  {
  public:
    typedef enum {
      initial = 0, /// Initial state, it transitions to idle once the internal thread has started
      idle,        ///< Internal thread runs, it does not capture data
      wait_first,  ///< Wait for the TTI trigger (if configured)
      wait,        ///< Wait for the period time to pass
      receive,     ///< Accumulate samples in ring buffer
      measure,     ///< Module is busy measuring
      quit         ///< Quit thread, no transitions are allowed
    } state_t;

  private:
    state_t                 state = initial;
    std::mutex              mutex;
    std::condition_variable cvar;

  public:
    /**
     * @brief Get the internal state
     * @return protected state
     */
    state_t get_state()
    {
      std::lock_guard<std::mutex> lock(mutex);
      return state;
    }

    /**
     * @brief Transitions to a different state, all transitions are allowed except from quit
     * @param new_state
     */
    void set_state(state_t new_state)
    {
      std::unique_lock<std::mutex> lock(mutex);
      // Do not allow transition from quit
      if (state != quit) {
        state = new_state;
      }

      // Notifies to the inner thread about the change of state
      cvar.notify_all();
    }

    /**
     * @brief Waits for a state transition to a state different than the provided, used for blocking the inner thread
     */
    void wait_change(state_t s)
    {
      std::unique_lock<std::mutex> lock(mutex);
      while (state == s) {
        cvar.wait(lock);
      }
    }
  };

  /**
   * @brief Computes the measurement trigger based on TTI and the last TTI trigger
   */
  bool receive_tti_trigger(uint32_t tti)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // If the elapsed time does not satisfy with the minimum time, do not trigger
    uint32_t elapsed_tti = TTI_SUB(tti, last_measure_tti);
    if (elapsed_tti < context.meas_period_ms and state.get_state() != internal_state::wait_first) {
      return false;
    }

    // If the TTI period is not configured, it will be always true
    if (context.trigger_tti_period == 0) {
      return true;
    }

    // Check if trigger condition is satisfied
    return tti % context.trigger_tti_period == context.trigger_tti_offset;
  }

  /**
   * @brief Writes baseband data in the internal soft-buffer
   * @param data Provides baseband data
   * @param nsamples Number of samples to write
   */
  void write(cf_t* data, uint32_t nsamples);

  /**
   * @brief Get the Radio Access Technology (RAT) that is being measured
   * @return The measured RAT
   */
  virtual srsran::srsran_rat_t get_rat() const = 0;

  /**
   * @brief Pure virtual function to perform measurements
   * @note The context is pass-by-value to protect it from concurrency. However, the buffer is pass-by-reference
   * as it is protected by the state.
   * @param context Provides current measurement context
   * @param buffer Provides current measurement context
   * @param rx_gain_offset Provides last received rx_gain_offset
   * @return True if the measurement functions are executed without errors, otherwise false
   */
  virtual bool measure_rat(const measure_context_t& context, std::vector<cf_t>& buffer, float rx_gain_offset) = 0;

  /**
   * @brief Measurement process helper method. Encapsulates the neighbour cell measurement functionality
   */
  void measure_proc();

  /**
   * @brief Internal asynchronous low priority thread, waits for measure internal state to execute the measurement
   * process. It stops when the internal state transitions to quit.
   */
  void run_thread() override;

  ///< Internal Thread priority, low by default
  const static int INTRA_FREQ_MEAS_PRIO = DEFAULT_PRIORITY + 5;

  /// Returns a copy of the current used context.
  measure_context_t get_context() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return context;
  }

  internal_state        state;
  srslog::basic_logger& logger;
  mutable std::mutex    mutex;
  uint32_t              last_measure_tti = 0;
  measure_context_t     context;

  std::vector<cf_t>   search_buffer;
  srsran_ringbuffer_t ring_buffer = {};
};

} // namespace scell
} // namespace srsue

#endif // SRSUE_INTRA_MEASURE_BASE_H
