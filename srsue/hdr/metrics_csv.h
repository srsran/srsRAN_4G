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

/******************************************************************************
 * File:        metrics_csv.h
 * Description: Metrics class writing to CSV file.
 *****************************************************************************/

#ifndef SRSUE_METRICS_CSV_H
#define SRSUE_METRICS_CSV_H

#include <fstream>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <stdint.h>
#include <string>

#include "srslte/common/metrics_hub.h"
#include "ue_metrics_interface.h"

namespace srsue {

class metrics_csv : public srslte::metrics_listener<ue_metrics_t>
{
public:
  metrics_csv(std::string filename, bool append = false);
  ~metrics_csv();

  void set_metrics(const ue_metrics_t& m, const uint32_t period_usec);
  void set_ue_handle(ue_metrics_interface* ue_);
  void set_flush_period(const uint32_t flush_period_sec);
  void stop();

private:
  std::string float_to_string(float f, int digits, bool add_semicolon = true);

  std::ofstream         file;
  ue_metrics_interface* ue               = nullptr;
  uint32_t              n_reports        = 0;
  std::mutex            mutex            = {};
  uint32_t              flush_period_sec = 0;
  uint32_t              flush_time_ms    = 0;
  uint64_t              time_ms          = 0;
  bool                  file_exists      = false;
};

} // namespace srsue

#endif // SRSUE_METRICS_CSV_H
