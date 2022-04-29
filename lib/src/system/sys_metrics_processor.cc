/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/system/sys_metrics_processor.h"
#include <fstream>
#include <sstream>
#include <sys/sysinfo.h>
#include <unistd.h>

using namespace srsran;

static const uint32_t cpu_count        = ::sysconf(_SC_NPROCESSORS_CONF);
static const float    ticks_per_second = ::sysconf(_SC_CLK_TCK);

sys_metrics_processor::sys_metrics_processor(srslog::basic_logger& logger) : logger(logger)
{
  if (cpu_count > metrics_max_supported_cpu) {
    logger.warning("Number of cpu is greater than supported. CPU metrics will be disabled.");
  }
}

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

/// Returns a null sys_metrics_t with the cpu count field filled.
static sys_metrics_t create_null_metrics()
{
  sys_metrics_t metrics;

  if (cpu_count > metrics_max_supported_cpu) {
    return metrics;
  }

  metrics.cpu_count = cpu_count;
  metrics.cpu_load.fill(0.f);
  return metrics;
}

sys_metrics_t sys_metrics_processor::get_metrics()
{
  auto     current_time = std::chrono::steady_clock::now();
  uint32_t measure_interval_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_query_time).count();

  // The time elapsed between 2 measures must be greater that 10 milliseconds.
  if (measure_interval_ms < 10u) {
    logger.info("Interval less than 10ms, skipping measurement.");
    return create_null_metrics();
  }

  sys_metrics_t metrics;

  // Get the memory metrics.
  calculate_mem_usage(metrics);

  // Calculate cpu metrics.
  calculate_cpu_metrics(metrics, measure_interval_ms / 1000.f);

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

  return ((current_query.stime + current_query.utime) - (last_query.stime + last_query.utime)) * 100.f /
         (cpu_count * ticks_per_second * delta_time_in_seconds);
}

sys_metrics_processor::cpu_metrics_t sys_metrics_processor::read_cpu_idle_from_line(const std::string& line) const
{
  std::istringstream reader(line);
  cpu_metrics_t      m;

  reader >> m.name >> m.user >> m.nice >> m.system >> m.idle >> m.iowait >> m.irq >> m.softirq;

  return m;
}

void sys_metrics_processor::calculate_cpu_metrics(sys_metrics_t& metrics, float delta_time_in_seconds)
{
  // When the number of cpu is higher than system_metrics_t supports, skip the cpu metrics.
  if (cpu_count > metrics_max_supported_cpu) {
    return;
  }

  metrics.cpu_count = cpu_count;

  std::ifstream file("/proc/stat");
  std::string   line;

  if (!file) {
    return;
  }

  int count = -1;
  while (std::getline(file, line)) {
    // First line is the CPU field that contains all the cores and thread. For now, we skip this one.
    if (count < 0) {
      ++count;
      continue;
    }

    // Parse all the cpus.
    if (line.find("cpu") != std::string::npos) {
      auto tmp   = read_cpu_idle_from_line(line);
      auto index = count++;
      if (tmp.idle < last_cpu_thread[index].idle) {
        metrics.cpu_load[index] = 0.f;
        continue;
      }

      metrics.cpu_load[index] = std::max(
          (1.f - (tmp.idle - last_cpu_thread[index].idle) / (ticks_per_second * delta_time_in_seconds)) * 100.f, 0.f);

      last_cpu_thread[index] = std::move(tmp);
    }
  }
}

/// Sets the memory parameters of the given metrics to zero.
static void set_mem_to_zero(sys_metrics_t& metrics)
{
  metrics.process_realmem_kB    = 0;
  metrics.process_virtualmem_kB = 0;
  metrics.process_realmem       = 0;
  metrics.system_mem            = 0;
}

/// Extracts and returns the memory size from the given line.
static int32_t read_memory_value_from_line(const std::string& line)
{
  std::istringstream reader(line);
  std::string        label, unit;
  int32_t            value;

  reader >> label >> value >> unit;

  return value;
}

static void calculate_percentage_memory(sys_metrics_t& metrics)
{
  std::ifstream file("/proc/meminfo");
  std::string   line;

  if (!file) {
    set_mem_to_zero(metrics);
    return;
  }

  struct meminfo_t {
    uint32_t total_kB   = 0;
    uint32_t free_kB    = 0;
    uint32_t buffers_kB = 0;
    uint32_t cached_kB  = 0;
    uint32_t slab_kB    = 0;
  };

  // Retrieve the data
  meminfo_t m_info;
  while (std::getline(file, line)) {
    // Looks for Virtual memory.
    if (line.find("MemTotal:") != std::string::npos) {
      m_info.total_kB = std::max(read_memory_value_from_line(line), 0);
    }
    if (line.find("MemFree:") != std::string::npos) {
      m_info.free_kB = std::max(read_memory_value_from_line(line), 0);
    }
    if (line.find("Buffers:") != std::string::npos) {
      m_info.buffers_kB = std::max(read_memory_value_from_line(line), 0);
    }
    if (line.find("Cached:") != std::string::npos) {
      m_info.cached_kB = std::max(read_memory_value_from_line(line), 0);
    }
    if (line.find("Slab:") != std::string::npos) {
      m_info.slab_kB = std::max(read_memory_value_from_line(line), 0);
    }
  }

  // Calculate the metrics.
  metrics.process_realmem = (metrics.process_realmem_kB <= m_info.total_kB)
                                ? 100.f * (float(metrics.process_realmem_kB) / m_info.total_kB)
                                : 0;
  metrics.system_mem =
      (1.f - float(m_info.buffers_kB + m_info.cached_kB + m_info.free_kB + m_info.slab_kB) / float(m_info.total_kB)) *
      100.f;
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
      // NOTE: std::max will clamp negative values to 0.
      metrics.process_virtualmem_kB = std::max(read_memory_value_from_line(line), 0);
      continue;
    }
    // Looks for physical memory.
    if (line.find("VmRSS:") != std::string::npos) {
      // NOTE: std::max will clamp negative values to 0.
      metrics.process_realmem_kB = std::max(read_memory_value_from_line(line), 0);
      continue;
    }
  }

  // Now calculate the memory usage in percentage.
  calculate_percentage_memory(metrics);
}
