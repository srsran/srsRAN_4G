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

/******************************************************************************
 * File:        metrics_hub.h
 * Description: Centralizes metrics interfaces to allow different metrics clients
 *              to get metrics
 *****************************************************************************/

#ifndef SRSLTE_METRICS_HUB_H
#define SRSLTE_METRICS_HUB_H

#include "srslte/common/threads.h"
#include "srslte/srslte.h"
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

namespace srslte {

template <typename metrics_t>
class metrics_interface
{
public:
  virtual bool get_metrics(metrics_t* m) = 0;
};

template <typename metrics_t>
class metrics_listener
{
public:
  virtual void set_metrics(const metrics_t& m, const uint32_t period_usec) = 0;
  virtual void stop()                                                      = 0;
};

template <typename metrics_t>
class metrics_hub : public periodic_thread
{
public:
  metrics_hub() : sleep_start(std::chrono::steady_clock::now()), periodic_thread("METRICS_HUB") {}
  bool init(metrics_interface<metrics_t>* m_, float report_period_secs_ = 1.0)
  {
    m = m_;
    // Start with user-default priority
    start_periodic(report_period_secs_ * 1e6, -2);
    return true;
  }
  void stop()
  {
    stop_thread();
    // stop all listeners
    for (uint32_t i = 0; i < listeners.size(); i++) {
      listeners[i]->stop();
    }
    thread_cancel();
    wait_thread_finish();
  }

  void add_listener(metrics_listener<metrics_t>* listener)
  {
    std::unique_lock<std::mutex> lock(mutex);
    listeners.push_back(listener);
  }

private:
  void run_period()
  {
    std::unique_lock<std::mutex> lock(mutex);

    // get current time and check how long we slept
    auto period_usec =
        std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - sleep_start);

    if (m) {
      metrics_t metric;
      m->get_metrics(&metric);
      for (uint32_t i = 0; i < listeners.size(); i++) {
        listeners[i]->set_metrics(metric, period_usec.count());
      }
    }
    // store start of sleep period
    sleep_start = std::chrono::steady_clock::now();
  }
  metrics_interface<metrics_t>*             m = nullptr;
  std::vector<metrics_listener<metrics_t>*> listeners;
  std::chrono::steady_clock::time_point     sleep_start;
  std::mutex                                mutex;
};

} // namespace srslte

#endif // SRSLTE_METRICS_HUB_H
