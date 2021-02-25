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

#include "srslte/system/sys_metrics_processor.h"
#include "sys/sysinfo.h"
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace srslte;

sys_metrics_processor::proc_stats_info::proc_stats_info()
{
  std::string line;
  {
    std::ifstream file("/proc/self/stat");
    std::getline(file, line);
  }

  std::istringstream reader(line);
  reader >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >>
      majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >> nice >> num_threads >> itrealvalue >>
      starttime >> vsize >> rss >> rsslim >> startcode >> endcode >> startstack >> kstkeip >> signal >> blocked >>
      sigignore >> sigcatch >> wchan >> nswap >> cnswap >> exit_signal >> processor >> rt_priority >> policy >>
      delaycct_blkio_ticks >> guest_time >> cguest_time >> start_data >> end_data >> start_brk >> arg_start >>
      arg_end >> env_start >> env_end >> exit_code;
}

sys_metrics_t sys_metrics_processor::get_metrics()
{
  auto     current_time = std::chrono::steady_clock::now();
  unsigned measure_interval_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_query_time).count();

  // The time elapsed between 2 measures must be greater that 100 milliseconds.
  if (measure_interval_ms < 100u) {
    return {};
  }

  sys_metrics_t metrics;

  // Get the memory metrics.
  mem_usage(metrics);

  // Get the stats from the proc.
  proc_stats_info current_query;
  metrics.thread_count      = current_query.num_threads;
  metrics.process_cpu_usage = cpu_usage(current_query, measure_interval_ms / 1000.f);

  // Update the last values.
  last_query_time = current_time;
  last_query      = std::move(current_query);

  return metrics;
}

float sys_metrics_processor::cpu_usage(const proc_stats_info& current_query, float delta_time_in_seconds) const
{
  // Error current value has to be greater than last value.
  if (current_query.stime < last_query.stime || current_query.utime < last_query.utime) {
    return -1.f;
  }

  // If current and last tick counter equals, means that the process didn't used CPU.
  if (current_query.stime == last_query.stime && current_query.utime == last_query.utime) {
    return 0.f;
  }

  static const unsigned cpu_count        = ::sysconf(_SC_NPROCESSORS_CONF);
  static const float    ticks_per_second = ::sysconf(_SC_CLK_TCK);

  return ((current_query.stime + current_query.utime) - (last_query.stime + last_query.utime)) * 100.f /
         (cpu_count * ticks_per_second * delta_time_in_seconds);
}

/// Extracts and returns the memory size from the given line.
static unsigned read_memory_value_from_line(const std::string& line)
{
  std::istringstream reader(line);
  std::string        label, unit;
  unsigned           value;

  reader >> label >> value >> unit;

  return value;
}

static void calculate_percentage_memory(sys_metrics_t& metrics)
{
  std::ifstream file("/proc/meminfo");
  std::string   line;

  // Total system's memory is in the first line.
  std::getline(file, line);
  unsigned long long total_mem_kB = read_memory_value_from_line(line);

  // System's available memory is in the third line.
  std::getline(file, line);
  std::getline(file, line);
  unsigned long long available_mem_kB = read_memory_value_from_line(line);

  // Calculate the metrics.
  metrics.process_realmem    = 100.f * (float(metrics.process_realmem_kB) / total_mem_kB);
  metrics.process_virtualmem = 100.f * (float(metrics.process_virtualmem) / total_mem_kB);
  metrics.system_mem         = (1.f - float(available_mem_kB) / float(total_mem_kB)) * 100.f;
}

void sys_metrics_processor::mem_usage(sys_metrics_t& metrics) const
{
  std::ifstream file("/proc/self/status");
  std::string   line;

  while (std::getline(file, line)) {
    // Looks for Virtual memory.
    if (line.find("VmSize:") != std::string::npos) {
      metrics.process_virtualmem_kB = read_memory_value_from_line(line);
      continue;
    }
    // Looks for physical memory.
    if (line.find("VmRSS:") != std::string::npos) {
      metrics.process_realmem_kB = read_memory_value_from_line(line);
      continue;
    }
  }

  // Now calculate the memory usage in percentage.
  calculate_percentage_memory(metrics);
}
