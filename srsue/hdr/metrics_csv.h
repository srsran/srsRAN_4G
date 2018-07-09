/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include <pthread.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream>

#include "srslte/common/metrics_hub.h"
#include "ue_metrics_interface.h"

namespace srsue {

class metrics_csv : public srslte::metrics_listener<ue_metrics_t>
{
public:
  metrics_csv(std::string filename);
  ~metrics_csv();

  void set_metrics(ue_metrics_t &m, const uint32_t period_usec);
  void set_ue_handle(ue_metrics_interface *ue_);
  void stop();

private:
  std::string float_to_string(float f, int digits, bool add_semicolon = true);

  float                 metrics_report_period;
  std::ofstream         file;
  ue_metrics_interface* ue;
  uint32_t              n_reports;
};

} // namespace srsue

#endif // SRSUE_METRICS_CSV_H
