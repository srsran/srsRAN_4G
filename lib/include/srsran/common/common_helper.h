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

/**
 * @file common_helper.h
 * @brief Common helper functions shared among srsRAN applications.
 */

#ifndef SRSRAN_COMMON_HELPER_H
#define SRSRAN_COMMON_HELPER_H

#include "srsran/srslog/srslog.h"
#include <fstream>
#include <sstream>
#include <thread>

namespace srsran {

inline void log_args(int argc, char* argv[], const std::string& service)
{
  std::ostringstream s1;
  s1 << "Using binary " << argv[0] << " with arguments: ";
  for (int32_t i = 1; i < argc; i++) {
    s1 << argv[i] << " ";
  }

  srslog::fetch_basic_logger(service, false).set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger(service).info("%s", s1.str().c_str());
}

inline void check_scaling_governor(const std::string& device_name)
{
  if (device_name == "zmq") {
    return;
  }
  int nof_cpus = std::thread::hardware_concurrency();
  for (int cpu = 0; cpu < nof_cpus; ++cpu) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor", cpu);
    std::ifstream file(buffer);
    if (file.is_open()) {
      std::stringstream ss;
      ss << file.rdbuf();
      if (ss.str().find("performance") == std::string::npos) {
        printf(
            "WARNING: cpu%d scaling governor is not set to performance mode. Realtime processing could be compromised. "
            "Consider setting it to performance mode before running the application.\n",
            cpu);
        break;
      }
    } else {
      printf("WARNING: Could not verify cpu%d scaling governor\n", cpu);
    }
  }
}

} // namespace srsran

#endif // SRSRAN_COMMON_HELPER_H
