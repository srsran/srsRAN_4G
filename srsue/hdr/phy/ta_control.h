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

#ifndef SRSUE_TA_CONTROL_H
#define SRSUE_TA_CONTROL_H

#include <inttypes.h>
#include <mutex>
#include <srslte/phy/common/phy_common.h>

namespace srsue {

class ta_control
{
private:
  srslte::log*       log_h = nullptr;
  mutable std::mutex mutex;
  uint32_t           next_base_nta    = 0;
  float              next_base_sec    = 0.0f;

public:
  /**
   * Sets the logging instance
   *
   * @param loh_h_ logging instance pointer
   */
  void set_logger(srslte::log* log_h_) { log_h = log_h_; }

  /**
   * Sets the next base time in seconds, discarding previous changes.
   *
   * @param ta_base_sec Time Alignment value in seconds
   */
  void set_base_sec(float ta_base_sec)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Forces next base
    next_base_sec = ta_base_sec;

    // Update base in nta
    next_base_nta = static_cast<uint32_t>(roundf(next_base_sec / SRSLTE_LTE_TS));

    // Log information information if available
    if (log_h) {
      log_h->info("PHY:   Set TA base: n_ta: %d, ta_usec: %.1f\n", next_base_nta, next_base_sec * 1e6f);
    }
  }

  /**
   * Increments (delta) the next base time. The value in seconds will be added to the next base.
   *
   * @param ta_delta_sec Time Alignment increment value in seconds
   */
  void add_delta_sec(float ta_delta_sec)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Increments the next base
    next_base_sec += ta_delta_sec;

    // Update base in nta
    next_base_nta = static_cast<uint32_t>(roundf(next_base_sec / SRSLTE_LTE_TS));

    // Log information information if available
    if (log_h) {
      log_h->info("PHY:   Set TA: ta_delta_usec: %.1f, n_ta: %d, ta_usec: %.1f\n",
                  ta_delta_sec * 1e6f,
                  next_base_nta,
                  next_base_sec * 1e6f);
    }
  }

  /**
   * Increments (delta) the next base time according to time alignment command from a Random Access Response (RAR).
   *
   * @param ta_cmd Time Alignment command
   */
  void add_ta_cmd_rar(uint32_t ta_cmd)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Update base nta
    next_base_nta += srslte_N_ta_new_rar(ta_cmd);

    // Update base in seconds
    next_base_sec = static_cast<float>(next_base_nta) * SRSLTE_LTE_TS;

    // Log information information if available
    if (log_h) {
      log_h->info(
          "PHY:   Set TA RAR: ta_cmd: %d, n_ta: %d, ta_usec: %.1f\n", ta_cmd, next_base_nta, next_base_sec * 1e6f);
    }
  }

  /**
   * Increments (delta) the next base time according to time alignment command from a MAC Control Element.
   *
   * @param ta_cmd Time Alignment command
   */
  void add_ta_cmd_new(uint32_t ta_cmd)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Update base nta
    next_base_nta = srslte_N_ta_new(next_base_nta, ta_cmd);

    // Update base in seconds
    next_base_sec = static_cast<float>(next_base_nta) * SRSLTE_LTE_TS;

    // Log information information if available
    if (log_h) {
      log_h->info("PHY:   Set TA: ta_cmd: %d, n_ta: %d, ta_usec: %.1f\n", ta_cmd, next_base_nta, next_base_sec * 1e6f);
    }
  }

  /**
   * Get the current time alignment in seconds
   *
   * @return Time alignment in seconds
   */
  float get_sec() const
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Returns the current base
    return next_base_sec;
  }

  /**
   * Get the current time alignment in microseconds
   *
   * @return Time alignment in microseconds
   */
  float get_usec() const
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Returns the current base
    return next_base_sec * 1e6f;
  }

  /**
   * Get the current time alignment in kilometers between the eNb and the UE
   *
   * @return Distance based on the current time base
   */
  float get_km() const
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Returns the current base, one direction distance
    return next_base_sec * (3.6f * 3e8f / 2.0f);
  }
};

} // namespace srsue

#endif // SRSUE_TA_CONTROL_H
