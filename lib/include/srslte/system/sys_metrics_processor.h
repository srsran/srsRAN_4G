/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSLTE_SYS_METRICS_PROCESSOR_H
#define SRSLTE_SYS_METRICS_PROCESSOR_H

#include "srslte/system/sys_metrics.h"
#include <chrono>
#include <string>

namespace srslte {

/// Process information from the system to create sys_metrics_t. The information is processed from the /proc/ system.
class sys_metrics_processor
{
  /// Helper class used to parse the information from the /proc/[pid]/stats.
  struct proc_stats_info {
    proc_stats_info();

    int           pid, ppid, pgrp, session, tty_nr, tpgid, exit_signal, processor, exit_code;
    unsigned      flags, rt_priority, policy;
    unsigned long minflt, cminflt, majflt, cmajflt, utime, stime, vsize, rsslim, startcode, endcode, startstack,
        kstkesp, kstkeip, signal, blocked, sigignore, sigcatch, wchan, nswap, cnswap, guest_time, start_data, end_data,
        start_brk, arg_start, arg_end, env_start, env_end;
    int long           cutime, cstime, priority, nice, num_threads, itrealvalue, rss, cguest_time;
    unsigned long long starttime, delaycct_blkio_ticks;
    char               state;
    std::string        comm;
  };

public:
  /// Measures and returns the system metrics.
  sys_metrics_t get_metrics();

private:
  /// Calculates and returns the cpu usage in %. current_query is the most recent proc_stats_info, and
  /// delta_time_in_seconds is the elapsed time between the last measure and current in seconds. NOTE: Returns -1.0f on
  /// error.
  float calculate_cpu_usage(const proc_stats_info& current_query, float delta_time_in_seconds) const;

  /// Calculate the memory parameters and writes them in metrics.
  /// NOTE: on error, metrics memory parameters are set to 0.
  void calculate_mem_usage(sys_metrics_t& metrics) const;

private:
  proc_stats_info                                    last_query;
  std::chrono::time_point<std::chrono::steady_clock> last_query_time = std::chrono::steady_clock::now();
};

} // namespace srslte

#endif // SRSLTE_SYS_METRICS_PROCESSOR_H
