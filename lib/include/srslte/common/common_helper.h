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

/**
 * @file common_helper.h
 * @brief Common helper functions shared among srsLTE applications.
 */

#ifndef SRSLTE_COMMON_HELPER_H
#define SRSLTE_COMMON_HELPER_H

#include "srslte/common/logmap.h"
#include <fstream>
#include <sstream>
#include <thread>

namespace srslte {

inline void log_args(int argc, char* argv[], const std::string& service)
{
  std::ostringstream s1;
  s1 << "Using binary " << argv[0] << " with arguments: ";
  for (int32_t i = 1; i < argc; i++) {
    s1 << argv[i] << " ";
  }
  s1 << std::endl;

  srslte::logmap::get(service)->set_level(srslte::LOG_LEVEL_INFO);
  srslte::logmap::get(service)->info("%s", s1.str().c_str());
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

} // namespace srslte

#endif // SRSLTE_COMMON_HELPER_H
