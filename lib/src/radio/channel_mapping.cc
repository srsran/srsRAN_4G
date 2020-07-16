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

#include "srslte/radio/channel_mapping.h"
#include "srslte/phy/utils/debug.h"
#include <sstream>

namespace srslte {

void channel_mapping::set_config(const uint32_t& nof_channels_x_dev_, const uint32_t& nof_antennas_)
{
  nof_channels_x_dev = nof_channels_x_dev_;
  nof_antennas       = nof_antennas_;
}

bool channel_mapping::allocate_freq(const uint32_t& logical_ch, const float& freq)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (allocated_channels.count(logical_ch)) {
    ERROR("allocate_freq: Carrier logical_ch=%d already allocated to channel=%d\n",
          logical_ch,
          allocated_channels[logical_ch].carrier_idx);
    return false;
  }

  // Find first available channel that supports this frequency and allocated it
  for (auto c = available_channels.begin(); c != available_channels.end(); ++c) {
    if (c->band.contains(freq)) {
      allocated_channels[logical_ch] = *c;
      available_channels.erase(c);
      return true;
    }
  }
  ERROR("allocate_freq: No channels available for frequency=%.1f %s\n", freq, to_string().c_str());
  return false;
}

bool channel_mapping::release_freq(const uint32_t& logical_ch)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (allocated_channels.count(logical_ch)) {
    available_channels.push_back(allocated_channels[logical_ch]);
    allocated_channels.erase(logical_ch);
    return true;
  }
  return false;
}

channel_mapping::device_mapping_t channel_mapping::get_device_mapping(const uint32_t& logical_ch,
                                                                      const uint32_t& antenna_idx) const
{
  std::lock_guard<std::mutex> lock(mutex);
  if (allocated_channels.count(logical_ch) > 0) {
    uint32_t carrier_idx = allocated_channels.at(logical_ch).carrier_idx;
    uint32_t channel_idx = carrier_idx * nof_antennas + antenna_idx;
    return {carrier_idx, channel_idx / nof_channels_x_dev, channel_idx % nof_channels_x_dev};
  }
  return {UINT32_MAX, UINT32_MAX, UINT32_MAX};
}

bool channel_mapping::is_allocated(const uint32_t& logical_ch) const
{
  std::lock_guard<std::mutex> lock(mutex);
  return allocated_channels.count(logical_ch) > 0;
}

std::string channel_mapping::to_string() const
{
  std::stringstream ss;
  ss << "[";
  for (const auto& c : allocated_channels) {
    uint32_t carrier_idx = allocated_channels.at(c.first).carrier_idx;
    uint32_t channel_idx = carrier_idx * nof_antennas;
    ss << "{carrier: " << c.first << ", device: " << channel_idx / nof_channels_x_dev
       << ", channel: " << channel_idx % nof_channels_x_dev
       << ", center_freq: " << (c.second.band.get_low() + c.second.band.get_high()) / 2e6 << " MHz },";
  }
  ss << "]";

  return ss.str();
}

} // namespace srslte