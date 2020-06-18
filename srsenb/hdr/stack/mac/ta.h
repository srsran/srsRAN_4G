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

#ifndef SRSENB_TA_H
#define SRSENB_TA_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"

#include <cmath>
#include <vector>

namespace srsenb {

/**
 * Time Aligment FSM parent/callback interface for pushing n TA values
 */
class mac_ta_ue_interface
{
public:
  virtual uint32_t set_ta(int ta) = 0;
};

/**
 * UE's FSM for controlling Time Aligment command generation.
 *
 * Initially the FSM starts at idle state which transitions to Measure as soon as start is called. Measurements are
 * collected while the FSM is in Measure state. Up to MAX_NOF_MEAS are stored. The FSM uses a minimum of MIN_NOF_MEAS
 * measurements to compute the TA average. The TA command is triggered as soon as the TA average is higher than
 * TA_N_THRESHOLD. After triggering a TA command, holds in prohibit state for PROHIBIT_PERIOD_MS before start collecting
 * measurements.
 *
 *  +------+  Start  +---------+   Trigger   +----------+
 *  | Idle | ------->| Measure |------------>| Prohibit |
 *  +------+         +---------+             +----------+
 *     ^                  ^                        |
 *     |                  |    Prohibit expires    |
 *   --+                  +------------------------+
 *
 *
 */
class ta
{
private:
  /// Control constants
  static constexpr uint32_t MAX_NOF_MEAS       = 16;  ///< Maximum number of measurements to store
  static constexpr uint32_t MIN_NOF_MEAS       = 4;   ///< Minimum number of measurements to compute
  static constexpr uint32_t MAX_MEAS_TIME_MS   = 100; ///< Maximum time of measurement
  static constexpr uint32_t PROHIBIT_PERIOD_MS = 20;  ///< Time to wait from the n TA value push to measure again
  static constexpr int32_t  TA_N_THRESHOLD     = 1;   ///< n TA value threshold

  /// Parent/callback object
  mac_ta_ue_interface* parent = nullptr;

  /// TA measure datatype
  typedef struct {
    uint32_t ts_ms; ///< Time in which the measurement was taken in ms
    float    ta_us; ///< TA measurement in microseconds
  } ta_meas_t;

  uint32_t               meas_t_ms  = 0; ///< Time counter in milliseconds
  uint32_t               meas_count = 0; ///< Number of measures in the buffer
  uint32_t               meas_idx   = 0; ///< Next mesurement index in the buffer
  std::vector<ta_meas_t> meas_values;

  // FSM states
  typedef enum {
    state_idle = 0, ///< Waits for start order
    state_measure,  ///< Performing measurement
    state_prohibit  ///< Waiting for HARQ to transmit CE command, NO measurement shall be stored
  } state_t;
  state_t state = state_idle;

  /**
   * Reset Measurement and timer counter
   */
  void reset_measurements()
  {
    meas_t_ms  = 0;
    meas_count = 0;
    meas_idx   = 0;
  }

  /**
   * Averages/extrapolates n TA value
   *
   * @return the required n TA value for the current time (meas_t_ms)
   */
  int get_ta_n()
  {
    float ta_us = 0.0f;

    // Average all measurements
    for (uint32_t i = 0; i < meas_count; i++) {
      // Write here a much fancier extrapolation algorithm
      ta_us += meas_values[i].ta_us;
    }
    if (meas_count) {
      ta_us /= static_cast<float>(meas_count);
    }

    // Return the n_ta value
    return static_cast<int>(std::roundf(ta_us * 1e-6f / SRSLTE_LTE_TS / 16.0f));
  }

  /**
   * Runs measure state
   * @return the number of enqueued MAC CE carrying TA commands
   */
  uint32_t run_state_measure()
  {
    uint32_t ret = 0;

    // Avoid processing if no measurement or no minimum covered or maximum measuring time is reached
    if (meas_count == 0 or (meas_count < MIN_NOF_MEAS and meas_t_ms < MAX_MEAS_TIME_MS)) {
      return ret;
    }

    // Get TA command value
    int ta_n = get_ta_n();

    // Send command
    if (abs(ta_n) > TA_N_THRESHOLD) {
      // Set UE TA
      ret = parent->set_ta(ta_n);

      // Reset measurement counter
      reset_measurements();

      // Transition to prohibit state
      state = state_prohibit;
    }

    return ret;
  }

  /**
   * Runs prohibit state
   * @return 0
   */
  uint32_t run_state_prohibit()
  {
    // Prohibit time expired
    if (meas_t_ms >= PROHIBIT_PERIOD_MS) {
      // Reset counters
      reset_measurements();

      // Go to measure
      state = state_measure;
    }

    return 0;
  }

  /**
   * Runs The internal FSM
   * @return the number of eneuqued MAC CE carrying TA commands
   */
  uint32_t run_fsm()
  {
    switch (state) {
      case state_idle:
        // Waits for Start order, do nothing
        return 0;
      case state_measure:
        return run_state_measure();
      case state_prohibit:
        return run_state_prohibit();
      default:; // Do nothing
    }

    return 0;
  }

public:
  /**
   * TA FSM Constructor
   * @param parent_ UE MAC object with TA callback setter
   */
  explicit ta(mac_ta_ue_interface* parent_) : parent(parent_), meas_values(MAX_NOF_MEAS)
  {
    /// Initial FSM run
    run_fsm();
  }

  /**
   * Gives an start order to the FSM
   */
  void start()
  {
    // Transition to idle only if the current state is idle
    if (state == state_idle) {
      state = state_measure;
    }
  }

  /**
   * Pushes TA measurement and runs internal FSM
   *
   * @param ta_us actual TA measurement in microseconds
   * @return the number of MAC CE carrying TA
   */
  uint32_t push_value(float ta_us)
  {
    // Put measurement if state is measurement
    if (state == state_measure) {
      // Set measurement
      meas_values[meas_idx].ts_ms = meas_t_ms;
      meas_values[meas_idx].ta_us = ta_us;

      // Increase pointer
      meas_idx = (meas_idx + 1) % static_cast<uint32_t>(meas_values.size());

      // Increase count
      if (meas_count < static_cast<uint32_t>(meas_values.size())) {
        meas_count++;
      }
    }

    // Run state machine
    return run_fsm();
  }

  /**
   * Increments internal timer 1 ms and runs internal FSM
   *
   * @param ta_us actual TA measurement in microseconds
   * @return the number of MAC CE carrying TA
   */
  uint32_t tick()
  {
    // Increase measurement timestamp counter
    meas_t_ms++;

    // Run state machine
    return run_fsm();
  }
};

} // namespace srsenb

#endif // SRSENB_TA_H
