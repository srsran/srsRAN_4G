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
 * File:        metrics_json.h
 * Description: Metrics class printing to a json file.
 *****************************************************************************/

#ifndef SRSUE_METRICS_JSON_H
#define SRSUE_METRICS_JSON_H

#include "srsran/srslog/log_channel.h"
#include "ue_metrics_interface.h"

namespace srsue {

class metrics_json : public srsran::metrics_listener<ue_metrics_t>
{
public:
  explicit metrics_json(srslog::log_channel& c) : log_c(c) {}

  void set_metrics(const ue_metrics_t& m, const uint32_t period_usec) override;
  void set_ue_handle(ue_metrics_interface* ue_);
  void stop() override {}

private:
  srslog::log_channel&  log_c;
  ue_metrics_interface* ue = nullptr;

  std::mutex mutex = {};
};

} // namespace srsue

#endif // SRSUE_METRICS_JSON_H
