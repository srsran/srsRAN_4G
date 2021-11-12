/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 * File:        metrics_csv.h
 * Description: Metrics class writing to CSV file.
 *****************************************************************************/

#ifndef SRSENB_METRICS_CSV_H
#define SRSENB_METRICS_CSV_H

#include <fstream>
#include <iostream>
#include <pthread.h>
#include <stdint.h>
#include <string>

#include "srsran/common/metrics_hub.h"
#include "srsran/interfaces/enb_metrics_interface.h"

namespace srsenb {

class metrics_csv : public srsran::metrics_listener<enb_metrics_t>
{
public:
  metrics_csv(std::string filename, enb_metrics_interface* enb_);
  ~metrics_csv();

  void set_metrics(const enb_metrics_t& m, const uint32_t period_usec);
  void stop();

private:
  std::string float_to_string(float f, int digits, bool add_semicolon = true);

  float                  metrics_report_period;
  std::ofstream          file;
  enb_metrics_interface* enb;
  uint32_t               n_reports;
};

} // namespace srsenb

#endif // SRSENB_METRICS_CSV_H
