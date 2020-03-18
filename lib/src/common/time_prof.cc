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

#include "srslte/common/time_prof.h"
#include <algorithm>
#include <numeric>

using namespace srslte;
using std::chrono::nanoseconds;

template <typename TUnit>
const char* get_tunit_str()
{
  return "";
}
template <>
const char* get_tunit_str<nanoseconds>()
{
  return "nsec";
}
template <>
const char* get_tunit_str<std::chrono::microseconds>()
{
  return "usec";
}
template <>
const char* get_tunit_str<std::chrono::milliseconds>()
{
  return "msec";
}
template <>
const char* get_tunit_str<std::chrono::seconds>()
{
  return "sec";
}
template <>
const char* get_tunit_str<std::chrono::minutes>()
{
  return "min";
}

void avg_time_stats::process(nanoseconds duration)
{
  count++;
  avg_val = avg_val * (count - 1) / count + static_cast<double>(duration.count()) / count;
  max_val = std::max(max_val, duration.count());
  min_val = std::min(min_val, duration.count());
  if (count % print_period == 0) {
    log_ptr->info("%s: mean,max,min=%0.1f,%ld,%ld usec, nof_samples=%ld",
                  name.c_str(),
                  avg_val / 1e3,
                  max_val / 1000,
                  min_val / 1000,
                  count);
  }
}

template <typename TUnit>
sliding_window_stats<TUnit>::sliding_window_stats(const char* name_, size_t print_period_, TUnit warn_thres_) :
  name(name_),
  sliding_window(print_period_),
  warn_thres(warn_thres_)
{
}

template <typename TUnit>
void sliding_window_stats<TUnit>::process(nanoseconds duration)
{
  using std::chrono::duration_cast;
  const char* unit_str = get_tunit_str<TUnit>();
  TUnit       dur      = std::chrono::duration_cast<TUnit>(duration);

  log_ptr->debug("%s: duration=%ld %s\n", name.c_str(), dur.count(), unit_str);
  if (warn_thres > TUnit{0} and dur > warn_thres) {
    log_ptr->warning("%s: detected long duration=%ld %s\n", name.c_str(), dur.count(), unit_str);
  }

  sliding_window[window_idx++] = duration;
  if (window_idx == sliding_window.size()) {
    nanoseconds tsum  = accumulate(sliding_window.begin(), sliding_window.end(), std::chrono::nanoseconds{0});
    nanoseconds tmax  = *std::max_element(sliding_window.begin(), sliding_window.end());
    nanoseconds tmin  = *std::min_element(sliding_window.begin(), sliding_window.end());
    double      tmean = static_cast<double>(duration_cast<TUnit>(tsum).count()) / sliding_window.size();

    log_ptr->info("%s: mean,max,min = %0.2f,%ld,%ld %s\n",
                  name.c_str(),
                  tmean,
                  duration_cast<TUnit>(tmax).count(),
                  duration_cast<TUnit>(tmin).count(),
                  unit_str);
    window_idx = 0;
  }
}

template class srslte::sliding_window_stats<std::chrono::microseconds>;
template class srslte::sliding_window_stats<std::chrono::milliseconds>;
