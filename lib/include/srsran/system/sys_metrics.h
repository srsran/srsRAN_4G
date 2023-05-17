/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
