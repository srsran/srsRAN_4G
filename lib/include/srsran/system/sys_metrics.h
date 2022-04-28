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

#ifndef SRSRAN_SYS_METRICS_H
#define SRSRAN_SYS_METRICS_H

#include <array>
#include <cstdint>

namespace srsran {

constexpr uint32_t metrics_max_supported_cpu = 32u;

/// Metrics of cpu usage, memory consumption and number of thread used by the process.
struct sys_metrics_t {
  uint32_t                                     process_realmem_kB    = 0;
  uint32_t                                     process_virtualmem_kB = 0;
  float                                        process_realmem       = 0.f;
  uint32_t                                     thread_count          = 0;
  float                                        process_cpu_usage     = 0.f;
  float                                        system_mem            = 0.f;
  uint32_t                                     cpu_count             = 0;
  std::array<float, metrics_max_supported_cpu> cpu_load              = {};
};

} // namespace srsran

#endif // SRSRAN_SYS_METRICS_H
