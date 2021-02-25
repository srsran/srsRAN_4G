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

/// Process the data from the proc_stats_info.
class sys_metrics_processor
{
  /// Contains the information of a process.
  struct proc_stats_info {
    proc_stats_info();

    /// Parsed every field in /proc/[pid]/stats file.
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
  /// Performs a new measure and returns the values.
  sys_metrics_t get_metrics();

private:
  /// Returns the cpu usage in %. current_query is the most recent proc_stats_info, and delta_time_in_seconds is the
  /// elapsed time between the last measure and current in seconds.
  /// NOTE: Returns -1.0f on error.
  float cpu_usage(const proc_stats_info& current_query, float delta_time_in_seconds) const;

  /// Returns the memory usage in kB. First element is the real mem whereas the second is the virtual mem.
  void mem_usage(sys_metrics_t& metrics) const;

private:
  proc_stats_info                                    last_query;
  std::chrono::time_point<std::chrono::steady_clock> last_query_time = std::chrono::steady_clock::now();
};

} // namespace srslte

#endif // SRSLTE_SYS_METRICS_PROCESSOR_H
