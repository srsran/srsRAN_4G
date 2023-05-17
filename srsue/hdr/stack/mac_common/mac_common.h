/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSUE_MAC_COMMON_H
#define SRSUE_MAC_COMMON_H

#include "srsran/common/string_helpers.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/srslog/srslog.h"
#include <map>
#include <mutex>

/**
 * @brief Common definitions/interfaces between LTE/NR MAC components
 *
 * @remark: So far only the trigger types are identical. The BSR report type and LCID mapping is implemented in RAT
 * specialications.
 */
namespace srsue {

// Helper class to protect access to RNTIs
class ue_rnti
{
public:
  void reset()
  {
    std::lock_guard<std::mutex> lock(mutex);
    crnti         = 0;
    rar_rnti      = 0;
    temp_rnti     = 0;
    tpc_rnti      = 0;
    sps_rnti      = 0;
    contention_id = 0;
  }
  uint16_t get_crnti()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return crnti;
  }
  uint16_t get_rar_rnti()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return rar_rnti;
  }
  uint16_t get_temp_rnti()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return temp_rnti;
  }
  uint16_t get_sps_rnti()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return sps_rnti;
  }
  uint64_t get_contention_id()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return contention_id;
  }
  void set_crnti(uint16_t rnti)
  {
    std::lock_guard<std::mutex> lock(mutex);
    crnti = rnti;
  }
  void set_rar_rnti(uint16_t rnti)
  {
    std::lock_guard<std::mutex> lock(mutex);
    rar_rnti = rnti;
  }
  void set_temp_rnti(uint16_t rnti)
  {
    std::lock_guard<std::mutex> lock(mutex);
    temp_rnti = rnti;
  }
  void set_contention_id(uint64_t id)
  {
    std::lock_guard<std::mutex> lock(mutex);
    contention_id = id;
  }
  void set_crnti_to_temp()
  {
    std::lock_guard<std::mutex> lock(mutex);
    crnti = temp_rnti;
  }
  void clear_temp_rnti()
  {
    std::lock_guard<std::mutex> lock(mutex);
    temp_rnti = SRSRAN_INVALID_RNTI;
  }
  void clear_rar_rnti()
  {
    std::lock_guard<std::mutex> lock(mutex);
    rar_rnti = SRSRAN_INVALID_RNTI;
  }
  void clear_crnti()
  {
    std::lock_guard<std::mutex> lock(mutex);
    crnti = SRSRAN_INVALID_RNTI;
  }

private:
  std::mutex mutex;
  uint16_t   crnti         = 0;
  uint16_t   rar_rnti      = 0;
  uint16_t   temp_rnti     = 0;
  uint16_t   tpc_rnti      = 0;
  uint16_t   sps_rnti      = 0;
  uint64_t   contention_id = 0;
};

// Helper class to control RNTI search windows in a protected manner
class rnti_window_safe
{
public:
  void reset()
  {
    std::lock_guard<std::mutex> lock(mutex);
    length = -1;
    start  = -1;
  }
  void set(int length_, int start_)
  {
    std::lock_guard<std::mutex> lock(mutex);
    length = length_;
    start  = start_;
  }
  int get_length()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return length;
  }
  int get_start()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return start;
  }
  bool is_set()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return start > 0;
  }
  bool is_in_window(int tti)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (start == 0 || length == 0) {
      return false;
    }
    if ((int)srsran_tti_interval(tti, start) < length + 5) {
      if (tti > start) {
        if (tti <= start + length) {
          return true;
        } else {
          start  = 0;
          length = 0;
          return false;
        }
      } else {
        if (tti <= (start + length) % 10240) {
          return true;
        } else {
          start  = 0;
          length = 0;
          return false;
        }
      }
    }
    return false;
  }

private:
  int        length = -1;
  int        start  = -1;
  std::mutex mutex;
};

// BSR trigger are common between LTE and NR
typedef enum { NONE, REGULAR, PADDING, PERIODIC } bsr_trigger_type_t;
char* bsr_trigger_type_tostring(bsr_trigger_type_t type);

/// Helper class to store a snapshot of buffer states for all LCGs/LCIDs
class mac_buffer_states_t
{
public:
  explicit mac_buffer_states_t() {}
  void reset()
  {
    nof_lcids_with_data = 0;
    nof_lcgs_with_data  = 0;
    last_non_zero_lcg   = -1;
    lcid_buffer_size.clear();
    lcg_buffer_size.clear();
  }
  std::string to_string()
  {
    fmt::memory_buffer buffer;

    fmt::format_to(buffer,
                   "nof_lcids_with_data={}, nof_lcgs_with_data={}, last_non_zero_lcg={} ",
                   nof_lcids_with_data,
                   nof_lcgs_with_data,
                   last_non_zero_lcg);

    fmt::format_to(buffer, "[");
    for (const auto& lcg : lcg_buffer_size) {
      fmt::format_to(buffer, "lcg{}={}, ", lcg.first, lcg.second);
    }
    fmt::format_to(buffer, "] ");

    fmt::format_to(buffer, "[");
    for (const auto& lcid : lcid_buffer_size) {
      fmt::format_to(buffer, "lcid{}={}, ", lcid.first, lcid.second);
    }
    fmt::format_to(buffer, "]");

    return srsran::to_c_str(buffer);
  }

  uint32_t get_total_buffer_size()
  {
    uint32_t buffer_size = 0;
    for (const auto& lcg : lcid_buffer_size) {
      buffer_size += lcg.second;
    }
    return buffer_size;
  }

  std::map<uint32_t, uint32_t> lcid_buffer_size;         // Buffer size entry for each LCID
  std::map<uint32_t, uint32_t> lcg_buffer_size;          // Entry for each LCG (sum of LCIDs of that LCG)
  uint32_t                     nof_lcids_with_data = 0;  // Is incremented when a LCID is found with data to transmit
  uint32_t                     nof_lcgs_with_data  = 0;  // Is incremented when a LCG is found with data to transmit
  int32_t                      last_non_zero_lcg   = -1; // only valid if nof_lcgs_with_data is at least one
};

} // namespace srsue

#endif // SRSUE_MAC_COMMON_H
