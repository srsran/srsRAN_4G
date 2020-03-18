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

namespace srslte {

#ifdef ENABLE_TIMEPROF

template <typename Prof>
class tprof
{
public:
  using tpoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

  template <typename... Args>
  explicit tprof(Args&&... args) : prof(std::forward<Args>(args)...)
  {
    srslte::logmap::get("TPROF")->set_level(LOG_LEVEL_INFO);
  }

  void start() { t1 = std::chrono::high_resolution_clock::now(); }

  std::chrono::nanoseconds stop()
  {
    auto t2 = std::chrono::high_resolution_clock::now();
    auto d  = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    prof.process(d);
    return d;
  }

  tpoint t1;
  Prof   prof;
};

template <typename Prof>
struct mutexed_tprof {
  using tpoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
  struct measure {
  public:
    measure(mutexed_tprof<Prof>* h_) : t1(std::chrono::high_resolution_clock::now()), h(h_) {}
    ~measure()
    {
      if (deferred) {
        stop();
      }
    }
    std::chrono::nanoseconds stop()
    {
      auto                        t2 = std::chrono::high_resolution_clock::now();
      std::lock_guard<std::mutex> lock(h->mutex);
      std::chrono::nanoseconds    d = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
      h->prof.process(d);
      return d;
    }
    void defer_stop() { deferred = true; }

    tpoint               t1;
    mutexed_tprof<Prof>* h;
    bool                 deferred = false;
  };

  template <typename... Args>
  explicit mutexed_tprof(Args&&... args) : prof(std::forward<Args>(args)...)
  {
    srslte::logmap::get("TPROF")->set_level(LOG_LEVEL_INFO);
  }
  measure start() { return measure{this}; }

  Prof prof;

private:
  std::mutex mutex;
};

#else

template <typename Prof>
struct tprof {
  template <typename... Args>
  explicit tprof(Args&&... args)
  {
  }

  void                     start() {}
  std::chrono::nanoseconds stop() { return {}; }
};

template <typename Prof>
struct mutexed_tprof {
  struct measure {
  public:
    std::chrono::nanoseconds stop() { return {}; }
    void                     defer_stop() {}
  };

  template <typename... Args>
  explicit mutexed_tprof(Args&&... args)
  {
  }

  measure start() { return measure{}; }

private:
  void process(long duration) {}
};

#endif

struct avg_time_stats {
  avg_time_stats(const char* name_, size_t print_period_) : name(name_), print_period(print_period_) {}
  void process(std::chrono::nanoseconds duration);

  srslte::log_ref log_ptr = srslte::logmap::get("TPROF");
  std::string     name;
  double          avg_val = 1;
  long            count = 0, max_val = 0, min_val = std::numeric_limits<long>::max();
  long            print_period = 0;
};

template <typename TUnit>
class sliding_window_stats
{
public:
  sliding_window_stats(const char* name_, size_t print_period_ = 10, TUnit warn_thres_ = 0);
  void process(std::chrono::nanoseconds duration);

  srslte::log_ref                       log_ptr = srslte::logmap::get("TPROF");
  std::string                           name;
  std::vector<std::chrono::nanoseconds> sliding_window;
  size_t                                window_idx = 0;
  TUnit                                 warn_thres = 0;
};
using sliding_window_stats_ms = sliding_window_stats<std::chrono::milliseconds>;

} // namespace srslte

#endif // SRSLTE_TIME_PROF_H
