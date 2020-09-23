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
#ifndef SRSUE_INTRA_MEASURE_H
#define SRSUE_INTRA_MEASURE_H

#include <srslte/common/log.h>
#include <srslte/common/threads.h>
#include <srslte/common/tti_sync_cv.h>
#include <srslte/srslte.h>

#include "scell_recv.h"

namespace srsue {
namespace scell {

// Class to perform intra-frequency measurements
class intra_measure : public srslte::thread
{
  /*
   * The intra-cell measurment has 5 different states:
   *  - idle: it has been initiated and it is waiting to get configured to start capturing samples. From any state
   *          except quit can transition to idle.
   *  - wait: waits for at least intra_freq_meas_period_ms since last receive start and goes to receive.
   *  - receive: captures base-band samples for intra_freq_meas_len_ms and goes to measure.
   *  - measure: enables the inner thread to start the measuring function and goes to wait.
   *  - quit: stops the inner thread and quits. Transition from any state measure state.
   *
   * FSM abstraction:
   *
   *  +------+   set_cells_to_meas   +------+   intra_freq_meas_period_ms   +---------+
   *  | Idle | --------------------->| Wait |------------------------------>| Receive |
   *  +------+                       +------+                               +---------+
   *     ^                              ^                                        |          stop  +------+
   *     |                              |                                        |          ----->| Quit |
   *   init                        +---------+    intra_freq_meas_len_ms         |                +------+
   * meas_stop                     | Measure |<----------------------------------+
   *                               +---------+
   */
public:
  // Interface for reporting new cell measurements
  class meas_itf
  {
  public:
    virtual void cell_meas_reset(uint32_t cc_idx)                                                           = 0;
    virtual void new_cell_meas(uint32_t cc_idx, const std::vector<rrc_interface_phy_lte::phy_meas_t>& meas) = 0;
  };

  /**
   * Constructor
   */
  intra_measure();

  /**
   * Destructor
   */
  ~intra_measure();

  /**
   * Initiation function, necessary to configure main parameters
   * @param common SRSUE phy_common instance pointer for providing intra_freq_meas_len_ms and intra_freq_meas_period_ms
   * @param rrc SRSUE PHY->RRC interface for supplying the RRC with the measurements
   * @param log_h Physical layer Logging filter pointer
   */
  void init(uint32_t cc_idx, phy_common* common, meas_itf* new_cell_itf, srslte::log* log_h);

  /**
   * Stops the operation of this component
   */
  void stop();

  /**
   * Sets the primmary cell, configures the cell bandwidth and sampling rate
   * @param earfcn Frequency the component is receiving base-band from. Used only for reporting the EARFCN to the RRC
   * @param cell Actual cell configuration
   */
  void set_primary_cell(uint32_t earfcn, srslte_cell_t cell);

  /**
   * Sets receiver gain offset to convert estimated dBFs to dBm in RSRP
   * @param rx_gain_offset Gain offset in dB
   */
  void set_rx_gain_offset(float rx_gain_offset_db);

  /**
   * Sets the PCI list of the cells this components needs to measure and starts the FSM for measuring
   * @param pci is the list of PCIs to measure
   */
  void set_cells_to_meas(const std::set<uint32_t>& pci);

  /**
   * Stops the measurment FSM, setting the inner state to idle.
   */
  void meas_stop();

  /**
   * Inputs the baseband IQ samples into the component, internal state dictates whether it will be written or not.
   * @param tti The current physical layer TTI, used for calculating the buffer write
   * @param data buffer with baseband IQ samples
   * @param nsamples number of samples to write
   */
  void write(uint32_t tti, cf_t* data, uint32_t nsamples);

  /**
   * Get EARFCN of this component
   * @return EARFCN
   */
  uint32_t get_earfcn() { return current_earfcn; };

  /**
   * Synchronous wait mechanism, used for testing purposes, it waits for the inner thread to return a measurement.
   */
  void wait_meas()
  { // Only used by scell_search_test
    meas_sync.wait();
  }

private:
  class internal_state ///< Internal state class, provides thread safe state management
  {
  public:
    typedef enum {
      idle = 0, ///< Initial state, internal thread runs, it does not capture data
      wait,     ///< Wait for the period time to pass
      receive,  ///< Accumulate samples in ring buffer
      measure,  ///< Module is busy measuring
      quit      ///< Quit thread, no transitions are allowed
    } state_t;

  private:
    state_t                 state = idle;
    std::mutex              mutex;
    std::condition_variable cvar;

  public:
    /**
     * Get the internal state
     * @return protected state
     */
    state_t get_state() { return state; }

    /**
     * Transitions to a different state, all transitions are allowed except from quit
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
     * Waits for a state transition change, used for blocking the inner thread
     */
    void wait_change()
    {
      std::unique_lock<std::mutex> lock(mutex);
      cvar.wait(lock);
    }
  };

  internal_state state;

  /**
   * Measurement process helper method. Encapusulates the neighbour cell measurement functionality
   */
  void measure_proc();

  /**
   * Internal asynchronous low priority thread, waits for measure internal state to execute the measurement process. It
   * stops when the internal state transitions to quit.
   */
  void run_thread() override;

  ///< Internal Thread priority, low by default
  const static int INTRA_FREQ_MEAS_PRIO = DEFAULT_PRIORITY + 5;

  scell_recv             scell                     = {};
  meas_itf*              new_cell_itf              = nullptr;
  srslte::log*           log_h                     = nullptr;
  uint32_t               cc_idx                    = 0;
  uint32_t               current_earfcn            = 0;
  uint32_t               current_sflen             = 0;
  srslte_cell_t          serving_cell              = {};
  std::set<uint32_t>     active_pci                = {};
  std::mutex             active_pci_mutex          = {};
  uint32_t               last_measure_tti          = 0;
  uint32_t               intra_freq_meas_len_ms    = 20;
  uint32_t               intra_freq_meas_period_ms = 200;
  uint32_t               rx_gain_offset_db         = 0;
  srslte::tti_sync_cv    meas_sync; // Only used by scell_search_test

  cf_t* search_buffer = nullptr;

  uint32_t            receive_cnt = 0;
  srslte_ringbuffer_t ring_buffer = {};

  srslte_refsignal_dl_sync_t refsignal_dl_sync = {};
};

} // namespace scell
} // namespace srsue

#endif // SRSUE_INTRA_MEASURE_H
