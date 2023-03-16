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
