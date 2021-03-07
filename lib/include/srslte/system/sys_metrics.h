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

#ifndef SRSLTE_SYS_METRICS_H
#define SRSLTE_SYS_METRICS_H

#include <cstdint>

namespace srslte {

/// Metrics of cpu usage, memory consumption and number of thread used by the process.
struct sys_metrics_t {
  uint32_t process_realmem_kB    = 0;
  uint32_t process_virtualmem_kB = 0;
  float    process_realmem       = -1.f;
  float    process_virtualmem    = -1.f;
  uint32_t thread_count          = 0;
  float    process_cpu_usage     = -1.f;
  float    system_mem            = -1.f;
};

} // namespace srslte

#endif // SRSLTE_SYS_METRICS_H
