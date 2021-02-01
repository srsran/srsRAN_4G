/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/common/time_prof.h"
#include <algorithm>
#include <inttypes.h>
#include <numeric>

using namespace srslte;
using std::chrono::nanoseconds;

// log utils

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

// tprof stats

avg_time_stats::avg_time_stats(const char* name_, const char* logname, size_t print_period_) :
  name(name_), logger(srslog::fetch_basic_logger(logname)), print_period(print_period_)
{}

void avg_time_stats::operator()(nanoseconds duration)
{
  count++;
  avg_val = avg_val * (count - 1) / count + static_cast<double>(duration.count()) / count;
  max_val = std::max<long>(max_val, duration.count());
  min_val = std::min<long>(min_val, duration.count());
  if (count % print_period == 0) {
    logger.info("%s: {mean, max, min}={%0.1f, %ld, %ld} usec, nof_samples=%ld",
                name.c_str(),
                avg_val / 1e3,
                max_val / 1000,
                min_val / 1000,
                count);
  }
}

template <typename TUnit>
sliding_window_stats<TUnit>::sliding_window_stats(const char* name_, const char* logname, size_t print_period_) :
  name(name_), logger(srslog::fetch_basic_logger(logname)), sliding_window(print_period_)
{}

template <typename TUnit>
void sliding_window_stats<TUnit>::operator()(nanoseconds duration)
{
  using std::chrono::duration_cast;
  const char* unit_str = get_tunit_str<TUnit>();
  TUnit       dur      = std::chrono::duration_cast<TUnit>(duration);

  logger.debug("%s: duration=%" PRId64 " %s", name.c_str(), dur.count(), unit_str);

  sliding_window[window_idx++] = duration;
  if (window_idx == sliding_window.size()) {
    nanoseconds tsum  = accumulate(sliding_window.begin(), sliding_window.end(), std::chrono::nanoseconds{0});
    nanoseconds tmax  = *std::max_element(sliding_window.begin(), sliding_window.end());
    nanoseconds tmin  = *std::min_element(sliding_window.begin(), sliding_window.end());
    double      tmean = static_cast<double>(duration_cast<TUnit>(tsum).count()) / sliding_window.size();

    logger.info("%s: {mean, max, min} = {%0.2f, %" PRId64 ", %" PRId64 "} %s",
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
