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
 */

#ifndef SRSRAN_TUNER_H
#define SRSRAN_TUNER_H

#include "srsran/config.h"
#include "srsran/phy/common/timestamp.h"
#include "srsran/srslog/logger.h"
#include <atomic>
#include <fstream>
#include <memory>
#include <string>
#include <thread>

// Class definition for the channel tuner
class srsran_channel_tuner_t {
public:
  // Public members
  std::atomic<float> tuner_attenuation;
  std::unique_ptr<std::thread> tuner_monitor_thread;
  srslog::basic_logger& logger;
  std::ifstream sock;
  std::string domain_socket_name;

  // Constructor
  srsran_channel_tuner_t(srslog::basic_logger& logger_ref,
                         const std::string& tuner_name = "Tuner",
                         const std::string& domain_socket_name = "/tmp/dlt.sock",
                         float attenuation = 1.0f)
      : tuner_attenuation(attenuation),
        logger(logger_ref),
        domain_socket_name(domain_socket_name) {
        sock.open(tuner_name);
  }
};

// C-compatible API for srsran_channel_tuner_t
#ifdef __cplusplus
extern "C" {
#endif

SRSRAN_API void srsran_channel_tuner_execute(srsran_channel_tuner_t*     q,
                                             const cf_t*               in,
                                             cf_t*                     out,
                                             uint32_t                  nsamples,
                                             const srsran_timestamp_t* ts);

SRSRAN_API void srsran_channel_tuner_free(srsran_channel_tuner_t* q);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_TUNER_H
