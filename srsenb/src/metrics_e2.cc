/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
