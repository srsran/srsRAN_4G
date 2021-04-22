/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_SYS_METRICS_PROCESSOR_H
#define SRSRAN_SYS_METRICS_PROCESSOR_H

#include "srsran/srslog/logger.h"
#include "srsran/system/sys_metrics.h"
#include <chrono>
#include <string>

namespace srsran {

/// Process information from the system to create sys_metrics_t. The information is processed from the /proc/ system.
class sys_metrics_processor
{
  /// Helper class used to parse the information from the /proc/[pid]/stats.
  struct proc_stats_info {
    proc_stats_info();

    // Initialize the variables that will be used.
    int32_t  num_threads = 0;
    uint32_t utime       = 0;
    uint32_t stime       = 0;

    // Rest of the information of the stats file.
    int32_t pid, ppid, pgrp, session, tty_nr, tpgid, exit_signal, processor, exit_code, cutime, cstime, priority, nice,
        itrealvalue, rss, cguest_time;
    uint32_t minflt, cminflt, majflt, cmajflt, vsize, rsslim, startcode, endcode, startstack, kstkesp, kstkeip, signal,
        blocked, sigignore, sigcatch, wchan, nswap, cnswap, guest_time, start_data, end_data, start_brk, arg_start,
        arg_end, env_start, env_end, flags, rt_priority, policy;
    uint64_t    starttime, delaycct_blkio_ticks;
    uint8_t     state;
    std::string comm;
  };

  /// Helper class to read the cpu metrics.
  struct cpu_metrics_t {
    std::string name    = "";
    int32_t     user    = 0;
    int32_t     nice    = 0;
    int32_t     system  = 0;
    int32_t     idle    = 0;
    int32_t     iowait  = 0;
    int32_t     irq     = 0;
    int32_t     softirq = 0;
  };

public:
  explicit sys_metrics_processor(srslog::basic_logger& logger);
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

  /// Calculate the cpu metrics and stores them in the given metrics. delta_time_in_seconds is the number of seconds
  /// elapsed since the last cpu metrics measurement.
  void calculate_cpu_metrics(sys_metrics_t& metrics, float delta_time_in_seconds);

  /// Returns the cpu metrics from the given line.
  cpu_metrics_t read_cpu_idle_from_line(const std::string& line) const;

private:
  srslog::basic_logger&                              logger;
  proc_stats_info                                    last_query                                 = {};
  cpu_metrics_t                                      last_cpu_thread[metrics_max_supported_cpu] = {};
  std::chrono::time_point<std::chrono::steady_clock> last_query_time = std::chrono::steady_clock::now();
};

} // namespace srsran

#endif // SRSRAN_SYS_METRICS_PROCESSOR_H
