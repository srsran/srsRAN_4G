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

struct tprof_handler {
  explicit tprof_handler(const char* name_) : name(name_) { log_ptr->set_level(LOG_LEVEL_INFO); }
  virtual void process(long sample) = 0;

  std::mutex      mutex;
  srslte::log_ref log_ptr = srslte::logmap::get("TPROF");
  std::string     name;
};

struct avg_tprof : public tprof_handler {
  avg_tprof(const char* name_, size_t print_period_) : tprof_handler(name_), print_period(print_period_) {}

  void process(long duration) final
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

  double avg_val = 1;
  long   count = 0, max_val = 0, min_val = std::numeric_limits<long>::max();
  long   print_period;
};

struct tprof_measure {
  using tpoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

  explicit tprof_measure(tprof_handler* h_) : h(h_) {}

  void start() { t1 = std::chrono::high_resolution_clock::now(); }

  void stop()
  {
    auto                        t2 = std::chrono::high_resolution_clock::now();
    std::lock_guard<std::mutex> lock(h->mutex);
    h->process(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
  }

private:
  tprof_handler* h;
  tpoint         t1;
};

struct tprof_measure_guard {
  tprof_measure_guard(tprof_handler* h_) : tmeas(h_) { tmeas.start(); }
  ~tprof_measure_guard() { tmeas.stop(); }
  tprof_measure_guard(const tprof_measure_guard&) = delete;
  tprof_measure_guard& operator=(const tprof_measure_guard&) = delete;

private:
  tprof_measure tmeas;
};

#else

namespace srslte {

struct tprof_handler {
};

struct avg_tprof : public tprof_handler {
  avg_tprof(const char*, size_t) {}
};

class tprof_measure
{
public:
  explicit tprof_measure(tprof_handler* h_) {}
  void start() {}
  void stop() {}
};

#endif

} // namespace srslte

#endif // SRSLTE_TIME_PROF_H
