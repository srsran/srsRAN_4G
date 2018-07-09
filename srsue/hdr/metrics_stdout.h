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
 * File:        metrics_stdout.h
 * Description: Metrics class printing to stdout.
 *****************************************************************************/

#ifndef SRSUE_METRICS_STDOUT_H
#define SRSUE_METRICS_STDOUT_H

#include <pthread.h>
#include <stdint.h>
#include <string>

#include "srslte/common/metrics_hub.h"
#include "ue_metrics_interface.h"

namespace srsue {

class metrics_stdout : public srslte::metrics_listener<ue_metrics_t>
{
public:
  metrics_stdout();

  void set_periodicity(float metrics_report_period_sec);
  void toggle_print(bool b);
  void set_metrics(ue_metrics_t &m, const uint32_t period_usec);
  void set_ue_handle(ue_metrics_interface *ue_);
  void stop() {};

private:
  std::string float_to_string(float f, int digits);
  std::string float_to_eng_string(float f, int digits);
  std::string int_to_eng_string(int f, int digits);

  bool                  do_print;
  uint8_t               n_reports;
  ue_metrics_interface* ue;
};

} // namespace srsue

#endif // SRSUE_METRICS_STDOUT_H
