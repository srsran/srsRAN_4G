/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/system/sys_metrics_processor.h"
#include <fstream>
#include <sstream>
#include <sys/sysinfo.h>
#include <unistd.h>

using namespace srsran;

sys_metrics_processor::proc_stats_info::proc_stats_info()
{
  std::string line;
  {
    std::ifstream file("/proc/self/stat");
    if (!file) {
      return;
    }

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
  uint32_t measure_interval_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_query_time).count();

  // The time elapsed between 2 measures must be greater that 100 milliseconds.
  if (measure_interval_ms < 100u) {
    return {};
  }

  sys_metrics_t metrics;

  // Get the memory metrics.
  calculate_mem_usage(metrics);

  // Get the stats from the proc.
  proc_stats_info current_query;
  metrics.thread_count      = current_query.num_threads;
  metrics.process_cpu_usage = calculate_cpu_usage(current_query, measure_interval_ms / 1000.f);

  // Update the last values.
  last_query_time = current_time;
  last_query      = std::move(current_query);

  return metrics;
}

float sys_metrics_processor::calculate_cpu_usage(const proc_stats_info& current_query,
                                                 float                  delta_time_in_seconds) const
{
  // Error current value has to be greater than last value.
  if (current_query.stime < last_query.stime || current_query.utime < last_query.utime) {
    return -1.f;
  }

  // If current and last tick counter equals, means that the process didn't used CPU.
  if (current_query.stime == last_query.stime && current_query.utime == last_query.utime) {
    return 0.f;
  }

  static const uint32_t cpu_count        = ::sysconf(_SC_NPROCESSORS_CONF);
  static const float    ticks_per_second = ::sysconf(_SC_CLK_TCK);

  return ((current_query.stime + current_query.utime) - (last_query.stime + last_query.utime)) * 100.f /
         (cpu_count * ticks_per_second * delta_time_in_seconds);
}

/// Extracts and returns the memory size from the given line.
static uint32_t read_memory_value_from_line(const std::string& line)
{
  std::istringstream reader(line);
  std::string        label, unit;
  uint32_t           value;

  reader >> label >> value >> unit;

  return value;
}

/// Sets the memory parameters of the given metrics to zero.
static void set_mem_to_zero(sys_metrics_t& metrics)
{
  metrics.process_realmem_kB    = 0;
  metrics.process_virtualmem_kB = 0;
  metrics.process_virtualmem    = 0;
  metrics.process_realmem       = 0;
  metrics.system_mem            = 0;
}

static void calculate_percentage_memory(sys_metrics_t& metrics)
{
  std::ifstream file("/proc/meminfo");
  std::string   line;

  if (!file) {
    set_mem_to_zero(metrics);
    return;
  }

  // Total system's memory is in the first line.
  std::getline(file, line);
  uint32_t total_mem_kB = read_memory_value_from_line(line);

  // System's available memory is in the third line.
  std::getline(file, line);
  std::getline(file, line);
  uint32_t available_mem_kB = read_memory_value_from_line(line);

  // Calculate the metrics.
  metrics.process_realmem    = 100.f * (float(metrics.process_realmem_kB) / total_mem_kB);
  metrics.process_virtualmem = 100.f * (float(metrics.process_virtualmem_kB) / total_mem_kB);
  metrics.system_mem         = (1.f - float(available_mem_kB) / float(total_mem_kB)) * 100.f;
}

void sys_metrics_processor::calculate_mem_usage(sys_metrics_t& metrics) const
{
  std::ifstream file("/proc/self/status");
  std::string   line;

  if (!file) {
    set_mem_to_zero(metrics);
    return;
  }

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
