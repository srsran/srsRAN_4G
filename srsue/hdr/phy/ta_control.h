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

#ifndef SRSUE_TA_CONTROL_H
#define SRSUE_TA_CONTROL_H

#include <inttypes.h>
#include <mutex>
#include <srsran/phy/common/phy_common.h>

namespace srsue {

class ta_control
{
private:
  static const size_t MAX_NOF_SPEED_VALUES = 50;    ///< Maximum number of data to store for speed calculation
  static const size_t MIN_NOF_SPEED_VALUES = 1;     ///< Minimum number of data for calculating the speed
  static const size_t MAX_AGE_SPEED_VALUES = 10000; ///< Maximum age of speed data in milliseconds. Discards older data.

  srslog::basic_logger& logger;
  mutable std::mutex    mutex;
  uint32_t              next_base_nta = 0;
  float                 next_base_sec = 0.0f;

  // Vector containing data for calculating speed. The first value is the time increment from TTI and the second value
  // is the distance increment from the TA command
  struct speed_data_t {
    uint32_t tti;
    float    delta_t;
    float    delta_d;
  };
  std::array<speed_data_t, MAX_NOF_SPEED_VALUES> speed_data = {};
  int32_t                                        last_tti   = -1; // Last TTI writen, -1 if none
  uint32_t                                       write_idx  = 0;
  uint32_t                                       read_idx   = 0;

  void reset_speed_data()
  {
    write_idx = 0;
    read_idx  = 0;
    last_tti  = -1;
  }

public:
  ta_control(srslog::basic_logger& logger) : logger(logger) {}

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
    next_base_nta = static_cast<uint32_t>(roundf(next_base_sec / SRSRAN_LTE_TS));

    // Reset speed data
    reset_speed_data();

    logger.info("PHY:   Set TA base: n_ta: %d, ta_usec: %.1f", next_base_nta, next_base_sec * 1e6f);
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
    next_base_nta = static_cast<uint32_t>(roundf(next_base_sec / SRSRAN_LTE_TS));

    logger.info("PHY:   Set TA: ta_delta_usec: %.1f, n_ta: %d, ta_usec: %.1f",
                ta_delta_sec * 1e6f,
                next_base_nta,
                next_base_sec * 1e6f);
  }

  /**
   * Increments (delta) the next base time according to time alignment command from a Random Access Response (RAR).
   *
   * @param ta_cmd Time Alignment command
   */
  void add_ta_cmd_rar(uint32_t tti, uint32_t ta_cmd)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Update base nta
    next_base_nta += srsran_N_ta_new_rar(ta_cmd);

    // Update base in seconds
    next_base_sec = static_cast<float>(next_base_nta) * SRSRAN_LTE_TS;

    // Reset speed data
    reset_speed_data();
    last_tti = tti;

    logger.info("PHY:   Set TA RAR: ta_cmd: %d, n_ta: %d, ta_usec: %.1f", ta_cmd, next_base_nta, next_base_sec * 1e6f);
  }

  /**
   * Increments (delta) the next base time according to time alignment command from a MAC Control Element.
   *
   * @param ta_cmd Time Alignment command
   */
  void add_ta_cmd_new(uint32_t tti, uint32_t ta_cmd)
  {
    std::lock_guard<std::mutex> lock(mutex);
    float                       prev_base_sec = next_base_sec;

    // Update base nta
    next_base_nta = srsran_N_ta_new(next_base_nta, ta_cmd);

    // Update base in seconds
    next_base_sec = static_cast<float>(next_base_nta) * SRSRAN_LTE_TS;

    logger.info("PHY:   Set TA: ta_cmd: %d, n_ta: %d, ta_usec: %.1f", ta_cmd, next_base_nta, next_base_sec * 1e6f);

    // Calculate speed data
    if (last_tti > 0) {
      float delta_t = TTI_SUB(tti, last_tti) * 1e-3f; // Calculate the elapsed time since last time command
      float delta_d = (next_base_sec - prev_base_sec) * 3e8f / 2.0f; // Calculate distance difference in metres

      // Write new data
      speed_data[write_idx].tti     = tti;
      speed_data[write_idx].delta_t = delta_t;
      speed_data[write_idx].delta_d = delta_d;

      // Advance write index
      write_idx = (write_idx + 1) % MAX_NOF_SPEED_VALUES;

      // Advance read index if overlaps with write
      if (write_idx == read_idx) {
        read_idx = (read_idx + 1) % MAX_NOF_SPEED_VALUES;
      }
    }
    last_tti = tti; // Update last TTI
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
    return next_base_sec * (3e8f / 2e3f);
  }

  /**
   * Calculates approximated speed in km/h from the TA commands
   *
   * @return Distance based on the current time base if enough data has been gathered
   */
  float get_speed_kmph(uint32_t tti)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Advance read pointer for old TTI
    while (read_idx != write_idx and TTI_SUB(tti, speed_data[read_idx].tti) > MAX_AGE_SPEED_VALUES) {
      read_idx = (read_idx + 1) % MAX_NOF_SPEED_VALUES;

      // If there us no data, make last_tti invalid to prevent invalid TTI difference
      if (read_idx == write_idx) {
        last_tti = -1;
      }
    }

    // Early return if there is not enough data to calculate speed
    uint32_t nof_values = ((write_idx + MAX_NOF_SPEED_VALUES) - read_idx) % MAX_NOF_SPEED_VALUES;
    if (nof_values < MIN_NOF_SPEED_VALUES) {
      return 0.0f;
    }

    // Compute speed from gathered data
    float sum_t = 0.0f;
    float sum_d = 0.0f;
    for (uint32_t i = read_idx; i != write_idx; i = (i + 1) % MAX_NOF_SPEED_VALUES) {
      sum_t += speed_data[i].delta_t;
      sum_d += speed_data[i].delta_d;
    }
    if (!std::isnormal(sum_t)) {
      return 0.0f; // Avoid zero division
    }
    float speed_mps = sum_d / sum_t; // Speed in m/s

    // Returns the speed in km/h
    return speed_mps * 3.6f;
  }
};

} // namespace srsue

#endif // SRSUE_TA_CONTROL_H
