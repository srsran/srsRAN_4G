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

namespace srslte {

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
  ERROR("allocate_freq: No channels available for frequency=%.1f\n", freq);
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

int channel_mapping::get_carrier_idx(const uint32_t& logical_ch)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (allocated_channels.count(logical_ch)) {
    return allocated_channels[logical_ch].carrier_idx;
  }
  return -1;
}

bool channel_mapping::is_allocated(const uint32_t& logical_ch)
{
  std::lock_guard<std::mutex> lock(mutex);
  return allocated_channels.count(logical_ch) > 0;
}

} // namespace srslte