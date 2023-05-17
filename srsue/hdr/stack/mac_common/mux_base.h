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

#ifndef SRSUE_MUX_BASE_H
#define SRSUE_MUX_BASE_H

#include "srsran/interfaces/mac_interface_types.h"

namespace srsue {

/**
 * @brief Common base class for UE MUX unit for 4G and 5G RAT
 *
 */
class mux_base
{
public:
  int setup_lcid(const srsran::logical_channel_config_t& config)
  {
    if (has_logical_channel(config.lcid)) {
      // update settings
      for (auto& channel : logical_channels) {
        if (channel.lcid == config.lcid) {
          channel = config;
          break;
        }
      }
      // warn user if there is another LCID with same prio
      for (auto& channel : logical_channels) {
        if (channel.priority == config.priority && channel.lcid != config.lcid) {
          srslog::fetch_basic_logger("MAC").error("LCID %d and %d have same priority.", channel.lcid, config.lcid);
          return SRSRAN_ERROR;
        }
      }
    } else {
      // add new entry
      logical_channels.push_back(config);
    }

    // sort according to priority (increasing is lower priority)
    std::sort(logical_channels.begin(), logical_channels.end(), priority_compare);

    return SRSRAN_SUCCESS;
  }

  void print_logical_channel_state(const std::string& info)
  {
    std::string logline = info;

    for (auto& channel : logical_channels) {
      logline += "\n";
      logline += "- lcid=";
      logline += std::to_string(channel.lcid);
      logline += ", lcg=";
      logline += std::to_string(channel.lcg);
      logline += ", prio=";
      logline += std::to_string(channel.priority);
      logline += ", Bj=";
      logline += std::to_string(channel.Bj);
      logline += ", PBR=";
      logline += std::to_string(channel.PBR);
      logline += ", BSD=";
      logline += std::to_string(channel.BSD);
      logline += ", buffer_len=";
      logline += std::to_string(channel.buffer_len);
      logline += ", sched_len=";
      logline += std::to_string(channel.sched_len);
    }
    srslog::fetch_basic_logger("MAC").debug("%s", logline.c_str());
  }

protected:
  static bool priority_compare(const srsran::logical_channel_config_t& u1, const srsran::logical_channel_config_t& u2)
  {
    return u1.priority <= u2.priority;
  }

  bool has_logical_channel(const uint32_t& lcid)
  {
    for (auto& channel : logical_channels) {
      if (channel.lcid == lcid) {
        return true;
      }
    }
    return false;
  }

  std::vector<srsran::logical_channel_config_t> logical_channels;
};

} // namespace srsue

#endif // SRSUE_MUX_BASE_H