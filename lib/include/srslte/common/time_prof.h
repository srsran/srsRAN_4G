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

template <typename Prof>
class tprof
{
public:
  using tpoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

  struct measure {
  public:
    measure(tprof<Prof>* h_) : t1(std::chrono::high_resolution_clock::now()), h(h_) {}
    ~measure()
    {
      if (deferred) {
        stop();
      }
    }
    void stop()
    {
      auto t2 = std::chrono::high_resolution_clock::now();
      h->process(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
    }
    void defer_stop() { deferred = true; }

    tpoint       t1;
    tprof<Prof>* h;
    bool         deferred = false;
  };

  template <typename... Args>
  explicit tprof(Args&&... args) : prof(std::forward<Args>(args)...)
  {
    srslte::logmap::get("TPROF")->set_level(LOG_LEVEL_INFO);
  }

  measure start() { return measure{this}; }

  virtual void process(long duration) { prof.process(duration); }

  Prof prof;
};

template <typename Prof>
struct mutexed_tprof : public tprof<Prof> {
  using tprof<Prof>::tprof;
  void process(long duration) final
  {
    std::lock_guard<std::mutex> lock(mutex);
    tprof<Prof>::prof.process(duration);
  }
  std::mutex mutex;
};

#else

namespace srslte {

template <typename Prof>
struct tprof {
  struct measure {
  public:
    void stop() {}
    void defer_stop() {}
  };

  template <typename... Args>
  explicit tprof(Args&&... args)
  {
  }

  measure start() { return measure{}; }

private:
  void process(long duration) {}
};
template <typename Prof>
using mutexed_tprof = tprof<Prof>;

#endif

struct avg_time_stats {
  avg_time_stats(const char* name_, size_t print_period_) : name(name_), print_period(print_period_) {}

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

} // namespace srslte

#endif // SRSLTE_TIME_PROF_H
