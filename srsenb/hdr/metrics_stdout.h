/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#ifndef SRSENB_METRICS_STDOUT_H
#define SRSENB_METRICS_STDOUT_H

#include <pthread.h>
#include <stdint.h>
#include <string>

#include "srslte/interfaces/enb_metrics_interface.h"

namespace srsenb {

class metrics_stdout
{
public:
  metrics_stdout();

  bool init(enb_metrics_interface *u, float report_period_secs=1.0);
  void stop();
  void toggle_print(bool b);
  static void* metrics_thread_start(void *m);
  void metrics_thread_run();

private:
  void        print_metrics();
  void        print_disconnect();
  std::string float_to_string(float f, int digits);
  std::string float_to_eng_string(float f, int digits);
  std::string int_to_eng_string(int f, int digits);
  
  enb_metrics_interface *enb_;

  bool          started;
  bool          do_print;
  pthread_t     metrics_thread;
  enb_metrics_t  metrics;
  float         metrics_report_period; // seconds
  uint8_t       n_reports;
};

} // namespace srsenb

#endif // SRSENB_METRICS_STDOUT_H
