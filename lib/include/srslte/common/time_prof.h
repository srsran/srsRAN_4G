/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifdef ENABLE_TIMEPROF

#include "srslte/common/logmap.h"
#include <chrono>

namespace srslte {

template <typename Callable>
class mutexed_tprof_measure
{
  using tpoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

public:
  template <typename... Args>
  explicit mutexed_tprof_measure(Args&&... args) : c(std::forward<Args>(args)...)
  {
    srslte::logmap::get("TPROF")->set_level(LOG_LEVEL_INFO);
  }

  uint8_t start()
  {
    auto                        t1 = std::chrono::high_resolution_clock::now();
    std::lock_guard<std::mutex> lock(mutex);
    auto                        ret = start_tpoints.insert(std::make_pair(next_id++, t1));
    return ret.first->first;
  }

  void stop(uint8_t id)
  {
    auto                        t2 = std::chrono::high_resolution_clock::now();
    std::lock_guard<std::mutex> lock(mutex);
    auto&                       t1 = start_tpoints[id];
    c.process(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
    start_tpoints.erase(id);
  }

private:
  std::mutex                          mutex;
  Callable                            c;
  std::unordered_map<uint8_t, tpoint> start_tpoints;
  uint8_t                             next_id = 0;
};

template <typename Callable>
class tprof_measure
{
  using tpoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

public:
  template <typename... Args>
  explicit tprof_measure(Args&&... args) : c(std::forward<Args>(args)...)
  {
    srslte::logmap::get("TPROF")->set_level(LOG_LEVEL_INFO);
  }

  void start() { t1 = std::chrono::high_resolution_clock::now(); }

  void stop()
  {
    auto t2 = std::chrono::high_resolution_clock::now();
    c.process(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
  }

private:
  Callable c;
  tpoint   t1;
};

struct avg_tprof {
  avg_tprof(const char* name_, size_t print_period_) : name(name_), print_period(print_period_) {}

  void process(long duration)
  {
    count++;
    avg_val = avg_val * (count - 1) / count + static_cast<double>(duration) / count;
    max_val = std::max(max_val, duration);
    min_val = std::min(min_val, duration);
    if (count % print_period == 0) {
      log_ptr->info("%s: Mean=%0.1fusec, Max=%ldusec, Min=%ldusec, nof_samples=%ld",
                    name.c_str(),
                    avg_val / 1e3,
                    max_val / 1000,
                    min_val / 1000,
                    count);
    }
  }

  srslte::log_ref log_ptr = srslte::logmap::get("TPROF");
  std::string     name;
  double          avg_val = 1;
  long            count = 0, max_val = 0, min_val = std::numeric_limits<long>::max();
  long            print_period;
};

#else

namespace srslte {

template <typename Callable>
class mutexed_tprof_measure
{
public:
  template <typename... Args>
  explicit mutexed_tprof_measure(Args&&... c)
  {
  }
  uint8_t start() { return 0; }
  void    stop(uint8_t id) {}
};

template <typename Callable>
class tprof_measure
{
public:
  template <typename... Args>
  explicit tprof_measure(Args&&... c)
  {
  }
  void start() {}
  void stop() {}
};

struct avg_tprof {
  avg_tprof(const char*, size_t) {}
};

#endif

} // namespace srslte

#endif // SRSLTE_TIME_PROF_H
