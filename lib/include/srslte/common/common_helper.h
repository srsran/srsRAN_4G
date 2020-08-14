/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

/**
 * @file common_helper.h
 * @brief Common helper functions shared among srsLTE applications.
 */

#ifndef SRSLTE_COMMON_HELPER_H
#define SRSLTE_COMMON_HELPER_H

#include "srslte/common/logmap.h"
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

inline void log_args(int argc, char* argv[], std::string service)
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
  std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
  bool          found = false;
  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      if (line.find("performance") != std::string::npos) {
        found = true;
        break;
      }
    }
  }
  if (not found) {
    printf("WARNING: cpu scaling governor is not set to performance mode.\n");
  }
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SRSLTE_COMMON_HELPER_H
