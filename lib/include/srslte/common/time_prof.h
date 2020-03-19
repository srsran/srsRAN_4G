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

#ifndef SRSLTE_TIME_PROF_H
#define SRSLTE_TIME_PROF_H

#include "srslte/common/logmap.h"
#include <chrono>

#ifdef ENABLE_TIMEPROF
#define TPROF_ENABLE_DEFAULT true
#else
#define TPROF_ENABLE_DEFAULT false
#endif

namespace srslte {

// individual time interval measure
class tprof_measure
{
public:
  using tpoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

  tprof_measure() = default;
  void                     start() { t1 = std::chrono::high_resolution_clock::now(); }
  std::chrono::nanoseconds stop()
  {
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
  }

private:
  tpoint t1;
};

template <typename Prof, bool Enabled = TPROF_ENABLE_DEFAULT>
class tprof
{
public:
  template <typename... Args>
  explicit tprof(Args&&... args) : prof(std::forward<Args>(args)...)
  {
  }

  void start() { meas.start(); }

  std::chrono::nanoseconds stop()
  {
    auto d = meas.stop();
    prof(d);
    return d;
  }

  tprof_measure meas;
  Prof          prof;
};

// specialization for when the tprof is disabled
template <typename Prof>
class tprof<Prof, false>
{
public:
  template <typename... Args>
  explicit tprof(Args&&... args)
  {
  }

  void start() {}

  std::chrono::nanoseconds stop() { return std::chrono::nanoseconds{0}; }
};

template <typename Prof, bool Enabled = TPROF_ENABLE_DEFAULT>
struct mutexed_tprof {
  struct measure {
  public:
    explicit measure(mutexed_tprof<Prof>* h_) : h(h_) { meas.start(); }
    ~measure()
    {
      if (deferred) {
        stop();
      }
    }
    std::chrono::nanoseconds stop()
    {
      auto                        d = meas.stop();
      std::lock_guard<std::mutex> lock(h->mutex);
      h->prof(d);
      return d;
    }
    void defer_stop() { deferred = true; }

    tprof_measure        meas;
    mutexed_tprof<Prof>* h;
    bool                 deferred = false;
  };

  template <typename... Args>
  explicit mutexed_tprof(Args&&... args) : prof(std::forward<Args>(args)...)
  {
  }
  measure start() { return measure{this}; }

  Prof prof;

private:
  std::mutex mutex;
};

template <typename Prof>
struct mutexed_tprof<Prof, false> {
  struct measure {
  public:
    std::chrono::nanoseconds stop() { return std::chrono::nanoseconds{0}; }
    void                     defer_stop() {}
  };

  template <typename... Args>
  explicit mutexed_tprof(Args&&... args)
  {
  }
  measure start() { return measure{}; }
};

struct avg_time_stats {
  avg_time_stats(const char* name_, const char* logname, size_t print_period_);
  void operator()(std::chrono::nanoseconds duration);

  srslte::log_ref log_ptr;
  std::string     name;
  double          avg_val = 1;
  long            count = 0, max_val = 0, min_val = std::numeric_limits<long>::max();
  long            print_period = 0;
};

template <typename TUnit>
class sliding_window_stats
{
public:
  sliding_window_stats(const char* name_, const char* logname, size_t print_period_ = 10);
  void operator()(std::chrono::nanoseconds duration);

  srslte::log_ref                       log_ptr;
  std::string                           name;
  std::vector<std::chrono::nanoseconds> sliding_window;
  size_t                                window_idx = 0;
};
using sliding_window_stats_ms = sliding_window_stats<std::chrono::milliseconds>;

} // namespace srslte

#endif // SRSLTE_TIME_PROF_H
