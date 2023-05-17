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

#include "srsenb/hdr/metrics_e2.h"
#include "srsran/phy/utils/vector.h"

using namespace srsenb;

void metrics_e2::set_metrics(const enb_metrics_t& m, const uint32_t period_usec)
{
  if (metrics_queue.size() > METRICS_BUFFER_SIZE) {
    metrics_queue.pop();
    metrics_queue.push(m);
  } else {
    metrics_queue.push(m);
  }

  // send new enb metrics to all registered SMs
  for (auto sm_ : e2sm_vec) {
    sm_->receive_e2_metrics_callback(m);
  }
}

bool metrics_e2::register_e2sm(e2sm* sm)
{
  e2sm_vec.push_back(sm);
  return true;
}

bool metrics_e2::unregister_e2sm(e2sm* sm)
{
  auto it = std::find(e2sm_vec.begin(), e2sm_vec.end(), sm);
  if (it != e2sm_vec.end()) {
    e2sm_vec.erase(it);
    return true;
  }
  return false;
}

bool metrics_e2::pull_metrics(enb_metrics_t* m)
{
  if (enb != nullptr) {
    if (!metrics_queue.empty()) {
      *m = metrics_queue.front();
      metrics_queue.pop();
      return true;
    }
  }
  return false;
}
